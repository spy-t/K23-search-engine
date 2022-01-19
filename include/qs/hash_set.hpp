#ifndef QS_HASH_SET_HPP
#define QS_HASH_SET_HPP

#include <qs/hash_table.hpp>
#include <qs/optional.hpp>
#include <qs/string.h>

namespace qs {
template <class K, class Hash = std::hash<K>> class hash_set {
  hash_table<K, char> table;

public:
  hash_set() = default;
  explicit hash_set(std::size_t capacity) : table{capacity} {};
  hash_set(const hash_set &other) = delete;
  hash_set(hash_set &&other) noexcept : table{std::move(other.table)} {}

  struct iterator;

  iterator insert(const K &key) {
    return iterator(this->table.insert(key, '\0'));
  }
  iterator insert(K &&key) {
    return iterator(this->table.insert(std::move(key), '\0'));
  }

  void remove(const K &key) { this->table.remove(key); }

  bool contains(const K &key) {
    return this->table.lookup(key) != this->table.end();
  }
  std::size_t get_size() { return this->table.get_size(); }

  qs::vector<K> keys() {
    qs::vector<K> ret{table.get_size() + (std::size_t)(table.get_size() * 0.75)};
    for (auto && el = table.begin(); el != table.end(); el++) {
      ret.push(el.key());
    }
    return ret;
  }

  struct iterator {
    using ht_iterator = typename hash_table<K, char>::iterator;
    friend class hash_set;

    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = K;
    using pointer = value_type *;
    using reference = value_type &;

  private:
    ht_iterator iter;

    explicit iterator(ht_iterator iter) : iter(iter) {}

  public:
    reference operator*() { return *this->operator->(); }
    pointer operator->() { return &iter.key(); }

    iterator &operator++() {
      ++iter;
      return *this;
    }

    iterator operator++(int) {
      iterator old = *this;
      ++iter;
      return old;
    }

    friend bool operator==(const iterator &a, const iterator &b) {
      return a.iter == b.iter;
    }
    friend bool operator!=(const iterator &a, const iterator &b) {
      return !(a == b);
    }
  };

  iterator begin() { return iterator(table.begin()); }
  iterator end() { return iterator(table.end()); }
};
} // namespace qs

#endif // QS_HASH_SET_HPP
