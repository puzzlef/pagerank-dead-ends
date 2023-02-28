Comparing strategies of **handling dead ends** with *PageRank algorithm* for
link analysis.

There are four ways **handling dead ends** for PageRank calculation.
1. Always **teleport** from dead ends.
2. Replace dead ends with self-loops (**loop**).
3. Add self-loops to all vertices, if it doesn't exist (**loop-all**).
4. **Remove** all dead ends recursively, calculate PageRank, and then find ranks of dead ends.

The four **dead end handling strategies** studied here include: *teleport*,
*loop*, *loop-all*, and *remove*. With all of them, the PageRank algorithm used
is the pull-based *standard power-iteration* approach that optionally accepts
*initial ranks*. If the *initial ranks* are not provided, the *rank* of each
vertex starts with `1/N`, where `N` is the *total number of vertices* in the
graph. The *rank* of a vertex in an iteration is calculated as `c₀ + αΣrₑ/dₑ`,
where `c₀` is the *common teleport contribution*, `α` is the *damping factor*
*(0.85)*, `rₑ` is the *previous rank* of vertex with an incoming edge, and `dₑ`
is the *out-degree* of the incoming-edge vertex. For the *teleport* strategy,
the *common teleport contribution** `c₀` is `(1-α)/N + αΣrₙ/N`, which includes
the *contribution due to a teleport from any vertex* in the graph due to the
damping factor `(1-α)/N`, and *teleport from dead ends* in the graph `αΣrₙ/N`.
For the other strategies, it is just `(1-α)/N` and does not change across
iterations (since there are no teleports).

For *fixed graphs*, the **teleport** strategy can be implemented by adding edges
from each dead end to all vertices in the graph, or achieving an equivalent
effect through the rank formula, as given above (used here). With dynamic
PageRank, if there exists any affected dead end (including dead ends in the
previous snapshot of the graph), all vertices are marked as affected.

The **loop** strategy can be implemented by adding a self loop to each dead end
in the graph (used here), or by conditionally including a self loop as an edge
for each dead end during non-teleport rank calculation. For temporal graphs,
determination of affected vertices can consider reachability from changed
vertices in the equivalent graph (used here), but changed vertices in the
original graph would be okay too.

The **loop-all** strategy can be implemented by adding a self loop to each
vertex (used here), or by disallowing addition of self loops to vertices in the
graph but including a self loop for each vertex during non-teleport rank
calculation. The process for temporal graphs is similar to that of the loop
strategy. This strategy is *fair*, since it does not give any *additional
importance* to web pages (vertices) pointing to themselves (as all vertices
implicitly point to themselves).

The **remove** strategy is more complicated however, since it involves removing
vertices from the graph. It can be implemented as follows. The first step
involves repeatedly removing dead ends from the graph, until there are no more
dead ends in the graph. This is because removing dead ends can introduce new
dead ends. Non-teleport rank calculation is then performed on the existing
vertices, until convergence. The sum of all ranks should now be approximately 1.
After that, ranks of the removed dead ends are calculated in the reverse order
of their removal. This is just a single iteration of non-teleport rank
calculation, since the dead ends do not affect the ranks of other vertices. The
ranks calculated for dead ends is an excess above the sum of 1, and hence the
final ranks can be obtained by scaling ranks of all vertices such that the final
sum is 1. An alternative implementation of this would require keeping track of
the equivalent graph, however for simplicity of implementation, the first
approach mentioned above is used here. The process for temporal graphs is
similar to that of the loop strategy.

Two experiments are conducted, one for comparing the difference in performance
of static PageRank with each of the dead end handling strategies on fixed
graphs, and the other for comparing the difference in performance of static,
incremental, and dynamic PageRank with each of the strategies on temporal
graphs. For the PageRank algorithm, convergence is reached when the L1 norm
between the ranks of *all* vertices in the current and previous iterations fall
below the tolerance value. Even for dynamic PageRank, ranks of all vertices are
considered for L1 norm, although the rank calculation is only done for affected
vertices. A damping factor of `0.85`, and a tolerance of `10^-6` is used. The maximum
number of iterations allowed is `500`. The PageRank computation is performed on a
Compressed Sparse Row (CSR) representation of the graph. The execution time
measured for each test case only includes the time required for PageRank
computation, including error calculation (L1 norm). With the remove strategy,
the time required for calculating ranks for the removed dead ends and scaling
for the final ranks, is included. However, the time required to generate the
equivalent graph (if needed), to generate CSR, to copy back results from CSR, or
allocate memory is not included.

<br>


### Using Static PageRank

With this experiment ([approach-static]) on *fixed graphs* (*static PageRank*
only), there appears to only be a minor average performance difference between
the dead end handling strategies. Both the **loop-all** and **remove**
strategies seem to perform the best on average. It is important to note however
that this varies depending upon the nature of the graph.

