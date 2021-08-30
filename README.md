Performance of **contribution-push** based vs **contribution-pull** based PageRank.

There are two ways (algorithmically) to think of the pagerank calculation.
1. Find pagerank by **pushing contribution** to *out-vertices*.
2. Find pagerank by **pulling contribution** from *in-vertices*.

This experiment was to try both of these approaches on a number of different
graphs, running each approach 5 times per graph to get a good time measure.
The **push** method is somewhat easier to implement, and is described in
[this lecture]. However, it requires multiple writes per source vertex.
On the other hand, the **pull** method requires 2 additional calculations
per-vertex, i.e., non-teleport contribution of each vertex, and, total
teleport contribution (to all vertices). However, it requires only 1 write
per destination vertex.

While it might seem that pull method would be a clear winner, the results
indicate that although **pull** is always **faster** than *push* approach,
the difference between the two depends on the nature of the graph. Note
that neither approach makes use of *SIMD instructions* which are available
on all modern hardware.

All outputs are saved in [out](out/) and a small part of the output is listed
here. Some [charts] are also included below, generated from [sheets]. The input
data used for this experiment is available at ["graphs"] (for small ones), and
the [SuiteSparse Matrix Collection]. This experiment was done with guidance
from [Prof. Dip Sankar Banerjee] and [Prof. Kishore Kothapalli].

<br>

