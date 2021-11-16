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
import datetime
import enum
import functools
import json
import logging
import os
import pathlib
import shutil
import subprocess
import sys
import uuid

import jinja2

from lib import litani
import lib.graph



class Gnuplot:
    def __init__(self):
        self._should_render = shutil.which("gnuplot") is not None


    def should_render(self):
        return self._should_render


    def render(self, gnu_file, out_file=None):
        if not self.should_render():
            raise UserWarning(
                "Should not call Gnuplot.render() if should_render() is False")

        cmd = ["gnuplot"]
        with subprocess.Popen(
                cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                stderr=subprocess.DEVNULL, text=True) as proc:
            out, _ = proc.communicate(input=gnu_file)
        if proc.returncode:
            logging.error("Failed to render gnuplot file:")
            logging.error(gnu_file)
            sys.exit(1)
        lines = [
            l for l in out.splitlines()
            if "<?xml version" not in l
        ]
        if out_file:
            with litani.atomic_write(out_file) as handle:
                print("\n".join(lines), file=handle)
        return lines


# ______________________________________________________________________________
# Job renderers
# ``````````````````````````````````````````````````````````````````````````````
# These classes takes a single job, stage, or pipeline, and renders some
# auxiliary page that will be linked to from the pipeline page. The classes then
# add to the job dict the URL where the page was rendered.
# ______________________________________________________________________________


class PipelineDepgraphRenderer:
    def __init__(self, should_render=True):
        is_graphviz_installed = shutil.which("dot") is not None
        if not is_graphviz_installed:
            logging.info(
                "Graphviz is not installed; pipeline dependency "
                "graph cannot be rendered")
        self.should_render = should_render and is_graphviz_installed


    def render_to_file(self, out_file, pipe):
        dot_graph = lib.graph.SinglePipelineGraph.render(pipe)
        out_file.parent.mkdir(exist_ok=True, parents=True)
        with open(out_file, "w") as handle:
            with subprocess.Popen(
                    ["dot", "-Tsvg"], text=True, stdin=subprocess.PIPE,
                    stdout=handle) as proc:
                proc.communicate(input=dot_graph)
            if proc.returncode:
                logging.error("Failed to run dot. Graph: ")
                logging.error(dot_graph)
                sys.exit(1)
        return True


    def render(self, render_root, pipe_url, pipe):
        if not self.should_render:
            return
        out_rel = pipe_url / "dependencies.svg"
        out_file = render_root / out_rel
        success = self.render_to_file(out_file=out_file, pipe=pipe)
        if success:
            pipe["dependencies_url"] = "dependencies.svg"



@dataclasses.dataclass
class MemoryTraceRenderer:
    jinja_env: jinja2.Environment
    gnuplot: Gnuplot


    @staticmethod
    def should_render(job):
        return job.get("memory_trace") \
            and job["memory_trace"].get("trace") \
            and len(job["memory_trace"]["trace"]) > 2


    def render_preview(self, job):
        job["memory_trace_preview"] = []
        if not self.gnuplot.should_render():
            return

        templ = self.jinja_env.get_template("memory-trace.jinja.gnu")
        gnu_file = templ.render(job=job)
        lines = self.gnuplot.render(gnu_file)
        job["memory_trace_preview"] = lines


    @staticmethod
    def render(_, jinja_env, job, gnuplot):
        mtr = MemoryTraceRenderer(jinja_env=jinja_env, gnuplot=gnuplot)
        mtr.render_preview(job)



@dataclasses.dataclass
class JobOutcomeTableRenderer:
    out_dir: pathlib.Path
    jinja_env: jinja2.Environment
    html_url: str = None
    json_url: str = None


    def get_json_url(self):
        return self.json_url


    def get_html_url(self):
        return self.html_url


    def render_table(self, table):
        templ = self.jinja_env.get_template("outcome_table.jinja.html")
        page = templ.render(table=table)

        self.out_dir.mkdir(exist_ok=True, parents=True)
        self.html_url = self.out_dir / "outcome.html"
        with open(self.html_url, "w") as handle:
            print(page, file=handle)

        self.json_url = self.out_dir / "outcome.json"
        with open(self.json_url, "w") as handle:
            print(json.dumps(table, indent=2), file=handle)


    @staticmethod
    def should_render(job):
        return job["complete"] and job["loaded_outcome_dict"]


    @staticmethod
    def render(out_dir, jinja_env, job):
        otr = JobOutcomeTableRenderer(out_dir=out_dir, jinja_env=jinja_env)
        otr.render_table(table=job["loaded_outcome_dict"])
        job["outcome_table_html_url"] = str(otr.get_html_url())
        job["outcome_table_json_url"] = str(otr.get_json_url())



