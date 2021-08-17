#include <cmath>
#include <string>
#include <vector>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <utility>
#include "src/main.hxx"

using namespace std;




template <class G, class T>
void printRow(const G& x, const PagerankResult<T>& a, const PagerankResult<T>& b, const char *tec) {
  auto e = l1Norm(b.ranks, a.ranks);
  print(x); printf(" [%09.3f ms; %03d iters.] [%.4e err.] %s\n", b.time, b.iterations, e, tec);
}

void runPagerankBatch(const string& data, int repeat, int skip, int batch) {
  vector<float> r0, r1, r2, r3;
  vector<float> s0, s1, s2, s3;
  vector<float> *init = nullptr;
  PagerankOptions<float> o = {repeat};

  DiGraph<> x;
  stringstream s(data);
  while (true) {
    // Skip some edges (to speed up execution)
    if (skip>0 && !readSnapTemporal(x, s, skip)) break;
    auto ksOld = vertices(x);
    auto a0 = pagerankTeleport(x, init, o);
    auto a1 = pagerankLoop(x, init, o);
    auto a2 = pagerankLoopAll(x, init, o);
    auto a3 = pagerankRemove(x, init, o);
    auto r0 = move(a0.ranks);
    auto r1 = move(a1.ranks);
    auto r2 = move(a2.ranks);
    auto r3 = move(a3.ranks);

    // Read edges for this batch.
    auto y = copy(x);
    if (!readSnapTemporal(y, s, batch)) break;
    auto ks = vertices(y);
    s0.resize(y.span());
    s1.resize(y.span());
    s2.resize(y.span());
    s3.resize(y.span());

    // Adjust ranks.
    adjustRanks(s0, r0, ksOld, ks, 0.0f, float(ksOld.size())/ks.size(), 1.0f/ks.size());
    adjustRanks(s1, r1, ksOld, ks, 0.0f, float(ksOld.size())/ks.size(), 1.0f/ks.size());
    adjustRanks(s2, r2, ksOld, ks, 0.0f, float(ksOld.size())/ks.size(), 1.0f/ks.size());
    adjustRanks(s3, r3, ksOld, ks, 0.0f, float(ksOld.size())/ks.size(), 1.0f/ks.size());

    // Find pagerank by teleporting to a random vertex from every dead end.
    auto b0 = pagerankTeleport(y, init, o);
    printRow(y, b0, b0, "pagerankTeleport (static)");
    auto c0 = pagerankTeleport(y, &s0, o);
    printRow(y, b0, c0, "pagerankTeleport (incremental)");
    auto d0 = pagerankTeleportDynamic(x, y, &s0, o);
    printRow(y, b0, d0, "pagerankTeleport (dynamic)");

    // Find pagerank by self-looping dead ends.
    auto b1 = pagerankLoop(y, init, o);
    printRow(y, b1, b1, "pagerankLoop (static)");
    auto c1 = pagerankLoop(y, &s1, o);
    printRow(y, b1, c1, "pagerankLoop (incremental)");
    auto d1 = pagerankLoopDynamic(x, y, &s1, o);
    printRow(y, b1, d1, "pagerankLoop (dynamic)");

    // Find pagerank by self-looping all vertices.
    auto b2 = pagerankLoopAll(y, init, o);
    printRow(y, b2, b2, "pagerankLoopAll (static)");
    auto c2 = pagerankLoopAll(y, &s2, o);
    printRow(y, b2, c2, "pagerankLoopAll (incremental)");
    auto d2 = pagerankLoopAllDynamic(x, y, &s2, o);
    printRow(y, b2, d2, "pagerankLoopAll (dynamic)");

    // Find pagerank by removing dead ends initially, and calculating their ranks after convergence.
    auto b3 = pagerankRemove(y, init, o);
    printRow(y, b3, b3, "pagerankRemove (static)");
    auto c3 = pagerankRemove(y, &s3, o);
    printRow(y, b3, c3, "pagerankRemove (incremental)");
    auto d3 = pagerankRemoveDynamic(x, y, &s3, o);
    printRow(y, b3, d3, "pagerankRemove (dynamic)");

    // New graph is now old.
    x = move(y);
  }
}


void runPagerank(const string& data, int repeat) {
  int M = countLines(data), steps = 100;
  printf("Temporal edges: %d\n", M);
  for (int batch=10, i=0; batch<M; batch*=i&1? 2:5, i++) {
    int skip = max(M/steps - batch, 0);
    printf("\n# Batch size %.0e\n", (double) batch);
    runPagerankBatch(data, repeat, skip, batch);
  }
}


int main(int argc, char **argv) {
  char *file = argv[1];
  int repeat = argc>2? stoi(argv[2]) : 5;
  printf("Using graph %s ...\n", file);
  string d = readFile(file);
  runPagerank(d, repeat);
    printf("\n");
  return 0;
}
