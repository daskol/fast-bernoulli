# fast-bernoulli

*fast generation of long sequencies of bernoulli-distributed random variables*

## Benchmarking

```
Run on (4 X 3400 MHz CPU s)
CPU Caches:
  L1 Data 32K (x2)
  L1 Instruction 32K (x2)
  L2 Unified 256K (x2)
  L3 Unified 4096K (x1)
Load Average: 0.34, 0.42, 0.47
--------------------------------------------------------------------------------
Benchmark                             Time          CPU  Iterations   Throughput
--------------------------------------------------------------------------------
BM_RandomNumberGeneration/1        11.7 ns      11.7 ns    58923466  2.54100 G/s
BM_RandomNumberGeneration/2        23.4 ns      23.3 ns    29917991  2.55359 G/s
BM_RandomNumberGeneration/4        46.9 ns      46.9 ns    14723058  2.54448 G/s
BM_RandomNumberGeneration/8        93.4 ns      93.4 ns     7407649  2.55298 G/s
BM_RandomNumberGeneration/16        187 ns       187 ns     3755715  2.54581 G/s
BM_RandomNumberGeneration/32        373 ns       373 ns     1819089  2.55792 G/s
BM_RandomNumberGeneration/64        746 ns       746 ns      932039  2.55797 G/s
BM_RandomNumberGeneration/128      1502 ns      1501 ns      466451  2.54100 G/s
BM_StdSampler/1                  321874 ns    321716 ns        2158  97.1352 k/s
BM_StdSampler/2                  642716 ns    642411 ns        1078  97.2897 k/s
BM_StdSampler/4                 1291029 ns   1289319 ns         542  96.9504 k/s
BM_StdSampler/8                 2595176 ns   2591568 ns         271  96.4667 k/s
BM_StdSampler/16                5170010 ns   5164404 ns         135  96.8166 k/s
BM_StdSampler/32               10332434 ns  10322721 ns          68  96.8737 k/s
BM_StdSampler/64               20627384 ns  20612815 ns          34  97.0270 k/s
BM_StdSampler/128              41304595 ns  41268982 ns          17  96.9251 k/s
BM_GenericSampler/1                 194 ns       194 ns     3641203  157.342 M/s
BM_GenericSampler/2                 345 ns       345 ns     2033325  177.114 M/s
BM_GenericSampler/4                 629 ns       626 ns     1115202  195.087 M/s
BM_GenericSampler/8                1187 ns      1186 ns      582127  205.890 M/s
BM_GenericSampler/16               2294 ns      2292 ns      305235  213.064 M/s
BM_GenericSampler/32               4511 ns      4508 ns      154634  216.629 M/s
BM_GenericSampler/64               8951 ns      8944 ns       76944  218.384 M/s
BM_GenericSampler/128             17886 ns     17871 ns       39090  218.586 M/s
BM_AvxSampler/1                     158 ns       158 ns     4483579  193.238 M/s
BM_AvxSampler/2                     305 ns       305 ns     2305749  200.274 M/s
BM_AvxSampler/4                     586 ns       586 ns     1186049  208.482 M/s
BM_AvxSampler/8                    1145 ns      1144 ns      612314  213.409 M/s
BM_AvxSampler/16                   2308 ns      2306 ns      303071  211.746 M/s
BM_AvxSampler/32                   4544 ns      4538 ns      153723  215.190 M/s
BM_AvxSampler/64                   9026 ns      9016 ns       77948  216.635 M/s
BM_AvxSampler/128                 17849 ns     17836 ns       39176  219.005 M/s
```
