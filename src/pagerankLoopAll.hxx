#pragma once
#include <vector>
#include "_main.hxx"
#include "selfLoop.hxx"
#include "deadEnds.hxx"
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
  auto xl = selfLoop(x, [&](int u) { return true; });
  return pagerankPlain(xl, q, o);
}
