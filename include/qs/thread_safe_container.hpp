#ifndef QS_THREAD_SAFE_CONTAINER_H
#define QS_THREAD_SAFE_CONTAINER_H

#include <pthread.h>

namespace qs {

template <typename T> class thread_safe_container {
  T data;
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

public:
  explicit thread_safe_container(T &&data) : data(std::move(data)) {}
  thread_safe_container() : data() {}

  thread_safe_container &operator=(thread_safe_container &&other) noexcept {
    if (this != &other) {
      this->data = std::move(other.data);
    }
    return *this;
  }

  ~thread_safe_container() { pthread_mutex_destroy(&this->mutex); }

  T *lock() {
    pthread_mutex_lock(&this->mutex);
    return &this->data;
  }

  void unlock() { pthread_mutex_unlock(&this->mutex); }

  T *get_data() { return &this->data; }
};
} // namespace qs

#endif // QS_THREAD_SAFE_CONTAINER_H
