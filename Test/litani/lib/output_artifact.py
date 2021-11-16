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


import dataclasses
import logging
import os
import pathlib
import shutil



class MissingOutput(Exception):
    pass



@dataclasses.dataclass
class Copier:
    """Copy output artifacts to a directory, raising MissingOutput if they don't exist"""

    artifacts_dir: pathlib.Path
    job_args: dict


    def copy_output_artifact(self, fyle):
        try:
            if os.path.isfile(fyle):
                shutil.copy(fyle, self.artifacts_dir)
                return
            if os.path.isdir(fyle):
                shutil.copytree(fyle, self.artifacts_dir, dirs_exist_ok=True)
                return
            raise FileNotFoundError
        except FileNotFoundError as e:
            if "phony_outputs" not in self.job_args:
                raise MissingOutput() from e

            if self.job_args["phony_outputs"] is None:
                raise MissingOutput() from e

            if not self.job_args["phony_outputs"]:
                # User supplied an empty list of phony outputs, so all outputs
                # are considered phony
                return

            if fyle in self.job_args["phony_outputs"]:
                return

            raise MissingOutput() from e
