#ifndef QS_JOB_H
#define QS_JOB_H

namespace qs {

struct job {
  virtual ~job() {}
  virtual void operator()() {}
};

} // namespace qs

#endif // QS_JOB_H
