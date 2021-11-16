NUM_STAGES = 4

def get_stages():
    stages = []
    for i in range(1, NUM_STAGES + 1):
        stages.append("stage_" + str(i))
    return stages

def get_init_args():
    return {
        "kwargs": {
            "project": "custom_stages",
            "stages": get_stages(),
        }
    }

def get_jobs():
    jobs = []
    for i in range(1, NUM_STAGES + 1):
        job = {
            "kwargs": {
                "command": "sleep 2",
                "ci-stage": "stage_" + str(i),
                "description": str(i),
                "pipeline": "siesta",
            }
        }
        jobs.append(job)
    return jobs

def get_run_build_args():
    return {}

def check_run(run):
    asserts = []

    stages = get_stages()
    stages_run = run["stages"]

    # Assert that the expected stages are
    # equal to the stages stored in the run
    asserts.append(stages == stages_run)

    pipe_stages = run["pipelines"][0]["ci_stages"]
    for pipe_stage in pipe_stages:
        # Assert that any stage in the pipeline is one of the expected stages
        asserts.append(pipe_stage["name"] in stages)
        for job in pipe_stage["jobs"]:
            # Assert that the stage of a job corresponds to the pipeline stage
            asserts.append(job["wrapper_arguments"]["ci_stage"] == pipe_stage["name"])
            # Assert that the description of a job corresponds to the expected one
            asserts.append("stage_" + job["wrapper_arguments"]["description"] == pipe_stage["name"])

    # Check that the number of asserts is the expected one
    num_asserts = 3 * NUM_STAGES + 1

    return num_asserts == len(asserts) and all(asserts)
