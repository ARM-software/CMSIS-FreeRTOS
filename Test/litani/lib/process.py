# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License").
# You may not use this file except in compliance with the License.
# A copy of the License is located at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# or in the "license" file accompanying this file. This file is distributed
# on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
# express or implied. See the License for the specific language governing
# permissions and limitations under the License


import abc
import asyncio
import dataclasses
import datetime
import decimal
import logging
import os
import platform
import signal
import subprocess
import sys

import lib.litani



class _MemoryProfiler:
    @abc.abstractmethod
    async def snapshot(self, root_pid):
        raise NotImplementedError



class _UnixMemoryProfiler(_MemoryProfiler):
    def __init__(self):
        self.pids = set()
        self.ps_cmd = "ps"
        self.ps_args = ["-x", "-o", "pid,ppid,rss,vsz"]


    async def snapshot(self, root_pid):
        """Return a dict containing memory usage of process and children"""

        ret = {
            "time":
                datetime.datetime.now(datetime.timezone.utc).strftime(
                    lib.litani.TIME_FORMAT_R),
            "rss": 0,
        }
        ps_output = await self._get_ps_output()

        self.pids.add(root_pid)
        current_pids = set(self.pids)
        seen_pids = set()
        while current_pids:
            new_pids = set()
            for pid in current_pids:
                if pid in seen_pids:
                    continue
                seen_pids.add(pid)
                self._add_usage(pid, ps_output, ret)
                for child in self._children_of(pid, ps_output):
                    if child not in seen_pids:
                        new_pids.add(child)
            current_pids = new_pids
        return ret


    @staticmethod
    def human_readable(memory):
        units = ["B", "KiB", "MiB", "GiB", "TiB"]
        idx = 0
        memory = decimal.Decimal(memory)
        while memory > 1023:
            idx += 1
            memory /= 1024
        memory_str = memory.quantize(
            decimal.Decimal("0.1"), rounding=decimal.ROUND_HALF_UP)
        return f"{memory_str} {units[idx]}"


    def compute_peak(self, trace):
        peak = {}
        for item in trace:
            for k, v in item.items():
                if k in ["time"]:
                    continue
                try:
                    peak[k] = max(peak[k], v)
                except KeyError:
                    peak[k] = v

        human_readable = {}
        for k, v in peak.items():
            human_readable[f"human_readable_{k}"] = self.human_readable(v)
        return {**peak, **human_readable}


    async def _get_ps_output(self):
        """ Format: {
            "fields": ["pid", "ppid", "rss", "vsz"],
            "processes": {
                123: {
                    "pid": 123,
                    "ppid": 457,
                    "rss": 435116,
                    "vsz": 5761524
                }
            }
        }"""
        proc = await asyncio.create_subprocess_exec(
            self.ps_cmd, *self.ps_args, stdout=subprocess.PIPE)
        stdout, _ = await proc.communicate()
        if proc.returncode:
            logging.error(
                "%s exited (return code %d)", " ".join(self.ps_cmd),
                proc.returncode)
            sys.exit(1)
        ret = {
            "fields": [],
            "processes": {}
        }
        for line in stdout.decode("utf-8").splitlines():
            if not ret["fields"]:
                ret["fields"] = [f.lower() for f in line.split()]
                continue
            for idx, value in enumerate(line.split()):
                if not idx:
                    current_pid = int(value)
                    ret["processes"][current_pid] = {"pid": current_pid}
                    continue
                field = ret["fields"][idx]
                value = int(value) * 1024 if field in ["rss", "vsz"] else int(value)
                ret["processes"][current_pid][field] = value
        return ret


    @staticmethod
    def _children_of(pid, ps_output):
        for child_pid, child_process in ps_output["processes"].items():
            if child_process["ppid"] == pid:
                yield child_pid


    @staticmethod
    def _add_usage(pid, ps_output, datum):
        try:
            process_record = ps_output["processes"][pid]
        except KeyError:
            return

        for field, value in process_record.items():
            if field in ["pid", "ppid"]:
                continue
            try:
                datum[field] += value
            except KeyError:
                datum[field] = value



