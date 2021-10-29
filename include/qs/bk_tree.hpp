#ifndef QS_BK_TREE_HPP
#define QS_BK_TREE_HPP

#include <qs/list.hpp>
#include <qs/optional.hpp>
#include <qs/vector.hpp>
#include <qs/search.hpp>

namespace qs {

template <typename T> using distance_func = std::function<int(T a, T b)>;

template <typename T> class bk_tree;

template <typename T> class bk_tree_node {
  friend class bk_tree<T>;

  T data;
  vector<bk_tree_node<T> *> *children;
  bk_tree_node<T> *parent;
  int distance_from_parent;

public:
  explicit bk_tree_node(T d, vector<bk_tree_node<T> *> *children = nullptr,
                        bk_tree_node<T> *parent = nullptr,
                        int distance_from_parent = 0)
      : data(d), children(children), parent(parent),
        distance_from_parent(distance_from_parent) {}

  ~bk_tree_node() {
    if (this->children->get_size() > 0) {
      delete this->children;
    }
  }

  friend bool operator<(const bk_tree_node<T> n1, const bk_tree_node<T> n2) {
    return n1.distance_from_parent < n2.distance_from_parent;
  }
  friend bool operator>(const bk_tree_node<T> n1, const bk_tree_node<T> n2) {
    return n1.distance_from_parent > n2.distance_from_parent;
  }
  friend bool operator==(const bk_tree_node<T> n1, const bk_tree_node<T> n2) {
    return n1.distance_from_parent == n2.distance_from_parent;
  }
  friend bool operator!=(const bk_tree_node<T> n1, const bk_tree_node<T> n2) {
    return n1.distance_from_parent != n2.distance_from_parent;
  }

  T get() { return this->data; }

  bool can_go_deeper() {
    return this->children != nullptr && this->children->get_size() > 0;
  }
};

template <typename T> class bk_tree {
  distance_func<T> d;
  bk_tree_node<T> *root;

  bk_tree_node<T> *find_parent_of_new_child(T data) {
    int dist_from_root;
    bk_tree_node<T> *curr_root = this->root;
    while (curr_root != nullptr && curr_root->can_go_deeper()) {
      dist_from_root = this->d(data, curr_root->get());
      auto tmp = new bk_tree_node<T>(data, nullptr, nullptr, dist_from_root);
      int found_index = qs::binary_search(
          curr_root->children->get_data(), curr_root->children->get_size(),
          tmp);
      if (found_index < 0) {
        return curr_root;
      } else {
        curr_root = (*curr_root->children)[found_index];
      }
    }
    return curr_root;
  }

public:
  bk_tree(distance_func<T> df, linked_list<T> *l) : d(df) {
    auto curr_list_node = l->head();
    while (curr_list_node != nullptr) {
      this->insert(curr_list_node->get());
      curr_list_node = curr_list_node->next();
    }
  }

  // TODO test if this creates leaks
  ~bk_tree() { delete this->root; }

  void insert(T data) {
    auto new_node = new bk_tree_node<T>(data);
    bk_tree_node<T> *parent = this->find_parent_of_new_child(data);
    if (parent == nullptr) {
      // this is the root
      this->root = new_node;
    } else {
      new_node->parent = parent;
      new_node->distance_from_parent = 0;
      if (parent->children == nullptr) {
        parent->children = new vector<bk_tree_node<T> *>();
      }
      std::size_t children_len = parent->children->get_size();
      std::size_t i;
      for (i = 0; i < children_len; i++) {
        if ((*parent->children)[i]->distance_from_parent >=
            new_node->distance_from_parent) {
          break;
        }
      }
      parent->children->insert_in_place(new_node, i);
    }
  }
};

} // namespace qs

#endif // QS_BK_TREE_HPP
