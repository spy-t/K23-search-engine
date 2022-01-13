#ifndef QS_ERROR_H
#define QS_ERROR_H

#include <cerrno>
#include <cstring>

#define QS_TRACE_ERR(op)                                                       \
  do {                                                                         \
    int err = (op);                                                            \
    if (err) {                                                                 \
      fprintf(stderr, "Error @ %s:%d : operation: '%s', error: %d - %s",       \
              __FILE__, __LINE__, #op, err, std::strerror(err));               \
    }                                                                          \
  } while (false);

#define QS_RETURN_IF_ERR(op)                                                   \
  do {                                                                         \
    int err = (op);                                                            \
    if (err) {                                                                 \
      fprintf(stderr, "Error @ %s:%d : operation: '%s', error: %d - %s",       \
              __FILE__, __LINE__, #op, err, std::strerror(err));               \
      return err;                                                              \
    }                                                                          \
  } while (false);

#define QS_RETURN_POINTER_IF_ERR(op)                                           \
  do {                                                                         \
    int err = (op);                                                            \
    if (err) {                                                                 \
      fprintf(stderr, "Error @ %s:%d : operation: '%s', error: %d - %s",       \
              __FILE__, __LINE__, #op, err, std::strerror(err));               \
      auto ret = new int(err);                                                 \
      return ret;                                                              \
    }                                                                          \
  } while (false);

#define QS_UNWRAP(op)                                                          \
  do {                                                                         \
    int err__ = (op);                                                          \
    if (err__ != 0) {                                                          \
      char buffer[512];                                                        \
      sprintf(buffer, "Error @ %s:%d : operation : %s, error: %d - %s",        \
              __FILE__, __LINE__, #op, err__, strerror(err__));                \
      throw std::runtime_error(buffer);                                        \
    }                                                                          \
  } while (false)

#endif
