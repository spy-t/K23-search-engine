#ifndef QS_HASH_TABLE_HPP
#define QS_HASH_TABLE_HPP

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

#include "hash.h"
#include "list.hpp"
#include "vector.hpp"

namespace qs {

// TODO abstract the key to a template
template <class V> class hash_table;

template <class V> class hash_table_item {
  V value;
  uint8_t *key;

  friend class hash_table<V>;

  bool operator==(hash_table_item<V> &other) {
    return strcmp((const char *)this->key, (const char *)other.key) == 0;
  }
  bool keycmp(const uint8_t *other) {
    return strcmp((const char *)this->key, (const char *)other) == 0;
  }

  explicit hash_table_item(const uint8_t *k, V value) : value(value) {
    const char *kk = (const char *)k;
    key = (uint8_t *)strdup(kk);
  }

  ~hash_table_item() { free(key); }

public:
  // No copy
  hash_table_item(const hash_table_item<V> &other) = delete;
  const hash_table_item &operator=(const hash_table_item<V> &other) = delete;

  V get() const { return value; }
  const uint8_t *get_key() const { return key; }
};

template <class V> class hash_table {
  using buckets_t = qs::vector<qs::linked_list<hash_table_item<V> *> *>;
  buckets_t buckets;
  std::size_t size;
  std::size_t capacity;

  constexpr static int n_primes = 26;
  static int get_prime(int index) {
    static const int primes[n_primes] = {
        53,        97,        193,      389,       769,       1543,
        3079,      6151,      12289,    24593,     49157,     98317,
        196613,    393241,    786433,   1572869,   3145739,   6291469,
        12582917,  25165843,  50331653, 100663319, 201326611, 402653189,
        805306457, 1610612741};
    return primes[index];
  }

  int find_prime(int n) {
    int res = n;
    for (int i = 0; i < hash_table::n_primes; ++i) {
      if (res < get_prime(i)) {
        return get_prime(i);
      }
    }
    return n;
  }

  void init_buckets(std::size_t size) {
    for (std::size_t i = 0; i < size; ++i) {
      buckets.push(new qs::linked_list<hash_table_item<V> *>());
    }
  }

  qs::linked_list<hash_table_item<V> *> *get_bucket(const uint8_t *key) {
    uint64_t hash = djb2(key) % capacity;
    return buckets[hash];
  }

  list_node<hash_table_item<V> *> *get_by_key(const uint8_t *key) {
    auto bucket = get_bucket(key);
    return get_by_key(bucket, key);
  }

  list_node<hash_table_item<V> *> *
  get_by_key(qs::linked_list<hash_table_item<V> *> *bucket,
             const uint8_t *key) {
    auto iter = bucket->head_m();
    while (iter != nullptr) {
      if (iter->get()->keycmp(key)) {
        return iter;
      }
      iter = iter->next_m();
    }
    return nullptr;
  }

  void resize() {
    int new_cap = find_prime(capacity * 2);
    buckets_t new_vec(new_cap);
    // Resize
    for (std::size_t i = 0; i < (std::size_t)new_cap; ++i) {
      new_vec.push(new qs::linked_list<hash_table_item<V> *>());
    }

    // Rehash
    for (std::size_t i = 0; i < capacity; ++i) {
      auto bucket = buckets[i];
      auto iter = bucket->head();
      while (iter != nullptr) {
        auto hash = djb2(iter->get()->key) % new_cap;
        auto new_bucket = new_vec[hash];
        new_bucket->append(iter->get());
        iter = iter->next();
      }
      delete bucket;
    }
    buckets = new_vec;
    capacity = new_cap;
  }

public:
  hash_table() : buckets(buckets_t(10)), size(0), capacity(10) {
    init_buckets(10);
  }

  explicit hash_table(std::size_t n_buckets)
      : buckets(buckets_t(n_buckets)), size(0), capacity(n_buckets) {
    init_buckets(n_buckets);
  }

  ~hash_table() {
    for (std::size_t i = 0; i < capacity; ++i) {
      auto list = buckets[i];
      auto head = list->head();
      while (head != nullptr) {
        delete head->get();
        head = head->next();
      }
      delete list;
    }
  }

  static const uint8_t *get_raw_key(const char *k) {
    return (const uint8_t *)k;
  }

  void insert(const uint8_t *key, V value) {

    // First check if an item with this key already exists in the bucket
    auto bucket = get_bucket(key);
    for (list_node<hash_table_item<V> *> *i = bucket->head_m(); i != nullptr;
         i = i->next_m()) {
      hash_table_item<V> *it = i->get();
      if (it->keycmp(key)) {
        return;
      }
    }
    double load_factor = (double)size / (double)capacity;
    if (load_factor >= 0.75) {
      resize();
      // Relocate the bucket because after the resize it might point to an
      // invalid location
      bucket = get_bucket(key);
    }

    auto item = new hash_table_item<V>(key, value);
    bucket->append(item);
    ++size;
  }

  optional<V> lookup(const uint8_t *key) {
    auto list_node = get_by_key(key);
    return list_node != nullptr ? optional(list_node->get()) : optional<V>();
  }

  void remove(const uint8_t *key) {
    auto bucket = get_bucket(key);
    auto list_node = get_by_key(bucket, key);
    if (list_node == nullptr) {
      return;
    }
    auto item = list_node->get();
    bucket->remove(list_node);
    delete item;
    --size;
  }

  template <typename F> void for_each(F fn) {
    for (std::size_t i = 0; i < buckets.get_size(); ++i) {
      auto bucket = buckets[i];
      auto head = bucket->head();
      while (head != nullptr) {
        hash_table_item<V> *item = head->get();
        fn(item);
        head = head->next();
      }
    }
  }
};

} // namespace qs

#endif // QS_HASH_TABLE_HPP
