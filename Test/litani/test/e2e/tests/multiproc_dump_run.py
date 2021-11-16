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

    ret = []
    for i in range(50):
        ret.extend([{
            "kwargs": {
                "command": f"sleep 1",
                "description": f"sleep-{i}",
                "outputs": f"output-{i}",
                "ci-stage": "build",
                "pipeline": "foo",
            }
        }, {
            "kwargs": {
                "command":
                    f"{litani_path} dump-run "
                    """| jq -e '.. | .jobs? | .[]? """
                    f"""| .complete or .wrapper_arguments.job_id != "sleep-{i}"'""",
                "description": f"test-{i}",
                "inputs": f"output-{i}",
                "ci-stage": "build",
                "pipeline": "foo",
            }
        }])
    return ret


def get_run_build_args():
    return {}


def check_run(run):
    pipe = run["pipelines"][0]
    jobs = pipe["ci_stages"][0]["jobs"]
    for job in jobs:
        if not job["outcome"] == "success":
            return False
    return True
