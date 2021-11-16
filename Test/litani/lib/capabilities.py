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

import json


_CAPABILITIES = {
    "atomic_report_update": "Report directory is rendered atomically",
    "report_expire": "Old report directories will contain a .litani-expired file",
    "dir_lock_api": "Deprecated",
    "dir_lock_api_v2": "lib.litani contains the LockableDirectory API",
    "outcome_table": "The --outcome-table flag is supported",
    "output_directory_flags": "The --output-directory --output-symlink and "
        "--output-prefix flags are supported",
    "pools": "Jobs can be added to task pools to limit parallelism",
    "memory_profile": "Litani can measure the memory usage of specific jobs",
    "aux": "Run contains an aux field for custom user data",
    "parallelism_metric": "Run contains process parallelism measurements",
    "phony_outputs": "The --phony-outputs flag is supported",
    "dump_run": "The dump-run command is supported",
}


def _human_readable_dump():
    longest_capability = 0
    for capability in _CAPABILITIES:
        if len(capability) > longest_capability:
            longest_capability = len(capability)

    for capability, capability_description in _CAPABILITIES.items():
        print("% *s:    %s" %
            (longest_capability, capability, capability_description))


def _machine_readable_dump():
    print(json.dumps(list(_CAPABILITIES.keys()), indent=2))


async def dump(args):
    if args.human_readable:
        _human_readable_dump()
    else:
        _machine_readable_dump()
