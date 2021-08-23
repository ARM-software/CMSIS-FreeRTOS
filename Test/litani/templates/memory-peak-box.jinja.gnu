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
{% for job in jobs -%}
0.4 {{ job["memory_trace"]["peak"]["rss"] }} {{ job["wrapper_arguments"]["pipeline_name"] }}
{% endfor %}{# job in jobs #}
EOD

set terminal svg noenhanced size 400,800

set format y '%.1s%cB'

set border 2 linecolor "#263238"
set ytics nomirror tc "#263238" font "Helvetica,14"
unset xtics
unset key

set title "Peak resident memory for {{ group_name }}" tc "#263238" font "Helvetica,14"

set xrange [0:1]

set boxwidth 0.2

plot '$data' using (0.2):2 with boxplot lc "#263238", \
      '' using 1:2:3 with labels left tc "#263238" font "Helvetica,10"
