#ifndef QS_QUEUE_H
#define QS_QUEUE_H

#include "error.h"
#include "list.hpp"
#include "optional.hpp"
#include <iostream>
#include <pthread.h>

namespace qs {

template <class T> class queue {
  linked_list<T> list;

public:
  queue() = default;
  bool empty() const { return list.get_size() == 0; }
  std::size_t size() const { return list.get_size(); }

  void enqueue(const T &item) { list.append(item); }
  void enqueue(T &&item) { list.append(std::move(item)); }

  qs::optional<T> dequeue() {
    auto head = list.head;
    if (head != nullptr) {
      qs::optional<T> ret{std::move(head->get())};
      list.remove(head);
      return ret;
    }
    return qs::optional<T>();
  }

  T *peek() {
    auto head = list.head;
    if (head != nullptr) {
      return &head->get();
    }
    return nullptr;
  }

  T *last() {
    auto tail = list.tail;
    if (tail != nullptr) {
      return &tail->get();
    }
    return nullptr;
  }
};

template <class T> class concurrent_queue {
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
  bool closed = false;
  queue<T> q;

public:
  bool is_closed() { return this->closed; }
  bool enqueue(const T &data) {
    QS_UNWRAP(pthread_mutex_lock(&this->mutex));
    bool previously_empty = q.empty();
    q.enqueue(data);
    QS_UNWRAP(pthread_mutex_unlock(&this->mutex));
    if (previously_empty) {
      QS_UNWRAP(pthread_cond_signal(&this->empty));
    }
    return previously_empty;
  }

  bool enqueue(T &&data) {
    QS_UNWRAP(pthread_mutex_lock(&this->mutex));
    bool previously_empty = q.empty();
    q.enqueue(std::move(data));
    QS_UNWRAP(pthread_mutex_unlock(&this->mutex));
    if (previously_empty) {
      QS_UNWRAP(pthread_cond_signal(&this->empty));
    }
    return previously_empty;
  }

  T *peek() {
    QS_UNWRAP(pthread_mutex_lock(&this->mutex));
    while (q.empty() && !this->closed) {
      QS_UNWRAP(pthread_cond_wait(&this->empty, &this->mutex));
    }
    auto ret = q.peek();
    QS_UNWRAP(pthread_mutex_unlock(&this->mutex));
    return ret;
  }

  qs::optional<T> dequeue(bool *empty_out) {
    QS_UNWRAP(pthread_mutex_lock(&this->mutex));
    while (q.empty() && !this->closed) {
      QS_UNWRAP(pthread_cond_wait(&this->empty, &this->mutex));
    }
    qs::optional<T> ret;
    if (q.empty()) {
      if (empty_out != nullptr)
        *empty_out = true;
    } else {
      auto item = q.dequeue();
      if (empty_out != nullptr)
        *empty_out = q.empty();
      ret = qs::optional<T>{std::move(item)};
      QS_UNWRAP(pthread_cond_signal(&this->empty));
    }
    QS_UNWRAP(pthread_mutex_unlock(&this->mutex));
    return ret;
  }

  void close() {
    QS_UNWRAP(pthread_mutex_lock(&this->mutex));

    if (!this->closed) {
      this->closed = true;
      QS_UNWRAP(pthread_cond_signal(&this->empty));
    }

    QS_UNWRAP(pthread_mutex_unlock(&this->mutex));
  }

  void wait_empty() {
    QS_UNWRAP(pthread_mutex_lock(&this->mutex));
    while (!q.empty()) {
      QS_UNWRAP(pthread_cond_wait(&this->empty, &this->mutex));
    }
    QS_UNWRAP(pthread_mutex_unlock(&this->mutex));
  }
};
} // namespace qs

#endif
