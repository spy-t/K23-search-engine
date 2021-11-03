#ifndef QS_BK_TREE_HPP
#define QS_BK_TREE_HPP

#include <iostream>
#include <qs/functions.hpp>
#include <qs/list.hpp>
#include <qs/optional.hpp>
#include <qs/search.hpp>
#include <qs/skip_list.hpp>

namespace qs {

#define QS_BK_TREE_SKIP_LIST_LEVELS 16

template <typename T> using distance_func = std::function<int(T &a, T &b)>;

template <typename V> class bk_tree_node {
  using node_p = bk_tree_node<V> *;
  using node_list = skip_list<node_p, QS_BK_TREE_SKIP_LIST_LEVELS>;

  static int sl_compare_func(node_p n1, node_p n2) {
    return n1->distance_from_parent - n2->distance_from_parent;
  }

  V data;
  int distance_from_parent{};
  node_list children;

  void add_child(node_p new_child, distance_func<V> dist_func) {
    if (this->children.get_size() > 0) {
      new_child->distance_from_parent = dist_func(this->data, new_child->data);
      optional<node_p> result = this->children.find(new_child);
      if (result.is_empty()) {
        this->children.insert(new_child);
      } else {
        result.get()->add_child(new_child, dist_func);
      }
    } else {
      new_child->distance_from_parent = dist_func(this->data, new_child->data);
      this->children.insert(new_child);
    }
  }

public:
  explicit bk_tree_node(V d)
      : data(d), distance_from_parent(0),
        children(node_list(bk_tree_node::sl_compare_func)) {}

  ~bk_tree_node() {
    functions::for_each(this->children.begin(), this->children.end(),
                        [](node_p curr) { delete curr; });
  }

  V &get() { return this->data; }

  void add_child(V child_data, distance_func<V> dist_func) {
    auto new_child = new bk_tree_node<V>(child_data);
    this->add_child(new_child, dist_func);
  }
#ifdef DEBUG
  const node_list &get_children() { return this->children; }
#endif
};

template <typename T> class bk_tree {
  using node_p = bk_tree_node<T> *;

  distance_func<T> d;
  node_p root;

public:
  friend class bk_tree_node<T>;

  bk_tree(const distance_func<T> df, linked_list<T> *l) : d(df), root(nullptr) {
    functions::for_each(l->begin(), l->end(), [this](list_node<T> &curr) {
      this->insert(curr.get());
    });
  }

  ~bk_tree() { delete this->root; }

  void insert(T data) {
    if (this->root == nullptr) {
      this->root = new bk_tree_node<T>(data);
    } else {
      this->root->add_child(data, this->d);
    }
  }

#ifdef DEBUG
  bk_tree_node<T> *get_root() const { return this->root; }
#endif
};

} // namespace qs

#endif // QS_BK_TREE_HPP
