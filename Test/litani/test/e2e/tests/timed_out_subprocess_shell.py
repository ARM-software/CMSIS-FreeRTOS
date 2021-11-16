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


def get_init_args():
    return {
        "kwargs": {
            "project": "foo",
        }
    }


def get_jobs():
    return [{
        "kwargs": {
            "command": """\
                python3 -c \"import subprocess; \
                subprocess.run('sleep 10', shell=True)\" \
            """,
            "ci-stage": "build",
            "pipeline": "foo",
            "timeout": "3",
        }
    }]


def get_run_build_args():
    return {}


def check_run(run):
    job = run["pipelines"][0]["ci_stages"][0]["jobs"][0]
    return all((job["timeout_reached"], job["duration"] < 5))
