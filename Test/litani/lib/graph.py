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
import os
import pathlib
import re
import textwrap

import lib.litani
import lib.litani_report



class Node:
    @staticmethod
    def escape(string):
        for match, repl in [(
            '"', '\\"'
        ), (
            ';', '\\;'
        )]:
            string = re.sub(match, repl, string)
        return string


    def html_escape(string):
        for match, repl in [(
            '&', '&amp;'
        ), (
            '"', '&quot;'
        ), (
            '<', '&lt;'
        ), (
            '>', '&gt;'
        )]:
            string = re.sub(match, repl, string)
        return string


class DependencyNode(Node):
    def __init__(self, fyle, line_width=40, **style):
        self.file = fyle
        self.id = hash(fyle)

        path = pathlib.Path(fyle)
        _, ext = os.path.splitext(path.name)

        self.style = style

        path_name = "\n".join(textwrap.wrap(path.name, width=line_width))
        self.style["label"] = f"{path_name}{ext}"


    def __hash__(self):
        return self.id


    def __eq__(self, other):
        return self.id == other.id


    def __str__(self):
        return '"{id}" [{style}];'.format(
            id=self.id, style=",".join([
                f'"{key}"="{Node.escape(value)}"'
                for key, value in self.style.items()]))



class CommandNode(Node):
    def __init__(
            self, pipeline_name, description, command, line_width=40, **style):
        self.id = hash(command)
        self.pipeline_name = '<BR/>'.join(
            textwrap.wrap(Node.html_escape(pipeline_name), width=line_width))
        if description:
            self.description = '<BR/>'.join(
                textwrap.wrap(Node.html_escape(description), width=line_width))
        else:
            self.description = ""
        self.command = '<BR/>'.join(
            textwrap.wrap(Node.html_escape(command), width=line_width))
        self.style = style

        self.style["shape"] = "plain"



    def __hash__(self):
        return self.id


    def __eq__(self, other):
        return self.id == other.id


    def __str__(self):
        if self.description:
            desc_cell = f"\n<TD><B>{self.description}</B></TD>"
        else:
            desc_cell = ""
        return '''"{id}" [label=<
            <TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0">
                <TR>
                    <TD><B>{pipeline_name}</B></TD>{desc_cell}
                </TR>
                <TR>
                    <TD COLSPAN="2">{command}</TD>
                </TR>
            </TABLE>> {style}];'''.format(
                id=self.id, command=self.command,
                desc_cell=desc_cell,
                pipeline_name=self.pipeline_name,
                style=",".join([
                    f'{key}="{Node.escape(value)}"'
                    for key, value in self.style.items()]))



class Edge:
    def __init__(self, src, dst, **style):
        self.src = src
        self.dst = dst
        self.style = style

    def __eq__(self, other):
        return all((self.src == other.src, self.dst == other.dst))


    def __hash__(self):
        return hash(self.src) + hash(self.dst)


    def __str__(self):
        return '"{src}" -> "{dst}" [{style}];'.format(
            src=self.src.id, dst=self.dst.id,
            style=",".join([
                f'"{key}"="{value}"' for key, value in self.style.items()]))



@dataclasses.dataclass
class SinglePipelineGraph:
    pipe: dict
    nodes: set = dataclasses.field(default_factory=set)
    edges: set = dataclasses.field(default_factory=set)


    def iter_jobs(self):
        for stage in self.pipe["ci_stages"]:
            for job in stage["jobs"]:
                yield job


    def build(self):
        for job in self.iter_jobs():
            args = job["wrapper_arguments"]
            cmd_node = self._make_cmd_node(
                job["complete"], job.get("outcome", None),
                args["pipeline_name"], args["description"], args["command"])
            self.nodes.add(cmd_node)

            for inputt in args.get("inputs") or []:
                in_node = lib.graph.DependencyNode(inputt)
                self.nodes.add(in_node)
                self.edges.add(lib.graph.Edge(src=in_node, dst=cmd_node))

            for output in args.get("outputs") or []:
                out_node = lib.graph.DependencyNode(output)
                self.nodes.add(out_node)
                self.edges.add(lib.graph.Edge(src=cmd_node, dst=out_node))


    @staticmethod
    def _make_cmd_node(complete, outcome, pipeline_name, description, command):
        cmd_style = {"style": "filled"}
        if complete and outcome == "success":
            cmd_style["fillcolor"] = "#90caf9"
        elif complete and outcome == "fail_ignored":
            cmd_style["fillcolor"] = "#ffecb3"
        elif complete and outcome == "fail":
            cmd_style["fillcolor"] = "#ef9a9a"
        elif complete:
            raise RuntimeError("Unknown outcome '%s'" % outcome)
        else:
            cmd_style["fillcolor"] = "#eceff1"
        return lib.graph.CommandNode(
            pipeline_name, description, command, **cmd_style)


    def as_dot(self):
        buf = ["digraph G {"]
        buf.append('bgcolor="transparent"')
        buf.extend([("  %s" % str(n)) for n in self.nodes])
        buf.extend([("  %s" % str(e)) for e in self.edges])
        buf.append("}")
        return "\n".join(buf)


    @staticmethod
    def render(pipe):
        spg = SinglePipelineGraph(pipe)
        spg.build()
        return spg.as_dot()



@dataclasses.dataclass
class PipelineChooser:
    pipelines: list


    def should_skip(self, pipeline):
        return self.pipelines and not pipeline in self.pipelines



@dataclasses.dataclass
class Graph:
    run: dict
    pipeline_chooser: PipelineChooser


    def iter_jobs(self):
        for pipe in self.run["pipelines"]:
            if self.pipeline_chooser.should_skip(pipe["name"]):
                continue
            for stage in pipe["ci_stages"]:
                for job in stage["jobs"]:
                    yield job


    def __str__(self):
        buf = ["digraph G {"]

        nodes = set()
        edges = set()

        for job in self.iter_jobs():
            args = job["wrapper_arguments"]
            cmd_node = CommandNode(
                args["pipeline_name"], args["description"], args["command"])
            nodes.add(cmd_node)
            if args["outputs"]:
                for output in args["outputs"]:
                    out_node = DependencyNode(output)
                    nodes.add(out_node)
                    edges.add(Edge(src=cmd_node, dst=out_node))
            if args["inputs"]:
                for inputt in args["inputs"]:
                    in_node = DependencyNode(inputt)
                    nodes.add(in_node)
                    edges.add(Edge(src=in_node, dst=cmd_node))

        buf.extend([("  %s" % str(n)) for n in nodes])
        buf.extend([("  %s" % str(e)) for e in edges])
        buf.append("}")
        return "\n".join(buf)



async def print_graph(args):
    lib.litani.add_jobs_to_cache()
    run = lib.litani_report.get_run_data(lib.litani.get_cache_dir())

    pc = PipelineChooser(args.pipelines)

    graph = Graph(run=run, pipeline_chooser=pc)
    print(graph)
