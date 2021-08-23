AWS Build Accumulator
=

AWS Build Accumulator collects build jobs from multiple sources before executing them
concurrently. It provides platform-independent job control (timeouts,
return code control) and an output format that is easy to render into
reports (for example, using the built-in renderer).  AWS Build Accumulator
shines where your project uses multiple different build systems or requires
a unified interface describing heterogeneous build jobs.

### Documentation

Hosted [here](https://awslabs.github.io/aws-build-accumulator/).


### Requirements

* Python 3
* [Ninja](https://ninja-build.org/)
  * `apt-get install ninja-build`, `brew install ninja`
* [Jinja](https://jinja.palletsprojects.com/en/2.11.x/)
  * `pip3 install jinja2`
* [Graphviz DOT](https://graphviz.org/)
  * `apt-get install graphviz`, `brew install graphviz`

Recommended:

* [Gnuplot](http://www.gnuplot.info/) to generate graphs on the dashboard
  * `apt-get install gnuplot`, `brew install gnuplot`

Optional:

* [Voluptuous](https://pypi.org/project/voluptuous/) to perform
  sanity-checking on internal data structures
  * `pip3 install voluptuous`
* [scdoc](https://git.sr.ht/~sircmpwn/scdoc) to build the documentation
  * `brew install scdoc`
