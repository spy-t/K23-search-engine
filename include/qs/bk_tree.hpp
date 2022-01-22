#ifndef QS_BK_TREE_HPP
#define QS_BK_TREE_HPP

#include <qs/functions.hpp>
#include <qs/hash_table.hpp>
#include <qs/list.hpp>
#include <qs/optional.hpp>
#include <qs/search.hpp>
#include <qs/skip_list.hpp>
#include <qs/string_view.h>
#include <qs/vector.hpp>

namespace qs {

typedef int (*distance_function)(qs::string_view, qs::string_view);

#define QS_BK_TREE_SKIP_LIST_LEVELS 16

template <typename T> class bk_tree;
template <typename T> class bk_tree_node;

template <typename T> class bk_tree_node {
  friend class bk_tree<T>;
  using node_p = bk_tree_node<T> *;
  using node_list = skip_list<node_p, QS_BK_TREE_SKIP_LIST_LEVELS>;

  static int sl_compare_func(const node_p &n1, const node_p &n2) {
    return n1->distance_from_parent - n2->distance_from_parent;
  }

  T data;
  int distance_from_parent{};
  node_p parent;
  node_list children;

public:
  explicit bk_tree_node(T d, node_p parent)
      : data{d}, distance_from_parent{0}, parent{parent},
        children{node_list(bk_tree_node::sl_compare_func)} {}

  bk_tree_node(bk_tree_node &&other) noexcept {
    functions::for_each(this->children.begin(), this->children.end(),
                        [](node_p curr) { delete curr; });
    this->children = other.children;
    this->parent = other.parent;
  }

  ~bk_tree_node() {
    functions::for_each(this->children.begin(), this->children.end(),
                        [](node_p curr) { delete curr; });
  }

  T &get() { return this->data; }

#ifdef QS_DEBUG
  const node_list &get_children() { return this->children; }
#endif
};

template <typename T> class bk_tree {
  using node_p = bk_tree_node<T> *;

  distance_function dist_func{};
  node_p root;
#ifdef QS_DEBUG
public:
#endif
  std::size_t depth = 0;

public:
  friend class bk_tree_node<T>;

  bk_tree() = default;
  explicit bk_tree(distance_function d) : dist_func(d), root(nullptr) {}
  template <class Iter>
  explicit bk_tree(Iter begin, Iter end, distance_function d)
      : dist_func(d), root(nullptr) {
    while (begin != end) {
      this->insert(*begin);
      begin++;
    }
  }
  template <class Iterable>
  explicit bk_tree(Iterable &it, distance_function d)
      : bk_tree(it.begin(), it.end(), d) {}

  bk_tree(bk_tree &&other) noexcept {
    this->root = std::move(other.root);
    this->dist_func = other.dist_func;
  }
  bk_tree &operator=(bk_tree &&other) noexcept {
    if (this != &other) {
      delete this->root;
      this->root = std::move(other.root);
      this->dist_func = other.dist_func;
    }
    return *this;
  }

  ~bk_tree() { delete this->root; }

  void insert(T data) {
    node_p curr_node = this->root;
    if (curr_node == nullptr) {
      this->root = new bk_tree_node<T>{data, nullptr};
      this->depth++;
      return;
    }
    int distance_from_parent;
    auto new_child = new bk_tree_node<T>{data, nullptr};
    std::size_t local_depth = 1;
    while (true) {
      local_depth++;
      distance_from_parent = dist_func(curr_node->data.get_string_view(),
                                       new_child->data.get_string_view());
      if (curr_node->children.get_size() > 0) {
        new_child->distance_from_parent = distance_from_parent;
        auto res = curr_node->children.find(new_child);
        if (res == curr_node->children.end()) {
          new_child->parent = curr_node;
          curr_node->children.insert(new_child);
          break;
        } else {
          curr_node = *res;
        }
      } else {
        new_child->distance_from_parent = distance_from_parent;
        curr_node->children.insert(new_child);
        break;
      }
    }
    if (this->depth < local_depth) {
      this->depth = local_depth;
    }
  }

public:
  template <typename Q>
  qs::linked_list<T *> match(int threshold, Q query) const {
    qs::linked_list<T *> ret{};
    node_p curr_node;
    int D;
    qs::vector<node_p> stack{this->depth * 2};
    int curr_stack_pos = 0;
    if (this->root == nullptr) {
      return ret;
    } else {
      stack.set(curr_stack_pos++, this->root);
    }

    while (curr_stack_pos > 0) {
      curr_node = stack.at(--curr_stack_pos);
      D = (*dist_func)(curr_node->data.get_string_view(),
                       query.get_string_view());
      if (D <= threshold) {
        ret.append(&curr_node->data);
      }
      int lower_bound = D - threshold;
      int upper_bound = D + threshold;
      for (auto child = curr_node->children.cbegin();
           child != curr_node->children.cend(); child++) {
        if ((*child)->distance_from_parent < lower_bound) {
          continue;
        } else if (child.operator*()->distance_from_parent <= upper_bound) {
          stack.set(curr_stack_pos++, *child);
        } else {
          break;
        }
      }
    }
    return ret;
  }

  template <typename Q> T *find(const Q what) const {
    node_p curr_node;
    int D;
    qs::vector<node_p> stack{this->depth * 2};
    int curr_stack_pos = 0;
    if (this->root == nullptr) {
      return nullptr;
    } else {
      stack.set(curr_stack_pos++, this->root);
    }

    while (curr_stack_pos > 0) {
      curr_node = stack.at(--curr_stack_pos);
      D = (*dist_func)(curr_node->data.get_string_view(), what.get_string_view());
      if (D == 0) {
        return &curr_node->data;
      }
      int lower_bound = D;
      int upper_bound = D;
      for (auto child = curr_node->children.cbegin();
           child != curr_node->children.cend(); child++) {
        if ((*child)->distance_from_parent < lower_bound) {
          continue;
        } else if (child.operator*()->distance_from_parent <= upper_bound) {
          stack.set(curr_stack_pos++, *child);
        } else {
          break;
        }
      }
    }
    return nullptr;
  }

#ifdef QS_DEBUG
  bk_tree_node<T> *get_root() const { return this->root; }
#endif
};

} // namespace qs

#endif // QS_BK_TREE_HPP
