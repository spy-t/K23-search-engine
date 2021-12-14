#ifndef QS_MEMORY_HPP
#define QS_MEMORY_HPP

#include <memory>
#include <qs/optional.hpp>
#include <stdexcept>
#include <type_traits>

namespace qs {

template <typename T> class unique_pointer {
  using pointer_type = typename std::add_pointer_t<T>;

private:
  pointer_type data;

public:
  explicit unique_pointer(pointer_type d) : data(d) {}

  unique_pointer(unique_pointer &other) = delete;
  unique_pointer &operator=(unique_pointer &other) = delete;

  unique_pointer(unique_pointer &&other) : data(other.data) {
    other.data = nullptr;
  }
  unique_pointer &operator=(unique_pointer &&other) {
    if (this != &other) {
      this->data = other.data;
      other.data = nullptr;
    }

    return *this;
  }

  typename std::add_lvalue_reference_t<T> operator*() {
    if (data != nullptr) {
      return *data;
    } else {
      throw std::runtime_error("Dereference of empty unique pointer");
    }
  }

  pointer_type operator->() {
    if (data != nullptr) {
      return data;
    } else {
      throw std::runtime_error("Dereference of empty unique pointer");
    }
  }

  pointer_type get() { return this->operator->(); }

  ~unique_pointer() {
    if (data != nullptr) {
      delete data;
    }
  }
};

template <typename T, class... Args>
unique_pointer<T> make_unique(Args &&...args) {
  return unique_pointer<T>(new T(std::forward<Args>(args)...));
}

} // namespace qs

#endif
