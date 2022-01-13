#ifndef QS_SCHEDULER_H
#define QS_SCHEDULER_H

#include "cyclic_buffer.hpp"
#include "job.hpp"
#include "queue.hpp"
#include "vector.hpp"

namespace qs {

// Some support code from http://stackoverflow.com/questions/42124866
template <class RetVal, class T, class... Args>
std::function<RetVal(Args...)> get_function_type(RetVal (T::*)(Args...) const);

template <class RetVal, class T, class... Args>
std::function<RetVal(Args...)> get_function_type(RetVal (T::*)(Args...));

template <class RetVal, class... Args>
std::function<RetVal(Args...)> get_function_type(RetVal (*)(Args...));

class worker {
  concurrent_queue<std::function<void()>> queue;

public:
  template <class Fun, class... Args>
  void enqueue(Fun &&fn, Args&&... args) {
    queue.enqueue(std::bind(std::move(fn), std::move(args...)));
  }

  void start() {
    while (true) {
      bool is_empty;
      auto job = queue.dequeue(is_empty);
      if (queue.is_closed()) {
        break;
      }
      if (!job.is_empty()) {
        job.get().operator()();
      }
    }
  }

  void stop() { queue.close(); }
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
  ~scheduler() = default;

  template <class Fun, class... Args>
  void submit_job(Fun &&fn, Args &&...args) {
    workers[current_worker].enqueue(std::move(fn), std::move(args...));
    current_worker = (current_worker + 1) % workers.get_size();
  }
  void wait_all_finish();
};

} // namespace qs

#endif // QS_SCHEDULER_H
