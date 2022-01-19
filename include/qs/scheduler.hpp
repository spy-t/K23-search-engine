#ifndef QS_SCHEDULER_H
#define QS_SCHEDULER_H

#include <qs/cyclic_buffer.hpp>
#include <qs/job.h>
#include <qs/queue.hpp>
#include <qs/vector.hpp>

namespace qs {

class worker {
  concurrent_queue<qs::job *> queue;

public:
  void enqueue(job *j) { queue.enqueue(j); }

  void start() {
    while (true) {
      auto job = queue.peek();
      auto is_empty = false;
      if (queue.is_closed()) {
        break;
      }
      if (job != nullptr) {
        (**job)();
        delete *job;
        queue.dequeue(is_empty);
      }
    }
  }

  void stop() { queue.close(); }

  void wait_done() { queue.wait_empty(); }
};

class scheduler {
private:
  qs::vector<pthread_t> thread_pool;
  qs::vector<worker> workers;
  std::size_t current_worker = 0;

  static void *start_worker(qs::worker *sched);

public:
  scheduler() = delete;
  explicit scheduler(std::size_t threads_count);

  ~scheduler() {
    for (auto &worker : workers) {
      worker.stop();
    }
    for (auto &thread_id : thread_pool) {
      pthread_join(thread_id, nullptr);
    }
  }

  void submit_job(job *j) {
    workers[current_worker].enqueue(j);
    current_worker = (current_worker + 1) % workers.get_size();
  }
  void wait_all_finish();
};

} // namespace qs

#endif // QS_SCHEDULER_H
