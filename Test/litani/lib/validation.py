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


import datetime
import logging

import lib.litani


_NO_VALIDATE = False


def _ms_time_str(string):
    try:
        datetime.datetime.strptime(string, lib.litani.TIME_FORMAT_MS)
    except RuntimeError as e:
        raise ValueError(
            "Date '%s' was not in the right format (expected '%s')" %
            (string, lib.litani.TIME_FORMAT_MS)) from e


def _time_str(string):
    try:
        datetime.datetime.strptime(string, lib.litani.TIME_FORMAT_R)
    except RuntimeError as e:
        raise ValueError(
            "Date '%s' was not in the right format (expected '%s')" %
            (string, lib.litani.TIME_FORMAT_R)) from e


# doc-gen
# {
#   "page": "litani-run.json",
#   "order": 1,
#   "title": "Schema for 'wrapper_arguments' key"
# }
def _single_job_schema():
    import voluptuous

    # The *wrapper_arguments* key to run.json maps to the following dict. None
    # of the values in this dict change at any point during the run; they are
    # mostly the same as the flags passed to *litani-add-job(1)* for this job.
    return {
        "job_id": str,
        # A globally-unique ID for this job.

        "command": str,
        # The command that litani will execute in a subshell.

        "ci_stage": str,
        # The name of the 'stage' that this job will execute in, used for
        # organizing the HTML dashboard.

        "verbose": bool,

        "timeout_ok": bool,
        # If true, then if this job times out, the outcome will be set to
        # 'success'.

        "pipeline_name": str,
        # The name of the 'pipeline' that this job will execute in, used for
        # organizing the HTML dashboard.

        "very_verbose": bool,

        "timeout_ignore": bool,
        # If true, then if this job times out, the outcome will be set to
        # 'fail_ignored'.

        "profile_memory": bool,
        # If true, then litani will regularly sample the memory usage of this
        # job's command while it runs. Samples are stored in the job's
        # *memory_trace*.

        "profile_memory_interval": int,
        # How frequently (in seconds) litani will profile the command's memory
        # use, if *profile_memory* is true.

        "cwd": voluptuous.Any(str, None),
        # The directory that litani will run the command in.

        "interleave_stdout_stderr": bool,
        # Whether the command's stderr will be sent to the stdout stream. If
        # true, the job's *stderr* key will be None and the *stdout* key will
        # contain lines from both the command's stdout and stderr.

        "pool": voluptuous.Any(str, None),
        # The pool that this job will execute in; if not null, then it must be a
        # key in the *pools* dict of the overall run.

        "tags": voluptuous.Any([str], None),
        # A list of user-specified tags. Litani mostly doesn't interpret these,
        # although the HTML dashboard generator does use some of them. Tags are
        # intended to help users find particular jobs for data analysis and can
        # contain arbitrary data.

        "timeout": voluptuous.Any(int, None),
        # The number of seconds that Litani will allow the job to run for before
        # sending SIGTERM followed by SIGKILL (see *signal(3)*).

        "inputs": voluptuous.Any([str], None),
        # The list of files that should be made up-to-date before the job will
        # run

        "outputs": voluptuous.Any([str], None),
        # The list of files that this job will make up-to-date after it
        # completes

        "description": voluptuous.Any(str, None),
        # A human-readable description of this job

        "status_file": voluptuous.Any(str, None),

        "stderr_file": voluptuous.Any(str, None),
        # A file to redirect stderr to, as well as buffering it internally

        "stdout_file": voluptuous.Any(str, None),
        # A file to redirect stdout to, as well as buffering it internally

        "ok_returns": voluptuous.Any([str], None),
        # A list of return codes. If the command exits with any of these return
        # codes (or 0), then the outcome will be set to 'success'.

        "outcome_table": voluptuous.Any(str, None),
        # A file to load an outcome table from.

        "phony_outputs": voluptuous.Any([str], None),
        # A list of outputs that Litani will not warn about if they were not
        # created by the job.

        "ignore_returns": voluptuous.Any([str], None),
        # A list of return codes. If the command exits with any of these return
        # codes (or 0), then the outcome will be set to 'fail_ignored'.

        "subcommand": voluptuous.Any("exec", "add-job"),
    }


def validate_single_job(job):
    global _NO_VALIDATE

    if _NO_VALIDATE:
        return
    try:
        import voluptuous
        import voluptuous.humanize
        schema = voluptuous.Schema(_single_job_schema(), required=True)
        voluptuous.humanize.validate_with_humanized_errors(job, schema)
    except (ImportError, ModuleNotFoundError):
        logging.debug("voluptuous not installed; not validating schema")
        _NO_VALIDATE = True


