#pragma once
#include <vector>
#include <unordered_set>
#include "_main.hxx"
#include "copy.hxx"
#include "transpose.hxx"
#include "deadEnds.hxx"
#include "pagerank.hxx"
#include "pagerankPlain.hxx"

using std::vector;
using std::unordered_set;




template <class G, class H, class J, class T>
void pagerankRemoveCalculate(vector<T>& a, const G& x, const H& xt, const J& ks, T p) {
  int N = x.order();
  T  c0 = (1-p)/N;
  for (int u : ks) {
    a[u] = c0;
    for (int v : xt.edges(u))
      a[u] += (p/x.degree(v)) * a[v];
  }
}




// Find pagerank by removing dead ends initially, and calculating their ranks after convergence (pull, CSR).
// @param x original graph
// @param q initial ranks (optional)
// @param o options {damping=0.85, tolerance=1e-6, maxIterations=500}
// @returns {ranks, iterations, time}
template <class G, class T=float>
PagerankResult<T> pagerankRemove(const G& x, const vector<T> *q=nullptr, PagerankOptions<T> o={}) {
  T p = o.damping;
  auto xr  = copy(x, [&](int u) { return !isDeadEnd(x, u); });
  auto a   = pagerankPlain(xr, q, o);
  auto xrt = transposeWithDegree(xr);
  auto ks  = deadEnds(x);
  a.time += measureDuration([&] { pagerankRemoveCalculate(a.ranks, xr, xrt, ks, p); }, o.repeat);
  multiplyValue(a.ranks, a.ranks, T(1)/sum(a.ranks));
  a.iterations++;
  return a;
}


template <class G, class T=float>
PagerankResult<T> pagerankRemoveDynamic(const G& x, const G& y, const vector<T> *q=nullptr, PagerankOptions<T> o={}) {
  T p = o.damping;
  auto xr  = copy(x, [&](int u) { return !isDeadEnd(x, u); });
  auto yr  = copy(y, [&](int u) { return !isDeadEnd(y, u); });
  auto a   = pagerankPlainDynamic(xr, yr, q, o);
  auto yrt = transposeWithDegree(y);
  auto ks  = deadEnds(y);
  a.time += measureDuration([&] { pagerankRemoveCalculate(a.ranks, yr, yrt, ks, p); }, o.repeat);
  multiplyValue(a.ranks, a.ranks, T(1)/sum(a.ranks));
  a.iterations++;
  return a;
}
