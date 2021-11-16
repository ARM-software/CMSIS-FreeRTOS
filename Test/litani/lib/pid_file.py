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

"""Write the current process ID to a file, or read a PID from that file.

This is used so that one Litani process can write its PID, and another process
can then read the PID to send POSIX signals to the first process (see also
signal(3) on macOS or signal(7) on Linux).
"""

import os

import lib.litani


_NAME = "run-pid"


def read():
    cache_dir = lib.litani.get_cache_dir()
    with open(cache_dir / _NAME) as handle:
        return int(handle.read().strip())


def write():
    pid = os.getpid()
    cache_dir = lib.litani.get_cache_dir()
    with lib.litani.atomic_write(cache_dir / _NAME) as handle:
        print(str(pid), file=handle)
