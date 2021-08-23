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


import unittest
import unittest.mock

import lib.job_outcome


class CBMCNegativeTest(unittest.TestCase):
    def setUp(self):
        self.table = {
            "comment": "",
            "outcomes": [{
                "type": "return-code",
                "value": 0,
                "action": "fail_ignored",
            }, {
                "type": "return-code",
                "value": 10,
                "action": "success",
            }, {
                "type": "wildcard",
                "action": "fail",
            }]
        }


    def assert_outcome_equals(self, jod, outcome):
        self.assertEqual(jod.get_job_fields(), outcome)


    def test_zero(self):
        proc = unittest.mock.Mock()
        proc.returncode = 0
        jod = lib.job_outcome.OutcomeTableDecider(
            self.table, proc.returncode, False, True)

        self.assert_outcome_equals(jod, {
                "outcome": "fail_ignored",
                "loaded_outcome_dict": self.table,
            })


    def test_success(self):
        proc = unittest.mock.Mock()
        proc.returncode = 10
        jod = lib.job_outcome.OutcomeTableDecider(
            self.table, proc.returncode, False, True)

        self.assert_outcome_equals(jod, {
                "outcome": "success",
                "loaded_outcome_dict": self.table,
            })



class CBMCTest(unittest.TestCase):
    def setUp(self):
        self.table = {
            "comment": "",
            "outcomes": [{
                "type": "return-code",
                "value": 0,
                "action": "success",
            }, {
                "type": "return-code",
                "value": 10,
                "action": "fail_ignored",
            }, {
                "type": "wildcard",
                "action": "fail",
            }]
        }


    def assert_outcome_equals(self, jod, outcome):
        self.assertEqual(jod.get_job_fields(), outcome)


    def test_success(self):
        proc = unittest.mock.Mock()
        proc.returncode = 0
        jod = lib.job_outcome.OutcomeTableDecider(
            self.table, proc.returncode, False, True)

        self.assert_outcome_equals(jod, {
                "outcome": "success",
                "loaded_outcome_dict": self.table,
            })


    def test_ten(self):
        proc = unittest.mock.Mock()
        proc.returncode = 10
        jod = lib.job_outcome.OutcomeTableDecider(
            self.table, proc.returncode, False, True)

        self.assert_outcome_equals(jod, {
                "outcome": "fail_ignored",
                "loaded_outcome_dict": self.table,
            })


    def test_one(self):
        proc = unittest.mock.Mock()
        proc.returncode = 1
        jod = lib.job_outcome.OutcomeTableDecider(
            self.table, proc.returncode, False, True)

        self.assert_outcome_equals(jod, {
                "outcome": "fail",
                "loaded_outcome_dict": self.table,
            })


    def test_timeout_one(self):
        proc = unittest.mock.Mock()
        proc.returncode = 1
        jod = lib.job_outcome.OutcomeTableDecider(
            self.table, proc.returncode, True, True)

        self.assert_outcome_equals(jod, {
                "outcome": "fail",
                "loaded_outcome_dict": self.table,
            })


    def test_timeout_ten(self):
        proc = unittest.mock.Mock()
        proc.returncode = 10
        jod = lib.job_outcome.OutcomeTableDecider(
            self.table, proc.returncode, True, True)

        self.assert_outcome_equals(jod, {
                "outcome": "fail",
                "loaded_outcome_dict": self.table,
            })


    def test_timeout_zero(self):
        # This is sort of weird, in that we wouldn't expect a process to return
        # 0 if it's been killed by timeout_happened. Nevertheless, failing is the right
        # thing to do in that case.
        proc = unittest.mock.Mock()
        proc.returncode = 0
        jod = lib.job_outcome.OutcomeTableDecider(
            self.table, proc.returncode, True, True)

        self.assert_outcome_equals(jod, {
                "outcome": "fail",
                "loaded_outcome_dict": self.table,
            })



class TimeoutPriorityTest(unittest.TestCase):
    def setUp(self):
        self.table = {
            "comment": "",
            "outcomes": [{
                "type": "return-code",
                "value": 0,
                "action": "success",
            }, {
                "type": "return-code",
                "value": 10,
                "action": "fail_ignored",
            }, {
                "type": "timeout",
                "action": "fail_ignored",
            }, {
                "type": "wildcard",
                "action": "fail",
            }]
        }


    def assert_outcome_equals(self, jod, outcome):
        self.assertEqual(jod.get_job_fields(), outcome)


    def test_success(self):
        proc = unittest.mock.Mock()
        proc.returncode = 0
        jod = lib.job_outcome.OutcomeTableDecider(
            self.table, proc.returncode, False, True)

        self.assert_outcome_equals(jod, {
                "outcome": "success",
                "loaded_outcome_dict": self.table,
            })


    def test_ten(self):
        proc = unittest.mock.Mock()
        proc.returncode = 10
        jod = lib.job_outcome.OutcomeTableDecider(
            self.table, proc.returncode, False, True)

        self.assert_outcome_equals(jod, {
                "outcome": "fail_ignored",
                "loaded_outcome_dict": self.table,
            })


    def test_one(self):
        proc = unittest.mock.Mock()
        proc.returncode = 1
        jod = lib.job_outcome.OutcomeTableDecider(
            self.table, proc.returncode, False, True)

        self.assert_outcome_equals(jod, {
                "outcome": "fail",
                "loaded_outcome_dict": self.table,
            })


    def test_timeout_one(self):
        proc = unittest.mock.Mock()
        proc.returncode = 1
        jod = lib.job_outcome.OutcomeTableDecider(
            self.table, proc.returncode, True, True)

        self.assert_outcome_equals(jod, {
                "outcome": "fail_ignored",
                "loaded_outcome_dict": self.table,
            })


    def test_timeout_ten(self):
        proc = unittest.mock.Mock()
        proc.returncode = 10
        jod = lib.job_outcome.OutcomeTableDecider(
            self.table, proc.returncode, True, True)

        self.assert_outcome_equals(jod, {
                "outcome": "fail_ignored",
                "loaded_outcome_dict": self.table,
            })


    def test_timeout_zero(self):
        proc = unittest.mock.Mock()
        proc.returncode = 0
        jod = lib.job_outcome.OutcomeTableDecider(
            self.table, proc.returncode, True, True)

        self.assert_outcome_equals(jod, {
                "outcome": "fail_ignored",
                "loaded_outcome_dict": self.table,
            })
