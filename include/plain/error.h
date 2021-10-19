#ifndef QS_ERROR_H
#define QS_ERROR_H

#define QS_TRACE_ERRNO                                                   \
  (std::to_string(errno) + " " + std::string(strerror(errno)) + " " +          \
   __FILE__ + ":" + std::to_string(__LINE__))

#define QS_TRACE_ERR(err)                                                \
  (std::to_string(errno) + " " + std::string(strerror(err)) + " " + __FILE__ + \
   ":" + std::to_string(__LINE__))

#endif
