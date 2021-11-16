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

import lib.run_printer



class TestRunConsistency(unittest.TestCase):
    def setUp(self):
        self.run = {
            "pipelines": [{
                "ci_stages": [{
                    "jobs": []
                }]
            }]
        }


    def add_jobs(self, *jobs):
        self.run["pipelines"][0]["ci_stages"][0]["jobs"].extend(jobs)


    def test_no_id(self):
        self.add_jobs({
            "complete": True,
            "wrapper_arguments": {
                "job_id": "job 1",
                "outputs": None,
                "inputs": None,
        }}, {
            "complete": True,
            "wrapper_arguments": {
                "job_id": "job 2",
                "outputs": None,
                "inputs": None,
        }})

        with self.assertRaises(lib.run_printer.InconsistentRunError):
            lib.run_printer.run_consistent_to_job(self.run, "job 3")


    def test_found_id(self):
        self.add_jobs({
            "complete": False,
            "wrapper_arguments": {
                "job_id": "job 1",
                "outputs": None,
                "inputs": None,
        }})

        self.assertTrue(
            lib.run_printer.run_consistent_to_job(self.run, "job 1"))


    def test_reverse_dep_incomplete(self):
        self.add_jobs({
            "complete": False,
            "wrapper_arguments": {
                "job_id": "job 1",
                "outputs": ["foo"],
                "inputs": None,
        }}, {
            "complete": False,
            "wrapper_arguments": {
                "job_id": "job 2",
                "outputs": None,
                "inputs": ["foo"],
        }})

        with self.assertRaises(lib.run_printer.InconsistentRunError):
            lib.run_printer.run_consistent_to_job(self.run, "job 2")


    def test_reverse_dep_complete(self):
        self.add_jobs({
            "complete": True,
            "wrapper_arguments": {
                "job_id": "job 1",
                "outputs": ["foo"],
                "inputs": None,
        }}, {
            "complete": False,
            "wrapper_arguments": {
                "job_id": "job 2",
                "outputs": None,
                "inputs": ["foo"],
        }})

        self.assertTrue(
            lib.run_printer.run_consistent_to_job(self.run, "job 2"))


    def test_unrelated_job_ok(self):
        self.add_jobs({
            "complete": True,
            "wrapper_arguments": {
                "job_id": "job 1",
                "outputs": ["foo"],
                "inputs": None,
        }}, {
            "complete": False,
            "wrapper_arguments": {
                "job_id": "job 2",
                "outputs": None,
                "inputs": ["foo"],
        }}, {
            "complete": False,
            "wrapper_arguments": {
                "job_id": "job 3",
                "outputs": ["bar"],
                "inputs": None,
        }})

        self.assertTrue(
            lib.run_printer.run_consistent_to_job(self.run, "job 2"))


    def test_both_deps_complete(self):
        self.add_jobs({
            "complete": True,
            "wrapper_arguments": {
                "job_id": "job 1",
                "outputs": ["foo"],
                "inputs": None,
        }}, {
            "complete": False,
            "wrapper_arguments": {
                "job_id": "job 2",
                "outputs": None,
                "inputs": ["foo", "bar"],
        }}, {
            "complete": True,
            "wrapper_arguments": {
                "job_id": "job 3",
                "outputs": ["bar"],
                "inputs": None,
        }})

        self.assertTrue(
            lib.run_printer.run_consistent_to_job(self.run, "job 2"))


    def test_one_of_two_incomplete(self):
        self.add_jobs({
            "complete": True,
            "wrapper_arguments": {
                "job_id": "job 1",
                "outputs": ["foo"],
                "inputs": None,
        }}, {
            "complete": False,
            "wrapper_arguments": {
                "job_id": "job 2",
                "outputs": None,
                "inputs": ["foo", "bar"],
        }}, {
            "complete": False,
            "wrapper_arguments": {
                "job_id": "job 3",
                "outputs": ["bar"],
                "inputs": None,
        }})

        with self.assertRaises(lib.run_printer.InconsistentRunError):
            lib.run_printer.run_consistent_to_job(self.run, "job 2")
