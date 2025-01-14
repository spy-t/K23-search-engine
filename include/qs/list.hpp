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

  friend linked_list<V>;
  friend class linked_list<list_node<V>>;
  explicit list_node(const V &value)
      : value(value), next_node(nullptr), prev_node(nullptr) {}
  explicit list_node(V &&value)
      : value(std::move(value)), next_node(nullptr), prev_node(nullptr) {}

  explicit list_node(const V &value, list_node<V> *prev, list_node<V> *next)
      : value(value), next_node(next), prev_node(prev) {}
  explicit list_node(V &&val, list_node<V> *prev, list_node<V> *next)
      : value(std::move(val)), next_node(next), prev_node(prev) {}

public:
  list_node() = delete;

  list_node(const list_node &other) = delete;
  list_node &operator=(const list_node &other) = delete;

  list_node(list_node &&other) noexcept
      : value(std::move(other.value)), next_node(other.next_node),
        prev_node(other.prev_node) {
    other.next_node = nullptr;
    other.prev_node = nullptr;
  }

  list_node &operator=(list_node &&other) noexcept {
    this->value = std::move(other.value);
    this->next_node = other.next_node;
    this->prev_node = other.prev_node;
    other.next_node = nullptr;
    other.prev_node = nullptr;

    return *this;
  }

  list_node<V> *next() const { return next_node; }
  list_node<V> *prev() const { return prev_node; }

  V &get() { return value; }
  V &operator*() { return this->get(); }
  bool operator==(const list_node<V> &other) {
    return this->value == other.value && this->next_node == other.next_node &&
           this->prev_node == other.prev_node;
  }
};

template <class V> class linked_list {
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
  list_node<V> *head;
  list_node<V> *tail;
  std::size_t size;

  linked_list() : head(nullptr), tail(nullptr), size(0) {}

  // No copy
  linked_list(const linked_list<V> &other)
      : head(nullptr), tail(nullptr), size(0) {
    for (auto &item : other) {
      this->append(item->get());
    }
  }
  linked_list<V> &operator=(const linked_list<V> &other) {
    if (this != &other) {
      this->~linked_list();
      auto iter = head;
      while (iter != nullptr) {
        this->append(iter->get());
      }
    }

    return *this;
  }

  linked_list(linked_list<V> &&other) noexcept
      : head(other.head), tail(other.tail), size(other.size) {
    other.head = nullptr;
    other.tail = nullptr;
    other.size = 0;
  }

  linked_list<V> &operator=(linked_list<V> &&other) noexcept {
    this->size = other.size;
    this->head = other.head;
    this->tail = other.tail;
    other.head = nullptr;
    other.tail = nullptr;
    other.size = 0;

    return *this;
  }

  ~linked_list() {
    auto iter = this->head;
    while (iter != nullptr) {
      auto tmp = iter;
      iter = iter->next();
      delete tmp;
    }
  }

  std::size_t get_size() const { return size; }

  list_node<V> &append(const V &value) {
    auto node = new list_node<V>(value, tail, nullptr);
    if (head == nullptr && tail == nullptr) {
      head = node;
      tail = node;
      ++size;
      return *node;
    }
    tail->next_node = node;
    tail = node;

    ++size;
    return *node;
  }

  list_node<V> &append(V &&value) {
    auto node = new list_node<V>(std::move(value), tail, nullptr);
    if (head == nullptr && tail == nullptr) {
      head = node;
      tail = node;
      ++size;
      return *node;
    }
    tail->next_node = node;
    tail = node;

    ++size;
    return *node;
  }

  list_node<V> &append(V value, list_node<V> &node) {
    auto new_node = new list_node<V>(value, std::addressof(node), node.next());
    insert_after(node, *new_node);

    if (node == *tail) {
      tail = new_node;
    }

    ++size;
    return *new_node;
  }
  list_node<V> &prepend(V value, list_node<V> &node) {
    auto new_node =
        new list_node<V>(value, node.prev_node, std::addressof(node));
    insert_before(node, *new_node);

    if (node == *head) {
      head = new_node;
    }

    ++size;
    return *new_node;
  }

  void remove(list_node<V> *node) {
    auto prev = node->prev_node;
    auto next = node->next_node;
    if (prev != nullptr) {
      prev->next_node = next;
    }
    if (next != nullptr) {
      next->prev_node = prev;
    }

    if (head == node) {
      head = next;
    }

    if (tail == node) {
      tail = prev;
    }

    --size;
    delete node;
  }

  struct iterator {
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = V;
    using pointer = value_type *;
    using reference = value_type &;

    list_node<V> *p;

  public:
    explicit iterator(list_node<V> *p) : p(p){};

    reference operator*() { return p->get(); };
    pointer operator->() { return &*this; };
    iterator &operator++() {
      p = p->next_node;
      return *this;
    }
    iterator operator++(int) {
      iterator tmp = *this;
      p = p->next_node;
      return tmp;
    }

    iterator &operator--() {
      p = p->prev_node;
      return *this;
    }
    iterator operator--(int) {
      iterator prev = *this;
      p = p->prev_node;
      return prev;
    }

    friend bool operator==(const iterator &a, const iterator &b) {
      return a.p == b.p;
    };
    friend bool operator!=(const iterator &a, const iterator &b) {
      return a.p != b.p;
    };
  };

  struct reverse_iterator {
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = V;
    using pointer = value_type *;
    using reference = value_type &;

    list_node<V> *p;

  public:
    explicit reverse_iterator(list_node<V> *p) : p(p){};

    reference operator*() { return p->get(); };
    pointer operator->() { return &*this; };
    reverse_iterator &operator++() {
      p = p->prev_node;
      return *this;
    }
    reverse_iterator operator++(int) {
      reverse_iterator prev = *this;
      p = p->prev_node;
      return prev;
    }

    reverse_iterator &operator--() {
      p = p->next_node;
      return *this;
    }
    reverse_iterator operator--(int) {
      reverse_iterator tmp = *this;
      p = p->next_node;
      return tmp;
    }

    friend bool operator==(const reverse_iterator &a,
                           const reverse_iterator &b) {
      return a.p == b.p;
    };
    friend bool operator!=(const reverse_iterator &a,
                           const reverse_iterator &b) {
      return a.p != b.p;
    };
  };

  iterator begin() { return iterator(this->head); };
  iterator end() { return iterator(nullptr); };

  reverse_iterator rbegin() { return reverse_iterator(this->tail); };
  reverse_iterator rend() { return reverse_iterator(nullptr); };
};

} // namespace qs
#endif // QS_LIST_HPP