Based on **GM-RATIO** comparison, the relative time for static PageRank
computation between *teleport*, *loop*, *loop-all*, and *remove* strategies
is `1.00 : 1.01 : 0.95 : 0.94`. Thus, *loop* is *1% slower (0.99x)* than
*teleport*, *loop-all* is *5% faster (1.05x)* than *teleport*, and *remove* is
*6% faster (1.06x)* than *teleport*. Here, *GM-RATIO* is obtained by taking the
*geometric mean (GM)* of time taken for PageRank computation on all graphs,
and then obtaining a ratio *relative* to the *teleport* strategy.

Based on **AM-RATIO** comparison, the *relative time* for static PageRank
computation between *teleport*, *loop*, *loop-all*, and *remove* is
`1.00 : 1.02 : 0.95 : 0.97`. Hence, *loop* is *2% slower (0.98x)* than
*teleport*, *loop-all* is *5% faster (1.05x)* than *teleport*, and *remove*
is *3% faster (1.03x)* than *teleport*. *AM-RATIO* is obtained in a process
similar to that of *GM-RATIO*, except that *arithmetic mean (AM)* is used
instead of GM.

For graphs with a large number of vertices, such as *soc-LiveJournal1*,
*indochina-2004*, *italy_osm*, *great-britain_osm*, *germany_osm*, and
*asia_osm*, it is observed that the *total sum of ranks* of vertices is
*not* close to `1` when **32-bit (float) floating point format** is used for
*rank of each vertex*. This is likely due to **precision issues**, as a
similar effect is not observed when *64-bit (double) floating point format*
is used for *vertex ranks*.

Additionally, this sum also varies between *teleport/loop* and *remove*
strategies when *32-bit floating point format* is used for *vertex ranks*
on *symmetric (undirected) graphs* such as *italy_osm*, *great-britain_osm*,
*germany_osm*, and *asia_osm*. Again, this is *not* observed when
*64-bit floating point format* is used for *rank of each vertex*. Ideally,
this sum should *not vary* between the strategies. This is because there
are no dead ends in an undirected graph, and thus the scaling factor for
the *remove* strategy is `1/sum(R) ≈ 1/1 ≈ 1`, where `R` is the *rank vector*
after PageRank computation (which is the same for *teleport*, *loop*, and
*remove* strategies on *undirected graphs*).

Thus, this difference between the sums among the *teleport/loop* and
*remove* strategies when using *32-bit floating point format* for *ranks*
is due to the same *precision issues* mentioned above associated with the
additional *rank scaling* performed with the *remove* strategy (in order
to ensure that the sum of ranks is 1 after computing ranks of dead ends).

[approach-static]: https://github.com/puzzlef/pagerank-dead-ends/tree/approach-static

<br>


### Using Dynamic PageRank

This experiment ([approach-dynamic]) compares the difference in performance of
static, incremental, and dynamic PageRank with each of the strategies on
temporal graphs. Each graph is updated with multiple batch sizes (`10^1`, `5×10^1`,
`10^2`, `5×10^2`, ...), until the entire graph is processed. For each batch size,
*static PageRank* is computed, along with *incremental* and *dynamic PageRank*
based on each of the four dead end handling strategies. All seven temporal
graphs used in this experiment are stored in a plain text file in `u, v, t`
format, where `u` is the *source vertex*, `v` is the *destination vertex*, and
`t` is the *UNIX epoch time* in *seconds*. These include: *CollegeMsg*,
*email-Eu-core-temporal*, *sx-mathoverflow*, *sx-askubuntu*, *sx-superuser*,
*wiki-talk-temporal*, and *sx-stackoverflow*. All of them are obtained from the
*Stanford Large Network Dataset Collection*. The rest of the process is similar
to that of the first experiment.

With the *second experiment* on *temporal graphs (static, incremental, and
dynamic PageRank)*, it is observed that the **remove** strategy performs the
best on all graphs. This is usually followed by both the **loop** and
**loop-all** strategies, which perform equally well, except on the *CollegeMsg*
and the *email-Eu-core-temporal* temporal graphs (where the *teleport* strategy
appears to perform better than the two). Based on AM-RATIO comparison (mentioned
below), it is observed that there is not much performance advantage of dynamic
PageRank over incremental PageRank. This indicates that for large temporal
graphs (since AM-RATIO assigns more importance to larger graphs), it might be
better to skip determination of affected vertices, and simply perform
incremental PageRank computation instead.

