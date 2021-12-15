#ifndef QS_HASH_TABLE_HPP
#define QS_HASH_TABLE_HPP

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

#include <qs/core.h>
#include <qs/hash.h>
#include <qs/list.hpp>
#include <qs/string.h>
#include <qs/vector.hpp>

namespace qs {

constexpr static int n_primes = 26;
static int get_prime(int index) {
  static const int primes[n_primes] = {
      53,        97,        193,       389,       769,       1543,     3079,
      6151,      12289,     24593,     49157,     98317,     196613,   393241,
      786433,    1572869,   3145739,   6291469,   12582917,  25165843, 50331653,
      100663319, 201326611, 402653189, 805306457, 1610612741};
  return primes[index];
}

QS_FORCE_INLINE int find_prime(int n) {
  int res = n;
  for (int i = 0; i < n_primes; ++i) {
    if (res < get_prime(i)) {
      return get_prime(i);
    }
  }
  return n;
}

// An open addressing hash table implementation
template <class K, class V, class Hash = std::hash<K>,
          class KEq = std::equal_to<K>>
class hash_table {

  using KeyStorage = typename std::aligned_storage_t<sizeof(K), alignof(K)>;
  using ValueStorage = typename std::aligned_storage_t<sizeof(V), alignof(V)>;

  struct key_pair {
    KeyStorage key;
    bool is_gravestone = true;

    key_pair() : is_gravestone(true){};
    key_pair(K &key) : key(key), is_gravestone(false){};
    key_pair(K &&key) : key(std::move(key)), is_gravestone(false){};

    const K &get_key() const {
      return *std::launder(reinterpret_cast<const K *>(&key));
    }
  };

  Hash hash_functor = Hash{};
  KEq key_equals = KEq{};
  std::size_t size;
  std::size_t capacity;
  key_pair *keys;
  ValueStorage *values;

  int find_available_position(const K &key) {
    std::size_t index = hash_functor(key) % capacity;
    std::size_t i = index;
    while (!this->keys[i].is_gravestone) {
      if (key_equals(this->keys[i].get_key(), key)) {
        return i;
      }
      ++i;
      if (i == capacity) {
        i = 0;
      } else if (i == index) {
        return -1;
      }
    }
    return i;
  }

  void resize() {
    key_pair *old_keys = this->keys;
    ValueStorage *old_values = this->values;
    std::size_t old_cap = this->capacity;
    this->capacity = find_prime(capacity * 2);
    this->size = 0;
    this->keys = new key_pair[this->capacity];
    this->values = new ValueStorage[this->capacity];
    for (std::size_t i = 0; i < old_cap; ++i) {
      if (!old_keys[i].is_gravestone) {
        insert(
            std::move(*std::launder(reinterpret_cast<K *>(&old_keys[i].key))),
            std::move(*std::launder(reinterpret_cast<V *>(&old_values[i]))));
      }
    }
    delete[] old_keys;
    delete[] old_values;
  }

  void maybe_resize() {
    float load_factor = (float)size / (float)capacity;
    if (load_factor >= 0.75) {
      resize();
    }
  }

public:
  struct iterator;

  explicit hash_table() : hash_table(10){};

  explicit hash_table(std::size_t capacity)
      : size(0), capacity(find_prime(capacity)),
        keys(new key_pair[this->capacity]),
        values(new ValueStorage[this->capacity]) {}

  hash_table(const hash_table &other) = delete;
  hash_table &operator=(const hash_table &other) = delete;

  ~hash_table() {
    for (std::size_t i = 0; i < capacity; ++i) {
      if (!keys[i].is_gravestone) {
        std::launder(reinterpret_cast<K *>(&keys[i].key))->~K();
        std::launder(reinterpret_cast<V *>(&values[i]))->~V();
      }
    }
    delete[] keys;
    delete[] values;
  }

  void insert(K &&key, V &&value) {
    maybe_resize();
    auto pos = find_available_position(key);
    assert(pos >= 0 && pos < capacity);

    if (this->keys[pos].is_gravestone) {
      new (&keys[pos].key) K(std::move(key));
      new (&values[pos]) V(std::move(value));
      keys[pos].is_gravestone = false;
      size++;
    }
  };

  void insert(const K &key, const V &value) {
    maybe_resize();
    auto pos = find_available_position(key);
    assert(pos >= 0 && pos < capacity);

    if (this->keys[pos].is_gravestone) {
      new (&keys[pos].key) K(key);
      new (&values[pos]) V(value);
      keys[pos].is_gravestone = false;
      size++;
    }
  };

  std::size_t get_size() const { return this->size; }

  iterator lookup(const K &key) {
    auto pos = find_available_position(key);
    if (pos >= 0 && !keys[pos].is_gravestone) {
      return iterator(pos, *this);
    } else {
      return end();
    }
  }

  void remove(const K &key) {
    auto pos = find_available_position(key);
    if (pos >= 0 && !keys[pos].is_gravestone) {
      std::launder(reinterpret_cast<K *>(&keys[pos].key))->~K();
      std::launder(reinterpret_cast<V *>(&values[pos]))->~V();
      keys[pos].is_gravestone = true;
      size--;
    }
  }

  struct iterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = V;
    using pointer = value_type *;
    using reference = value_type &;

  private:
    key_pair *keys;
    ValueStorage *values;
    std::size_t pos;
    std::size_t capacity;

  public:
    explicit iterator(std::size_t pos, hash_table &ht)
        : keys(ht.keys), values(ht.values), pos(pos), capacity(ht.capacity) {}

    QS_FORCE_INLINE reference operator*() { return *this->operator->(); }
    QS_FORCE_INLINE pointer operator->() {
      return std::launder(reinterpret_cast<V *>(&values[pos]));
    }

    iterator &operator++() {
      pos++;
      while (pos != capacity && keys[pos].is_gravestone) {
        pos++;
      }
      return *this;
    }

    iterator operator++(int) {
      iterator old = *this;
      pos++;
      while (pos != capacity && keys[pos].is_gravestone) {
        pos++;
      }
      return old;
    }

    friend bool operator==(const iterator &a, const iterator &b) {
      return a.pos == b.pos;
    }
    friend bool operator!=(const iterator &a, const iterator &b) {
      return !(a == b);
    }

    QS_FORCE_INLINE const K &key() {
      return *std::launder(reinterpret_cast<K *>(&keys[pos].key));
    }

    QS_FORCE_INLINE reference value() { return *this->operator->(); }
  };

  iterator begin() {
    if (size > 0) {
      std::size_t i = 0;
      while (i < capacity && keys[i].is_gravestone) {
        ++i;
      }
      return iterator(i, *this);
    } else {
      return end();
    }
  }
  iterator end() { return iterator(capacity, *this); }
};
} // namespace qs

#endif // QS_HASH_TABLE_HPP
