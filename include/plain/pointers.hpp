#ifndef PLAINLIB_POINTERS_HPP
#define PLAINLIB_POINTERS_HPP

#include "functions/ops.hpp"
#include <utility>

namespace pl {

template <class T> class unique_ptr {
  T *ptr;

public:
  unique_ptr() : ptr(nullptr) {}
  explicit unique_ptr(std::nullptr_t) : ptr(nullptr) {}
  explicit unique_ptr(T *ptr) : ptr(ptr) {}
  ~unique_ptr() { delete ptr; }

  // Copy is not allowed
  unique_ptr(const unique_ptr &other) = delete;
  unique_ptr &operator=(const unique_ptr &) = delete;

  // Move is allowed
  unique_ptr(unique_ptr &&other) noexcept : ptr(std::move(other.ptr)) {
    other.ptr = nullptr;
  }
  unique_ptr &operator=(unique_ptr &&other) noexcept {
    if (this == other) {
      return *this;
    }
    ptr = std::move(other.ptr);
    other.ptr = nullptr;
    return *this;
  }

  T *release() noexcept {
    T *tmp = nullptr;
    pl::functions::swap(tmp, ptr);
    return tmp;
  }

  T &operator*() const { return *ptr; }

  T *operator->() const { return ptr; }
  T *get() const { return ptr; }

  bool exists() const { return ptr != nullptr; }
};

template <class T, class... Args> unique_ptr<T> make_unique(Args &&...args) {
  return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

} // namespace pl

#endif
