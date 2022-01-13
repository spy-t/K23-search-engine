#include "include/qs/scheduler.hpp"

#include <cstdlib>
#include <pthread.h>

namespace qs {

void *scheduler::start_worker(qs::worker *w) {
  w->start();
  return nullptr;
}

scheduler::scheduler(std::size_t threads_count)
    : thread_pool{threads_count}, workers{threads_count} {
  for (std::size_t i{0}; i < threads_count; ++i) {
    pthread_t thread_id;
    workers.push(qs::worker{});
    QS_UNWRAP(pthread_create(
        &thread_id, nullptr,
        reinterpret_cast<void *(*)(void *)>(this->start_worker), &workers[i]));
    thread_pool.push(thread_id);
  }
}

void scheduler::wait_all_finish() {
  for (auto &worker : workers) {
    worker.stop();
  }
  for (auto &thread_id : thread_pool) {
    QS_UNWRAP(pthread_join(thread_id, nullptr));
  }
}

} // namespace qs
