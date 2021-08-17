#pragma once
#include <vector>

using std::vector;




template <class G>
bool isDeadEnd(const G& x, int u) {
  return x.degree(u) == 0;
}


template <class G, class F>
void deadEndsForEach(const G& x, F fn) {
  for (int u : x.vertices())
    if (isDeadEnd(x, u)) fn(u);
}


template <class G>
auto deadEnds(const G& x) {
  vector<int> a; deadEndsForEach(x, [&](int u) { a.push_back(u); });
  return a;
}
