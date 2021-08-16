#pragma once
#include <vector>

using std::vector;




template <class G, class F>
void deadEndsForEach(const G& x, F fn) {
  for (int u : x.vertices())
    if (x.degree(u)==0) fn(u);
}


template <class G>
auto deadEnds(const G& x) {
  vector<int> a; deadEndsForEach(x, [&](int u) { a.push_back(u); });
  return a;
}
