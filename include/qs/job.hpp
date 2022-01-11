#ifndef QS_SEARCH_JOB_H
#define QS_SEARCH_JOB_H

#include <functional>

namespace qs {

template <typename T> struct job {
  using job_t = void *(*)(T args);
  job_t f;
  T args;

  job() = default;
  job(job_t f, T args) : f(f), args(args) {}
  job(const job &other) = default;
  job(job &other) = default;
  job(job &&other) noexcept = default;
  job &operator=(const job &other) = default;

  void *operator()() const { return this->f(this->args); }
};

} // namespace qs

#endif // QS_SEARCH_JOB_H
