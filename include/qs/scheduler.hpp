#ifndef QS_SCHEDULER_H
#define QS_SCHEDULER_H

#include "cyclic_buffer.hpp"
#include "job.hpp"
#include "vector.hpp"

namespace qs {

template <typename A> class scheduler {
private:
  using worker_t = int(scheduler *);
  pthread_mutex_t work_mtx = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t there_is_work = PTHREAD_COND_INITIALIZER;
  bool stop = false;
  qs::vector<pthread_t> thread_pool;
  static int worker(qs::scheduler<A> *sched) {
    while (true) {
      QS_RETURN_IF_ERR(pthread_mutex_lock(&sched->work_mtx))
      while (!sched->stop && sched->jobs.is_empty()) {
        QS_RETURN_IF_ERR(
            pthread_cond_wait(&sched->there_is_work, &sched->work_mtx))
      }
      auto should_stop = sched->stop;
      // pop job out of the job buffer
      qs::optional<job<A>> j;
      if (!should_stop) {
        j = sched->jobs.pop();
      }
      QS_RETURN_IF_ERR(pthread_mutex_unlock(&sched->work_mtx))
      QS_RETURN_IF_ERR(pthread_cond_signal(&sched->there_is_work))
      if (should_stop)
        break;
      if (!j.is_empty()) {
        QS_RETURN_IF_ERR(pthread_mutex_lock(&sched->working_mtx))
        sched->working++;
        QS_RETURN_IF_ERR(pthread_mutex_unlock(&sched->working_mtx))
        j.get()();
        QS_RETURN_IF_ERR(pthread_mutex_lock(&sched->working_mtx))
        sched->working--;
        QS_RETURN_IF_ERR(pthread_mutex_unlock(&sched->working_mtx))
        pthread_cond_signal(&sched->job_done);
      }
    }
    return 0;
  }
  pthread_mutex_t working_mtx = PTHREAD_MUTEX_INITIALIZER;
  std::size_t working = 0;
  pthread_cond_t job_done = PTHREAD_COND_INITIALIZER;

public:
  concurrent_cyclic_buffer<job<A>> jobs;
  scheduler() = delete;
  explicit scheduler(std::size_t threads_count)
      : thread_pool(threads_count), jobs(256) {
    for (std::size_t i = 0; i < threads_count; i++) {
      this->thread_pool.push(pthread_t{});
      int err = pthread_create(
          &this->thread_pool[i], nullptr,
          reinterpret_cast<void *(*)(void *)>(this->worker), this);
      if (err) {
        throw std::runtime_error(std::strerror(err));
      }
    }
  }
  ~scheduler() {
    this->stop = true;
    pthread_cond_signal(&there_is_work);
    int thread_err = 0;
    for (auto &thread : this->thread_pool) {
      QS_TRACE_ERR(pthread_join(thread, nullptr))
      if (thread_err) {
        std::fprintf(stderr,
                     "A thread returned with error: (errno - %d): '%s'\n",
                     thread_err, std::strerror(thread_err));
      }
      thread_err = 0;
    }
    QS_TRACE_ERR(pthread_mutex_destroy(&this->work_mtx))
    QS_TRACE_ERR(pthread_cond_destroy(&this->there_is_work))
    QS_TRACE_ERR(pthread_mutex_destroy(&this->working_mtx))
    QS_TRACE_ERR(pthread_cond_destroy(&this->job_done))
  }
  int submit_job(job<A> &j) {
    this->jobs.push(j);
    int err;
    if ((err = pthread_cond_signal(&this->there_is_work))) {
      throw std::runtime_error(std::strerror(err));
    }
    return 0;
  }
  int wait_all_finish() {
    QS_RETURN_IF_ERR(pthread_mutex_lock(&this->working_mtx))
    while (this->working || !this->jobs.is_empty()) {
      QS_RETURN_IF_ERR(pthread_cond_wait(&this->job_done, &this->working_mtx))
    }
    QS_RETURN_IF_ERR(pthread_mutex_unlock(&this->working_mtx))
    return 0;
  }
};

} // namespace qs

#endif // QS_SCHEDULER_H
