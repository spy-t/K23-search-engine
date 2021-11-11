#ifndef QS_VECTOR_HPP
#define QS_VECTOR_HPP

#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <utility>

#include <qs/functions.hpp>

namespace qs {
template <typename T> class vector {
private:
  using T_storage = typename std::aligned_storage<sizeof(T), alignof(T)>::type;
  T_storage *data;
  std::size_t size;
  std::size_t capacity;

  void resize() {
    capacity = (std::size_t)(capacity * 1.5);
    T_storage *new_data_slice = new T_storage[capacity];
    functions::copy_uninitialized(data, data + size, new_data_slice);
    T_storage *old_data_slice = data;
    data = new_data_slice;
    delete[] old_data_slice;
  }

  void maybe_resize() {
    if ((((size + 1) / capacity) * 100) >= 75) {
      resize();
    }
  }

public:
  vector() : data(new T_storage[10]), size(0), capacity(10) {}
  explicit vector(std::size_t capacity)
      : data(new T_storage[capacity]), size(0), capacity(capacity) {}

  // Copy operations
  vector(const vector &other)
      : data(new T_storage[other.capacity]), size(other.size),
        capacity(other.capacity) {
    functions::copy_uninitialized(other.data, other.data + other.size, data);
  }
  vector &operator=(const vector &other) {
    if (this != &other) {
      T_storage *new_data = new T_storage[other.capacity];
      T_storage *old_data = data;
      functions::copy_uninitialized(other.data, other.data + other.size,
                                    new_data);
      data = new_data;
      capacity = other.capacity;
      size = other.size;
      if (old_data != nullptr) {
        delete[] old_data;
      }
    }
    return *this;
  };

  // Move operations
  vector(vector &&other) noexcept : data(nullptr), size(0), capacity(0) {
    data = other.data;
    size = other.size;
    capacity = other.capacity;

    other.data = nullptr;
    other.size = 0;
    other.capacity = 0;
  };

  vector &operator=(vector &&other) noexcept {
    if (this != &other) {
      this->~vector();
      data = other.data;
      size = other.size;
      capacity = other.capacity;

      other.data = nullptr;
      other.size = 0;
      other.capacity = 0;
    }

    return *this;
  }

  ~vector() {
    if (data != nullptr) {
      for (std::size_t i = 0; i < size; ++i) {
        std::launder(reinterpret_cast<T *>(&data[i]))->~T();
      }
      delete[] data;
    }
  }

  void push(const T &elem) {
    maybe_resize();
    new (&data[size]) T(elem);
    size++;
  }

  void push(T &&elem) {
    maybe_resize();
    new (&data[size]) T(std::move(elem));
    size++;
  }

  void set(std::size_t index, const T &elem) {
    if (index >= capacity) {
      throw std::runtime_error("index out of bounds");
    }
    if (index < size) {
      // Destruct the object that was at that position
      std::launder(reinterpret_cast<T *>(&data[index]))->~T();
      new (&data[index]) T(elem);
    } else {
      push(elem);
    }
  }

  void set(std::size_t index, T &&elem) {
    if (index >= capacity) {
      throw std::runtime_error("index out of bounds");
    }
    if (index < size) {
      std::launder(reinterpret_cast<T *>(&data[index]))->~T();
      new (&data[index]) T(std::move(elem));
    } else {
      push(std::move(elem));
    }
  }

  // unchecked dereference at index
  T &operator[](std::size_t index) {
    return *std::launder(reinterpret_cast<T *>(&data[index]));
  }

  // checked dereference at index
  T &at(std::size_t index) {
    if (index >= size) {
      throw std::runtime_error("index out of bounds");
    }
    return this->operator[](index);
  }

  std::size_t get_size() const { return size; }

  // Returns a reference to the underlying buffer. IT SHOULD NOT BE MODIFIED
  T *get_data() const { return std::launder(reinterpret_cast<T *>(data)); }

  struct iterator {
    T_storage *p;

  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = value_type *;
    using reference = value_type &;

    explicit iterator(T_storage *p) : p(p) {}
    reference operator*() { return *this->operator->(); }
    pointer operator->() { return std::launder(reinterpret_cast<T *>(p)); }

    iterator &operator++() {
      p++;
      return *this;
    }
    iterator operator++(int) {
      iterator tmp = *this;
      p++;
      return tmp;
    }

    iterator &operator--() {
      p--;
      return *this;
    }
    iterator operator--(int) {
      iterator tmp = *this;
      p--;
      return tmp;
    }

    friend bool operator==(const iterator &a, const iterator &b) {
      return a.p == b.p;
    }
    friend bool operator!=(const iterator &a, const iterator &b) {
      return a.p != b.p;
    }
  };

  iterator begin() { return iterator(this->data); }
  iterator end() { return iterator(&this->data[this->size]); }

  auto rbegin() { return std::make_reverse_iterator(end()); }
  auto rend() { return std::make_reverse_iterator(begin()); }
};
} // namespace qs
#endif
