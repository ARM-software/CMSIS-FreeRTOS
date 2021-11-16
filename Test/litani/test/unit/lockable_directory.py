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

import lib.litani



class TestLockableDirectory(unittest.TestCase):
    def setUp(self):
        self.temp_dir = tempfile.TemporaryDirectory(prefix="litani-test")
        self.temp_dir_p = pathlib.Path(self.temp_dir.name)


    def tearDown(self):
        self.temp_dir.cleanup()


    def test_cannot_initially_acquire(self):
        lock_dir = lib.litani.LockableDirectory(self.temp_dir_p)
        self.assertFalse(lock_dir.acquire())


    def test_multiproc_cannot_initially_acquire(self):
        lock_dir = lib.litani.LockableDirectory(self.temp_dir_p)
        self.assertFalse(lock_dir.acquire())
        lock_dir_2 = lib.litani.LockableDirectory(self.temp_dir_p)
        self.assertFalse(lock_dir.acquire())


    def test_can_acquire_after_release(self):
        lock_dir = lib.litani.LockableDirectory(self.temp_dir_p)
        lock_dir.release()
        self.assertTrue(lock_dir.acquire())
        lock_dir.release()
        self.assertTrue(lock_dir.acquire())


    def test_multiproc_can_acquire_after_release(self):
        lock_dir = lib.litani.LockableDirectory(self.temp_dir_p)
        lock_dir.release()

        lock_dir_2 = lib.litani.LockableDirectory(self.temp_dir_p)
        self.assertTrue(lock_dir_2.acquire())


    def test_no_double_acquire(self):
        lock_dir = lib.litani.LockableDirectory(self.temp_dir_p)
        lock_dir.release()
        self.assertTrue(lock_dir.acquire())
        self.assertFalse(lock_dir.acquire())


    def test_multiproc_no_double_acquire(self):
        lock_dir = lib.litani.LockableDirectory(self.temp_dir_p)
        lock_dir.release()
        lock_dir_2 = lib.litani.LockableDirectory(self.temp_dir_p)

        self.assertTrue(lock_dir.acquire())
        self.assertFalse(lock_dir_2.acquire())


    def test_repeat_no_double_acquire(self):
        lock_dir = lib.litani.LockableDirectory(self.temp_dir_p)
        lock_dir.release()
        self.assertTrue(lock_dir.acquire())
        self.assertFalse(lock_dir.acquire())

        lock_dir.release()

        self.assertTrue(lock_dir.acquire())
        self.assertFalse(lock_dir.acquire())


    def test_try_acquire(self):
        lock_dir = lib.litani.LockableDirectory(self.temp_dir_p)
        lock_dir.release()
        with lock_dir.try_acquire():
            pass # No exception


    def test_no_double_try_acquire(self):
        lock_dir = lib.litani.LockableDirectory(self.temp_dir_p)
        lock_dir.release()
        with lock_dir.try_acquire():
            with self.assertRaises(lib.litani.AcquisitionFailed):
                with lock_dir.try_acquire():
                    pass


    def test_multiproc_no_double_try_acquire(self):
        lock_dir = lib.litani.LockableDirectory(self.temp_dir_p)
        lock_dir.release()
        lock_dir_2 = lib.litani.LockableDirectory(self.temp_dir_p)

        with lock_dir.try_acquire():
            with self.assertRaises(lib.litani.AcquisitionFailed):
                with lock_dir_2.try_acquire():
                    pass


    def test_no_try_acquire_acquire(self):
        lock_dir = lib.litani.LockableDirectory(self.temp_dir_p)
        lock_dir.release()
        with lock_dir.try_acquire():
            self.assertFalse(lock_dir.acquire())


    def test_multiproc_no_try_acquire_acquire(self):
        lock_dir = lib.litani.LockableDirectory(self.temp_dir_p)
        lock_dir.release()
        lock_dir_2 = lib.litani.LockableDirectory(self.temp_dir_p)

        with lock_dir.try_acquire():
            self.assertFalse(lock_dir_2.acquire())


    def test_no_acquire_try_acquire(self):
        lock_dir = lib.litani.LockableDirectory(self.temp_dir_p)
        with self.assertRaises(lib.litani.AcquisitionFailed):
            with lock_dir.try_acquire():
                pass


    def test_multiproc_no_acquire_try_acquire(self):
        lock_dir = lib.litani.LockableDirectory(self.temp_dir_p)
        lock_dir_2 = lib.litani.LockableDirectory(self.temp_dir_p)
        with self.assertRaises(lib.litani.AcquisitionFailed):
            with lock_dir_2.try_acquire():
                pass


    def test_context_manager_releases(self):
        lock_dir = lib.litani.LockableDirectory(self.temp_dir_p)
        lock_dir.release()
        with lock_dir.try_acquire():
            pass
        self.assertTrue(lock_dir.acquire())
