#include "include/qs/scheduler.hpp"

#include <cstdlib>
#include <pthread.h>

namespace qs {

void *scheduler::worker(void *args) {
  auto sched = reinterpret_cast<qs::scheduler *>(args);

  while (true) {
    QS_RETURN_POINTER_IF_ERR(pthread_mutex_lock(&sched->work_mtx))
    while (!sched->stop && sched->jobs.is_empty()) {
      QS_RETURN_POINTER_IF_ERR(
          pthread_cond_wait(&sched->there_is_work, &sched->work_mtx))
    }
    auto should_stop = sched->stop;
    auto has_work = !sched->jobs.is_empty();
    // pop job out of the job buffer
    job *j;
    if (!should_stop && has_work)
      j = sched->jobs.pop();

    auto more_work = !sched->jobs.is_empty();
    QS_RETURN_POINTER_IF_ERR(pthread_mutex_unlock(&sched->work_mtx))
    if (should_stop || more_work) {
      // notify other threads that something must be done
      QS_RETURN_POINTER_IF_ERR(pthread_cond_signal(&sched->there_is_work))
      if (should_stop)
        break;
    }
    if (j != nullptr) {
      QS_RETURN_POINTER_IF_ERR(pthread_mutex_lock(&sched->working_mtx))
      sched->working++;
      QS_RETURN_POINTER_IF_ERR(pthread_mutex_unlock(&sched->working_mtx))
      j->f(j->args);
      QS_RETURN_POINTER_IF_ERR(pthread_mutex_lock(&sched->working_mtx))
      sched->working--;
      QS_RETURN_POINTER_IF_ERR(pthread_mutex_unlock(&sched->working_mtx))
      pthread_cond_signal(&sched->job_done);
    }
  }
  return nullptr;
}

scheduler::scheduler(std::size_t threads_count)
    : jobs(256), thread_pool(threads_count) {
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
  int thread_err = 0;
  for (auto &thread : this->thread_pool) {
    QS_TRACE_ERR(pthread_join(thread, (void **)&thread_err))
    if (thread_err) {
      std::fprintf(stderr, "A thread returned with error: (errno - %d): '%s'\n",
                   thread_err, std::strerror(thread_err));
    }
    thread_err = 0;
  }
  QS_TRACE_ERR(pthread_mutex_destroy(&this->work_mtx))
  QS_TRACE_ERR(pthread_cond_destroy(&this->there_is_work))
  QS_TRACE_ERR(pthread_mutex_destroy(&this->working_mtx))
  QS_TRACE_ERR(pthread_cond_destroy(&this->job_done))
}

int scheduler::submit_job(job &j) {
  this->jobs.push(j);
  int err;
  if ((err = pthread_cond_signal(&this->there_is_work))) {
    throw std::runtime_error(std::strerror(err));
  }
  return 0;
}

int scheduler::wait_all_finish() {
  QS_RETURN_IF_ERR(pthread_mutex_lock(&this->working_mtx))
  while (this->working) {
    QS_RETURN_IF_ERR(pthread_cond_wait(&this->job_done, &this->working_mtx))
  }
  QS_RETURN_IF_ERR(pthread_mutex_unlock(&this->working_mtx))
  return 0;
}

} // namespace qs
