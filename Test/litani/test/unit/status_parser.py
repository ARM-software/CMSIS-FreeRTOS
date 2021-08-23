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


import pathlib
import tempfile
import unittest
import unittest.mock

import lib.ninja



class TestNinjaStatusParser(unittest.TestCase):
    def setUp(self):
        self.sp = lib.ninja._StatusParser()


    def test_bad_status(self):
        self.assertEqual(None, self.sp.parse_status("foo bar"))


    def test_usual_ninja_status(self):
        self.assertEqual(None, self.sp.parse_status("[24/42] foo bar"))


    def test_expected_status(self):
        self.assertEqual({
                "running": 34,
                "finished": 53,
                "total": 91,
                "message": "hello world"
            }, self.sp.parse_status("<ninja>:34/53/91 hello world"))
