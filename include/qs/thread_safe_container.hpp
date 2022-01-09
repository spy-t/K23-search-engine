#ifndef QS_THREAD_SAFE_CONTAINER_H
#define QS_THREAD_SAFE_CONTAINER_H

#include <pthread.h>

namespace qs {

template <typename T> class thread_safe_container {
private:
  T data;
  std::size_t readers = 0;
  bool writing = false;
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t read_cond = PTHREAD_COND_INITIALIZER;
  pthread_cond_t write_cond = PTHREAD_COND_INITIALIZER;

  int lock_unlock_for_read(bool lock) {
    QS_RETURN_IF_ERR(pthread_mutex_lock(&this->mutex))
    while (this->writing) {
      QS_RETURN_IF_ERR(pthread_cond_wait(&this->read_cond, &this->mutex))
    }
    if (lock) {
      this->readers++;
    } else if (this->readers > 0) {
      this->readers--;
    }
    auto can_write = this->readers == 0;
    QS_RETURN_IF_ERR(pthread_mutex_unlock(&this->mutex))
    if (can_write)
      QS_RETURN_IF_ERR(pthread_cond_signal(&this->write_cond))
    return 0;
  }

  int lock_unlock_for_write(bool lock) {
    QS_RETURN_IF_ERR(pthread_mutex_lock(&this->mutex))
    while ((lock && this->writing) || this->readers > 0) {
      QS_RETURN_IF_ERR(pthread_cond_wait(&this->write_cond, &this->mutex))
    }
    this->writing = lock;
    QS_RETURN_IF_ERR(pthread_mutex_unlock(&this->mutex));
    if (!lock)
      QS_RETURN_IF_ERR(pthread_cond_signal(&this->read_cond))
    return 0;
  }

public:
  explicit thread_safe_container(T &&data) : data(std::move(data)) {}
  thread_safe_container() : data() {}

  thread_safe_container &operator=(thread_safe_container &&other) noexcept {
    if (this != &other) {
      this->data = std::move(other.data);
    }
    return *this;
  }

  ~thread_safe_container() {
    pthread_mutex_destroy(&this->mutex);
    pthread_cond_destroy(&this->read_cond);
    pthread_cond_destroy(&this->write_cond);
  }

  T *lock_for_read() {
    if (this->lock_unlock_for_read(true))
      return nullptr;
    return &this->data;
  }

  void unlock_for_read() { this->lock_unlock_for_read(false); }

  T *lock_for_write() {
    if (this->lock_unlock_for_write(true))
      return nullptr;
    return &this->data;
  }

  void unlock_for_write() { this->lock_unlock_for_write(false); }

  T *get_data() {
    return &this->data;
  }
};
} // namespace qs

#endif // QS_THREAD_SAFE_CONTAINER_H
