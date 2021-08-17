#include <vector>
#include <string>
#include <cstdio>
#include <iostream>
#include "src/main.hxx"

using namespace std;




template <class G>
void runPagerank(const G& x, int repeat) {
  vector<float> *init = nullptr;
  PagerankOptions<float> o = {repeat};

  // Find pagerank by teleporting to a random vertex from every dead end.
  auto a1 = pagerankTeleport(x, init, o);
  auto e1 = l1Norm(a1.ranks, a1.ranks);
  printf("[%09.3f ms; %03d iters.] [%.4e err.] pagerankTeleport\n", a1.time, a1.iterations, e1);

  // Find pagerank by self-looping dead ends.
  auto a2 = pagerankLoop(x, init, o);
  auto e2 = l1Norm(a2.ranks, a1.ranks);
  printf("[%09.3f ms; %03d iters.] [%.4e err.] pagerankLoop\n", a2.time, a2.iterations, e2);

  // Find pagerank by self-looping all vertices.
  auto a3 = pagerankLoopAll(x, init, o);
  auto e3 = l1Norm(a3.ranks, a1.ranks);
  printf("[%09.3f ms; %03d iters.] [%.4e err.] pagerankLoopAll\n", a3.time, a3.iterations, e3);

  // Find pagerank by removing dead ends initially, and calculating their ranks after convergence.
  auto a4 = pagerankRemove(x, init, o);
  auto e4 = l1Norm(a4.ranks, a1.ranks);
  printf("[%09.3f ms; %03d iters.] [%.4e err.] pagerankRemove\n", a4.time, a4.iterations, e4);
}


int main(int argc, char **argv) {
  char *file = argv[1];
  int repeat = argc>2? stoi(argv[2]) : 5;
  printf("Loading graph %s ...\n", file);
  auto x = readMtx(file); println(x);
  runPagerank(x, repeat);
  printf("\n");
  return 0;
}
