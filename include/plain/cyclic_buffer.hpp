#ifndef PLAINLIB_CYCLIC_BUFFER_HPP
#define PLAINLIB_CYCLIC_BUFFER_HPP

#include "error.h"
#include <cstdint>
#include <cstring>
#include <errno.h>
#include <pthread.h>
#include <stdexcept>
#include <string>

namespace pl {
template <class T> class cyclic_buffer {
  std::size_t size;
  std::size_t capacity;
  T *buffer;
  std::size_t read;
  std::size_t write;

  std::size_t pos_increment(std::size_t pos) const {
    return (pos + 1) % capacity;
  }
  std::size_t pos_decrement(T *pos) const {
    return (pos + capacity - 1) % capacity;
  }

public:
  explicit cyclic_buffer(std::size_t size)
      : size(0), capacity(size + 1), buffer(new T[size + 1]), read(0),
        write(0) {}

  ~cyclic_buffer() { delete[] buffer; };

  bool is_empty() const { return write == read; }

  bool is_full() const { return pos_increment(write) == read; }

  // This cyclic buffer overwrites. If there is a need for discarding or maybe
  // blocking it should be implemented in a subclass
  bool push(T elem) {
    buffer[write] = elem;
    write = pos_increment(write);
    size++;
    return true;
  }

  T *pop() {
    if (is_empty()) {
      return nullptr;
    }

    T *ret = &buffer[read];
    read = pos_increment(read);
    size--;
    return ret;
  }
};

// Thread safe cyclic buffer
template <class T> class concurrent_cyclic_buffer : public cyclic_buffer<T> {
  using cb = cyclic_buffer<T>;

  pthread_mutex_t write_lock = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t read_lock = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t is_full_cond = PTHREAD_COND_INITIALIZER;

public:
  ~concurrent_cyclic_buffer() {
    pthread_mutex_destroy(&read_lock);
    pthread_mutex_destroy(&write_lock);
    pthread_cond_destroy(&is_full_cond);
  }
  concurrent_cyclic_buffer(std::size_t size) : cyclic_buffer<T>(size) {}
  bool push(T elem) {
    int ret;
    if ((ret = pthread_mutex_lock(&write_lock)) != 0) {
      throw std::runtime_error(PLAINLIB_TRACE_ERR(ret));
    }
    if (cb::is_full()) {
      // Wait for the buffer to have an empty spot
      if ((ret = pthread_cond_wait(&is_full_cond, &write_lock)) != 0) {
        throw std::runtime_error(PLAINLIB_TRACE_ERR(ret));
      }
    }
    bool r = cb::push(elem);
    pthread_mutex_unlock(&write_lock);
    return r;
  }

  T *pop() {
    int ret;
    if ((ret = pthread_mutex_lock(&read_lock)) != 0) {
      throw std::runtime_error(PLAINLIB_TRACE_ERR(ret));
    }
    bool should_signal = cb::is_full();
    T *elem = cb::pop();
    if (should_signal) {
      // Wake up a waiting writer to write to the buffer
      if ((ret = pthread_cond_signal(&is_full_cond)) != 0) {
        throw std::runtime_error(PLAINLIB_TRACE_ERR(ret));
      }
    }
    pthread_mutex_unlock(&read_lock);
    return elem;
  }
};
} // namespace pl
#endif
