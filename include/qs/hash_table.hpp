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

template <class K, class V, class Hash = std::hash<K>,
          class KEq = std::equal_to<K>>
class hash_table;
template <class K, class V, class Hash = std::hash<K>,
          class KEq = std::equal_to<K>>
class oa_hash_table;

template <class K, class V, class KEq = std::equal_to<K>>
class hash_table_item {
  V value;
  K key;
  KEq key_equals = KEq{};

  friend class hash_table<K, V>;
  friend class oa_hash_table<K, V>;

  bool operator==(hash_table_item<K, V> &other) {
    return key_equals(this->key, other.key);
  }
  bool keycmp(const K &other) { return this->key == other; }

  explicit hash_table_item(const K &k, V &value) : value(value), key(k) {}
  explicit hash_table_item(K &&k, V &value) : value(value), key(std::move(k)) {}
  explicit hash_table_item(const K &k, V &&value)
      : value(std::move(value)), key(k) {}
  explicit hash_table_item(K &&k, V &&value)
      : value(std::move(value)), key(std::move(k)) {}

public:
  V &get() { return value; }
  V &operator*() { return value; }
  const K &get_key() const { return key; }
};

template <class K, class V, class Hash, class KEq> class hash_table {
  using item_t = hash_table_item<K, V, KEq>;
  using bucket_t = qs::linked_list<item_t>;
  using buckets_t = qs::vector<bucket_t>;
  std::size_t size;
  std::size_t capacity;
  buckets_t buckets;
  Hash hash_functor = Hash{};

  void init_buckets(std::size_t size) {
    for (std::size_t i = 0; i < size; ++i) {
      buckets.push(qs::linked_list<item_t>());
    }
  }

  bucket_t &get_bucket(const K &key) {
    uint64_t hash = hash_functor(key) % capacity;
    return buckets[hash];
  }

  list_node<item_t> *get_by_key(const K &key) {
    auto bucket = get_bucket(key);
    return get_by_key(bucket, key);
  }

  list_node<item_t> *get_by_key(bucket_t &bucket, const K &key) {
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
      new_vec.push(qs::linked_list<item_t>());
    }

    // Rehash
    for (std::size_t i = 0; i < capacity; ++i) {
      auto &bucket = buckets[i];
      auto iter = bucket.head;
      while (iter != nullptr) {
        auto hash = hash_functor(iter->get().key) % new_cap;
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

  hash_table()
      : size(0), capacity(find_prime(10)), buckets(buckets_t(capacity)) {
    init_buckets(capacity);
  }

  explicit hash_table(std::size_t n_buckets)
      : size(0), capacity(find_prime(n_buckets)), buckets(buckets_t(capacity)) {
    init_buckets(capacity);
  }

  hash_table(const hash_table &other) = delete;
  hash_table &operator=(const hash_table &other) = delete;

  std::size_t get_size() { return size; }

  void insert(K &&key, V &&value) {
    insert(item_t(std::move(key), std::move(value)));
  }

  void insert(K &&key, V &value) { insert(item_t(std::move(key), value)); }

  void insert(const K &key, V &value) { insert(item_t(key, value)); }

  void insert(const K &key, V &&value) {
    insert(item_t(key, std::move(value)));
  }

  void insert(item_t &&i) {
    auto &bucket = get_bucket(i.key);
    for (auto &n : bucket) {
      if (n.keycmp(i.key)) {
        return;
      }
    }
    bucket.append(std::move(i));
    ++size;
    float load_factor = (float)size / (float)capacity;
    if (load_factor >= 0.75) {
      resize();
    }
  }

  iterator lookup(const K &key) {
    uint64_t hash = hash_functor(key) % capacity;
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

  void remove(const K &key) {
    auto &bucket = get_bucket(key);
    auto list_node = get_by_key(bucket, key);
    if (list_node == nullptr) {
      return;
    }
    bucket.remove(list_node);
    --size;
  }

  struct iterator {
    friend class hash_table<K, V>;

  private:
    static inline list_node<item_t> *find_first_available(buckets_t &buckets,
                                                          std::size_t &offset) {
      if (offset >= buckets.get_size()) {
        return nullptr;
      }
      list_node<item_t> *current = buckets[offset].head;
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
    using value_type = V;
    using pointer = value_type *;
    using reference = value_type &;

    list_node<item_t> *current;
    buckets_t *buckets;
    std::size_t offset;

    explicit iterator(list_node<item_t> *current, buckets_t *buckets,
                      std::size_t offset)
        : current(current), buckets(buckets), offset(offset) {}
    iterator(iterator &other) = default;
    iterator &operator=(iterator &other) = default;

    reference operator*() { return current->get().get(); }
    pointer operator->() { return &current->get().get(); }

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
    list_node<item_t> *current =
        iterator::find_first_available(buckets, offset);

    if (current == nullptr) {
      return end();
    } else {
      return iterator(current, &buckets, offset);
    }
  }

  iterator end() { return iterator(nullptr, &buckets, buckets.get_size()); }
};

// An open addressing hash table implementation
template <class K, class V, class Hash, class KEq> class oa_hash_table {

  using KeyStorage = typename std::aligned_storage_t<sizeof(K), alignof(K)>;
  using ValueStorage = typename std::aligned_storage_t<sizeof(V), alignof(V)>;
  using item_t = hash_table_item<K, V, KEq>;

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

  explicit oa_hash_table() : oa_hash_table(10){};

  explicit oa_hash_table(std::size_t capacity)
      : size(0), capacity(find_prime(capacity)),
        keys(new key_pair[this->capacity]),
        values(new ValueStorage[this->capacity]) {}

  oa_hash_table(const oa_hash_table &other) = delete;
  oa_hash_table &operator=(const oa_hash_table &other) = delete;

  ~oa_hash_table() {
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

    key_pair *keys;
    ValueStorage *values;
    std::size_t pos;
    std::size_t capacity;

    explicit iterator(std::size_t pos, oa_hash_table &ht)
        : keys(ht.keys), values(ht.values), pos(pos), capacity(ht.capacity) {}

    reference operator*() { return *this->operator->(); }
    pointer operator->() {
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
