//
// Created by coder on 16-9-8.
//


#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "mem_manager.h"

using std::vector;

struct Point {
  Point(int x, int y) : x(x), y(y) {}

  int x, y;
};

TEST_CASE("test small object pool", "[Small Object Pool]") {
  constexpr int kMaxIter = 35;

  SmallObjPool<Point, 10> pool;
  vector<Point *> points;

  for (int i = 0; i < kMaxIter; ++i) {
    auto p = pool.Create(i, i + 1);
    REQUIRE(p);
    points.push_back(p);
  }

  for (int i = 0; i < kMaxIter; ++i) {
    auto p = points[i];
    REQUIRE(p->x == i);
    REQUIRE(p->y == i + 1);
  }

  for (int i = kMaxIter / 2; i < kMaxIter; ++i) {
    auto p = points[i];
    // pool.Destroy(p);
    points.pop_back();
  }

  for (int i = kMaxIter / 2; i < kMaxIter; ++i) {
    auto p = pool.Create(i, i + 1);
    REQUIRE(p);
    points.push_back(p);
  }

  for (int i = 0; i < kMaxIter; ++i) {
    auto p = points[i];
    REQUIRE(p->x == i);
    REQUIRE(p->y == i + 1);
  }
}

TEST_CASE("test multi times", "[multi Pool]") {
  for (int times = 0; times < 100; ++times) {
    SmallObjPool<int> pool;
    for (int i = 0; i < 1000; ++i) {
      pool.Create();
    }
  }
}
