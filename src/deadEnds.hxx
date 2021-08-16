#pragma once
#include <vector>

using std::vector;




template <class G>
auto deadEnds(const G& x) {
  vector<int> a;
  for (int u : x.vertices())
    if (x.degree(u)==0) a.push_back(u);
  return a;
}
