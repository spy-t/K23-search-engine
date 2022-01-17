#ifndef QS_JOB_H
#define QS_JOB_H

namespace qs {

struct job {
  void *(*fun)(void *f);
  void *args;
};

} // namespace qs

#endif // QS_JOB_H
