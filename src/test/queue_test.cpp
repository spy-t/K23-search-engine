#include "catch_amalgamated.hpp"

#include <qs/queue.hpp>

#include <thread>

TEST_CASE("the concurrent queue works as expected", "[queue]") {
  qs::concurrent_queue<int> q;

  std::thread thread_pool[16];
  for (std::size_t i{0}; i < 16; ++i) {
    thread_pool[i] = std::thread(
        [](unsigned int num, qs::concurrent_queue<int> *queue) {
          queue->enqueue(num);
        },
        i, &q);
  }

  for (std::size_t i{0}; i < 16; ++i) {
    thread_pool[i].join();
  }

  int wanted = 0;
  for (std::size_t i{0}; i < 16; ++i) {
    wanted += i;
  }

  bool is_empty = false;
  int got = 0;
  while (!is_empty) {
    got += q.dequeue(&is_empty).get();
  }

  REQUIRE(got == wanted);
}
