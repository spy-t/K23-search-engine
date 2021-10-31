#ifndef QS_LIST_HPP
#define QS_LIST_HPP
#include <cstdlib>
#include <exception>
#include <qs/optional.hpp>
#include <stdexcept>
#include <utility>

namespace qs {

template <class V> class linked_list;

template <class V> class list_node {
  V value;
  list_node<V> *next_node;
  list_node<V> *prev_node;
  linked_list<V> *list;

  friend class linked_list<V>;

  explicit list_node(const V value, linked_list<V> *list)
      : value(value), next_node(nullptr), prev_node(nullptr), list(list) {
    if (list == nullptr) {
      throw std::runtime_error(
          "can't instantiate a list node without an associated list");
    }
  }
  explicit list_node(const V value, linked_list<V> *list, list_node<V> *prev,
                     list_node<V> *next)
      : value(value), next_node(next), prev_node(prev), list(list) {
    if (list == nullptr) {
      throw std::runtime_error(
          "can't instantiate a list node without an associated list");
    }
  }

  bool is_part_of(linked_list<V> *other_list) { return list == other_list; }

public:
  list_node() = delete;
  list_node(const list_node<V> &other) = delete;
  list_node<V> &operator=(const list_node<V> &other) = delete;

  const list_node<V> *next() const { return next_node; }
  const list_node<V> *prev() const { return prev_node; }

  // non const variants
  list_node<V> *next_m() const { return next_node; }
  list_node<V> *prev_m() const { return prev_node; }

  V get() const { return value; }
};

template <class V> class linked_list {
  list_node<V> *head_node;
  list_node<V> *tail_node;
  std::size_t size;

  // Assumes the new node's pointers have been set in the constructor
  void insert_before(list_node<V> &node, list_node<V> &new_node) {
    auto prev = node.prev_node;
    if (prev != nullptr) {
      prev->next_node = &new_node;
    }
    node.prev_node = &new_node;
  }

  void insert_after(list_node<V> &node, list_node<V> &new_node) {
    auto next = node.next_node;
    if (next != nullptr) {
      next->prev_node = &new_node;
    }
    node.next_node = &new_node;
  }

  list_node<V> *_find(list_node<V> *starting_node, V data) {
    auto curr_node = starting_node;
    while (curr_node != nullptr && curr_node->get() != data) {
      curr_node = curr_node->next();
    }
    return curr_node;
  }

public:
  linked_list() : head_node(nullptr), tail_node(nullptr), size(0) {}

  // No copy
  linked_list(const linked_list<V> &other) = delete;
  linked_list<V> &operator=(const linked_list<V> &other) = delete;

  ~linked_list() {
    auto iter = head_node;
    while (iter != nullptr) {
      auto tmp = iter;
      iter = iter->next_node;
      delete tmp;
    }
  }

  const list_node<V> *head() const { return head_node; }
  const list_node<V> *tail() const { return tail_node; }

  // non const variants
  list_node<V> *head_m() const { return head_node; }
  list_node<V> *tail_m() const { return tail_node; }

  std::size_t get_size() const { return size; }

  list_node<V> *append(const V value) {
    auto node = new list_node<V>(value, this, tail_node, nullptr);
    if (head_node == nullptr && tail_node == nullptr) {
      head_node = node;
      tail_node = node;
      ++size;
      return node;
    }
    tail_node->next_node = node;
    tail_node = node;

    ++size;
    return node;
  }

  list_node<V> *append(V value, list_node<V> *node) {
    if (node == nullptr) {
      throw std::invalid_argument("can't append after a null node");
    }
    if (!node->is_part_of(this)) {
      throw std::runtime_error(
          "can't append after a node that is not a member of this list");
    }
    auto new_node = new list_node<V>(value, this, node, node->next_node);
    insert_after(*node, *new_node);

    if (node == tail_node) {
      tail_node = new_node;
    }

    ++size;
    return new_node;
  }
  list_node<V> *prepend(V value, list_node<V> *node) {
    if (node == nullptr) {
      throw std::invalid_argument("can't append after a null node");
    }
    if (!node->is_part_of(this)) {
      throw std::runtime_error(
          "can't append after a node that is not a member of this list");
    }
    auto new_node = new list_node<V>(value, this, node->prev_node, node);
    insert_before(*node, *new_node);

    if (node == head_node) {
      head_node == new_node;
    }

    ++size;
    return new_node;
  }

  void remove(list_node<V> *node) {
    if (!node->is_part_of(this)) {
      throw std::runtime_error(
          "can't remove a node that is not a member of this list");
    }
    auto prev = node->prev_node;
    auto next = node->next_node;
    if (prev != nullptr) {
      prev->next_node = next;
    }
    if (next != nullptr) {
      next->prev_node = prev;
    }

    if (head_node == node) {
      head_node = next;
    }

    if (tail_node == node) {
      tail_node = prev;
    }

    --size;
    delete node;
  }

  qs::optional<V> find(V data) {
    list_node<V> *ln = this->_find(this->head_node, data, false);
    return ln != nullptr ? optional(ln->get()) : optional<V>();
  }
};

} // namespace qs
#endif // QS_LIST_HPP
