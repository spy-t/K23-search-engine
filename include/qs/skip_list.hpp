#ifndef QS_SKIP_LIST_HPP
#define QS_SKIP_LIST_HPP

#include <cstdint>
#include <ctime>
#include <random>
#include <type_traits>

namespace qs {

template <class T, std::size_t L> class skip_list;

template <class T, std::size_t L> class skip_list {
  using sl_compare_func = typename std::add_pointer<int(T, T)>::type;

  std::size_t levels = L;
  std::size_t size;
  std::mt19937_64 rng;

  sl_compare_func cmp;

  template <class V> class skip_list_node {
    skip_list_node<V> *next;
    skip_list_node<V> *prev;
    skip_list_node<V> *top;
    skip_list_node<V> *bottom;

    T data;

    friend skip_list;

  public:
    skip_list_node(V data)
        : next(nullptr), prev(nullptr), top(nullptr), bottom(nullptr),
          data(data) {}
    skip_list_node(V data, skip_list_node<V> *prev, skip_list_node<V> *next,
                   skip_list_node<V> *bottom, skip_list_node<V> *top)
        : data(data), prev(prev), next(next), bottom(bottom), top(top) {}
  };

  skip_list_node<T> *nodes[L];

  int random_level() {
    std::uniform_int_distribution<int> dist(0, levels - 1);
    return dist(rng);
  }

  void insert_with_path(T data, skip_list_node<T> *(&descent_path)[L]) {
    int height = random_level();
    ++size;

    skip_list_node<T> *bottom = nullptr;

    for (std::size_t i = 0; i <= (std::size_t)height; ++i) {
      skip_list_node<T> *n = new skip_list_node<T>(data);
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

  void insert_node(T data, skip_list_node<T> *(&descent_path)[L],
                   skip_list_node<T> *start_node, std::size_t level) {
    if (start_node == nullptr && level != 0) {
      descent_path[level] = nullptr;
      return insert_node(data, descent_path, nodes[level - 1], level - 1);
    }

    skip_list_node<T> *n = start_node;

    while (n != nullptr) {
      int cmp_res = cmp(n->data, data);
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

  skip_list_node<T> *find_node(T data, skip_list_node<T> *start,
                               std::size_t level) {
    if (start == nullptr && level != 0) {
      return find_node(data, nodes[level - 1], level - 1);
    }

    skip_list_node<T> *n = start;

    while (n != nullptr) {
      int cmp_res = cmp(n->data, data);
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
  [[maybe_unused]] explicit skip_list(sl_compare_func fn)
      : size(0), rng(std::time(nullptr)), cmp(fn) {
    for (std::size_t i = 0; i < levels; ++i) {
      nodes[i] = nullptr;
    }
  }

  ~skip_list() {
    for (std::size_t i = 0; i < levels; ++i) {
      skip_list_node<T> *n = nodes[i];
      while (n != nullptr) {
        auto tmp = n;
        n = n->next;
        delete tmp;
      }
    }
  }

  void insert(T data) {
    if (nodes[0] == nullptr) {
      int height = random_level();
      for (std::size_t i = 0; i <= (std::size_t)height; ++i) {
        auto node = new skip_list_node<T>(data);
        nodes[i] = node;
        if (i != 0) {
          node->bottom = nodes[i - 1];
          node->bottom->top = node;
        }
      }
      ++size;
      return;
    }

    skip_list_node<T> *descent_path[L];
    insert_node(data, descent_path, nodes[levels - 1], levels - 1);
  }

  optional<T> find(T data) {
    skip_list_node<T> *n = find_node(data, nodes[levels - 1], levels - 1);
    return n != nullptr ? optional(n->data) : optional<T>();
  }

  void remove(T data) {
    skip_list_node<T> *n = find_node(data, nodes[levels - 1], levels - 1);
    if (n == nullptr) {
      return;
    }

    // Go to the bottom level
    skip_list_node<T> *b = n->bottom;
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
};
} // namespace qs

#endif
