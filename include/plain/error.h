#ifndef PLAINLIB_ERROR_H
#define PLAINLIB_ERROR_H

#define PLAINLIB_TRACE_ERRNO                                                   \
  (std::to_string(errno) + " " + std::string(strerror(errno)) + " " +          \
   __FILE__ + ":" + std::to_string(__LINE__))

#define PLAINLIB_TRACE_ERR(err)                                                \
  (std::to_string(errno) + " " + std::string(strerror(err)) + " " + __FILE__ + \
   ":" + std::to_string(__LINE__))

#endif