```bash
$ g++ -std=c++17 -O3 main.cxx
$ ./a.out ~/data/email-Eu-core-temporal.txt
$ ./a.out ~/data/CollegeMsg.txt
$ ...

# ...
#
# Using graph sx-stackoverflow.txt ...
# Temporal edges: 63497051
# order: 2601977 size: 36233450 {}
#
# # Batch size 1e+1
# [05002.973 ms; 057 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [00202.562 ms; 003 iters.] [1.9117e-6 err.] pagerankTeleport (incremental)
# [00848.309 ms; 013 iters.] [4.2257e-4 err.] pagerankTeleport (dynamic)
# [03737.366 ms; 044 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [00145.952 ms; 002 iters.] [1.5363e-6 err.] pagerankLoop (incremental)
# [00133.297 ms; 002 iters.] [1.5312e-6 err.] pagerankLoop (dynamic)
# [03871.805 ms; 044 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [00135.460 ms; 002 iters.] [1.5719e-6 err.] pagerankLoopAll (incremental)
# [00121.147 ms; 002 iters.] [1.5658e-6 err.] pagerankLoopAll (dynamic)
# [03483.059 ms; 052 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [03666.520 ms; 059 iters.] [2.7844e-6 err.] pagerankRemove (incremental)
# [03223.608 ms; 058 iters.] [1.5855e-2 err.] pagerankRemove (dynamic)
#
# # Batch size 5e+1
# [04745.368 ms; 057 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [00367.308 ms; 005 iters.] [3.3459e-6 err.] pagerankTeleport (incremental)
# [00917.424 ms; 015 iters.] [4.2427e-4 err.] pagerankTeleport (dynamic)
# [03546.313 ms; 044 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [00296.277 ms; 004 iters.] [2.8986e-6 err.] pagerankLoop (incremental)
# [00269.449 ms; 004 iters.] [2.8962e-6 err.] pagerankLoop (dynamic)
# [03651.929 ms; 044 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [00267.314 ms; 004 iters.] [3.0449e-6 err.] pagerankLoopAll (incremental)
# [00239.670 ms; 004 iters.] [3.0453e-6 err.] pagerankLoopAll (dynamic)
# [03349.399 ms; 052 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [03539.277 ms; 059 iters.] [2.7845e-6 err.] pagerankRemove (incremental)
# [03079.910 ms; 058 iters.] [1.5855e-2 err.] pagerankRemove (dynamic)
#
# # Batch size 1e+2
# [04751.115 ms; 057 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [00444.750 ms; 006 iters.] [4.0269e-6 err.] pagerankTeleport (incremental)
# [00948.332 ms; 015 iters.] [4.2293e-4 err.] pagerankTeleport (dynamic)
# [03545.227 ms; 044 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [00367.382 ms; 006 iters.] [3.5223e-6 err.] pagerankLoop (incremental)
# [00333.304 ms; 006 iters.] [3.5198e-6 err.] pagerankLoop (dynamic)
# [03683.217 ms; 044 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [00349.473 ms; 005 iters.] [3.7478e-6 err.] pagerankLoopAll (incremental)
# [00309.862 ms; 005 iters.] [3.7485e-6 err.] pagerankLoopAll (dynamic)
# [03346.159 ms; 052 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [03549.176 ms; 059 iters.] [2.7846e-6 err.] pagerankRemove (incremental)
# [03079.790 ms; 058 iters.] [1.5855e-2 err.] pagerankRemove (dynamic)
#
# # Batch size 5e+2
# [04781.578 ms; 057 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [00705.591 ms; 011 iters.] [6.0043e-6 err.] pagerankTeleport (incremental)
# [01082.306 ms; 017 iters.] [4.2555e-4 err.] pagerankTeleport (dynamic)
# [03562.163 ms; 044 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [00600.090 ms; 010 iters.] [5.0818e-6 err.] pagerankLoop (incremental)
# [00542.173 ms; 010 iters.] [5.0824e-6 err.] pagerankLoop (dynamic)
# [03675.961 ms; 044 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [00596.994 ms; 010 iters.] [5.2962e-6 err.] pagerankLoopAll (incremental)
# [00527.997 ms; 010 iters.] [5.2972e-6 err.] pagerankLoopAll (dynamic)
# [03357.602 ms; 052 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [03548.026 ms; 059 iters.] [2.7858e-6 err.] pagerankRemove (incremental)
# [03092.220 ms; 058 iters.] [1.5855e-2 err.] pagerankRemove (dynamic)
#
# # Batch size 1e+3
# [04780.596 ms; 057 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [00912.707 ms; 013 iters.] [6.7187e-6 err.] pagerankTeleport (incremental)
# [01204.037 ms; 018 iters.] [4.2759e-4 err.] pagerankTeleport (dynamic)
# [03555.759 ms; 044 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [00770.909 ms; 013 iters.] [5.6900e-6 err.] pagerankLoop (incremental)
# [00696.765 ms; 013 iters.] [5.6965e-6 err.] pagerankLoop (dynamic)
# [03680.053 ms; 044 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [00769.479 ms; 012 iters.] [5.8276e-6 err.] pagerankLoopAll (incremental)
# [00677.719 ms; 012 iters.] [5.8364e-6 err.] pagerankLoopAll (dynamic)
# [03357.934 ms; 052 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [03555.368 ms; 059 iters.] [2.7859e-6 err.] pagerankRemove (incremental)
# [03095.234 ms; 058 iters.] [1.5854e-2 err.] pagerankRemove (dynamic)
#
# # Batch size 5e+3
# [04768.934 ms; 057 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [01578.418 ms; 022 iters.] [7.5360e-6 err.] pagerankTeleport (incremental)
# [01631.930 ms; 024 iters.] [4.3287e-4 err.] pagerankTeleport (dynamic)
# [03555.564 ms; 044 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [01358.960 ms; 021 iters.] [6.3014e-6 err.] pagerankLoop (incremental)
# [01227.135 ms; 021 iters.] [6.3166e-6 err.] pagerankLoop (dynamic)
# [03676.204 ms; 044 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [01329.939 ms; 020 iters.] [6.5633e-6 err.] pagerankLoopAll (incremental)
# [01177.258 ms; 020 iters.] [6.5629e-6 err.] pagerankLoopAll (dynamic)
# [03356.558 ms; 052 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [03551.939 ms; 059 iters.] [2.7932e-6 err.] pagerankRemove (incremental)
# [03086.757 ms; 058 iters.] [1.5857e-2 err.] pagerankRemove (dynamic)
#
# # Batch size 1e+4
# [04777.149 ms; 057 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [01892.988 ms; 025 iters.] [7.6619e-6 err.] pagerankTeleport (incremental)
# [01849.380 ms; 027 iters.] [4.3164e-4 err.] pagerankTeleport (dynamic)
# [03555.936 ms; 044 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [01614.177 ms; 024 iters.] [6.4769e-6 err.] pagerankLoop (incremental)
# [01458.247 ms; 024 iters.] [6.4854e-6 err.] pagerankLoop (dynamic)
# [03684.463 ms; 044 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [01608.712 ms; 024 iters.] [6.7163e-6 err.] pagerankLoopAll (incremental)
# [01422.144 ms; 024 iters.] [6.7158e-6 err.] pagerankLoopAll (dynamic)
# [03373.415 ms; 052 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [03548.945 ms; 059 iters.] [2.8002e-6 err.] pagerankRemove (incremental)
# [03096.622 ms; 058 iters.] [1.5851e-2 err.] pagerankRemove (dynamic)
#
# # Batch size 5e+4
# [04775.092 ms; 057 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [02639.549 ms; 034 iters.] [2.2245e-4 err.] pagerankTeleport (incremental)
# [02442.101 ms; 034 iters.] [4.2591e-4 err.] pagerankTeleport (dynamic)
# [03558.741 ms; 044 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [02224.945 ms; 032 iters.] [6.5667e-6 err.] pagerankLoop (incremental)
# [02024.157 ms; 032 iters.] [6.5757e-6 err.] pagerankLoop (dynamic)
# [03676.653 ms; 044 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [02227.225 ms; 032 iters.] [6.8989e-6 err.] pagerankLoopAll (incremental)
# [01965.698 ms; 032 iters.] [6.9042e-6 err.] pagerankLoopAll (dynamic)
# [03366.986 ms; 052 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [03546.215 ms; 060 iters.] [2.8175e-6 err.] pagerankRemove (incremental)
# [03100.640 ms; 058 iters.] [1.5816e-2 err.] pagerankRemove (dynamic)
#
# # Batch size 1e+5
# [04778.694 ms; 057 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [02949.117 ms; 038 iters.] [1.1854e-5 err.] pagerankTeleport (incremental)
# [02716.446 ms; 038 iters.] [4.2523e-4 err.] pagerankTeleport (dynamic)
# [03558.060 ms; 044 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [02479.410 ms; 036 iters.] [6.5906e-6 err.] pagerankLoop (incremental)
# [02249.130 ms; 036 iters.] [6.5941e-6 err.] pagerankLoop (dynamic)
# [03673.136 ms; 044 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [02500.679 ms; 035 iters.] [6.9285e-6 err.] pagerankLoopAll (incremental)
# [02195.977 ms; 035 iters.] [6.9358e-6 err.] pagerankLoopAll (dynamic)
# [03355.939 ms; 052 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [03550.462 ms; 060 iters.] [2.8607e-6 err.] pagerankRemove (incremental)
# [03094.177 ms; 058 iters.] [1.5762e-2 err.] pagerankRemove (dynamic)
#
# # Batch size 5e+5
# [04762.115 ms; 057 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [03662.999 ms; 046 iters.] [1.1385e-5 err.] pagerankTeleport (incremental)
# [03370.239 ms; 046 iters.] [3.9182e-4 err.] pagerankTeleport (dynamic)
# [03548.544 ms; 044 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [03093.522 ms; 044 iters.] [6.4936e-6 err.] pagerankLoop (incremental)
# [02809.636 ms; 044 iters.] [6.4926e-6 err.] pagerankLoop (dynamic)
# [03691.086 ms; 044 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [03121.277 ms; 043 iters.] [6.9331e-6 err.] pagerankLoopAll (incremental)
# [02752.587 ms; 043 iters.] [6.9493e-6 err.] pagerankLoopAll (dynamic)
# [03356.887 ms; 052 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [03551.440 ms; 060 iters.] [2.9354e-6 err.] pagerankRemove (incremental)
# [03112.631 ms; 058 iters.] [1.5293e-2 err.] pagerankRemove (dynamic)
#
# # Batch size 1e+6
# [04916.922 ms; 056 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [04086.868 ms; 050 iters.] [7.3622e-6 err.] pagerankTeleport (incremental)
# [03791.540 ms; 049 iters.] [3.4816e-4 err.] pagerankTeleport (dynamic)
# [03693.799 ms; 044 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [03431.281 ms; 046 iters.] [6.4576e-6 err.] pagerankLoop (incremental)
# [03157.004 ms; 046 iters.] [6.4582e-6 err.] pagerankLoop (dynamic)
# [03859.655 ms; 044 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [03479.083 ms; 046 iters.] [6.8607e-6 err.] pagerankLoopAll (incremental)
# [03088.012 ms; 046 iters.] [6.8601e-6 err.] pagerankLoopAll (dynamic)
# [03491.715 ms; 052 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [03678.819 ms; 060 iters.] [2.3293e-5 err.] pagerankRemove (incremental)
# [03267.009 ms; 059 iters.] [1.4546e-2 err.] pagerankRemove (dynamic)
#
# # Batch size 5e+6
# [05390.181 ms; 056 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [05293.096 ms; 056 iters.] [6.9308e-6 err.] pagerankTeleport (incremental)
# [04933.615 ms; 056 iters.] [3.2059e-4 err.] pagerankTeleport (dynamic)
# [04211.794 ms; 045 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [04494.648 ms; 051 iters.] [5.8819e-6 err.] pagerankLoop (incremental)
# [04188.283 ms; 051 iters.] [5.8825e-6 err.] pagerankLoop (dynamic)
# [04424.609 ms; 045 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [04528.085 ms; 050 iters.] [6.1182e-6 err.] pagerankLoopAll (incremental)
# [04182.273 ms; 050 iters.] [6.1178e-6 err.] pagerankLoopAll (dynamic)
# [03902.650 ms; 052 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [04220.964 ms; 061 iters.] [1.0379e-4 err.] pagerankRemove (incremental)
# [03758.471 ms; 060 iters.] [8.6993e-3 err.] pagerankRemove (dynamic)
#
# # Batch size 1e+7
# [05955.395 ms; 057 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [05912.912 ms; 057 iters.] [6.0776e-6 err.] pagerankTeleport (incremental)
# [05609.784 ms; 057 iters.] [6.4179e-4 err.] pagerankTeleport (dynamic)
# [04694.696 ms; 046 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [05016.746 ms; 050 iters.] [5.3995e-6 err.] pagerankLoop (incremental)
# [04672.290 ms; 050 iters.] [5.4004e-6 err.] pagerankLoop (dynamic)
# [05010.651 ms; 047 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [05104.116 ms; 049 iters.] [5.6157e-6 err.] pagerankLoopAll (incremental)
# [04551.011 ms; 049 iters.] [5.6146e-6 err.] pagerankLoopAll (dynamic)
# [04562.193 ms; 053 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [04649.649 ms; 061 iters.] [1.8976e-4 err.] pagerankRemove (incremental)
# [04235.037 ms; 061 iters.] [4.5333e-3 err.] pagerankRemove (dynamic)
#
# # Batch size 5e+7
# [07307.860 ms; 058 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [07700.429 ms; 059 iters.] [3.3479e-6 err.] pagerankTeleport (incremental)
# [07232.864 ms; 059 iters.] [5.5646e-5 err.] pagerankTeleport (dynamic)
# [06753.947 ms; 055 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [06666.423 ms; 054 iters.] [2.8859e-6 err.] pagerankLoop (incremental)
# [06233.059 ms; 054 iters.] [2.8869e-6 err.] pagerankLoop (dynamic)
# [07050.675 ms; 055 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [06818.487 ms; 054 iters.] [2.9766e-6 err.] pagerankLoopAll (incremental)
# [06273.915 ms; 054 iters.] [2.9767e-6 err.] pagerankLoopAll (dynamic)
# [06444.480 ms; 058 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [06665.566 ms; 062 iters.] [4.8887e-6 err.] pagerankRemove (incremental)
# [06036.619 ms; 062 iters.] [2.4701e-3 err.] pagerankRemove (dynamic)
```