def validate_run(run):
    global _NO_VALIDATE

    if _NO_VALIDATE:
        return
    try:
        import voluptuous
        import voluptuous.humanize
        outcome = _outcome()
        schema = voluptuous.Schema(_run_schema(), required=True)
        voluptuous.humanize.validate_with_humanized_errors(run, schema)
    except (ImportError, ModuleNotFoundError):
        logging.debug("voluptuous not installed; not validating schema")
        _NO_VALIDATE = True


# doc-gen
# {
#   "page": "litani-run.json",
#   "order": 3,
#   "title": "Schema for a job or ci_stage outcome"
# }
def _outcome():
    import voluptuous

    # Outcomes and ci_stages have an *"outcome"* (though, confusingly, the key
    # is *"status"* for ci_stages). "fail_ignored" means that the job failed but
    # the user specified that the job's dependencies should run anyway. If a
    # pipeline contains a job whose outcome is "fail_ignored", then the status
    # of the pipeline will be "fail" after all of its jobs complete.

    return voluptuous.Any("success", "fail", "fail_ignored")
# end-doc-gen

# doc-gen
# {
#   "page": "litani-run.json",
#   "order": 2,
#   "title": "Schema for a pipeline or run status"
# }
def _status():
    import voluptuous

    # pipelines and runs have a *"status"*. The status is "in_progress" when some
    # of the jobs are incomplete and either "success" or "fail" once all jobs
    # complete.

    return voluptuous.Any("success", "fail", "in_progress")
# end-doc-gen


# doc-gen
# {
#   "page": "litani-run.json",
#   "order": 0,
#   "title": "Schema for entire run.json file"
# }
def _run_schema():
    import voluptuous

    return {
        "run_id": str,
        # A globally-unique ID for the run.

        "project": str,
        # A name for the project that this run is part of. This name is used by
        # the HTML report generator and can be used to group related sets of
        # runs, but is otherwise not used by litani.

        "stages": [str],
        # The CI stages that each job can be a member of. Stage names can
        # be provided through the --stages flag of *litani-init(1)*. Default
        # stages "build", "test" and "report" are used if the flag is not used.

        "pools": {voluptuous.Optional(str): int},
        # A mapping from pool names to the depth of the pool. Jobs can be a
        # member of zero or one pool. The depth of a pool that a set of jobs
        # belong to limits the number of those jobs that litani will run in
        # parallel.

        "start_time": _time_str,
        # The time at which the run started.

        "version": str,
        # The version string of the Litani binary that ran this run.

        "version_major": int,
        # Litani's major version number.

        "version_minor": int,
        # Litani's minor version number.

        "version_patch": int,
        # Litani's patch version number.

        "release_candidate": bool,
        # false if this version of Litani is a tagged release.

        voluptuous.Optional("end_time"): _time_str,
        # The time at which the run ended. This key will only exist if *status*
        # is not equal to "in_progress".

        "status": _status(),
        # The state of this run, see the status schema below.

        "aux": dict,
        # A free-form dict that users can add custom information into. There are
        # no constraints on the format of this dict, but it is recommended that
        # users add their information to a sub-dict with a key that indicates
        # its function. For example, to add information pertaining to a CI run,
        # users might add a key called "continuous_integration_data" whose value
        # is a sub-dict containing all required fields.

        "parallelism": voluptuous.Any({
        # This dict contains information about the parallelism level of the jobs
        # that litani runs. This is to measure whether the run is using as many
        # processor cores as possible over the duration of the run.

            voluptuous.Optional("trace"): [{
            # A list of samples of the run's concurrency level.

                "time": _ms_time_str,
                # The time at which the sample was taken.

                "finished": int,
                # How many jobs have finished

                "running": int,
                # How many jobs are running

                "total": int,
                # The total number of jobs

            }],

            voluptuous.Optional("max_parallelism"): int,
            # The maximum parallelism attained over the run

            voluptuous.Optional("n_proc"): voluptuous.Any(None, int),
            # The number of processors detected on this machine

        }),

        "pipelines": [{
        # Each pipeline contains ci_stages which contain jobs.

            "url": str,

            "name": str,
            # The pipeline name. The set of pipeline names are all the names
            # passed to the --pipeline-name flag of *litani-add-job(1)*.

            "status": _status(),
            # The pipeline's state, see the status schema below.

            "ci_stages": [{
            # Each ci_stage contains a list of jobs.

                "url": str,
                "complete": bool,
                # Whether all the jobs in this stage are complete.

                "name": str,
                # The stage's name. This is any of the *stages* of
                # the project.

                "status": _outcome(),
                # The stage's state, see the outcome schema below.

                "progress": voluptuous.All(int, voluptuous.Range(min=0, max=100)),

                "jobs": [voluptuous.Any({
                # The list of all the jobs in this ci_stage in this pipeline.
                # There are three different forms the value of this key can
                # take.

                    "complete": False,
                    # If *complete* is false and no *start_time* key exists,
                    # then this job has not yet started.

                    "duration_str": None,

                    "wrapper_arguments": _single_job_schema(),
                    # The arguments passed to this job, see the
                    # single_job_schema schema below.

                }, {
                    "complete": False,
                    # If *complete* is false but the *start_time* key exists,
                    # then the job has started running but has not yet finished.

                    "start_time": _time_str,
                    # The time at which the job started running.

                    "duration_str": None,

                    "wrapper_arguments": _single_job_schema(),
                    # The arguments passed to this job, see the
                    # single_job_schema schema below.

                }, {
                    "duration": int,
                    # How long the job ran for.

                    "complete": True,
                    # If *complete* is true, then the job has terminated.

                    "outcome": _outcome(),
                    # The job's outcome, see the outcome schema below.

                    "end_time": _time_str,
                    # The time at which the job completed.

                    "start_time": _time_str,
                    # The time at which the job started running.

                    "timeout_reached": bool,
                    # Whether the job reached its timeout limit.

                    "command_return_code": int,
                    # The command's return code.

                    "wrapper_return_code": int,

                    "stderr": voluptuous.Any([str], None),
                    # A list of strings that the command printed to its stderr.

                    "stdout": voluptuous.Any([str], None),
                    # A list of strings that the command printed to its stdout.

                    "duration_str": voluptuous.Any(str, None),
                    # A human-readable duration of this job (HH:MM:SS).

                    "wrapper_arguments": _single_job_schema(),
                    # The arguments passed to this job, see the
                    # single_job_schema schema below.

                    "loaded_outcome_dict": voluptuous.Any(dict, None),
                    # If *wrapper_arguments["outcome_table"]* is not null, the
                    # value of this key will be the deserialized data loaded
                    # from the outcome table file.

                    "memory_trace": {
                    # If *profile_memory* was set to true in the wrapper
                    # arguments for this job, this dict will contain samples of
                    # the command's memory usage.

                        voluptuous.Optional("peak"): {
                        # The command's peak memory usage.

                            "rss": int,
                            # Peak resident set

                            "vsz": int,
                            # Peak virtual memory size

                            "human_readable_rss": str,
                            # Peak resident set

                            "human_readable_vsz": str,
                            # Peak virtual memory size

                        },

                        voluptuous.Optional("trace"): [{
                        # A list of samples of memory usage.

                            "rss": int,
                            # Resident set

                            "vsz": int,
                            # Virtual memory

                            "time": _time_str,
                            # The time at which the sample was taken

                        }],
                    },
                })],
            }],
        }],

        "latest_symlink": voluptuous.Any(str, None),
        # The symbolic link to the report advertised to users
    }
