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
  queue() {}
  bool empty() const { return list.get_size() == 0; }
  std::size_t size() const { return list.get_size(); }
  void enqueue(T item) { list.append(item); }
  // TODO fix this API
  void dequeue(T *r) {
    auto head = list.head;
    if (head != nullptr) {
      *r = head->get();
      list.remove(head);
    }
  }

  void peek(T *r) {
    auto head = list.head;
    if (head != nullptr) {
      *r = head->get();
    }
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

  qs::optional<T> dequeue(bool &empty_out) {
    QS_UNWRAP(pthread_mutex_lock(&this->mutex));
    while (q.empty() && !this->closed) {
      QS_UNWRAP(pthread_cond_wait(&this->empty, &this->mutex));
    }
    qs::optional<T> ret;
    if (q.empty()) {
      empty_out = true;
    } else {
      T item;
      q.dequeue(&item);
      empty_out = q.empty();
      ret = qs::optional<T>{std::move(item)};
    }
    QS_UNWRAP(pthread_mutex_unlock(&this->mutex));
    return ret;
  }

  void close() {
    QS_UNWRAP(pthread_mutex_lock(&this->mutex));

    if (!this->closed) {
      this->closed = true;
      QS_UNWRAP(pthread_cond_broadcast(&this->empty));
    }

    QS_UNWRAP(pthread_mutex_unlock(&this->mutex));
  }

  bool is_empty() {
    QS_UNWRAP(pthread_mutex_lock(&this->mutex));
    bool ret = q.empty();
    QS_UNWRAP(pthread_mutex_unlock(&this->mutex));
    return ret;
  }
};
} // namespace qs

#endif
