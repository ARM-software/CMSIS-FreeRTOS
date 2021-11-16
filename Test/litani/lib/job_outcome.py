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
import json
import logging
import os

import lib.validation

################################################################################
# Decider classes
# ``````````````````````````````````````````````````````````````````````````````
# Each of these classes has get_job_fields method.  The class evaluates what
# the result of a single Litani job is and returns that result in the dict.
################################################################################



@dataclasses.dataclass
class OutcomeTableDecider:
    """Decide what the result of a job is based on an outcome table.

    An 'outcome table' is a mapping from 'outcomes'---like return codes,
    timeouts, or a wildcard---to whether or not the job is successful. This
    class takes a user-specified or default outcome table, and decides what the
    result of a single Litani job was by iterating through the table.
    """

    table: dict
    return_code: int
    timeout_reached: bool
    loaded_from_file: bool


    def _get_wildcard_outcome(self):
        for outcome in self.table["outcomes"]:
            if outcome["type"] == "wildcard":
                return outcome["action"]
        raise UserWarning(
            "Outcome table contains no wildcard rule: %s" % json.dumps(
                self.table, indent=2))


    def _get_timeout_outcome(self):
        for outcome in self.table["outcomes"]:
            if outcome["type"] == "timeout":
                return outcome["action"]
        return None


    def _get_return_code_outcome(self, return_code):
        for outcome in self.table["outcomes"]:
            if outcome["type"] == "return-code" and \
                    outcome["value"] == return_code:
                return outcome["action"]
        return None


    def get_job_fields(self):
        return {
            "outcome": self.get_outcome(),
            "loaded_outcome_dict":
                self.table if self.loaded_from_file else None
        }


    def get_outcome(self):
        timeout_outcome = self._get_timeout_outcome()
        if self.timeout_reached:
            if timeout_outcome:
                return timeout_outcome
            return self._get_wildcard_outcome()

        rc_outcome = self._get_return_code_outcome(self.return_code)
        if rc_outcome:
            return rc_outcome

        return self._get_wildcard_outcome()



################################################################################
# Utilities
################################################################################


def _get_default_outcome_dict(args):
    """Litani's default behavior if the user does not specify an outcome table.

    This is not a constant dict as it also depends on whether the user passed in
    command-line flags that affect how the result is decided, like
    --ignore-returns etc.
    """

    outcomes = []
    if args.timeout_ok:
        outcomes.append({
            "type": "timeout",
            "action": "success",
        })
    elif args.timeout_ignore:
        outcomes.append({
            "type": "timeout",
            "action": "fail_ignored",
        })

    if args.ok_returns:
        for rc in args.ok_returns:
            outcomes.append({
                "type": "return-code",
                "value": int(rc),
                "action": "success",
            })

    if args.ignore_returns:
        for rc in args.ignore_returns:
            outcomes.append({
                "type": "return-code",
                "value": int(rc),
                "action": "fail_ignored",
            })

    outcomes.extend([{
        "type": "return-code",
        "value": 0,
        "action": "success",
    }, {
        "type": "wildcard",
        "action": "fail",
    }])

    return {"outcomes": outcomes}


def _get_outcome_table_job_decider(args, return_code, timeout_reached):
    if args.outcome_table:
        _, ext = os.path.splitext(args.outcome_table)
        with open(args.outcome_table) as handle:
            if ext == ".json":
                outcome_table = json.load(handle)
            elif ext == ".yaml":
                import yaml
                outcome_table = yaml.safe_load(handle)
            else:
                raise UserWarning("Unsupported outcome table format (%s)" % ext)
        loaded_from_file = True
    else:
        loaded_from_file = False
        outcome_table = _get_default_outcome_dict(args)

    logging.debug("Using outcome table: %s", json.dumps(outcome_table, indent=2))
    lib.validation.validate_outcome_table(outcome_table)

    return OutcomeTableDecider(
        outcome_table, return_code, timeout_reached,
        loaded_from_file=loaded_from_file)


################################################################################
# Entry point
################################################################################


def fill_in_result(runner, job_data, args):
    """Add fields pertaining to job result to job_data dict

    The 'result' of a job can be evaluated in several ways. The most simple
    mechanism, where a return code of 0 means success and anything else is a
    failure, is encoded by the "default outcome table". Users can also supply
    their own outcome table as a JSON file, and other mechanisms could be
    available in the future.

    Depending on how we are to evaluate the result, we construct an instance of
    one of the Decider classes in this module, and use the Decider to evaluate
    the result of the job. The result is a dict, whose keys and values we add to
    the job's dict.
    """

    job_data["complete"] = True
    job_data["timeout_reached"] = runner.reached_timeout()
    job_data["command_return_code"] = runner.get_return_code()
    job_data["memory_trace"] = runner.get_memory_trace()

    # These get set by the deciders
    job_data["loaded_outcome_dict"] = None

    decider = _get_outcome_table_job_decider(
        args, runner.get_return_code(), runner.reached_timeout())

    fields = decider.get_job_fields()
    for k, v in fields.items():
        job_data[k] = v
    job_data["wrapper_return_code"] = 1 if job_data["outcome"] == "fail" else 0
