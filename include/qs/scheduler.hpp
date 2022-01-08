#ifndef QS_SCHEDULER_H
#define QS_SCHEDULER_H

#include "cyclic_buffer.hpp"
#include "job.hpp"
#include "vector.hpp"

namespace qs {

class scheduler {
private:
  using worket_t = void *(void *);
  pthread_mutex_t work_mtx = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t there_is_work = PTHREAD_COND_INITIALIZER;
  bool stop = false;
  concurrent_cyclic_buffer<job> jobs;
  qs::vector<pthread_t> thread_pool;
  static worket_t worker;

public:
  scheduler() = delete;
  explicit scheduler(std::size_t threads_count);
  ~scheduler();
  void submit_job(job &j);
};

} // namespace qs

#endif // QS_SCHEDULER_H
