Comparing various strategies of **handling dead ends** for *incremental/dynamic PageRank* ([pull], [CSR]).

There are four ways **handling dead ends** for PageRank calculation.
1. Always **teleport** from dead ends.
2. Replace dead ends with self-loops (**loop**).
3. Add self-loops to all vertices, if it doesn't exist (**loop-all**).
4. **Remove** all dead ends recursively, calculate PageRank, and then find ranks of dead ends.

The four **dead end handling strategies** studied here include: *teleport*, *loop*, *loop-all*, and *remove*. With all of them, the PageRank algorithm used is the pull-based *standard power-iteration* approach that optionally accepts *initial ranks*. If the *initial ranks* are not provided, the *rank* of each vertex starts with `1/N`, where `N` is the *total number of vertices* in the graph. The *rank* of a vertex in an iteration is calculated as `c₀ + αΣrₑ/dₑ`, where `c₀` is the *common teleport contribution*, `α` is the *damping factor (0.85)*, `rₑ` is the *previous rank* of vertex with an incoming edge, and `dₑ` is the *out-degree* of the incoming-edge vertex. For the *teleport* strategy, the *common teleport contribution** `c₀` is `(1-α)/N + αΣrₙ/N`, which includes the *contribution due to a teleport from any vertex* in the graph due to the damping factor `(1-α)/N`, and *teleport from dead ends* in the graph `αΣrₙ/N`. For the other strategies, it is just `(1-α)/N` and does not change across iterations (since there are no teleports).

For *fixed graphs*, the **teleport** strategy can be implemented by adding edges from each dead end to all vertices in the graph, or achieving an equivalent effect through the rank formula, as given above (used here). With dynamic PageRank, if there exists any affected dead end (including dead ends in the previous snapshot of the graph), all vertices are marked as affected.

The **loop** strategy can be implemented by adding a self loop to each dead end in the graph (used here), or by conditionally including a self loop as an edge for each dead end during non-teleport rank calculation. For temporal graphs, determination of affected vertices can consider reachability from changed vertices in the equivalent graph (used here), but changed vertices in the original graph would be okay too.

The **loop-all** strategy can be implemented by adding a self loop to each vertex (used here), or by disallowing addition of self loops to vertices in the graph but including a self loop for each vertex during non-teleport rank calculation. The process for temporal graphs is similar to that of the loop strategy. This strategy is *fair*, since it does not give any *additional importance* to web pages (vertices) pointing to themselves (as all vertices implicitly point to themselves).

The **remove** strategy is more complicated however, since it involves removing vertices from the graph. It can be implemented as follows. The first step involves repeatedly removing dead ends from the graph, until there are no more dead ends in the graph. This is because removing dead ends can introduce new dead ends. Non-teleport rank calculation is then performed on the existing vertices, until convergence. The sum of all ranks should now be approximately 1. After that, ranks of the removed dead ends are calculated in the reverse order of their removal. This is just a single iteration of non-teleport rank calculation, since the dead ends do not affect the ranks of other vertices. The ranks calculated for dead ends is an excess above the sum of 1, and hence the final ranks can be obtained by scaling ranks of all vertices such that the final sum is 1. An alternative implementation of this would require keeping track of the equivalent graph, however for simplicity of implementation, the first approach mentioned above is used here. The process for temporal graphs is similar to that of the loop strategy.

Two experiments are conducted, one for comparing the difference in performance of static PageRank with each of the dead end handling strategies on fixed graphs, and the other for comparing the difference in performance of static, incremental, and dynamic PageRank with each of the strategies on temporal graphs. For the PageRank algorithm, convergence is reached when the L1 norm between the ranks of *all* vertices in the current and previous iterations fall below the tolerance value. Even for dynamic PageRank, ranks of all vertices are considered for L1 norm, although the rank calculation is only done for affected vertices. A damping factor of 0.85, and a tolerance of 10-6 is used. The maximum number of iterations allowed is 500. The PageRank computation is performed on a Compressed Sparse Row (CSR) representation of the graph. The execution time measured for each test case only includes the time required for PageRank computation, including error calculation (L1 norm). With the remove strategy, the time required for calculating ranks for the removed dead ends and scaling for the final ranks, is included. However, the time required to generate the equivalent graph (if needed), to generate CSR, to copy back results from CSR, or allocate memory is not included.

