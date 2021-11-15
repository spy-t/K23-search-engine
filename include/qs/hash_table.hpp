#ifndef QS_HASH_TABLE_HPP
#define QS_HASH_TABLE_HPP

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

#include <qs/hash.h>
#include <qs/list.hpp>
#include <qs/string.h>
#include <qs/vector.hpp>

namespace qs {

// TODO abstract the key to a template
template <class V> class hash_table;

template <class V> class hash_table_item {
  V value;
  qs::string key;

  friend class hash_table<V>;

  bool operator==(hash_table_item<V> &other) { return this->key == other.key; }
  bool keycmp(const qs::string &other) { return this->key == other; }
  bool keycmp(const char *other) { return this->key == other; }

  explicit hash_table_item(const qs::string &k, V &value)
      : value(value), key(k) {}
  explicit hash_table_item(qs::string &&k, V &&value)
      : value(std::move(value)), key(std::move(k)) {}

public:
  // No copy
  hash_table_item(const hash_table_item<V> &other) = delete;
  hash_table_item &operator=(const hash_table_item<V> &other) = delete;

  hash_table_item(hash_table_item<V> &&other)
      : value(std::move(other.value)), key(std::move(other.key)) {}
  hash_table_item &operator=(hash_table_item<V> &&other) {
    this->value = std::move(other.value);
    this->key = std::move(other.key);

    return *this;
  };

  V &get() { return value; }
  V &operator*() { return value; }
  const qs::string &get_key() const { return key; }
};

template <class V> class hash_table {
  using buckets_t = qs::vector<qs::linked_list<hash_table_item<V>>>;
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
      buckets.push(qs::linked_list<hash_table_item<V>>());
    }
  }

  qs::linked_list<hash_table_item<V>> &get_bucket(const qs::string &key) {
    return get_bucket(key.get_buffer());
  }

  qs::linked_list<hash_table_item<V>> &get_bucket(const char *key) {
    uint64_t hash = djb2((const uint8_t *)key) % capacity;
    return buckets[hash];
  }

  list_node<hash_table_item<V>> *get_by_key(const qs::string &key) {
    get_by_key(key.get_buffer());
  }

  list_node<hash_table_item<V>> *get_by_key(const char *key) {
    auto bucket = get_bucket(key);
    return get_by_key(bucket, key);
  }

  list_node<hash_table_item<V>> *
  get_by_key(qs::linked_list<hash_table_item<V>> &bucket,
             const qs::string &key) {
    return get_by_key(bucket, key.get_buffer());
  }

  list_node<hash_table_item<V>> *
  get_by_key(qs::linked_list<hash_table_item<V>> &bucket, const char *key) {
    auto iter = bucket.head;
    while (iter != nullptr) {
      if (iter->get().keycmp(key)) {
        return iter;
      }
      iter = iter->next();
    }
    return nullptr;
  }

  void resize() {
    int new_cap = find_prime(capacity * 2);
    buckets_t new_vec(new_cap);
    // Resize
    for (std::size_t i = 0; i < (std::size_t)new_cap; ++i) {
      new_vec.push(qs::linked_list<hash_table_item<V>>());
    }

    // Rehash
    for (std::size_t i = 0; i < capacity; ++i) {
      auto &bucket = buckets[i];
      auto iter = bucket.head;
      while (iter != nullptr) {
        auto hash =
            djb2((const uint8_t *)iter->get().key.get_buffer()) % new_cap;
        auto &new_bucket = new_vec[hash];
        new_bucket.append(std::move(iter->get()));
        iter = iter->next();
      }
    }
    buckets = std::move(new_vec);
    capacity = new_cap;
  }

