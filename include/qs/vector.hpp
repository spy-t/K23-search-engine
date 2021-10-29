#ifndef QS_VECTOR_HPP
#define QS_VECTOR_HPP

#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <utility>

#include "functions.hpp"

namespace qs {
template <typename T> class vector {
private:
  T *data;
  std::size_t size;
  std::size_t capacity;

  void resize() {
    capacity = (std::size_t)(capacity * 1.5);
    T *new_data_slice = new T[capacity];
    qs::functions::copy(data, data + size, new_data_slice);
    T *old_data_slice = data;
    data = new_data_slice;
    delete[] old_data_slice;
  }

public:
  vector() : data(new T[10]), size(0), capacity(10) {}
  explicit vector(std::size_t capacity)
      : data(new T[capacity]), size(0), capacity(capacity) {}

  // Copy operations
  vector(const vector &other)
      : data(new T[other.capacity]), size(other.size),
        capacity(other.capacity) {
    qs::functions::copy(other.data, other.data + other.size, data);
  }
  vector &operator=(const vector &other) {
    if (this != &other) {
      T *new_data = new T[other.capacity];
      T *old_data = data;
      qs::functions::copy(other.data, other.data + other.size, new_data);
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
      delete[] data;
    }
  }

  void push(T elem) {
    if ((((size + 1) / capacity) * 100) >= 75) {
      resize();
    }
    data[size++] = elem;
  }

  void insert_in_place(T elem, int index) {
    if (index < 0) {
      throw std::runtime_error("index out of bounds");
    }
    size++;
    if (((size / capacity) * 100) >= 75) {
      capacity = (std::size_t)(capacity * 1.5);
    }
    T *new_data = new T[capacity];
    T *left_slice_start = data;
    T *left_slice_end = data + (index - 1);
    T *right_slice_start = data + index;
    T *right_slice_end = data + (size - 1);
    qs::functions::copy(left_slice_start, left_slice_end, new_data);
    new_data[index] = elem;
    qs::functions::copy(right_slice_start, right_slice_end, new_data + index + 1);
    T *old_data = data;
    data = new_data;
    delete[] old_data;
  }

  void set(int index, T elem) {
    if (index < 0 || (std::size_t)index >= capacity) {
      throw std::runtime_error("index out of bounds");
    }
    data[index] = elem;
  }

  T &operator[](int index) const {
    if (index < 0 || (std::size_t)index >= capacity) {
      throw std::runtime_error("index out of bounds");
    }
    return data[index];
  }

  std::size_t get_size() const { return size; }

  using predicate = std::function<bool(T)>;
  T find(predicate pred) {
    for (std::size_t i = 0; i < size; ++i) {
      if (pred(data[i])) {
        return data[i];
      }
    }
    throw std::runtime_error("Could not find element");
  }

  using consumer = std::function<void(T)>;
  void for_each(consumer c) {
    for (std::size_t i = 0; i < size; ++i) {
      c(data[i]);
    }
  }

  // Returns a reference to the underlying buffer. IT SHOULD NOT BE MODIFIED
  T *get_data() const { return data; }
};
} // namespace qs
#endif
