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
# permissions and limitations under the License.


import datetime

import lib.litani


def _ms_time_str(string):
    try:
        datetime.datetime.strptime(string, lib.litani.TIME_FORMAT_MS)
    except RuntimeError as e:
        raise ValueError(
            "Date '%s' was not in the right format (expected '%s')" %
            (string, lib.litani.TIME_FORMAT_MS)) from e


def _time_str(string):
    try:
        datetime.datetime.strptime(string, lib.litani.TIME_FORMAT_R)
    except RuntimeError as e:
        raise ValueError(
            "Date '%s' was not in the right format (expected '%s')" %
            (string, lib.litani.TIME_FORMAT_R)) from e


def _get_single_job_arguments():
    import voluptuous
    return {
        "job_id": str,
        "command": str,
        "ci_stage": str,
        "verbose": bool,
        "timeout_ok": bool,
        "pipeline_name": str,
        "very_verbose": bool,
        "timeout_ignore": bool,
        "profile_memory": bool,
        "profile_memory_interval": int,
        "cwd": voluptuous.Any(str, None),
        "interleave_stdout_stderr": bool,
        "pool": voluptuous.Any(str, None),
        "tags": voluptuous.Any([str], None),
        "timeout": voluptuous.Any(int, None),
        "inputs": voluptuous.Any([str], None),
        "outputs": voluptuous.Any([str], None),
        "description": voluptuous.Any(str, None),
        "status_file": voluptuous.Any(str, None),
        "stderr_file": voluptuous.Any(str, None),
        "stdout_file": voluptuous.Any(str, None),
        "ok_returns": voluptuous.Any([str], None),
        "outcome_table": voluptuous.Any(str, None),
        "ignore_returns": voluptuous.Any([str], None),
        "subcommand": voluptuous.Any("exec", "add-job"),
    }


def validate_single_job(job):
    import voluptuous
    import voluptuous.humanize
    schema = voluptuous.Schema(
        _get_single_job_arguments(), required=True)
    voluptuous.humanize.validate_with_humanized_errors(job, schema)


def validate_run(run):
    import voluptuous
    import voluptuous.humanize
    outcome = voluptuous.Any("success", "fail", "fail_ignored")
    schema = voluptuous.Schema({
        "aux": dict,
        "run_id": str,
        "project": str,
        "pools": {voluptuous.Optional(str): int},
        "start_time": _time_str,
        "version": lib.litani.VERSION,
        "version_major": lib.litani.VERSION_MAJOR,
        "version_minor": lib.litani.VERSION_MINOR,
        "version_patch": lib.litani.VERSION_PATCH,
        voluptuous.Optional("end_time"): _time_str,
        "status": voluptuous.Any("in_progress", "fail", "success"),
        "parallelism": voluptuous.Any({
            voluptuous.Optional("trace"): [{
                "time": _ms_time_str,
                "finished": int,
                "running": int,
                "total": int,
            }],
            voluptuous.Optional("max_parallelism"): int,
            voluptuous.Optional("n_proc"): voluptuous.Any(None, int),
        }),
        "pipelines": [{
            "url": str,
            "name": str,
            "status": voluptuous.Any("in_progress", "fail", "success"),
            "ci_stages": [{
                "url": str,
                "complete": bool,
                "name": voluptuous.Any("build", "test", "report"),
                "status": voluptuous.Any("fail", "fail_ignored", "success"),
                "progress": voluptuous.All(int, voluptuous.Range(min=0, max=100)),
                "jobs": [voluptuous.Any({
                    "complete": False,
                    "duration_str": voluptuous.Any(str, None),
                    "wrapper_arguments": _get_single_job_arguments(),
                }, {
                    "complete": False,
                    "start_time": _time_str,
                    "duration_str": voluptuous.Any(str, None),
                    "wrapper_arguments": _get_single_job_arguments(),
                }, {
                    "duration": int,
                    "complete": True,
                    "outcome": outcome,
                    "end_time": _time_str,
                    "start_time": _time_str,
                    "timeout_reached": bool,
                    "command_return_code": int,
                    "wrapper_return_code": int,
                    "stderr": voluptuous.Any([str], None),
                    "stdout": voluptuous.Any([str], None),
                    "duration_str": voluptuous.Any(str, None),
                    "wrapper_arguments": _get_single_job_arguments(),
                    "loaded_outcome_dict": voluptuous.Any(dict, None),
                    "memory_trace": {
                        voluptuous.Optional("peak"): {
                            "rss": int,
                            "vsz": int,
                            "human_readable_rss": str,
                            "human_readable_vsz": str,
                        },
                        voluptuous.Optional("trace"): [{
                            "rss": int,
                            "vsz": int,
                            "time": _time_str,
                    }]},
                })]
            }]
        }]
    }, required=True)
    voluptuous.humanize.validate_with_humanized_errors(run, schema)
