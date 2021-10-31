#ifndef QS_BK_TREE_HPP
#define QS_BK_TREE_HPP

#include <qs/list.hpp>
#include <qs/optional.hpp>
#include <qs/search.hpp>
#include <qs/vector.hpp>

namespace qs {

template <typename T> using distance_func = std::function<int(T a, T b)>;

template <typename T> class bk_tree;

template <typename T> class bk_tree_node {
  friend class bk_tree<T>;

  T data;
  int distance_from_parent;
  vector<bk_tree_node<T> *> *children;
  bk_tree_node<T> *parent;

public:
  explicit bk_tree_node(
      T d, int distance_from_parent = 0,
      vector<bk_tree_node<T> *> *children = new vector<bk_tree_node<T> *>(),
      bk_tree_node<T> *parent = nullptr)
      : data(d), distance_from_parent(distance_from_parent), children(children),
        parent(parent) {}

  ~bk_tree_node() {
    if (this->children != nullptr) {
      delete this->children;
    }
  }

  T get() { return this->data; }

  bool can_go_deeper() {
    return this->children != nullptr && this->children->get_size() > 0;
  }
};

template <typename T> class bk_tree {
  distance_func<T> d;
  bk_tree_node<T> *root;

  int binary_search_children(vector<bk_tree_node<T> *> *children,
                             int search_dist) {
    int left = 0;
    int right = children->get_size() - 1;
    int middle;

    bk_tree_node<T> **data = children->get_data();
    std::cout << data[0]->get() << "\n";

    while (left <= right) {
      middle = (left + right) / 2;
      if (data[middle]->distance_from_parent < search_dist) {
        left = middle + 1;
      } else if (data[middle]->distance_from_parent > search_dist) {
        right = middle - 1;
      } else {
        return middle;
      }
    }
    return -1;
  }

  bk_tree_node<T> *find_parent_of_new_child(T data, int &dist_from_root) {
    bk_tree_node<T> *curr_root = this->root;
    while (curr_root != nullptr && curr_root->can_go_deeper()) {
      dist_from_root = this->d(data, curr_root->get());
      int found_index =
          this->binary_search_children(curr_root->children, dist_from_root);
      if (found_index < 0) {
        return curr_root;
      } else {
        curr_root = (*curr_root->children)[found_index];
      }
    }
    if (curr_root != nullptr) {
      dist_from_root = this->d(data, curr_root->get());
    } else {
      dist_from_root = 0;
    }
    return curr_root;
  }

  void insert_child_sorted(vector<bk_tree_node<T> *> *children,
                           bk_tree_node<T> *new_child) {
    std::size_t children_len = children->get_size();
    std::size_t i;
    for (i = 0; i < children_len; i++) {
      if ((*children)[i]->distance_from_parent >=
          new_child->distance_from_parent) {
        break;
      }
    }
    children->insert_in_place(new_child, i);
  }

public:
  bk_tree(distance_func<T> df, linked_list<T> *l) : d(df) {
    this->root = nullptr;
    auto curr_list_node = l->head();
    while (curr_list_node != nullptr) {
      this->insert(curr_list_node->get());
      curr_list_node = curr_list_node->next();
    }
  }

  // TODO test if this creates leaks
  ~bk_tree() { delete this->root; }

  void insert(T data) {
    int dist_from_parent = 0;
    bk_tree_node<T> *parent =
        this->find_parent_of_new_child(data, dist_from_parent);
    auto new_node =
        new bk_tree_node<T>(data, dist_from_parent, nullptr, parent);
    if (parent == nullptr) {
      this->root = new_node;
    } else {
      if (parent->children == nullptr) {
        parent->children = new vector<bk_tree_node<T> *>();
      }
      this->insert_child_sorted(parent->children, new_node);
    }
  }
};

} // namespace qs

#endif // QS_BK_TREE_HPP
