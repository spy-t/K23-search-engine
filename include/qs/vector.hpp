#ifndef QS_VECTOR_HPP
#define QS_VECTOR_HPP

#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
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
  vector(const vector &&other) noexcept : data(nullptr), size(0), capacity(0) {
    *this = std::move(other);
  };

  vector &operator=(vector &&other) noexcept {
    if (this != &other) {
      if (data != nullptr) {
        delete[] data;
      }
      data = other.data;
      size = other.size;
      capacity = other.capacity;

      other.data = nullptr;
      other.size = 0;
      other.capacity = 0;
    }
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
    if ((((size + 1) / capacity) * 100) >= 75) {
      resize();
    }
    new (&data[size]) T(elem);
    size++;
  }

  void push(T &&elem) {
    if ((((size + 1) / capacity) * 100) >= 75) {
      resize();
    }
    new (&data[size]) T(std::move(elem));
    size++;
  }

  void set(int index, T &&elem) {
    if (index < 0 || (std::size_t)index >= capacity) {
      throw std::runtime_error("index out of bounds");
    }
    data[index] = std::move(elem);
  }

  // unchecked dereference at index
  T &operator[](int index) {
    return *std::launder(reinterpret_cast<T *>(&data[index]));
  }

  // checked dereference at index
  T &at(int index) {
    if (index < 0 || (std::size_t)index >= capacity) {
      throw std::runtime_error("index out of bounds");
    }
    return *std::launder(reinterpret_cast<T *>(&data[index]));
  }

  std::size_t get_size() const { return size; }

  // Returns a reference to the underlying buffer. IT SHOULD NOT BE MODIFIED
  T *get_data() const { return std::launder(reinterpret_cast<T *>(data)); }
};
} // namespace qs
#endif