# end-doc-gen


def validate_outcome_table(table):
    try:
        import voluptuous
    except ImportError:
        logging.debug("Skipping outcome table validation as voluptuous is not installed")
        return

    schema = voluptuous.Schema(_outcome_table_schema())
    voluptuous.humanize.validate_with_humanized_errors(table, schema)


# doc-gen
# {
#   "page": "litani-outcome-table.json",
#   "order": 0,
#   "title": "Schema for user-provided outcome table"
# }
def _outcome_table_schema():
    import voluptuous

    return {
        voluptuous.Optional("comment"): str,
        # A description of the outcome table as a whole.

        "outcomes": [
        # The outcome of the job will be the first item in this list that
        # matches.

            voluptuous.Any({
                "type": "return-code",
                # If the return code of the job matches the value of *value*,
                # the outcome will be set to the value of *action*. The value
                # of the optional *comment* key can contain a human-readable
                # explanation for this outcome.

                "value": int,

                "action": _outcome(),

                voluptuous.Optional("comment"): str,

            }, {
                "type": "timeout",
                # If this job timed out, the outcome will be set to the value of
                # *action*. The value of the optional *comment* key can contain
                # a human-readable explanation for this outcome.

                "action": _outcome(),

                voluptuous.Optional("comment"): str,

            }, {
                "type": "wildcard",
                # The *"wildcard"* action type matches any job and sets its
                # outcome to the value of *action*. It is recommended to place a
                # *wildcard* action as the last element of the list of
                # *outcomes* to catch all jobs that were not matched by a
                # previous rule.

                "action": _outcome(),

                voluptuous.Optional("comment"): str,

        })]
    }
