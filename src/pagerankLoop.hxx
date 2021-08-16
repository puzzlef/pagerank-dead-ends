#pragma once
#include <vector>
#include "_main.hxx"
#include "copy.hxx"
#include "transpose.hxx"
#include "deadEnds.hxx"
#include "pagerank.hxx"
#include "pagerankPlain.hxx"

using std::vector;




// Find pagerank by self-looping dead ends (pull, CSR).
// @param x original graph
// @param q initial ranks (optional)
// @param o options {damping=0.85, tolerance=1e-6, maxIterations=500}
// @returns {ranks, iterations, time}
template <class G, class T=float>
PagerankResult<T> pagerankLoop(const G& x, const vector<T> *q=nullptr, PagerankOptions<T> o={}) {
  T p = o.damping;
  // add self loops
  auto y  = copy(x);
  auto ks = deadEnds(x);
  for (int u : ks)
    y.addEdge(u, u);
  auto yt = transposeWithDegree(y);
  // find plain pagerank
  return pagerankPlain(yt, q, o);
}
