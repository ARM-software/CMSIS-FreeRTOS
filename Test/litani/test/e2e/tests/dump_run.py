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


import json
import os
import pathlib
import sys


def get_init_args():
    return {
        "kwargs": {
            "project": "foo",
        }
    }


def get_jobs():
    litani_path = pathlib.Path(os.environ["LITANI_E2E_LITANI_PATH"])
    if not litani_path.exists():
        raise UserWarning("Could not find litani executable")

    return [{
        "kwargs": {
            "command": f"sleep 2",
            "description": "SLEEP",
            "outputs": "sleep-output",
            "ci-stage": "build",
            "pipeline": "foo",
        }
    }, {
        "kwargs": {
            "command": f"{litani_path.resolve()} dump-run",
            "description": "DUMP",
            "inputs": "sleep-output",
            "timeout": 20,
            "ci-stage": "build",
            "pipeline": "foo",
        }
    }, {
        "kwargs": {
            "command": "sleep 10",
            "description": "LONG",
            "ci-stage": "build",
            "pipeline": "foo",
        }
    }]


def get_run_build_args():
    return {}


def perror(*args, **kwargs):
    kwargs = {
        **kwargs, "file": sys.stderr
    }
    print(*args, **kwargs)


def check_run(run):
    pipe = run["pipelines"][0]
    jobs = pipe["ci_stages"][0]["jobs"]
    dump_job = [
        j for j in jobs
        if j["wrapper_arguments"]["description"] == "DUMP"
    ]
    if not dump_job:
        perror("Dump job doesn't exist")
        return False

    run_dump = "\n".join(dump_job[0]["stdout"])
    try:
        run_so_far = json.loads(run_dump)
    except json.decoder.JSONDecodeError:
        perror("JSON decode")
        perror(run_dump)
        return False

    pipe_so_far = run_so_far["pipelines"][0]
    jobs = pipe_so_far["ci_stages"][0]["jobs"]

    sleep_job = [
        j for j in jobs
        if j["wrapper_arguments"]["description"] == "SLEEP"
    ]
    if not sleep_job:
        perror("sleep job doesn't exist")
        return False

    if not all((
            sleep_job[0]["complete"],
            sleep_job[0]["duration"] > 1,
    )):
        perror("sleep job format")
        return False

    long_job = [
        j for j in jobs
        if j["wrapper_arguments"]["description"] == "LONG"
    ]
    if not long_job:
        perror("long job doesn't exist")
        return False

    if long_job[0]["complete"]:
        perror("long job already complete before dumping run")
        return False

    return True