[![](https://i.imgur.com/T99UVU6.png)][sheetp]
[![](https://i.imgur.com/ho5WsoW.png)][sheetp]
[![](https://i.imgur.com/l2J2JAp.png)][sheetp]
[![](https://i.imgur.com/btB7PaP.png)][sheetp]
[![](https://i.imgur.com/MRrBPY5.png)][sheetp]
[![](https://i.imgur.com/31UZETz.png)][sheetp]

[![](https://i.imgur.com/gaIoVl0.png)][sheetp]
[![](https://i.imgur.com/K1WxNau.png)][sheetp]
[![](https://i.imgur.com/oz6AtV5.png)][sheetp]
[![](https://i.imgur.com/kSkYCDZ.png)][sheetp]
[![](https://i.imgur.com/Opjvo3x.png)][sheetp]
[![](https://i.imgur.com/X4npWjd.png)][sheetp]

[![](https://i.imgur.com/Cr31WvY.png)][sheetp]
[![](https://i.imgur.com/NCcMMjg.png)][sheetp]
[![](https://i.imgur.com/Ha1urnv.png)][sheetp]
[![](https://i.imgur.com/AxL1RCN.png)][sheetp]
[![](https://i.imgur.com/boT5xvr.png)][sheetp]
[![](https://i.imgur.com/sfi3dMq.png)][sheetp]

[![](https://i.imgur.com/Js55Na2.png)][sheetp]
[![](https://i.imgur.com/pelUyYV.png)][sheetp]
[![](https://i.imgur.com/8Jq7rc6.png)][sheetp]
[![](https://i.imgur.com/3JsWE5B.png)][sheetp]
[![](https://i.imgur.com/OEE7RWB.png)][sheetp]
[![](https://i.imgur.com/wMFn605.png)][sheetp]

<br>
<br>


## References

- [PageRank Algorithm, Mining massive Datasets (CS246), Stanford University](https://www.youtube.com/watch?v=ke9g8hB0MEo)
- [SuiteSparse Matrix Collection]

<br>
<br>

[![](https://i.imgur.com/89cRRdY.jpg)](https://www.youtube.com/watch?v=iMdq5_5eib0)

[Prof. Dip Sankar Banerjee]: https://sites.google.com/site/dipsankarban/
[Prof. Kishore Kothapalli]: https://cstar.iiit.ac.in/~kkishore/
[SuiteSparse Matrix Collection]: https://suitesparse-collection-website.herokuapp.com
["graphs"]: https://github.com/puzzlef/graphs
[charts]: https://photos.app.goo.gl/LbBCvQRbsJe59pYL7
[sheets]: https://docs.google.com/spreadsheets/d/1-m6pIOu0QWGqGi3xuMxI5CK9ODmqlH-5sC4CJ3KXKwQ/edit?usp=sharing
[sheetp]: https://docs.google.com/spreadsheets/d/e/2PACX-1vRHPhiWm0rfTiqT0p2vwMCwl6_U42Q8UfxaYzhUS1QSFjgI0Ms5r0qTljS4lYyOBFJrvNQUZxJkvDlJ/pubhtml
