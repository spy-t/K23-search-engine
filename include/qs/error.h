#ifndef QS_ERROR_H
#define QS_ERROR_H

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

#endif
