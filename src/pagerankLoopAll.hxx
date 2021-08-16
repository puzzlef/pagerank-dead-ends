#pragma once
#include <vector>
#include "_main.hxx"
#include "copy.hxx"
#include "transpose.hxx"
#include "pagerank.hxx"
#include "pagerankPlain.hxx"

using std::vector;




// Find pagerank by self-looping all vertices (pull, CSR).
// @param x original graph
// @param q initial ranks (optional)
// @param o options {damping=0.85, tolerance=1e-6, maxIterations=500}
// @returns {ranks, iterations, time}
template <class G, class T=float>
PagerankResult<T> pagerankLoopAll(const G& x, const vector<T> *q=nullptr, PagerankOptions<T> o={}) {
  T p = o.damping;
  // add self loops
  auto y  = copy(x);
  for (int u : x.vertices())
    y.addEdge(u, u);
  auto yt = transposeWithDegree(y);
  // find plain pagerank
  return pagerankPlain(yt, q, o);
}
