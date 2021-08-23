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
            "command": "sleep 2",
            "description": "first",
            "ci-stage": "build",
            "pipeline": "foo",
        }
    }, {
        "kwargs": {
            "command": "sleep 2",
            "description": "second",
            "ci-stage": "build",
            "pipeline": "foo",
        },
    }, {
        "kwargs": {
            "command": "sleep 2",
            "description": "third",
            "ci-stage": "build",
            "pipeline": "foo",
        },
    }]


def get_run_build_args():
    return {}


def check_run(run):
    jobs = run["pipelines"][0]["ci_stages"][0]["jobs"]

    first = [j for j in jobs if j["wrapper_arguments"]["description"] == "first"][0]
    second = [j for j in jobs if j["wrapper_arguments"]["description"] == "second"][0]
    third = [j for j in jobs if j["wrapper_arguments"]["description"] == "third"][0]

    return all((
        first["end_time"] > second["start_time"],
        second["end_time"] > third["start_time"],
        third["end_time"] > first["start_time"],
    ))
