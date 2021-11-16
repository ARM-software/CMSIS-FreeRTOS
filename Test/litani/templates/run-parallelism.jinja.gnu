{#-
 # Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 #
 # Licensed under the Apache License, Version 2.0 (the "License").
 # You may not use this file except in compliance with the License.
 # A copy of the License is located at
 #
 #     http://www.apache.org/licenses/LICENSE-2.0
 #
 # or in the "license" file accompanying this file. This file is
 # distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF
 # ANY KIND, either express or implied. See the License for the specific
 # language governing permissions and limitations under the License.
-#}

$data << EOD
{% for sample in trace -%}
{{ sample["time"] }} {{ sample["running"] }} {{ n_proc }} {{ n_proc + 0.5 }} {% if loop.last %} "# cores: {{ n_proc}}" {% endif %}
{% endfor %}{# sample in trace #}
EOD

set terminal svg noenhanced size 720,320

set border 3 linecolor "#263238"

set ylabel "# parallel jobs"

set ytics nomirror tc "#263238" font "Helvetica,9"
set xtics nomirror

set xdata time
set timefmt "%Y-%m-%dT%H:%M:%SZ"
set format x "%H:%M:%S"
unset key

plot '$data' using 1:2 with lines lc "#ab47bc", \
  '' using 1:3 with lines lc "#cc0000", \
  '' using 1:4:5 with labels tc "#cc0000"
