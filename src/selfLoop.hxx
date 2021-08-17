#pragma once
#include "copy.hxx"




template <class G, class F>
void selfLoopTo(G& a, F fn) {
  for (int u : a.vertices())
    if (fn(u)) a.addEdge(u, u);
}

template <class G, class F>
auto selfLoop(const G& x, F fn) {
  auto a = copy(x); selfLoopTo(a, fn);
  return a;
}