@dataclasses.dataclass
class ParallelismGraphRenderer:
    """Renders parallelism of run over time"""
    run: dict
    env: jinja2.Environment
    gnuplot: Gnuplot


    # The parallelism trace includes timestamps with microsecond precision, but
    # gnuplot can't deal with fractional seconds. So just for the purposes of
    # the graph, return the maximum parallelism encountered at each second.  We
    # still leave the microsecond stamps in the JSON file for those who need it.
    @staticmethod
    def process_trace(trace):

        def overwrite(field, op, d1, d2):
            d1[field] = op(d1[field], d2[field])

        tmp = {}
        for item in trace:
            ms_precision = datetime.datetime.strptime(
                item["time"], lib.litani.TIME_FORMAT_MS)
            s_precision = ms_precision.strftime(lib.litani.TIME_FORMAT_R)
            try:
                tmp[s_precision]
            except KeyError:
                tmp[s_precision] = dict(item)
                tmp[s_precision].pop("time")
            else:
                overwrite("finished", min, tmp[s_precision], item)
                overwrite("running", max, tmp[s_precision], item)
                overwrite("total", max, tmp[s_precision], item)
        return sorted(
            [{"time": time, **item} for time, item in tmp.items()],
            key=lambda x: x["time"])


    def render(self, template_name):
        if not all((
                self.gnuplot.should_render(),
                self.run["parallelism"].get("trace")
        )):
            return []

        gnu_templ = self.env.get_template(template_name)
        gnu_file = gnu_templ.render(
            n_proc=self.run["parallelism"].get("n_proc"),
            max_parallelism=self.run["parallelism"].get("max_parallelism"),
            trace=self.process_trace(self.run["parallelism"]["trace"]))

        return [self.gnuplot.render(gnu_file)]



@dataclasses.dataclass
class StatsGroupRenderer:
    """Renders graphs for jobs that are part of a 'stats group'."""
    run: dict
    env: jinja2.Environment
    gnuplot: Gnuplot


    def get_stats_groups(self, job_filter):
        ret = {}
        for job in jobs_of(self.run):
            if "tags" not in job["wrapper_arguments"]:
                continue
            if not job["wrapper_arguments"]["tags"]:
                continue
            stats_group = None
            for tag in job["wrapper_arguments"]["tags"]:
                kv = tag.split(":")
                if kv[0] != "stats-group":
                    continue
                if len(kv) != 2:
                    logging.warning(
                        "No value for stats-group in job '%s'",
                        job["wrapper_arguments"]["description"])
                    continue
                stats_group = kv[1]
                break

            if not (stats_group and job_filter(job)):
                continue

            try:
                ret[stats_group].append(job)
            except KeyError:
                ret[stats_group] = [job]

        return sorted((k, v) for k, v in ret.items())


    def render(self, job_filter, template_name):
        if not self.gnuplot.should_render():
            return []
        stats_groups = self.get_stats_groups(job_filter)
        svgs = []
        gnu_templ = self.env.get_template(template_name)
        for group_name, jobs in stats_groups:
            if len(jobs) < 2:
                continue
            gnu_file = gnu_templ.render(
                group_name=group_name, jobs=jobs)
            svg_lines = self.gnuplot.render(gnu_file)
            svgs.append(svg_lines)
        return svgs



def get_run(cache_dir):
    with open(cache_dir / litani.CACHE_FILE) as handle:
        ret = json.load(handle)
    ret["pipelines"] = {}

    for job in ret["jobs"]:
        status_file = litani.get_status_dir() / ("%s.json" % job["job_id"])
        try:
            with open(str(status_file)) as handle:
                status = json.load(handle)
        except FileNotFoundError:
            status = {
                "complete": False,
                "wrapper_arguments": job,
            }

        pipeline_name = status["wrapper_arguments"]["pipeline_name"]
        ci_stage = status["wrapper_arguments"]["ci_stage"]

        try:
            ret["pipelines"][pipeline_name]["ci_stages"][ci_stage]["jobs"].append(status)
        except KeyError:
            try:
                ret["pipelines"][pipeline_name]["ci_stages"][ci_stage]["jobs"] = [status]
            except KeyError:
                try:
                    ret["pipelines"][pipeline_name]["ci_stages"][ci_stage] = {
                        "jobs": [status]
                    }
                except KeyError:
                    try:
                        ret["pipelines"][pipeline_name]["ci_stages"] = {
                            ci_stage: {
                                "name": ci_stage,
                                "jobs": [status],
                            }
                        }
                    except KeyError:
                        ret["pipelines"][pipeline_name] = {
                            "name": pipeline_name,
                            "ci_stages":  {
                                ci_stage: {
                                    "jobs": [status]
                                }
                            },
                        }
    ret.pop("jobs")
    return ret


