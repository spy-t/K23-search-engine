#ifndef PLAINLIB_QUEUE_H
#define PLAINLIB_QUEUE_H

#include "list.hpp"
#include <iostream>

namespace pl {

template <class T> class queue {
  linked_list<T> list;

public:
  queue() {}
  bool empty() const { return list.get_size() == 0; }
  std::size_t size() const { return list.get_size(); }
  void enqueue(T item) { list.append(item); }
  // TODO fix this API
  void dequeue(T *r) {
    auto head = list.head_m();
    if (head != nullptr) {
      *r = head->get();
      list.remove(head);
    }
  }

  void peek(T *r) {
    auto head = list.head();
    if (head != nullptr) {
      *r = head->get();
    }
  }
};
} // namespace pl

#endif
