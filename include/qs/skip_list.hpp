#ifndef QS_SKIP_LIST_HPP
#define QS_SKIP_LIST_HPP

#include <cstdint>
#include <ctime>
#include <qs/list.hpp>
#include <qs/optional.hpp>
#include <random>
#include <type_traits>

namespace qs {

template <class T, std::size_t L> class skip_list;

template <class T, std::size_t L> class skip_list {
  class skip_list_node;
  using sl_compare_func =
      typename std::add_pointer<int(const T &, const T &)>::type;

  std::size_t levels = L;
  std::size_t size;
  std::mt19937_64 rng;

  sl_compare_func cmp;
  skip_list_node *nodes[L] = {0};
  qs::linked_list<T> data_list;

  class skip_list_node {
    skip_list_node *next = nullptr;
    skip_list_node *prev = nullptr;
    skip_list_node *top = nullptr;
    skip_list_node *bottom = nullptr;

    list_node<T> *data_ptr = nullptr;

    friend skip_list;

  public:
    skip_list_node(list_node<T> *data)
        : next(nullptr), prev(nullptr), top(nullptr), bottom(nullptr),
          data_ptr(data) {}

    skip_list_node(list_node<T> *data, skip_list_node *prev,
                   skip_list_node *next, skip_list_node *bottom,
                   skip_list_node *top)
        : data_ptr(data), prev(prev), next(next), bottom(bottom), top(top) {}

    T &operator*() { return data_ptr->get(); }
  };

  int random_level() {
    std::uniform_int_distribution<int> dist(0, levels - 1);
    return dist(rng);
  }

  void insert_with_path(list_node<T> *data,
                        skip_list_node *(&descent_path)[L]) {
    int height = random_level();
    ++size;

    skip_list_node *bottom = nullptr;

    for (std::size_t i = 0; i <= (std::size_t)height; ++i) {
      skip_list_node *n = new skip_list_node(data);
      n->prev = descent_path[i];

      // Head edge case
      if (descent_path[i] == nullptr) {
        if (nodes[i] != nullptr) {
          n->next = nodes[i];
          nodes[i]->prev = n;
        }
        nodes[i] = n;
      } else {
        n->next = descent_path[i]->next;
        if (descent_path[i]->next != nullptr) {
          descent_path[i]->next->prev = n;
        }
        descent_path[i]->next = n;
      }

      n->bottom = bottom;
      if (bottom != nullptr) {
        bottom->top = n;
      }
      bottom = n;
    }
  }

  void insert_node(list_node<T> *data, skip_list_node *(&descent_path)[L],
                   skip_list_node *start_node, std::size_t level) {
    if (start_node == nullptr && level != 0) {
      descent_path[level] = nullptr;
      return insert_node(data, descent_path, nodes[level - 1], level - 1);
    }

    skip_list_node *n = start_node;

    while (n != nullptr) {
      int cmp_res =
          n->data_ptr != nullptr ? cmp(n->data_ptr->get(), data->get()) : 1;
      if (cmp_res == 0) {
        // Update not supported
        return;
      } else if (cmp_res > 0 && level != 0) {
        descent_path[level] = n->prev;
        if (n->prev != nullptr) {
          return insert_node(data, descent_path, n->prev->bottom, level - 1);
        } else {
          return insert_node(data, descent_path, nodes[level - 1], level - 1);
        }
      } else if (cmp_res < 0 && level != 0) {
        // If we are at the end of the current layer
        if (n->next == nullptr) {
          descent_path[level] = n;
          return insert_node(data, descent_path, n->bottom, level - 1);
        }
      } else if (cmp_res > 0 && level == 0) {
        // We found the position we need to insert into
        descent_path[0] = n->prev;
        return insert_with_path(data, descent_path);
      } else if (n->next == nullptr && level == 0) {
        // We arrived at the end of the list.
        descent_path[0] = n;
        return insert_with_path(data, descent_path);
      }
      n = n->next;
    }
  }

  skip_list_node *find_node(T &data, skip_list_node *start, std::size_t level) {
    if (start == nullptr && level != 0) {
      return find_node(data, nodes[level - 1], level - 1);
    }

    skip_list_node *n = start;

    while (n != nullptr) {
      int cmp_res = n->data_ptr != nullptr ? cmp(n->data_ptr->get(), data) : 1;
      if (cmp_res == 0) {
        return n;
      } else if (cmp_res > 0 && level != 0) {
        if (n->prev != nullptr) {
          return find_node(data, n->prev->bottom, level - 1);
        } else {
          return find_node(data, nodes[level - 1], level - 1);
        }
      } else if (cmp_res < 0) {
        if (level != 0) {
          // If we are at the end of the current layer
          if (n->next == nullptr) {
            return find_node(data, n->bottom, level - 1);
          }
        }
      }
      n = n->next;
    }

    return nullptr;
  }

public:
  struct iterator;
  explicit skip_list(sl_compare_func fn)
      : size(0), rng(std::time(nullptr)), cmp(fn) {
    for (std::size_t i = 0; i < levels; ++i) {
      nodes[i] = nullptr;
    }
  }

  // Copy operations do not preserve the same level structure to save time
  skip_list(const skip_list &other) : cmp(other.cmp) {
    auto iter = other.data_list.head;
    while (iter != nullptr) {
      this->insert(iter->get());
      iter = iter->next();
    }
  }

  // Copy operations do not preserve the same level structure to save time
  skip_list &operator=(const skip_list &other) {
    if (*this != other) {
      cmp = other.cmp;
      auto iter = other.data_list.head;
      while (iter != nullptr) {
        this->insert(iter->get());
        iter = iter->next();
      }
    }

    return *this;
  }

  ~skip_list() {
    for (std::size_t i = 0; i < levels; ++i) {
      skip_list_node *n = nodes[i];
      while (n != nullptr) {
        auto tmp = n;
        n = n->next;
        delete tmp;
      }
    }
  }

  void insert(T &data) {
    auto &ln = data_list.append(data);
    if (nodes[0] == nullptr) {
      int height = random_level();
      for (std::size_t i = 0; i <= (std::size_t)height; ++i) {
        auto node = new skip_list_node(std::addressof(ln));
        nodes[i] = node;
        if (i != 0) {
          node->bottom = nodes[i - 1];
          node->bottom->top = node;
        }
      }
      ++size;
      return;
    }

    skip_list_node *descent_path[L];
    insert_node(std::addressof(ln), descent_path, nodes[levels - 1],
                levels - 1);
  }

  void insert(T &&data) {
    auto &ln = data_list.append(std::move(data));
    if (nodes[0] == nullptr) {
      int height = random_level();
      for (std::size_t i = 0; i <= (std::size_t)height; ++i) {
        auto node = new skip_list_node(std::addressof(ln));
        nodes[i] = node;
        if (i != 0) {
          node->bottom = nodes[i - 1];
          node->bottom->top = node;
        }
      }
      ++size;
      return;
    }

    skip_list_node *descent_path[L];
    insert_node(std::addressof(ln), descent_path, nodes[levels - 1],
                levels - 1);
  }

  iterator find(T &data) {
    skip_list_node *n = find_node(data, nodes[levels - 1], levels - 1);
    return iterator(n);
  }

  void remove(T &data) {
    skip_list_node *n = find_node(data, nodes[levels - 1], levels - 1);
    remove(iterator(n));
  }

  void remove(iterator iter) {
    skip_list_node *n = iter->curr;
    if (n == nullptr) {
      return;
    }
    data_list.remove(n->data_ptr);

    // Go to the bottom level
    skip_list_node *b = n->bottom;
    while (b != nullptr) {
      n = b;
      b = n->bottom;
    }

    std::size_t i = 0;
    while (n != nullptr) {
      auto pn = n;
      if (n->prev != nullptr) {
        n->prev->next = n->next;
      }
      if (n->next != nullptr) {
        n->next->prev = n->prev;
      }

      // Head edge case
      if (nodes[i] == n) {
        nodes[i] = n->next;
      }

      n = n->top;

      delete pn;
      ++i;
    }
    --size;
  }

  std::size_t get_size() { return size; }

  struct iterator {
    friend class skip_list<T, L>;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T *;
    using reference = T &;

    skip_list_node *curr;
    skip_list_node *prev;

  public:
    explicit iterator(skip_list_node *start) : curr(start), prev(nullptr){};

    iterator(const iterator &other) : curr(other.curr), prev(other.prev) {}
    iterator &operator=(const iterator &other) {
      if (this != other) {
        this->curr = other.curr;
        this->prev = other.prev;
      }
      return *this;
    }

    reference operator*() const { return curr->data_ptr->get(); };
    pointer operator->() const { return &(curr->data_ptr->get()); };
    iterator &operator++() {
      prev = curr;
      curr = curr->next;
      return *this;
    };
    iterator operator++(int) {
      iterator tmp = *this;
      prev = curr;
      curr = curr->next;
      return tmp;
    }
    iterator operator--() {
      curr = prev;
      prev = curr->prev;
      return *this;
    }
    iterator operator--(int) {
      iterator tmp = *this;
      curr = prev;
      prev = curr->prev;
      return tmp;
    }
    friend bool operator==(const iterator &a, const iterator &b) {
      return a.curr == b.curr;
    };
    friend bool operator!=(const iterator &a, const iterator &b) {
      return a.curr != b.curr;
    };
  };

  iterator begin() {
    return this->size == 0 ? iterator(nullptr) : iterator(this->nodes[0]);
  };
  iterator end() { return iterator(nullptr); };
};
} // namespace qs

#endif