def job_sorter(j1, j2):
    if j1 is None or j2 is None:
        raise ValueError("Jobs must be non-None")
    if not ("start_time" in j1 or "start_time" in j2):
        return 0
    if not "start_time" in j1:
        return 1
    if not "start_time" in j2:
        return -1
    if j1["start_time"] != j2["start_time"]:
        return -1 if j1["start_time"] < j2["start_time"] else 1
    if not ("end_time" in j1 or "end_time" in j2):
        return 0
    if not "end_time" in j1:
        return 1
    if not "end_time" in j2:
        return -1
    return -1 if j1["end_time"] < j2["end_time"] else 1


class StageStatus(enum.IntEnum):
    FAIL = 0
    FAIL_IGNORED = 1
    SUCCESS = 2


def add_stage_stats(stage, stage_name, pipeline_name):
    n_complete_jobs = len([j for j in stage["jobs"] if j["complete"]])
    if stage["jobs"]:
        stage["progress"] = int(n_complete_jobs * 100 / len(stage["jobs"]))
        stage["complete"] = n_complete_jobs == len(stage["jobs"])
    else:
        stage["progress"] = 0
        stage["complete"] = True
    status = StageStatus.SUCCESS
    for job in stage["jobs"]:
        try:
            if not job["complete"]:
                continue
            if job["outcome"] == "fail":
                status = StageStatus.FAIL
            elif job["outcome"] == "fail_ignored" and status == StageStatus.SUCCESS:
                status = StageStatus.FAIL_IGNORED
        except KeyError:
            logging.error(
                "Could not find key in stage: %s",
                json.dumps(stage, indent=2))
            sys.exit(1)
    stage["status"] = status.name.lower()
    stage["url"] = "artifacts/%s/%s" % (pipeline_name, stage_name)
    stage["name"] = stage_name


class PipeStatus(enum.IntEnum):
    FAIL = 0
    IN_PROGRESS = 1
    SUCCESS = 2


def add_pipe_stats(pipe):
    pipe["url"] = "pipelines/%s" % pipe["name"]
    incomplete = [s for s in pipe["ci_stages"] if not s["complete"]]
    if incomplete:
        pipe["status"] = PipeStatus.IN_PROGRESS
    else:
        pipe["status"] = PipeStatus.SUCCESS
    for stage in pipe["ci_stages"]:
        if stage["status"] in ["fail", "fail_ignored"]:
            pipe["status"] = PipeStatus.FAIL
            break


def add_run_stats(run):
    status = PipeStatus.SUCCESS
    if [p for p in run["pipelines"] if p["status"] == PipeStatus.IN_PROGRESS]:
        status = PipeStatus.IN_PROGRESS
    if [p for p in run["pipelines"] if p["status"] == PipeStatus.FAIL]:
        status = PipeStatus.FAIL
    run["status"] = status.name.lower()
    for pipe in run["pipelines"]:
        pipe["status"] = pipe["status"].name.lower()


def add_job_stats(jobs):
    for job in jobs:
        if not ("start_time" in job and "end_time" in job):
            job["duration_str"] = None
        else:
            s = datetime.datetime.strptime(
                job["start_time"], litani.TIME_FORMAT_R)
            e = datetime.datetime.strptime(
                job["end_time"], litani.TIME_FORMAT_R)
            seconds = (e - s).seconds
            job["duration_str"] = s_to_hhmmss(seconds)
            job["duration"] = seconds


def sort_run(run):
    pipelines = []
    js = functools.cmp_to_key(job_sorter)
    for pipe in run["pipelines"].values():
        stages = []
        for stage in run["stages"]:
            try:
                pipeline_stage = pipe["ci_stages"][stage]
            except KeyError:
                pipe["ci_stages"][stage] = {"jobs": []}
                pipeline_stage = pipe["ci_stages"][stage]
            jobs = sorted(pipeline_stage["jobs"], key=js)
            add_job_stats(jobs)
            pipeline_stage["jobs"] = jobs
            add_stage_stats(pipeline_stage, stage, pipe["name"])
            stages.append(pipeline_stage)

        pipe["ci_stages"] = stages
        add_pipe_stats(pipe)
        pipelines.append(pipe)
    pipelines = sorted(pipelines, key=lambda p: p["status"])
    run["pipelines"] = pipelines
    add_run_stats(run)


def get_run_data(cache_dir):
    run = get_run(cache_dir)
    sort_run(run)
    return run


def s_to_hhmmss(s):
    h, s = divmod(s, 3600)
    m, s = divmod(s, 60)
    if h:
        return "{h:02d}h {m:02d}m {s:02d}s".format(h=h, m=m, s=s)
    if m:
        return "{m:02d}m {s:02d}s".format(m=m, s=s)
    return "{s:02d}s".format(s=s)


def jobs_of(run):
    for pipe in run["pipelines"]:
        for stage in pipe["ci_stages"]:
            for job in stage["jobs"]:
                yield job


