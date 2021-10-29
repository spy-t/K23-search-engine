#ifndef QS_BK_TREE_H
#define QS_BK_TREE_H

#include <qs/list.hpp>
#include <qs/optional.hpp>
#include <qs/vector.hpp>

namespace qs {

template <typename T> using distance_func = int (*)(T a, T b);

template <typename T> class bk_tree;

template <typename T> class bk_tree_node {
  friend class bk_tree<T>;

  vector<bk_tree_node<T>> *children;
  bk_tree_node<T> *parent;

  T data;

public:
  explicit bk_tree_node(T d)
      : data(d), children(nullptr), parent(nullptr) {}

  ~bk_tree_node() {
    if (this->children->get_size() > 0) {
      delete this->children;
    }
  }

  T get() {
    return this->data;
  }

  bk_tree_node<T> *children_nodes() {
    return this->children;
  }

  bk_tree_node<T> *parent_node() {
    return this->parent;
  }
};

template <typename T> class bk_tree {
  distance_func<T> d;
  bk_tree_node<T> *root;

public:
  bk_tree(distance_func<T> df, linked_list<T> *l) : d(df) {
    list_node<T> *curr_list_node = l->head();
    while (curr_list_node != nullptr) {
      this->insert(curr_list_node->get());
      curr_list_node = curr_list_node->next();
    }
  }

  // TODO test if this creates leaks
  ~bk_tree() {
    delete this->root;
  }

  // TODO
  void insert(T data) {

  }

  // TODO
  optional<T> lookup(T data) {
    return optional<T>();
  }
};

} // namespace qs

#endif // QS_BK_TREE_H
