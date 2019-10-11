# fast-bernoulli

*fast generation of long sequencies of bernoulli-distributed random variables*

## Benchmarking

```
2019-10-12 02:35:03
Run on (4 X 3400 MHz CPU s)
CPU Caches:
  L1 Data 32K (x2)
  L1 Instruction 32K (x2)
  L2 Unified 256K (x2)
  L3 Unified 4096K (x1)
Load Average: 0.26, 0.30, 0.31
--------------------------------------------------------------------------------
Benchmark                             Time          CPU  Iterations   Throughput
--------------------------------------------------------------------------------
BM_RandomNumberGeneration/1        41.5 ns      41.4 ns    17262529  736.578 M/s
BM_RandomNumberGeneration/2        78.3 ns      77.9 ns     8940608  783.348 M/s
BM_RandomNumberGeneration/4         153 ns       152 ns     4568154  800.546 M/s
BM_RandomNumberGeneration/8         308 ns       308 ns     2273198  792.776 M/s
BM_RandomNumberGeneration/16        606 ns       606 ns     1147880  806.073 M/s
BM_RandomNumberGeneration/32       1199 ns      1198 ns      582027  815.069 M/s
BM_RandomNumberGeneration/64       2389 ns      2388 ns      293328  818.054 M/s
BM_RandomNumberGeneration/128      4786 ns      4783 ns      147228  816.740 M/s
BM_StdSampler/1                  563420 ns    563212 ns        1235  55.4853 k/s
BM_StdSampler/2                 1127229 ns   1126693 ns         620  55.4721 k/s
BM_StdSampler/4                 2255536 ns   2254256 ns         311  55.4507 k/s
BM_StdSampler/8                 4507455 ns   4505311 ns         156  55.4901 k/s
BM_StdSampler/16                9089400 ns   9042107 ns          77  55.2968 k/s
BM_StdSampler/32               18035676 ns  18026889 ns          39  55.4727 k/s
BM_StdSampler/64               36032876 ns  36017599 ns          19  55.5284 k/s
BM_StdSampler/128              72260140 ns  72208458 ns          10  55.3952 k/s
BM_GenericSampler/1                 906 ns       905 ns      767330  33.7125 M/s
BM_GenericSampler/2                1725 ns      1724 ns      405692  35.3983 M/s
BM_GenericSampler/4                3346 ns      3345 ns      209536  36.4988 M/s
BM_GenericSampler/8                6562 ns      6559 ns      105979  37.2210 M/s
BM_GenericSampler/16              13100 ns     13087 ns       52898  37.3107 M/s
BM_GenericSampler/32              25993 ns     25983 ns       26995  37.5844 M/s
BM_GenericSampler/64              51680 ns     51654 ns       13374  37.8119 M/s
BM_GenericSampler/128            104076 ns    103545 ns        6745  37.7252 M/s
BM_AvxSampler/1                     624 ns       624 ns     1111391  48.8905 M/s
BM_AvxSampler/2                    1204 ns      1203 ns      575640  50.7213 M/s
BM_AvxSampler/4                    2331 ns      2330 ns      299467  52.3988 M/s
BM_AvxSampler/8                    4562 ns      4559 ns      153490  53.5472 M/s
BM_AvxSampler/16                   8991 ns      8987 ns       77969  54.3348 M/s
BM_AvxSampler/32                  17790 ns     17783 ns       39135  54.9150 M/s
BM_AvxSampler/64                  35532 ns     35516 ns       19654  54.9933 M/s
BM_AvxSampler/128                 71017 ns     70982 ns        9759  55.0314 M/s
BM_JitGenericSampler/1              906 ns       905 ns      770395  33.7147 M/s
BM_JitGenericSampler/2             1722 ns      1721 ns      399302  35.4650 M/s
BM_JitGenericSampler/4             3378 ns      3361 ns      208338  36.3195 M/s
BM_JitGenericSampler/8             6611 ns      6609 ns      105095  36.9420 M/s
BM_JitGenericSampler/16           13075 ns     13069 ns       53433  37.3606 M/s
BM_JitGenericSampler/32           25895 ns     25884 ns       27050  37.7288 M/s
BM_JitGenericSampler/64           51654 ns     51632 ns       13341  37.8276 M/s
BM_JitGenericSampler/128         103482 ns    103431 ns        6735  37.7669 M/s
BM_JitAvxSampler/1                  511 ns       510 ns     1364082  59.7807 M/s
BM_JitAvxSampler/2                  967 ns       966 ns      726244  63.1767 M/s
BM_JitAvxSampler/4                 1846 ns      1846 ns      378779  66.1401 M/s
BM_JitAvxSampler/8                 3609 ns      3608 ns      194109  67.6757 M/s
BM_JitAvxSampler/16                7098 ns      7094 ns       97528  68.8279 M/s
BM_JitAvxSampler/32               13994 ns     13986 ns       49589  69.8233 M/s
BM_JitAvxSampler/64               28070 ns     27925 ns       25127  69.9421 M/s
BM_JitAvxSampler/128              55667 ns     55640 ns       12393  70.2053 M/s
```
