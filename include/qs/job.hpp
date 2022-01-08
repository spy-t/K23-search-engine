#ifndef QS_SEARCH_JOB_H
#define QS_SEARCH_JOB_H

#include <functional>

namespace qs {

struct job {
  std::function<void *(void * args)> f;
  void *args;
};

} // namespace qs

#endif // QS_SEARCH_JOB_H
