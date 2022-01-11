#ifndef QS_SEARCH_JOB_H
#define QS_SEARCH_JOB_H

#include <functional>

namespace qs {

struct job {
  std::function<void *(void *args)> f;
  void *args;

  job() {
    this->f = [](void *a) { return a; };
    this->args = nullptr;
  }
  job(std::function<void *(void *args)> f, void *args)
      : f(std::move(f)), args(args) {}

  void *operator()() const {
    return this->f(this->args);
  }
};

} // namespace qs

#endif // QS_SEARCH_JOB_H
