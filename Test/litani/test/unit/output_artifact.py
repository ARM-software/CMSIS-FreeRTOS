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


import shutil
import pathlib
import unittest
import unittest.mock

import lib.output_artifact



class TestOutputArtifact(unittest.TestCase):
    def setUp(self):
        shutil.copy = unittest.mock.Mock(side_effect=FileNotFoundError(""))
        self.artifacts_dir = pathlib.Path()


    def test_no_flag(self):
        job_args = {}
        copier = lib.output_artifact.Copier(self.artifacts_dir, job_args)

        with self.assertRaises(lib.output_artifact.MissingOutput):
            copier.copy_output_artifact("foo")


    def test_null_flag(self):
        job_args = {
            "phony_outputs": None,
        }
        copier = lib.output_artifact.Copier(self.artifacts_dir, job_args)

        with self.assertRaises(lib.output_artifact.MissingOutput):
            copier.copy_output_artifact("foo")


    def test_all_phony(self):
        job_args = {
            "phony_outputs": [],
        }
        copier = lib.output_artifact.Copier(self.artifacts_dir, job_args)
        copier.copy_output_artifact("foo")


    def test_different_phony(self):
        job_args = {
            "phony_outputs": ["bar"],
        }
        copier = lib.output_artifact.Copier(self.artifacts_dir, job_args)

        with self.assertRaises(lib.output_artifact.MissingOutput):
            copier.copy_output_artifact("foo")


    def test_file_phony(self):
        job_args = {
            "phony_outputs": ["foo"],
        }
        copier = lib.output_artifact.Copier(self.artifacts_dir, job_args)
        copier.copy_output_artifact("foo")
