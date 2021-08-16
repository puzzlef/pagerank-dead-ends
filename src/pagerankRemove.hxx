#pragma once
#include <vector>
#include "_main.hxx"
#include "copy.hxx"
#include "transpose.hxx"
#include "deadEnds.hxx"
#include "pagerank.hxx"
#include "pagerankPlain.hxx"

using std::vector;




template <class G, class H, class J, class T>
void pagerankRemoveCalculate(vector<T>& a, const G& y, const H& yt, const J& ks, T p) {
  int N = y.order();
  T  c0 = (1-p)/N;
  for (int u : ks) {
    a[u] = c0;
    for (int v : yt.edges(u))
      a[u] += (p/y.degree(v)) * a[v];
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
  // remove dead ends
  auto y  = copy(x);
  auto ks = deadEnds(x);
  for (int u : ks)
    y.removeVertex(u);
  auto yt = transposeWithDegree(y);
  // find plain pagerank
  auto a  = pagerankPlain(yt, q, o);
  // calculate ranks of dead ends
  a.time += measureDuration([&] { pagerankRemoveCalculate(a.ranks, y, yt, ks, p); }, o.repeat);
  multiplyValue(a.ranks, T(1)/sum(a.ranks));
  a.iterations++;
  return a;
}
