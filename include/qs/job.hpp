#ifndef QS_SEARCH_JOB_H
#define QS_SEARCH_JOB_H

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
};

} // namespace qs

#endif // QS_SEARCH_JOB_H
