#ifndef QS_BK_TREE_HPP
#define QS_BK_TREE_HPP

#include <iostream>
#include <qs/functions.hpp>
#include <qs/list.hpp>
#include <qs/optional.hpp>
#include <qs/search.hpp>
#include <qs/skip_list.hpp>
#include <qs/vector.hpp>

namespace qs {

#define QS_BK_TREE_SKIP_LIST_LEVELS 16

template <typename T> using distance_func = std::function<int(T &a, T &b)>;
template <typename T> class bk_tree;

template <typename V> class bk_tree_node {
  friend class bk_tree<V>;
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

  void add_child(V child_data, distance_func<V> dist_func) {
    auto new_child = new bk_tree_node<V>(child_data);
    this->add_child(new_child, dist_func);
  }

  void match(const distance_func<V> dist_func, int threshold, V query,
             int parent_to_query, qs::vector<V> &result) {
    qs::functions::for_each(
        this->children.begin(), this->children.end(),
        [dist_func, &threshold, &query, &parent_to_query, &result](node_p n) {
          int dist = dist_func(n->data, query);
          if (dist <= threshold) {
            result.push(n->data);
          }
          if (parent_to_query - threshold <= n->distance_from_parent &&
              n->distance_from_parent <= parent_to_query + threshold) {
            n->match(dist_func, threshold, query, dist, result);
          }
        });
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

  explicit bk_tree(const distance_func<T> d) : d(d), root(nullptr) {}

  ~bk_tree() { delete this->root; }

  void insert(T data) {
    if (this->root == nullptr) {
      this->root = new bk_tree_node<T>(data);
    } else {
      this->root->add_child(data, this->d);
    }
  }

  qs::vector<T> match(int threshold, T query) const {
    if (this->root == nullptr) {
      return qs::vector<T>(0);
    }
    auto ret = qs::vector<T>();
    int D = this->d(query, this->root->data);
    if (D <= threshold) {
      ret.push(this->root->data);
    }
    this->root->match(this->d, threshold, query, D, ret);
    return ret;
  }

#ifdef DEBUG
  bk_tree_node<T> *get_root() const { return this->root; }
#endif
};

} // namespace qs

#endif // QS_BK_TREE_HPP
