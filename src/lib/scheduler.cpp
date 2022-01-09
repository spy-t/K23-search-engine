#include "include/qs/scheduler.hpp"

#include <cstdlib>
#include <pthread.h>

namespace qs {

#define QS_WORKER_CHECK_ERR(err)                                               \
  if (err) {                                                                   \
    auto ret = new int(err);                                                   \
    return ret;                                                                \
  }

void *scheduler::worker(void *args) {
  auto sched = reinterpret_cast<qs::scheduler *>(args);

  while (true) {
    QS_WORKER_CHECK_ERR(pthread_mutex_lock(&sched->work_mtx))
    while (!sched->stop && sched->jobs.is_empty()) {
      QS_WORKER_CHECK_ERR(
          pthread_cond_wait(&sched->there_is_work, &sched->work_mtx))
    }
    auto should_stop = sched->stop;
    auto has_work = !sched->jobs.is_empty();
    // pop job out of the job buffer
    job *j;
    if (!should_stop && has_work)
      j = sched->jobs.pop();

    auto more_work = !sched->jobs.is_empty();
    QS_WORKER_CHECK_ERR(pthread_mutex_unlock(&sched->work_mtx))
    if (should_stop || more_work) {
      // notify other threads that something must be done
      QS_WORKER_CHECK_ERR(pthread_cond_signal(&sched->there_is_work))
      if (should_stop)
        break;
    }
    if (j != nullptr)
      j->f(j->args);
  }
  return nullptr;
}

scheduler::scheduler(std::size_t threads_count) : jobs(256), thread_pool(32) {
  for (std::size_t i = 0; i < threads_count; i++) {
    this->thread_pool.push(pthread_t{});
    int err =
        pthread_create(&this->thread_pool[i], nullptr, this->worker, this);
    if (err) {
      throw std::runtime_error(std::strerror(err));
    }
  }
}

scheduler::~scheduler() {
  this->stop = true;
  pthread_cond_signal(&there_is_work);
  int err;
  int thread_err = 0;
  for (auto &thread : this->thread_pool) {
    if ((err = pthread_join(thread, (void **)&thread_err))) {
      std::fprintf(stderr, "Pthread join: error: '%s'\n", std::strerror(err));
    } else if (thread_err) {
      std::fprintf(stderr, "A thread returned with error: (errno - %d): '%s'\n",
                   thread_err, std::strerror(thread_err));
    }
    thread_err = 0;
  }
}

void scheduler::submit_job(job &j) {
  this->jobs.push(j);
  int err;
  if ((err = pthread_cond_signal(&this->there_is_work))) {
    throw std::runtime_error(std::strerror(err));
  }
}

} // namespace qs