This experiment compares the difference in performance of static, incremental, and dynamic PageRank with each of the strategies on temporal graphs. Each graph is updated with multiple batch sizes (101, 5×101, 102, 5×102, ...), until the entire graph is processed. For each batch size, *static PageRank* is computed, along with *incremental* and *dynamic PageRank* based on each of the four dead end handling strategies. All seven temporal graphs used in this experiment are stored in a plain text file in `u, v, t` format, where `u` is the *source vertex*, `v` is the *destination vertex*, and `t` is the *UNIX epoch time* in *seconds*. These include: *CollegeMsg*, *email-Eu-core-temporal*, *sx-mathoverflow*, *sx-askubuntu*, *sx-superuser*, *wiki-talk-temporal*, and *sx-stackoverflow*. All of them are obtained from the *Stanford Large Network Dataset Collection*. The rest of the process is similar to that of the first experiment.

With the *second experiment* on *temporal graphs (static, incremental, and dynamic PageRank)*, it is observed that the **remove** strategy performs the best on all graphs. This is usually followed by both the **loop** and **loop-all** strategies, which perform equally well, except on the *CollegeMsg* and the *email-Eu-core-temporal* temporal graphs (where the *teleport* strategy appears to perform better than the two). Based on AM-RATIO comparison (mentioned below), it is observed that there is not much performance advantage of dynamic PageRank over incremental PageRank. This indicates that for large temporal graphs (since AM-RATIO assigns more importance to larger graphs), it might be better to skip determination of affected vertices, and simply perform incremental PageRank computation instead.

Based on **GM-RATIO** comparison, the relative time for **static PageRank** computation between *teleport*, *loop*, *loop-all*, and *remove* strategies is `1.00 : 0.80 : 0.78 : 0.56`. Hence, *loop* is *20% faster (1.25x)* than *teleport*, *loop* is *22% faster (1.28x)* than *teleport*, and *remove* is *44% faster (1.79x)* than *teleport*. Similarly, the relative time for **incremental PageRank** computation between the strategies is `0.73 : 0.62 : 0.63 : 0.43`. Hence, *loop* is *15% faster (1.18x)* than *teleport*, *loop* is *14% faster (1.16x)* than *teleport*, and *remove* is *41% faster (1.70x)* than *teleport*. Again, the relative time for **dynamic PageRank** computation between the strategies is `0.66 : 0.53 : 0.52 : 0.37`. Hence, *loop* is *20% faster (1.25x)* than *teleport*, *loop* is *21% faster (1.27x)* than *teleport*, and *remove* is *44% faster (1.78x)* than *teleport*. Here, *GM-RATIO* is obtained by taking the *geometric mean (GM)* of time taken for PageRank computation on all graphs, for each specific batch size. Then, *GM* is taken across all batch sizes, and a *ratio* is obtained *relative* to the *teleport* strategy.

Based on **AM-RATIO** comparison, the *relative time* for **static PageRank** computation between *teleport*, *loop*, *loop-all*, and *remove* is `1.00 : 0.92 : 0.87 : 0.71` (all batch sizes). Hence, *loop* is *8% faster (1.09x)* than *teleport*, *loop-all* is *13% faster (1.15x)* than *teleport*, and *remove* is *29% faster (1.41x)* than *teleport*. Similarly, the relative time for **incremental PageRank** computation between the strategies is `0.89 : 0.75 : 0.74 : 0.65`. Hence, *loop* is *16% faster (1.19x)* than *teleport*, *loop* is *17% faster (1.20x)* than *teleport*, and *remove* is *27% faster (1.37x)* than *teleport*. Again, the relative time for **dynamic PageRank** computation between the strategies is `0.85 : 0.71 : 0.72 : 0.65`. Hence, *loop* is *16% faster (1.20x)* than *teleport*, *loop* is *15% faster (1.18x)* than *teleport*, and *remove* is *24% faster (1.31x)* than *teleport*. *AM-RATIO* is obtained in a process similar to that of *GM-RATIO*, except that *arithmetic mean (AM)* is used instead of GM.

All outputs are saved in [out](out/) and a small part of the output is listed here. Some [charts] are also included below, generated from [sheets]. The input data used for this experiment is available at ["graphs"] (for small ones), and the [SuiteSparse Matrix Collection]. This experiment was done with guidance from [Prof. Dip Sankar Banerjee] and [Prof. Kishore Kothapalli].

<br>

