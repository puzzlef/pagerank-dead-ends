#pragma once
#include <vector>
#include <algorithm>
#include "_main.hxx"
#include "vertices.hxx"
#include "edges.hxx"
#include "csr.hxx"
#include "transpose.hxx"
#include "dynamic.hxx"
#include "pagerank.hxx"

using std::vector;
using std::swap;




template <class T>
void pagerankFactor(vector<T>& a, const vector<int>& vdata, int i, int n, T p) {
  for (int u=i; u<i+n; u++) {
    int d = vdata[u];
    a[u] = d>0? p/d : 0;
  }
}


template <class T>
void pagerankCalculate(vector<T>& a, const vector<T>& c, const vector<int>& vfrom, const vector<int>& efrom, int i, int n, T c0) {
  for (int v=i; v<i+n; v++)
    a[v] = c0 + sumAt(c, sliceIter(efrom, vfrom[v], vfrom[v+1]));
}


template <class T>
int pagerankPlainLoop(vector<T>& a, vector<T>& r, vector<T>& c, const vector<T>& f, const vector<int>& vfrom, const vector<int>& efrom, const vector<int>& vdata, int i, int n, int N, T p, T E, int L) {
  T c0 = (1-p)/N;
  int l = 1;
  for (; l<L; l++) {
    if (l==1) multiply(c, r, f, 0, N);  // 1st time, find contrib for all
    else      multiply(c, r, f, i, n);  // nth time, only those that changed
    pagerankCalculate(a, c, vfrom, efrom, i, n, c0);  // only changed
    T el = l1Norm(a, r, 0, N);  // full error check, partial can be done too (i, n)
    if (el < E) break;
    swap(a, r);
  }
  return l;
}


template <class H, class J, class FL, class T=float>
PagerankResult<T> pagerankPlainCore(const H& xt, const J& ks, int i, int n, FL fl, const vector<T> *q, PagerankOptions<T> o) {
  int  N = xt.order();
  T    p = o.damping;
  T    E = o.tolerance;
  int  L = o.maxIterations, l = 0;
  auto vfrom = sourceOffsets(xt, ks);
  auto efrom = destinationIndices(xt, ks);
  auto vdata = vertexData(xt, ks);
  vector<T> a(N), r(N), c(N), f(N);
  float t = measureDurationMarked([&](auto mark) {
    if (q) r = compressContainer(xt, *q, ks);
    else fill(r, T(1)/N);
    multiplyValue(r, r, T(1)/coalesce(sum(r), T(1)));  // must sum to one (remove)!
    copy(a, r);  // copy old ranks
    if (N==0 || n==0) return;  // skip if nothing to do!
    mark([&] { pagerankFactor(f, vdata, 0, N, p); });
    mark([&] { l = fl(a, r, c, f, vfrom, efrom, vdata, i, n, N, p, E, L); });  // with full error check, partial can be done too (E*n/N)
  }, o.repeat);
  return {decompressContainer(xt, a, ks), l, t};
}




// Find pagerank using a single thread (pull, CSR).
// @param x original graph
// @param q initial ranks (optional)
// @param o options {damping=0.85, tolerance=1e-6, maxIterations=500}
// @returns {ranks, iterations, time}
template <class G, class FL, class T=float>
PagerankResult<T> pagerankPlain(const G& x, FL fl, const vector<T> *q=nullptr, PagerankOptions<T> o={}) {
  int  N  = x.order();
  auto xt = transposeWithDegree(x);
  return pagerankPlainCore(xt, xt.vertices(), 0, N, fl, q, o);
}

template <class G, class T=float>
PagerankResult<T> pagerankPlain(const G& x, const vector<T> *q=nullptr, PagerankOptions<T> o={}) {
  return pagerankPlain(x, pagerankPlainLoop<T>, q, o);
}


template <class G, class FD, class FL, class T=float>
PagerankResult<T> pagerankPlainDynamic(const G& x, const G& y, FD fd, FL fl, const vector<T> *q=nullptr, PagerankOptions<T> o={}) {
  auto [ks, n] = fd(x, y);
  auto yt = transposeWithDegree(y);
  return pagerankPlainCore(yt, ks, 0, n, fl, q, o);
}

template <class G, class T=float>
PagerankResult<T> pagerankPlainDynamic(const G& x, const G& y, const vector<T> *q=nullptr, PagerankOptions<T> o={}) {
  auto xt = transposeWithDegree(x);
  auto yt = transposeWithDegree(y);
  auto [ks, n] = dynamicVertices(x, xt, y, yt);
  return pagerankPlainCore(yt, ks, 0, n, pagerankPlainLoop<T>, q, o);
}