@dataclasses.dataclass
class _MemoryProfileAccumulator:
    profiler: _MemoryProfiler
    profile_interval: int
    pid: int = None
    trace: dict = dataclasses.field(default_factory=dict)


    def set_pid(self, pid):
        self.pid = pid


    async def __call__(self):
        try:
            while True:
                if not self.pid:
                    await asyncio.sleep(1)
                    continue
                result = await self.profiler.snapshot(self.pid)
                if result:
                    try:
                        self.trace["trace"].append(result)
                    except KeyError:
                        self.trace["trace"] = [result]
                await asyncio.sleep(self.profile_interval)
        except asyncio.CancelledError:
            if "trace" in self.trace:
                self.trace["peak"] = self.profiler.compute_peak(
                    self.trace["trace"])



@dataclasses.dataclass
class _Process:
    command: str
    interleave_stdout_stderr: bool
    timeout: int
    cwd: str
    job_id: str
    proc: subprocess.CompletedProcess = None
    stdout: str = None
    stderr: str = None
    timeout_reached: bool = None


    async def __call__(self):
        if self.interleave_stdout_stderr:
            pipe = asyncio.subprocess.STDOUT
        else:
            pipe = asyncio.subprocess.PIPE

        env = dict(os.environ)
        env[lib.litani.ENV_VAR_JOB_ID] = self.job_id

        proc = await asyncio.create_subprocess_shell(
            self.command, stdout=asyncio.subprocess.PIPE, stderr=pipe,
            cwd=self.cwd, env=env, start_new_session=True)
        self.proc = proc

        timeout_reached = False
        try:
            out, err = await asyncio.wait_for(
                proc.communicate(), timeout=self.timeout)
        except asyncio.TimeoutError:
            pgid = os.getpgid(proc.pid)
            os.killpg(pgid, signal.SIGTERM)
            await asyncio.sleep(1)
            try:
                os.killpg(pgid, signal.SIGKILL)
            except ProcessLookupError:
                pass
            out, err = await proc.communicate()
            timeout_reached = True

        self.stdout = out
        self.stderr = err
        self.timeout_reached = timeout_reached



class Runner:
    @staticmethod
    def get_memory_profiler(profile_memory, system):
        if not profile_memory:
            return None

        return {
            "Linux": _UnixMemoryProfiler(),
            "Darwin": _UnixMemoryProfiler(),
        }.get(system, None)


    def __init__(
            self, command, interleave_stdout_stderr, cwd, timeout,
            profile_memory, profile_interval, job_id):
        self.tasks = []
        self.runner = _Process(
            command=command, interleave_stdout_stderr=interleave_stdout_stderr,
            cwd=cwd, timeout=timeout, job_id=job_id)
        self.tasks.append(self.runner)

        self.profiler = None
        profiler = self.get_memory_profiler(profile_memory, platform.system())
        if profiler:
            self.profiler = _MemoryProfileAccumulator(profiler, profile_interval)
            self.tasks.append(self.profiler)


    async def __call__(self):
        tasks = []
        for task in self.tasks:
            tasks.append(asyncio.create_task(task()))
        if self.profiler:
            for _ in range(10):
                if self.runner.proc:
                    self.profiler.set_pid(self.runner.proc.pid)
                    break
                await asyncio.sleep(1)
        _, pending = await asyncio.wait(
            tasks, return_when=asyncio.FIRST_COMPLETED)

        for task in pending:
            task.cancel()
            await task


    def get_return_code(self):
        return self.runner.proc.returncode


    def get_stdout(self):
        if self.runner.stdout:
            return self.runner.stdout.decode("utf-8")
        return None


    def get_stderr(self):
        if self.runner.stderr:
            return self.runner.stderr.decode("utf-8")
        return None


    def reached_timeout(self):
        return self.runner.timeout_reached


    def get_memory_trace(self):
        return self.profiler.trace if self.profiler else {}