```bash
$ g++ -std=c++17 -O3 main.cxx
$ ./a.out ~/data/email-Eu-core-temporal.txt
$ ./a.out ~/data/CollegeMsg.txt
$ ...

# (SHORTENED)
# ...
#
# Using graph sx-stackoverflow.txt ...
# Temporal edges: 63497051
# order: 2601977 size: 36233450 {}
#
# # Batch size 1e+1
# [05371.348 ms; 057 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [03970.383 ms; 041 iters.] [4.3594e-4 err.] pagerankTeleport (incremental)
# [03876.052 ms; 043 iters.] [1.0784e-2 err.] pagerankTeleport (dynamic)
# [04793.207 ms; 051 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [02565.950 ms; 028 iters.] [6.5816e-6 err.] pagerankLoop (incremental)
# [02803.016 ms; 032 iters.] [1.0635e-2 err.] pagerankLoop (dynamic)
# [04644.528 ms; 051 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [03233.549 ms; 036 iters.] [3.3369e-6 err.] pagerankLoopAll (incremental)
# [03249.540 ms; 036 iters.] [1.0813e-2 err.] pagerankLoopAll (dynamic)
# [03630.708 ms; 053 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [02736.803 ms; 041 iters.] [6.7006e-6 err.] pagerankRemove (incremental)
# [02721.396 ms; 040 iters.] [3.6837e-2 err.] pagerankRemove (dynamic)
#
# # Batch size 5e+1
# [05272.199 ms; 057 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [03941.268 ms; 041 iters.] [4.3594e-4 err.] pagerankTeleport (incremental)
# [03867.063 ms; 043 iters.] [8.5991e-4 err.] pagerankTeleport (dynamic)
# [04792.341 ms; 051 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [02546.866 ms; 028 iters.] [6.6080e-6 err.] pagerankLoop (incremental)
# [02909.436 ms; 033 iters.] [6.3630e-4 err.] pagerankLoop (dynamic)
# [04669.082 ms; 051 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [03196.980 ms; 036 iters.] [3.3378e-6 err.] pagerankLoopAll (incremental)
# [03271.879 ms; 037 iters.] [7.8648e-4 err.] pagerankLoopAll (dynamic)
# [03643.068 ms; 053 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [02724.356 ms; 041 iters.] [6.6790e-6 err.] pagerankRemove (incremental)
# [02748.172 ms; 040 iters.] [1.4858e-3 err.] pagerankRemove (dynamic)
#
# # Batch size 1e+2
# [05458.374 ms; 057 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [04023.023 ms; 041 iters.] [4.3596e-4 err.] pagerankTeleport (incremental)
# [03919.908 ms; 043 iters.] [8.5820e-4 err.] pagerankTeleport (dynamic)
# [04736.250 ms; 051 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [02516.317 ms; 028 iters.] [6.6234e-6 err.] pagerankLoop (incremental)
# [02825.006 ms; 033 iters.] [6.3595e-4 err.] pagerankLoop (dynamic)
# [04666.515 ms; 051 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [03261.254 ms; 036 iters.] [3.3615e-6 err.] pagerankLoopAll (incremental)
# [03306.337 ms; 037 iters.] [7.8661e-4 err.] pagerankLoopAll (dynamic)
# [03647.409 ms; 053 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [02791.048 ms; 041 iters.] [6.6668e-6 err.] pagerankRemove (incremental)
# [02717.653 ms; 041 iters.] [1.4858e-3 err.] pagerankRemove (dynamic)
#
# # Batch size 5e+2
# [05266.721 ms; 057 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [03944.053 ms; 041 iters.] [4.3600e-4 err.] pagerankTeleport (incremental)
# [03827.947 ms; 043 iters.] [8.5792e-4 err.] pagerankTeleport (dynamic)
# [04731.476 ms; 051 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [02617.663 ms; 029 iters.] [6.7600e-6 err.] pagerankLoop (incremental)
# [02793.562 ms; 033 iters.] [6.3410e-4 err.] pagerankLoop (dynamic)
# [04595.286 ms; 051 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [03149.276 ms; 036 iters.] [3.4130e-6 err.] pagerankLoopAll (incremental)
# [03238.974 ms; 037 iters.] [7.8522e-4 err.] pagerankLoopAll (dynamic)
# [03588.677 ms; 053 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [02705.601 ms; 041 iters.] [6.6880e-6 err.] pagerankRemove (incremental)
# [02730.835 ms; 041 iters.] [1.4865e-3 err.] pagerankRemove (dynamic)
#
# # Batch size 1e+3
# [05410.102 ms; 057 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [04011.095 ms; 041 iters.] [4.3603e-4 err.] pagerankTeleport (incremental)
# [03936.863 ms; 044 iters.] [8.5552e-4 err.] pagerankTeleport (dynamic)
# [04862.910 ms; 051 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [02632.436 ms; 029 iters.] [6.8730e-6 err.] pagerankLoop (incremental)
# [02861.522 ms; 033 iters.] [6.3282e-4 err.] pagerankLoop (dynamic)
# [04671.301 ms; 051 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [03211.310 ms; 037 iters.] [3.4650e-6 err.] pagerankLoopAll (incremental)
# [03280.428 ms; 037 iters.] [7.8425e-4 err.] pagerankLoopAll (dynamic)
# [03658.603 ms; 053 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [02778.086 ms; 042 iters.] [6.7267e-6 err.] pagerankRemove (incremental)
# [02695.196 ms; 041 iters.] [1.4872e-3 err.] pagerankRemove (dynamic)
#
# # Batch size 5e+3
# [05374.343 ms; 057 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [03987.516 ms; 042 iters.] [4.3611e-4 err.] pagerankTeleport (incremental)
# [03891.565 ms; 044 iters.] [8.6132e-4 err.] pagerankTeleport (dynamic)
# [04766.119 ms; 051 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [02697.255 ms; 031 iters.] [7.2113e-6 err.] pagerankLoop (incremental)
# [02919.008 ms; 034 iters.] [6.2588e-4 err.] pagerankLoop (dynamic)
# [04723.243 ms; 051 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [03290.932 ms; 037 iters.] [3.6155e-6 err.] pagerankLoopAll (incremental)
# [03255.293 ms; 038 iters.] [7.7974e-4 err.] pagerankLoopAll (dynamic)
# [03636.819 ms; 053 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [02777.736 ms; 042 iters.] [6.8112e-6 err.] pagerankRemove (incremental)
# [02753.543 ms; 042 iters.] [1.4945e-3 err.] pagerankRemove (dynamic)
#
# # Batch size 1e+4
# [05466.620 ms; 057 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [03976.898 ms; 043 iters.] [4.3610e-4 err.] pagerankTeleport (incremental)
# [03939.298 ms; 044 iters.] [8.6273e-4 err.] pagerankTeleport (dynamic)
# [04862.625 ms; 051 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [02863.829 ms; 032 iters.] [7.3308e-6 err.] pagerankLoop (incremental)
# [02942.877 ms; 035 iters.] [6.2011e-4 err.] pagerankLoop (dynamic)
# [04720.880 ms; 051 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [03252.195 ms; 038 iters.] [3.7109e-6 err.] pagerankLoopAll (incremental)
# [03316.540 ms; 038 iters.] [7.7665e-4 err.] pagerankLoopAll (dynamic)
# [03695.355 ms; 053 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [02813.647 ms; 043 iters.] [6.8294e-6 err.] pagerankRemove (incremental)
# [02811.126 ms; 043 iters.] [1.5040e-3 err.] pagerankRemove (dynamic)
#
# # Batch size 5e+4
# [05377.410 ms; 057 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [04037.713 ms; 044 iters.] [4.3611e-4 err.] pagerankTeleport (incremental)
# [03918.675 ms; 045 iters.] [8.5426e-4 err.] pagerankTeleport (dynamic)
# [04847.765 ms; 051 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [03066.993 ms; 036 iters.] [7.4830e-6 err.] pagerankLoop (incremental)
# [03133.407 ms; 037 iters.] [5.9698e-4 err.] pagerankLoop (dynamic)
# [04604.290 ms; 051 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [03261.156 ms; 039 iters.] [4.1140e-6 err.] pagerankLoopAll (incremental)
# [03294.912 ms; 040 iters.] [7.6608e-4 err.] pagerankLoopAll (dynamic)
# [03622.459 ms; 053 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [02821.860 ms; 045 iters.] [6.8141e-6 err.] pagerankRemove (incremental)
# [02782.257 ms; 045 iters.] [1.6220e-3 err.] pagerankRemove (dynamic)
#
# # Batch size 1e+5
# [05400.055 ms; 057 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [04128.715 ms; 045 iters.] [4.3621e-4 err.] pagerankTeleport (incremental)
# [04040.939 ms; 046 iters.] [8.5883e-4 err.] pagerankTeleport (dynamic)
# [04710.257 ms; 051 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [03263.639 ms; 038 iters.] [7.4915e-6 err.] pagerankLoop (incremental)
# [03140.323 ms; 039 iters.] [5.8047e-4 err.] pagerankLoop (dynamic)
# [04674.926 ms; 051 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [03341.458 ms; 040 iters.] [4.4658e-6 err.] pagerankLoopAll (incremental)
# [03379.463 ms; 041 iters.] [7.6095e-4 err.] pagerankLoopAll (dynamic)
# [03645.009 ms; 053 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [02946.419 ms; 046 iters.] [6.7777e-6 err.] pagerankRemove (incremental)
# [02790.277 ms; 046 iters.] [1.7630e-3 err.] pagerankRemove (dynamic)
#
# # Batch size 5e+5
# [05233.694 ms; 057 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [04269.627 ms; 049 iters.] [7.5238e-6 err.] pagerankTeleport (incremental)
# [04058.459 ms; 049 iters.] [3.9015e-4 err.] pagerankTeleport (dynamic)
# [04559.731 ms; 051 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [03590.872 ms; 044 iters.] [7.3134e-6 err.] pagerankLoop (incremental)
# [03523.050 ms; 045 iters.] [5.1104e-4 err.] pagerankLoop (dynamic)
# [04511.206 ms; 051 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [03520.224 ms; 044 iters.] [5.7580e-6 err.] pagerankLoopAll (incremental)
# [03535.754 ms; 044 iters.] [7.4686e-4 err.] pagerankLoopAll (dynamic)
# [03584.380 ms; 053 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [03080.927 ms; 050 iters.] [6.6171e-6 err.] pagerankRemove (incremental)
# [02989.051 ms; 050 iters.] [2.9415e-3 err.] pagerankRemove (dynamic)
#
# # Batch size 1e+6
# [05035.187 ms; 057 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [04304.463 ms; 051 iters.] [7.7943e-6 err.] pagerankTeleport (incremental)
# [04043.712 ms; 051 iters.] [3.4792e-4 err.] pagerankTeleport (dynamic)
# [04494.860 ms; 051 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [03793.552 ms; 047 iters.] [7.0675e-6 err.] pagerankLoop (incremental)
# [03445.999 ms; 047 iters.] [5.1622e-4 err.] pagerankLoop (dynamic)
# [04304.716 ms; 051 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [03594.472 ms; 046 iters.] [6.2605e-6 err.] pagerankLoopAll (incremental)
# [03367.684 ms; 046 iters.] [7.8028e-4 err.] pagerankLoopAll (dynamic)
# [03500.143 ms; 053 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [03166.234 ms; 052 iters.] [6.7201e-6 err.] pagerankRemove (incremental)
# [02979.418 ms; 052 iters.] [4.0616e-3 err.] pagerankRemove (dynamic)
#
# # Batch size 5e+6
# [05561.274 ms; 056 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [05428.330 ms; 056 iters.] [7.0833e-6 err.] pagerankTeleport (incremental)
# [05212.883 ms; 056 iters.] [3.2031e-4 err.] pagerankTeleport (dynamic)
# [04969.569 ms; 052 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [04977.689 ms; 052 iters.] [5.9813e-6 err.] pagerankLoop (incremental)
# [04604.742 ms; 052 iters.] [4.5803e-4 err.] pagerankLoop (dynamic)
# [05016.221 ms; 052 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [04494.151 ms; 051 iters.] [6.0251e-6 err.] pagerankLoopAll (incremental)
# [04759.798 ms; 051 iters.] [8.9832e-4 err.] pagerankLoopAll (dynamic)
# [03828.461 ms; 054 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [03913.141 ms; 056 iters.] [6.2728e-6 err.] pagerankRemove (incremental)
# [03818.287 ms; 056 iters.] [9.8153e-3 err.] pagerankRemove (dynamic)
#
# # Batch size 1e+7
# [06321.874 ms; 057 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [06317.034 ms; 057 iters.] [6.0978e-6 err.] pagerankTeleport (incremental)
# [05751.746 ms; 057 iters.] [6.4174e-4 err.] pagerankTeleport (dynamic)
# [06024.062 ms; 053 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [05406.426 ms; 053 iters.] [5.0584e-6 err.] pagerankLoop (incremental)
# [05040.958 ms; 053 iters.] [4.1711e-4 err.] pagerankLoop (dynamic)
# [05448.712 ms; 053 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [05238.460 ms; 052 iters.] [5.1731e-6 err.] pagerankLoopAll (incremental)
# [05176.814 ms; 052 iters.] [8.7106e-4 err.] pagerankLoopAll (dynamic)
# [04502.444 ms; 055 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [04385.614 ms; 057 iters.] [6.6382e-6 err.] pagerankRemove (incremental)
# [04659.534 ms; 056 iters.] [1.0733e-2 err.] pagerankRemove (dynamic)
#
# # Batch size 5e+7
# [07228.501 ms; 058 iters.] [0.0000e+0 err.] pagerankTeleport (static)
# [07329.928 ms; 059 iters.] [3.5756e-6 err.] pagerankTeleport (incremental)
# [07316.968 ms; 059 iters.] [5.4804e-5 err.] pagerankTeleport (dynamic)
# [07570.421 ms; 055 iters.] [0.0000e+0 err.] pagerankLoop (static)
# [06877.400 ms; 056 iters.] [3.3854e-6 err.] pagerankLoop (incremental)
# [06411.061 ms; 056 iters.] [2.4537e-4 err.] pagerankLoop (dynamic)
# [06633.721 ms; 055 iters.] [0.0000e+0 err.] pagerankLoopAll (static)
# [06573.800 ms; 055 iters.] [3.5277e-6 err.] pagerankLoopAll (incremental)
# [05739.313 ms; 055 iters.] [1.2089e-4 err.] pagerankLoopAll (dynamic)
# [06190.616 ms; 058 iters.] [0.0000e+0 err.] pagerankRemove (static)
# [06187.886 ms; 059 iters.] [3.9415e-6 err.] pagerankRemove (incremental)
# [06499.398 ms; 059 iters.] [5.0810e-3 err.] pagerankRemove (dynamic)
```

