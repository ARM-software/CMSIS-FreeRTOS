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
{% for sample in job["memory_trace"]["trace"] -%}
{{ sample["time"] }} {{ sample["rss"] }} {{ sample["vsz"] }}
{% endfor %}{# sample in job["memory_trace"]["trace"] #}
EOD

set terminal svg noenhanced size 565,96

set border 2 linecolor "#263238"

set log y 10
set format y "%.1s%cB"
set ytics nomirror tc "#263238" font "Helvetica,9"

set xdata time
set timefmt "%Y-%m-%dT%H:%M:%SZ"
unset xtics
unset key

set style line 100 lt 1 lc "#00000000" lw 2
set style line 101 lt 1 lc rgb "#ee000000" lw 1
set grid mxtics mytics ls 100, ls 101

plot '$data' using 1:2 with lines lc "#ab47bc", \
      '' using 1:3 with lines lc "#4caf50"
