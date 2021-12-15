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
template <typename T, typename Q = T> struct distance_func {
  virtual ~distance_func() = default;
  virtual int operator()(const T &a, const T &b) const = 0;
  virtual int operator()(const T &a, const T &b, int max) const = 0;
  virtual int operator()(const Q &a, const T &b) const = 0;
  virtual int operator()(const Q &a, const T &b, int max) const = 0;
  virtual int operator()(const T &a, const Q &b) const = 0;
  virtual int operator()(const T &a, const Q &b, int max) const = 0;
};

template <typename T, typename Q> class bk_tree;

template <typename T, typename Q = T> class bk_tree_node {
  friend class bk_tree<T, Q>;
  using node_p = bk_tree_node<T, Q> *;
  using node_list = skip_list<node_p, QS_BK_TREE_SKIP_LIST_LEVELS>;

  static int sl_compare_func(const node_p &n1, const node_p &n2) {
    return n1->distance_from_parent - n2->distance_from_parent;
  }

  T data;
  int distance_from_parent{};
  node_list children;

  void add_child(node_p new_child, const distance_func<T, Q> &dist_func) {
    if (this->children.get_size() > 0) {
      new_child->distance_from_parent = dist_func(this->data, new_child->data);
      auto result = this->children.find(new_child);
      if (result == this->children.end()) {
        this->children.insert(new_child);
      } else {
        (*result)->add_child(new_child, dist_func);
      }
    } else {
      new_child->distance_from_parent = dist_func(this->data, new_child->data);
      this->children.insert(new_child);
    }
  }

  void add_child(T child_data, const distance_func<T, Q> &dist_func) {
    auto new_child = new bk_tree_node<T>(child_data);
    this->add_child(new_child, dist_func);
  }

  void match(const distance_func<T, Q> &dist_func, int threshold, Q query,
             int parent_to_query, qs::linked_list<T> &result) {
    int lower_bound = parent_to_query - threshold;
    int upper_bound = parent_to_query + threshold;
    for (auto i = this->children.begin(); i != this->children.end(); i++) {
      int dist = dist_func((*i)->data, query, upper_bound);
      if (dist <= threshold) {
        result.append((*i)->data);
      }

      if ((*i)->distance_from_parent < lower_bound) {
        continue;
      } else if ((*i)->distance_from_parent <= upper_bound) {
        (*i)->match(dist_func, threshold, query, dist, result);
      } else {
        break;
      }
    }
  }

public:
  explicit bk_tree_node(T d)
      : data(d), distance_from_parent(0),
        children(node_list(bk_tree_node::sl_compare_func)) {}

  ~bk_tree_node() {
    functions::for_each(this->children.begin(), this->children.end(),
                        [](node_p curr) { delete curr; });
  }

  T &get() { return this->data; }

#ifdef DEBUG
  const node_list &get_children() { return this->children; }
#endif
};

template <typename T, typename Q = T> class bk_tree {
  using node_p = bk_tree_node<T, Q> *;

  const distance_func<T, Q> *d;
  node_p root;

public:
  friend class bk_tree_node<T, Q>;

  bk_tree() = default;
  explicit bk_tree(const distance_func<T, Q> *d) : d(d), root(nullptr) {}
  template <class Iter>
  explicit bk_tree(Iter begin, Iter end, const distance_func<T, Q> *d)
      : d(d), root(nullptr) {
    while (begin != end) {
      this->insert(*begin);
      begin++;
    }
  }
  template <class Iterable>
  explicit bk_tree(Iterable &it, const distance_func<T, Q> *d)
      : bk_tree(it.begin(), it.end(), d) {}

  ~bk_tree() { delete this->root; }

  void insert(T data) {
    if (this->root == nullptr) {
      this->root = new bk_tree_node<T, Q>(data);
    } else {
      this->root->add_child(data, *d);
    }
  }

  qs::linked_list<T> match(int threshold, Q query) const {
    if (this->root == nullptr) {
      return qs::linked_list<T>();
    }
    auto ret = qs::linked_list<T>();
    int D = (*d)(query, this->root->data);
    if (D <= threshold) {
      ret.append(this->root->data);
    }
    this->root->match(*d, threshold, query, D, ret);
    return ret;
  }

  qs::optional<T> find(Q what) const {
    auto res = this->match(0, what);
    if (res.get_size() != 1) {
      return qs::optional<T>();
    } else {
      return qs::optional<T>(res.head->get());
    }
  }

#ifdef DEBUG
  bk_tree_node<T, Q> *get_root() const { return this->root; }
#endif
};

} // namespace qs

#endif // QS_BK_TREE_HPP
