#ifndef QS_CRIT_BIT_TREE_HPP
#define QS_CRIT_BIT_TREE_HPP

#include <qs/string.h>
#include <qs/optional.hpp>
#include <qs/vector.hpp>
namespace qs {
//template <typename T> using prefix_handler = void (*)(qs::string key, T data);

template <typename T> class crit_bit_tree_node {
  qs::string key;
  T data;
  bool isExternal;
public:
  crit_bit_tree_node(const string &key, T data) : key(key), data(data) {}
  T getData() const { return data; }
  ~crit_bit_tree_node() {}


};

template <typename T> class crit_bit_tree {
  crit_bit_tree_node<T> *root;

public:
  crit_bit_tree(){
    root = nullptr;
  }
  void insert(qs::string key, T data){
  };
  bool contains(qs::string key){
  };
  qs::optional<T> get(qs::string key){
  };

  vector<T> get_all_prefixes(qs::string key){
  }

};

} // namespace qs

#endif // QS_CRIT_BIT_TREE_HPP
