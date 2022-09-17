Comparing various strategies of *handling dead ends* for PageRank ([pull], [CSR]).

There can be four ways for dealing with dead ends:
1. Find pagerank by **teleporting** to a random vertex from every dead end.
2. Find pagerank by **self-looping** dead ends.
3. Find pagerank by **self-looping all** vertices.
4. Find pagerank by **removing** dead ends initially, and calculating their ranks after convergence.

**Dead ends** (or *dangling nodes*) are defined as vertices *without any*
*out-edges*. Computing PageRank in the presence of dead ends in a graph causes
rank to *leak* out of such vertices, eventually leading to *zero rank* for
*each vertex* (not what we want). Mathematically, presence of such dead ends
makes the *transition probability matrix not stochastic*. The most common way
to deal with this issue is to **teleport** to a random vertex (with equal
probability) from every dead end. In the random surfer model, this means the
surfer jumps to a random web page on visiting a page with no links. This
requires an additional *common teleport contribution* calculation in every
iteration. Another approach originally suggested in the PageRank paper, it
to **remove** the dead ends for the initial iterations, and add them back
at the end. This approach has a serious issue as removal of dead ends can
introduce new dead ends. A yet another approach involves adding **self-loops**
to dead ends. In the random surfer model, this means stays in the same page
upon visiting a page with no links (but as usual, can jump to a random page
a probability of `1-α`, where `α` is the *damping factor*). Unlike the
*teleport* based approach, this does not require any additional calculation,
and even allows *per-iteration communitcation free partitioned* PageRank
computation (in SCC-based levelwise order) as done is this [STICD paper].
Another extension to this approach is to add **self-loops to all** vertices
in the graph. This means the random surfer always has the option to stay in
the same page (or every page always links itself).

This experiment was to try each of these approaches on a number of different
graphs, running each approach 5 times per graph to get a good time measure.
The **remove** approach implemented here performs only a *single step removal*
of dead ends, and not recursively. After PageRank computation converges on
this removed graph, ranks of dead ends are *calculated* and finally all ranks
are *scaled* to sum to 1. It is also important to note that each of the four
approaches are *semantically different*, and thus the ranks computed by each
approach is *different*.

With this experiment on *fixed graphs* (*static PageRank* only), there
appears to only be a minor average performance difference between the dead end
handling strategies. Both the **loop-all** and **remove** strategies seem to
perform the best on average. It is important to note however that this varies
depending upon the nature of the graph.

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

All outputs are saved in [out](out/) and a small part of the output is listed
here. Some [charts] are also included below, generated from [sheets]. The input
data used for this experiment is available at ["graphs"] (for small ones), and
the [SuiteSparse Matrix Collection]. This experiment was done with guidance
from [Prof. Dip Sankar Banerjee] and [Prof. Kishore Kothapalli].

<br>

```bash
$ g++ -std=c++17 -O3 main.cxx
$ ./a.out ~/data/min-1DeadEnd.mtx
$ ./a.out ~/data/min-2SCC.mtx
$ ...

# ...
#
# Loading graph /home/subhajit/data/web-Stanford.mtx ...
# order: 281903 size: 2312497 {}
# [00409.751 ms; 063 iters.] [0.0000e+00 err.] pagerankTeleport
# [00395.462 ms; 063 iters.] [4.7640e-03 err.] pagerankLoop
# [00355.871 ms; 054 iters.] [1.1641e-01 err.] pagerankLoopAll
# [00393.593 ms; 063 iters.] [1.5229e-03 err.] pagerankRemove
#
# Loading graph /home/subhajit/data/soc-LiveJournal1.mtx ...
# order: 4847571 size: 68993773 {}
# [12019.400 ms; 051 iters.] [0.0000e+00 err.] pagerankTeleport
# [13563.278 ms; 058 iters.] [3.6227e-01 err.] pagerankLoop
# [13551.666 ms; 057 iters.] [3.8861e-01 err.] pagerankLoopAll
# [11926.444 ms; 052 iters.] [5.0604e-02 err.] pagerankRemove
#
# ...
```

[![](https://i.imgur.com/rVcRh8V.png)][sheetp]
[![](https://i.imgur.com/NtoWc2H.png)][sheetp]
[![](https://i.imgur.com/8TZm7eS.png)][sheetp]
[![](https://i.imgur.com/1AhHvhc.png)][sheetp]

<br>
<br>


## References

- [Deeper Inside PageRank](https://www.slideshare.net/SubhajitSahu/deeper-inside-PageRank-notes)
- [PageRank Algorithm, Mining massive Datasets (CS246), Stanford University](https://www.youtube.com/watch?v=ke9g8hB0MEo)
- [SuiteSparse Matrix Collection]

<br>
<br>

[![](https://i.imgur.com/lbSSW9m.jpg)](https://www.youtube.com/watch?v=cGadX6kAYZk)
[![ORG](https://img.shields.io/badge/org-puzzlef-green?logo=Org)](https://puzzlef.github.io)
[![DOI](https://zenodo.org/badge/396745082.svg)](https://zenodo.org/badge/latestdoi/396745082)

[Prof. Dip Sankar Banerjee]: https://sites.google.com/site/dipsankarban/
[Prof. Kishore Kothapalli]: https://cstar.iiit.ac.in/~kkishore/
[SuiteSparse Matrix Collection]: https://suitesparse-collection-website.herokuapp.com
["graphs"]: https://github.com/puzzlef/graphs
[STICD paper]: https://www.slideshare.net/SubhajitSahu/sticd-algorithmic-techniques-for-efficient-parallel-pagerank-computation-on-realworld-graphs
[pull]: https://github.com/puzzlef/pagerank-push-vs-pull
[CSR]: https://github.com/puzzlef/pagerank-class-vs-csr
[charts]: https://photos.app.goo.gl/KccG7WLmJVBXsZBeA
[sheets]: https://docs.google.com/spreadsheets/d/1dvHgNP8YbpGbpOXZTnGrotgAwufN-DmVfS01uq1qs6s/edit?usp=sharing
[sheetp]: https://docs.google.com/spreadsheets/d/e/2PACX-1vSclJIjkqQKAgbDZZz8iVVqZ8JzT5vPECB0df9P8xj5FqEckuVgP1oEz1UaWQd6cwrmssu0j5HyVKoi/pubhtml
