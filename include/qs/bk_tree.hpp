#ifndef QS_BK_TREE_HPP
#define QS_BK_TREE_HPP

#include <iostream>
#include <qs/functions.hpp>
#include <qs/list.hpp>
#include <qs/optional.hpp>
#include <qs/search.hpp>
#include <qs/skip_list.hpp>
#include <qs/string_view.h>
#include <qs/vector.hpp>
#include <qs/pair.hpp>
#include <qs/hash_table.hpp>

namespace qs {

typedef int (*distance_function)(qs::string_view, qs::string_view, int);

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
  node_list children;
  qs::hash_table<qs::pair<qs::string>,int>* cache;

  void add_child(node_p new_child, const distance_function &dist_func) {
    auto string_pair = pair(this->data.get_string_view().copy(),new_child->data.get_string_view().copy());
    auto iter = cache->lookup(string_pair);
    int n_distance_from_parent;

    if (iter != cache->end()){
      n_distance_from_parent = *iter;
    } else{
      n_distance_from_parent = dist_func(
          this->data.get_string_view(), new_child->data.get_string_view(),
          std::numeric_limits<int>::max());
      cache->insert(string_pair,n_distance_from_parent);
    }
    if (this->children.get_size() > 0) {
      new_child->distance_from_parent = n_distance_from_parent;
      auto result = this->children.find(new_child);
      if (result == this->children.end()) {
        this->children.insert(new_child);
      } else {
        (*result)->add_child(new_child, dist_func);
      }
    } else {
      new_child->distance_from_parent = n_distance_from_parent;
      this->children.insert(new_child);
    }
  }

  void add_child(T child_data, const distance_function &dist_func) {
    auto new_child = new bk_tree_node<T>(child_data,cache);
    this->add_child(new_child, dist_func);
  }

  template <typename Q>
  void match(const distance_function &df, int threshold, Q query,
             int parent_to_query, qs::linked_list<T *> &result) {
    int lower_bound = parent_to_query - threshold;
    int upper_bound = parent_to_query + threshold;
    for (auto i = this->children.begin(); i != this->children.end(); i++) {
      int dist;
      auto string_pair = pair((*i)->data.get_string_view().copy(),query.get_string_view().copy());
      auto iter = cache->lookup(string_pair);
      if (iter != this->cache->end()){
        dist = *iter;
      } else {
        dist = df((*i)->data.get_string_view(), query.get_string_view(),
               upper_bound);
        this->cache.insert(string_pair,dist);
      }

      if (dist <= threshold) {
        result.append(&(*i)->data);
      }

      if ((*i)->distance_from_parent < lower_bound) {
        continue;
      } else if ((*i)->distance_from_parent <= upper_bound) {
        (*i)->match(df, threshold, query, dist, result);
      } else {
        break;
      }
    }
  }

public:
  explicit bk_tree_node(T d,qs::hash_table<qs::pair<qs::string>,int>* cache)
      : data(d), distance_from_parent(0),
        children(node_list(bk_tree_node::sl_compare_func)), cache(cache) {}

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

  distance_function dist_func;
  node_p root;
  qs::hash_table<qs::pair<qs::string>,int> cache;

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

  ~bk_tree() { delete this->root; }

  void insert(T data) {
    if (this->root == nullptr) {
      this->root = new bk_tree_node<T>(data,&cache);
    } else {
      this->root->add_child(data, dist_func);
    }
  }

  template <typename Q>
  qs::linked_list<T *> match(int threshold, const Q query) const {
    if (this->root == nullptr) {
      return qs::linked_list<T *>();
    }
    auto ret = qs::linked_list<T *>();
    int D;
    auto string_pair = pair(this->root->data.get_string_view().copy(),query.get_string_view().copy());
    auto iter = cache.lookup(string_pair);
    if (iter != this->cache.end()){
      D = *iter;
    } else {
      D = (*dist_func)(this->root->data.get_string_view(),
                   query.get_string_view(), std::numeric_limits<int>::max());
      this->cache.insert(string_pair,D);
    }

    if (D <= threshold) {
      ret.append(&this->root->data);
    }
    this->root->match(dist_func, threshold, query, D, ret);
    return ret;
  }

  template <typename Q> qs::optional<T *> find(const Q what) const {
    auto res = this->match(0, what);
    if (res.get_size() != 1) {
      return qs::optional<T *>();
    } else {
      return qs::optional<T *>(res.head->get());
    }
  }

#ifdef QS_DEBUG
  bk_tree_node<T> *get_root() const { return this->root; }
#endif
};

} // namespace qs

#endif // QS_BK_TREE_HPP
