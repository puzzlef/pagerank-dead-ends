#pragma once
#include <utility>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include "_main.hxx"
#include "vertices.hxx"

using std::vector;
using std::unordered_set;
using std::max;
using std::make_pair;




// ADJUST-RANKS
// ------------

template <class T, class J>
void adjustRanks(vector<T>& a, const vector<T>& r, J&& ks0, J&& ks1, T radd, T rmul, T rset) {
  auto ksNew = setDifference(ks1, ks0);
  for (int k : ks0)
    a[k] = (r[k]+radd)*rmul;
  for (int k : ksNew)
    a[k] = rset;
}

template <class T, class J>
auto adjustRanks(int N, const vector<T>& r, J&& ks0, J&& ks1, T radd, T rmul, T rset) {
  vector<T> a(N); adjustRanks(a, r, ks0, ks1, radd, rmul, rset);
  return a;
}




// DYNAMIC-VERTICES
// ----------------

template <class G, class F>
void changedVerticesForEach(const G& x, const G& y, F fn) {
  int S = max(x.span(), y.span());
  for (int u=0; u<S; u++) {
    if (!y.hasVertex(u)) continue;
    if (!x.hasVertex(u)) fn(u);
    else if (!verticesEqual(x, u, y, u)) fn(u);
  }
}

template <class G>
auto changedVertices(const G& x, const G& y) {
  vector<int> a; changedVerticesForEach(x, y, [&](int u) { a.push_back(u); });
  return a;
}


template <class G, class F>
void affectedVerticesForEach(const G& x, const G& y, F fn) {
  int S = max(x.span(), y.span()); vector<bool> vis(S);
  changedVerticesForEach(x, y, [&](int u) { dfsDoLoop(vis, y, u, fn); });
}

template <class G>
auto affectedVertices(const G& x, const G& y) {
  vector<int> a; affectedVerticesForEach(x, y, [&](int u) { a.push_back(u); });
  return a;
}


template <class G>
auto dynamicVertices(const G& x, const G& y) {
  vector<int> aff, org; unordered_set<int> affs;
  affectedVerticesForEach(x, y, [&](int u) {
    aff.push_back(u); affs.insert(u);
  });
  for (int u : x.vertices())
    if (affs.count(u)==0) org.push_back(u);
  return make_pair(aff, org)
}
