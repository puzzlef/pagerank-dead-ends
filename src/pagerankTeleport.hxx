#pragma once
#include <utility>
#include <vector>
#include <algorithm>
#include "_main.hxx"
#include "dynamic.hxx"
#include "deadEnds.hxx"
#include "pagerank.hxx"
#include "pagerankPlain.hxx"

using std::vector;
using std::swap;
using std::make_pair;
using std::any_of;




template <class T>
T pagerankTeleportContribution(const vector<T>& r, const vector<int>& vfrom, const vector<int>& efrom, const vector<int>& vdata, int N, T p) {
  T a = (1-p)/N;
  for (int u=0; u<N; u++)
    if (vdata[u]==0) a += p*r[u]/N;
  return a;
}


template <class T>
int pagerankTeleportLoop(vector<T>& a, vector<T>& r, vector<T>& c, const vector<T>& f, const vector<int>& vfrom, const vector<int>& efrom, const vector<int>& vdata, int i, int n, int N, T p, T E, int L) {
  int l = 1;
  for (; l<L; l++) {
    if (l==1) multiply(c, r, f, 0, N);  // 1st time, find contrib for all
    else      multiply(c, r, f, i, n);  // nth time, only those that changed
    T c0 = pagerankTeleportContribution(r, vfrom, efrom, vdata, N, p); // all vertices needed!
    pagerankCalculate(a, c, vfrom, efrom, i, n, c0);  // only changed
    T el = l1Norm(a, r, 0, N);  // full error check, partial can be done too (i, n)
    if (el < E) break;
    swap(a, r);
  }
  return l;
}


template <class G>
auto pagerankTeleportDynamicVertices(const G& x, const G& y) {
  auto [ks, n] = dynamicVertices(x, y);
  auto fd = [&](int u) { return isDeadEnd(x, u) || isDeadEnd(y, u); };
  if (any_of(ks.begin(), ks.begin()+n, fd)) n = ks.size();
  return make_pair(ks, n);
}




// Find pagerank by teleporting to a random vertex from every dead end (pull, CSR).
// @param x orginal graph
// @param q initial ranks (optional)
// @param o options {damping=0.85, tolerance=1e-6, maxIterations=500}
// @returns {ranks, iterations, time}
template <class G, class T=float>
PagerankResult<T> pagerankTeleport(const G& x, const vector<T> *q=nullptr, PagerankOptions<T> o={}) {
  return pagerankPlain(x, pagerankTeleportLoop<T>, q, o);
}


template <class G, class T=float>
PagerankResult<T> pagerankTeleportDynamic(const G& x, const G& y, const vector<T> *q=nullptr, PagerankOptions<T> o={}) {
  auto fd = [&](const auto& x, const auto& y) { return pagerankTeleportDynamicVertices(x, y); };
  return pagerankPlainDynamic(x, y, fd, pagerankTeleportLoop<T>, q, o);
}
