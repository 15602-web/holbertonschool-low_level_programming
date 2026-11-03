# Measurement Methodology

All execution times are measured using the <clock()> function from the <time.h> library. It returns the CPU time consumed, which is then converted to seconds using <CLOCKS_PER_SEC.>

The files baseline_loop.c (100,000,000 iterations) and comparison_algorithms.c (50,000 elements) were each run three times, with no changes to the source code.

In instrumentation_lab.c, each phase is wrapped by a start time reading (start) and an end time reading (end). A global measurement (TOTAL) encompasses all three phases.

Finally, the printf calls are made only after all measurements have been taken, so that the time spent on output is not included in the results.

# Observed Performance Differences

**Baseline Execution Measurement**

The program was executed three consecutive times. it gave 0.226550 seconds as the first result, 0.236100 seconds as the second result then 0.227494 seconds. the average time is 0.230048 s and the spread

between the fastest and lowest is 0.009550 s. The slowest run took about 4.2 % longer than the fastest, even though the code was strictly identical.

**Algorithmic Efficiency Comparison**

The naive algorithm gave 2.028074 seconds for the first run, 1.880994 seconds and 1.896762 seconds for the second and last run, with an average time of 1.935277 s for all of them.

Single-pass algorithm time: 0.000099 seconds for the first one, the second single pass: 0.000103 seconds, and the third single pass: 0.000101 seconds (average 1.935277 s).

The naive version is about 19,000 times slower than the Single pass, the reason is because the naive version contain two loops and the single pass a single one.

**Controlled Instrumentation Experiment**
| Phase | Temps (s) | Part du TOTAL |
|---|---|---|
| BUILD_DATA | 0,000147 | 43,1 % |
| PROCESS | 0,000122 | 35,8 % |
| REDUCE | 0,000068 | 19,9 % |
| TOTAL | 0,000341 | 100 % |

In the instrumented run, BUILD_DATA is the slowest phase (0.000147 s, about 43.1 % of TOTAL), followed by PROCESS (0.000122 s, about 35.8 %), which performs a division and a modulo per element. 

REDUCE, which only multiplies and adds, is the fastest phase (0.000068 s, about 19.9 %). The three phases add up to 0.000337 s, 

and the remaining 0.000004 s gap to TOTAL (0.000341 s) comes from the intermediate clock() calls placed between the phases.

**Relation Between Runtime and Energy Consumption**

Since energy is roughly "power x time", the naive algorithm (1.935277 s) would consume about 19,000 times more energy than the single-pass one (0.000101 s), assuming constant power. 

This is an estimate only, no energy was measured, and the experiments only show where CPU time is spent.

**Limitations of the Experiment**

No energy measurement: only execution time was recorded, so any conclusion about energy remains an assumption.

Limited number of runs: three runs per program are not enough for reliable statistics. The single-pass times range from 0.000099 s to 0.000103 s (a spread of about 4 % of the average),

but at this scale a difference of a few microseconds corresponds to only a few timer ticks.

Very short durations: the instrumented program runs in 0.000341 s, close to the timer resolution. The 0.000025 s gap between BUILD_DATA and PROCESS is therefore not reliable.

CPU time versus wall-clock time: clock() reports the CPU time consumed by the process, which can differ from the real elapsed time.

Different outputs: the naive and single-pass algorithms do not return the same value, so the comparison only concerns execution time, not the results produced.

**Practical Engineering Takeaway**

Algorithm choice has by far the greatest impact: removing the nested loop reduced the time from about 1.94 s to about 0.0001 s, a gain much larger than any difference between individual instructions. 

Per-phase instrumentation then shows where to focus the effort, since BUILD_DATA and PROCESS together account for about 79 % of the instrumented total. 

Measurements must be repeated before drawing conclusions, as even the baseline loop varied by about 4 % between runs. 

Finally, a lower CPU time suggests a lower energy cost, but only a dedicated energy measurement could confirm it.