def get_dashboard_svgs(run, env, gnuplot):
    stats_renderer = StatsGroupRenderer(run, env, gnuplot)
    p_renderer = ParallelismGraphRenderer(run, env, gnuplot)

    return {
        "Runtime": stats_renderer.render(
            lambda j: "duration" in j, "runtime-box.jinja.gnu"),
        "Memory Usage": stats_renderer.render(
            lambda j: j.get("memory_trace") and \
                j["memory_trace"].get("peak") and \
                j["memory_trace"]["peak"].get("rss"),
            "memory-peak-box.jinja.gnu"),
        "Parallelism": p_renderer.render("run-parallelism.jinja.gnu"),
    }


def get_git_hash():
    try:
        cmd = ["git", "show", "--no-patch", "--pretty=format:%h"]
        litani_dir = pathlib.Path(os.path.realpath(__file__)).parent
        proc = subprocess.run(
            cmd, text=True, stdout=subprocess.PIPE, check=True, cwd=litani_dir)
        return proc.stdout.strip()
    except RuntimeError:
        return None


def get_summary(run):
    ret = {
        "in_progress": 0,
        "success": 0,
        "total": 0,
        "fail": 0,
    }
    for pipe in run["pipelines"]:
        ret["total"] += 1
        ret[pipe["status"]] += 1
    return ret


def render(run, report_dir, pipeline_depgraph_renderer):
    temporary_report_dir = litani.get_report_data_dir() / str(uuid.uuid4())
    temporary_report_dir.mkdir(parents=True)
    old_report_dir_path = litani.get_report_dir().resolve()
    old_report_dir = litani.ExpireableDirectory(old_report_dir_path)

    artifact_dir = temporary_report_dir / "artifacts"
    shutil.copytree(litani.get_artifacts_dir(), artifact_dir)

    template_dir = pathlib.Path(__file__).parent.parent / "templates"
    env = jinja2.Environment(
        loader=jinja2.FileSystemLoader(str(template_dir)),
        autoescape=jinja2.select_autoescape(
            enabled_extensions=('html'),
            default_for_string=True))

    render_artifact_indexes(artifact_dir, env)

    gnuplot = Gnuplot()
    svgs = get_dashboard_svgs(run, env, gnuplot)

    litani_report_archive_path = os.getenv("LITANI_REPORT_ARCHIVE_PATH")

    dash_templ = env.get_template("dashboard.jinja.html")
    page = dash_templ.render(
        run=run, svgs=svgs, litani_hash=get_git_hash(),
        litani_version=litani.VERSION,
        litani_report_archive_path=litani_report_archive_path,
        summary=get_summary(run))
    with litani.atomic_write(temporary_report_dir / "index.html") as handle:
        print(page, file=handle)

    with litani.atomic_write(temporary_report_dir / litani.RUN_FILE) as handle:
        print(json.dumps(run, indent=2), file=handle)

    pipe_templ = env.get_template("pipeline.jinja.html")
    for pipe in run["pipelines"]:
        pipeline_depgraph_renderer.render(
            render_root=temporary_report_dir,
            pipe_url=pathlib.Path(pipe["url"]), pipe=pipe)
        for stage in pipe["ci_stages"]:
            for job in stage["jobs"]:
                if JobOutcomeTableRenderer.should_render(job):
                    JobOutcomeTableRenderer.render(
                        temporary_report_dir / pipe["url"], env, job)
                if MemoryTraceRenderer.should_render(job):
                    MemoryTraceRenderer.render(
                        temporary_report_dir / pipe["url"], env, job, gnuplot)

        pipe_page = pipe_templ.render(run=run, pipe=pipe)
        with litani.atomic_write(
                temporary_report_dir / pipe["url"] / "index.html") as handle:
            print(pipe_page, file=handle)


    temp_symlink_dir = report_dir.with_name(report_dir.name + str(uuid.uuid4()))
    os.symlink(temporary_report_dir, temp_symlink_dir)
    os.rename(temp_symlink_dir, report_dir)

    # Release lock so that other processes can read from this directory
    new_report_dir = litani.LockableDirectory(report_dir.resolve())
    new_report_dir.release()

    if old_report_dir_path.exists():
        old_report_dir.expire()
    litani.unlink_expired()


def render_artifact_indexes(artifact_dir, env):
    def dirs_needing_indexes():
        for root, dirs, fyles in os.walk(artifact_dir):
            if "index.html" not in fyles:
                yield pathlib.Path(root), dirs, fyles

    index_templ = env.get_template("file-list.jinja.html")
    for dyr, dirs, files in dirs_needing_indexes():
        page = index_templ.render(
            file_list=sorted(files), dir_list=sorted(dirs), root=str(dyr))
        with litani.atomic_write(dyr / "index.html") as handle:
            print(page, file=handle)