public:
  struct iterator;

  hash_table() : buckets(buckets_t(get_prime(10))), size(0), capacity(10) {
    init_buckets(10);
  }

  explicit hash_table(std::size_t n_buckets)
      : buckets(buckets_t(n_buckets)), size(0), capacity(n_buckets) {
    init_buckets(n_buckets);
  }

  std::size_t get_size() { return size; }

  static const uint8_t *get_raw_key(const char *k) {
    return (const uint8_t *)k;
  }

  void insert(qs::string &&key, V &&value) {
    insert(hash_table_item<V>(std::move(key), std::move(value)));
  }

  void insert(const qs::string &key, V &value) {
    insert(hash_table_item<V>(key, value));
  }

  void insert(const qs::string &key, V &&value) {
    insert(hash_table_item<V>(key, std::move(value)));
  }

  void insert(const char *key, V &value) {
    insert(hash_table_item<V>(qs::string(key), value));
  }

  void insert(const char *key, V &&value) {
    insert(hash_table_item<V>(qs::string(key), std::move(value)));
  }

  void insert(hash_table_item<V> &&i) {
    // First check if an item with this key already exists in the bucket
    auto bucket = &get_bucket(i.key);
    for (auto &n : *bucket) {
      if (n.keycmp(i.key)) {
        return;
      }
    }
    double load_factor = (double)size / (double)capacity;
    if (load_factor >= 0.75) {
      resize();
      // Relocate the bucket because after the resize it might point to an
      // invalid location
      bucket = &get_bucket(i.key);
    }

    bucket->append(std::move(i));
    ++size;
  }

  iterator lookup(const qs::string &key) { return lookup(key.get_buffer()); }

  iterator lookup(const char *key) {
    uint64_t hash = djb2((const uint8_t *)key) % capacity;
    auto &bucket = buckets[hash];
    auto n = bucket.head;
    while (n != nullptr) {
      if (n->get().keycmp(key)) {
        break;
      }
      n = n->next();
    }
    return n != nullptr ? iterator(n, &buckets, hash) : end();
  }

  void remove(const qs::string &key) { remove(key.get_buffer()); }

  void remove(const char *key) {
    auto &bucket = get_bucket(key);
    auto list_node = get_by_key(bucket, key);
    if (list_node == nullptr) {
      return;
    }
    bucket.remove(list_node);
    --size;
  }

  struct iterator {
    friend class hash_table<V>;

  private:
    static inline list_node<hash_table_item<V>> *
    find_first_available(buckets_t &buckets, std::size_t &offset) {
      if (offset >= buckets.get_size()) {
        return nullptr;
      }
      list_node<hash_table_item<V>> *current = buckets[offset].head;
      if (current != nullptr) {
        return current;
      }
      for (offset = offset + 1; offset < buckets.get_size(); ++offset) {
        current = buckets[offset].head;
        if (current != nullptr) {
          return current;
        }
      }

      return current;
    }

    void empty() {
      current = nullptr;
      offset = buckets->get_size();
    }

  public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = hash_table_item<V>;
    using pointer = value_type *;
    using reference = value_type &;

    list_node<hash_table_item<V>> *current;
    buckets_t *buckets;
    std::size_t offset;

    explicit iterator(list_node<hash_table_item<V>> *current,
                      buckets_t *buckets, std::size_t offset)
        : current(current), buckets(buckets), offset(offset) {}
    iterator(iterator &other) = default;
    iterator &operator=(iterator &other) = default;

    reference operator*() { return current->get(); }
    pointer operator->() { return &current->get(); }

    iterator &operator++() {
      current = current->next();
      if (current == nullptr) {
        current = find_first_available(*buckets, ++offset);
        if (current == nullptr) {
          empty();
        }
      }
      return *this;
    }

    iterator operator++(int) {
      iterator old = *this;
      current = current->next();
      if (current == nullptr) {
        current = find_first_available(*buckets, ++offset);
        if (current == nullptr) {
          empty();
        }
      }
      return old;
    }

    friend bool operator==(const iterator &a, const iterator &b) {
      return a.current == b.current && a.offset == b.offset;
    }
    friend bool operator!=(const iterator &a, const iterator &b) {
      return !(a == b);
    }
  };

  iterator begin() {
    std::size_t offset = 0;
    list_node<hash_table_item<V>> *current =
        iterator::find_first_available(buckets, offset);

    if (current == nullptr) {
      return end();
    } else {
      return iterator(current, &buckets, offset);
    }
  }

  iterator end() { return iterator(nullptr, &buckets, buckets.get_size()); }
};

} // namespace qs

#endif // QS_HASH_TABLE_HPP