Based on **GM-RATIO** comparison, the relative time for **static PageRank**
computation between *teleport*, *loop*, *loop-all*, and *remove* strategies is
`1.00 : 0.80 : 0.78 : 0.56`. Hence, *loop* is *20% faster (1.25x)* than
*teleport*, *loop* is *22% faster (1.28x)* than *teleport*, and *remove* is *44%*
*faster (1.79x)* than *teleport*. Similarly, the relative time for **incremental**
**PageRank** computation between the strategies is `0.73 : 0.62 : 0.63 : 0.43`.
Hence, *loop* is *15% faster (1.18x)* than *teleport*, *loop* is *14% faster*
*(1.16x)* than *teleport*, and *remove* is *41% faster (1.70x)* than *teleport*.
Again, the relative time for **dynamic PageRank** computation between the
strategies is `0.66 : 0.53 : 0.52 : 0.37`. Hence, *loop* is *20% faster (1.25x)*
than *teleport*, *loop* is *21% faster (1.27x)* than *teleport*, and *remove* is
*44% faster (1.78x)* than *teleport*. Here, *GM-RATIO* is obtained by taking the
*geometric mean (GM)* of time taken for PageRank computation on all graphs, for
each specific batch size. Then, *GM* is taken across all batch sizes, and a
*ratio* is obtained *relative* to the *teleport* strategy.

Based on **AM-RATIO** comparison, the *relative time* for **static PageRank**
computation between *teleport*, *loop*, *loop-all*, and *remove* is `1.00 : 0.92 : 0.87 : 0.71`
(all batch sizes). Hence, *loop* is *8% faster (1.09x)* than
*teleport*, *loop-all* is *13% faster (1.15x)* than *teleport*, and *remove* is
*29% faster (1.41x)* than *teleport*. Similarly, the relative time for
**incremental PageRank** computation between the strategies is `0.89 : 0.75 : 0.74 : 0.65`.
Hence, *loop* is *16% faster (1.19x)* than *teleport*, *loop* is
*17% faster (1.20x)* than *teleport*, and *remove* is *27% faster (1.37x)* than
*teleport*. Again, the relative time for **dynamic PageRank** computation
between the strategies is `0.85 : 0.71 : 0.72 : 0.65`. Hence, *loop* is *16%*
*faster (1.20x)* than *teleport*, *loop* is *15% faster (1.18x)* than *teleport*,
and *remove* is *24% faster (1.31x)* than *teleport*. *AM-RATIO* is obtained in
a process similar to that of *GM-RATIO*, except that *arithmetic mean (AM)* is
used instead of GM.

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

[approach-dynamic]: https://github.com/puzzlef/pagerank-dead-ends/tree/approach-dynamic

<br>


### Other experiments

- [approach-static-print-float-sum](https://github.com/puzzlef/pagerank-dead-ends/tree/approach-static-print-float-sum)
- [approach-static-print-double-sum](https://github.com/puzzlef/pagerank-dead-ends/tree/approach-static-print-double-sum)

<br>
<br>


## References

- [Deeper Inside PageRank](https://www.slideshare.net/SubhajitSahu/deeper-inside-PageRank-notes)
- [PageRank Algorithm, Mining massive Datasets (CS246), Stanford University](https://www.youtube.com/watch?v=ke9g8hB0MEo)
- [SuiteSparse Matrix Collection]

<br>
<br>


[![](https://i.imgur.com/QESpBcO.jpg)](https://www.youtube.com/watch?v=SAKc2yijs4g)
[![ORG](https://img.shields.io/badge/org-puzzlef-green?logo=Org)](https://puzzlef.github.io)
[![DOI](https://zenodo.org/badge/396745082.svg)](https://zenodo.org/badge/latestdoi/396745082)


[Prof. Dip Sankar Banerjee]: https://sites.google.com/site/dipsankarban/
[Prof. Kishore Kothapalli]: https://cstar.iiit.ac.in/~kkishore/
[SuiteSparse Matrix Collection]: https://suitesparse-collection-website.herokuapp.com
["graphs"]: https://github.com/puzzlef/graphs
[STICD paper]: https://www.slideshare.net/SubhajitSahu/sticd-algorithmic-techniques-for-efficient-parallel-pagerank-computation-on-realworld-graphs
[charts]: https://photos.app.goo.gl/LbBCvQRbsJe59pYL7
[sheets]: https://docs.google.com/spreadsheets/d/1-m6pIOu0QWGqGi3xuMxI5CK9ODmqlH-5sC4CJ3KXKwQ/edit?usp=sharing
[sheetp]: https://docs.google.com/spreadsheets/d/e/2PACX-1vRHPhiWm0rfTiqT0p2vwMCwl6*U42Q8UfxaYzhUS1QSFjgI0Ms5r0qTljS4lYyOBFJrvNQUZxJkvDlJ/pubhtml
