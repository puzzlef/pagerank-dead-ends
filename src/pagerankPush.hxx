#pragma once
#include <algorithm>
#include "_main.hxx"
#include "pagerank.hxx"

using std::swap;




template <class C, class G, class T>
void pagerankPushOnce(C& a, const C& r, const G& x, T p) {
  int N = x.order();
  fill(a, (1-p)/N);
  for (int u : x.vertices()) {
    int d = x.degree(u);
    if (d > 0) addValueAt(a, p*r[u]/d, x.edges(u));
    else addValue(a, p*r[u]/N);
  }
}

template <class C, class G, class T>
int pagerankPushLoop(C& a, C& r, const G& x, T p, T E, int L) {
  int l = 0;
  T e0 = T();
  for (; l<L; l++) {
    pagerankPushOnce(a, r, x, p);
    T e1 = absError(a, r);
    if (e1<E || e1==e0) break;
    swap(a, r);
    e0 = e1;
  }
  return l;
}

template <class C, class G, class T>
int pagerankPushCore(C& a, C& r, const G& x, const C *q, T p, T E, int L) {
  int N = x.order();
  if (q) copy(r, *q);
  else fill(r, T(1)/N);
  return pagerankPushLoop(a, r, x, p, E, L);
}


// Find pagerank by pushing contribution to out-vertices.
// @param x original graph
// @param q initial ranks (optional)
// @param o options {damping=0.85, tolerance=1e-6, maxIterations=500}
// @returns {ranks, iterations, time}
template <class G, class T=float>
PagerankResult<T> pagerankPush(const G& x, const vector<T> *q=nullptr, PagerankOptions<T> o={}) {
  T    p = o.damping;
  T    E = o.tolerance;
  int  L = o.maxIterations, l;
  auto a = x.vertexContainer(T());
  auto r = x.vertexContainer(T());
  float t = measureDuration([&]() { l = pagerankPushCore(a, r, x, q, p, E, L); }, o.repeat);
  fillAt(a, T(), x.nonVertices());
  return {a, l, t};
}