[![](https://i.imgur.com/lPtHl7B.gif)][sheetp]
[![](https://i.imgur.com/oZ1usFX.gif)][sheetp]
[![](https://i.imgur.com/fk8MWJq.gif)][sheetp]
[![](https://i.imgur.com/fpcXNxX.gif)][sheetp]
[![](https://i.imgur.com/aZnagUZ.gif)][sheetp]
[![](https://i.imgur.com/m91YKqV.gif)][sheetp]

[![](https://i.imgur.com/Sgo8ccp.gif)][sheetp]
[![](https://i.imgur.com/wpXaJlC.gif)][sheetp]
[![](https://i.imgur.com/XrzukTE.gif)][sheetp]
[![](https://i.imgur.com/chmGYKy.gif)][sheetp]
[![](https://i.imgur.com/Vx16D0W.gif)][sheetp]
[![](https://i.imgur.com/WdJGaaw.gif)][sheetp]

[![](https://i.imgur.com/Xx9Qt8f.gif)][sheetp]
[![](https://i.imgur.com/GpHQYgg.gif)][sheetp]
[![](https://i.imgur.com/jda4UYL.gif)][sheetp]
[![](https://i.imgur.com/r15xdHL.gif)][sheetp]
[![](https://i.imgur.com/rC3ybiA.gif)][sheetp]
[![](https://i.imgur.com/yJB4Rvp.gif)][sheetp]

[![](https://i.imgur.com/HCHrO4E.gif)][sheetp]
[![](https://i.imgur.com/WaEcDPw.gif)][sheetp]
[![](https://i.imgur.com/wTopoU3.gif)][sheetp]
[![](https://i.imgur.com/HwiH9mi.gif)][sheetp]
[![](https://i.imgur.com/ylOeglS.gif)][sheetp]
[![](https://i.imgur.com/C3DU0MU.gif)][sheetp]

[![](https://i.imgur.com/Otfr4ft.gif)][sheetp]
[![](https://i.imgur.com/a9PPcHc.gif)][sheetp]
[![](https://i.imgur.com/FUMLmuS.gif)][sheetp]
[![](https://i.imgur.com/31ipEUp.gif)][sheetp]
[![](https://i.imgur.com/CEV8p9t.gif)][sheetp]
[![](https://i.imgur.com/KKYYXHr.gif)][sheetp]

[![](https://i.imgur.com/dDuCZCB.gif)][sheetp]
[![](https://i.imgur.com/yElhrDY.gif)][sheetp]
[![](https://i.imgur.com/CVNMSTO.gif)][sheetp]
[![](https://i.imgur.com/3aZu6uX.gif)][sheetp]
[![](https://i.imgur.com/4sPJI7i.gif)][sheetp]
[![](https://i.imgur.com/JvYs4uf.gif)][sheetp]

[![](https://i.imgur.com/VjDd0KN.gif)][sheetp]
[![](https://i.imgur.com/R51X1Ke.gif)][sheetp]
[![](https://i.imgur.com/K9AKKgq.gif)][sheetp]
[![](https://i.imgur.com/To0IdP0.gif)][sheetp]
[![](https://i.imgur.com/2yf6Oip.gif)][sheetp]
[![](https://i.imgur.com/68BjaAg.gif)][sheetp]


[![](https://i.imgur.com/S3hYb5Y.png)][sheetp]
[![](https://i.imgur.com/01kCXyl.png)][sheetp]
[![](https://i.imgur.com/7oS0kBT.png)][sheetp]
[![](https://i.imgur.com/uCItMVs.png)][sheetp]
[![](https://i.imgur.com/RPsD3II.png)][sheetp]
[![](https://i.imgur.com/ugPVm0k.png)][sheetp]

[![](https://i.imgur.com/KbTXHv4.png)][sheetp]
[![](https://i.imgur.com/HMS8xAW.png)][sheetp]
[![](https://i.imgur.com/tZJ9avx.png)][sheetp]
[![](https://i.imgur.com/XrGp9QW.png)][sheetp]
[![](https://i.imgur.com/qMmHHTM.png)][sheetp]
[![](https://i.imgur.com/dOgNacC.png)][sheetp]

[![](https://i.imgur.com/GKS8wPt.png)][sheetp]
[![](https://i.imgur.com/WSqgQOY.png)][sheetp]
[![](https://i.imgur.com/pZVI6jg.png)][sheetp]
[![](https://i.imgur.com/0S2lhDp.png)][sheetp]
[![](https://i.imgur.com/daQvkII.png)][sheetp]
[![](https://i.imgur.com/EoAsfrD.png)][sheetp]

[![](https://i.imgur.com/zJztOBH.png)][sheetp]
[![](https://i.imgur.com/VaZgKF9.png)][sheetp]
[![](https://i.imgur.com/CjGiytR.png)][sheetp]
[![](https://i.imgur.com/nzUzcrZ.png)][sheetp]
[![](https://i.imgur.com/KnPIS7I.png)][sheetp]
[![](https://i.imgur.com/JOZTttu.png)][sheetp]

<br>
<br>


## References

- [Deeper Inside PageRank](https://www.slideshare.net/SubhajitSahu/deeper-inside-PageRank-notes)
- [PageRank Algorithm, Mining massive Datasets (CS246), Stanford University](https://www.youtube.com/watch?v=ke9g8hB0MEo)
- [SuiteSparse Matrix Collection]

<br>
<br>

[![](https://i.imgur.com/QESpBcO.jpg)](https://www.youtube.com/watch?v=SAKc2yijs4g)
[![DOI](https://zenodo.org/badge/397148950.svg)](https://zenodo.org/badge/latestdoi/397148950)

[Prof. Dip Sankar Banerjee]: https://sites.google.com/site/dipsankarban/
[Prof. Kishore Kothapalli]: https://cstar.iiit.ac.in/~kkishore/
[SuiteSparse Matrix Collection]: https://suitesparse-collection-website.herokuapp.com
["graphs"]: https://github.com/puzzlef/graphs
[pull]: https://github.com/puzzlef/pagerank-push-vs-pull
[CSR]: https://github.com/puzzlef/pagerank-class-vs-csr
[charts]: https://photos.app.goo.gl/LbBCvQRbsJe59pYL7
[sheets]: https://docs.google.com/spreadsheets/d/1-m6pIOu0QWGqGi3xuMxI5CK9ODmqlH-5sC4CJ3KXKwQ/edit?usp=sharing
[sheetp]: https://docs.google.com/spreadsheets/d/e/2PACX-1vRHPhiWm0rfTiqT0p2vwMCwl6*U42Q8UfxaYzhUS1QSFjgI0Ms5r0qTljS4lYyOBFJrvNQUZxJkvDlJ/pubhtml
