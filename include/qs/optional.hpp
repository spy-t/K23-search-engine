#ifndef QS_OPTIONAL_H
#define QS_OPTIONAL_H
#include <stdexcept>
#include <type_traits>

namespace qs {

template <typename T> class optional {

private:
  struct empty_marker {};
  union {
    empty_marker e;
    T value;
  };
  bool has_value;

  void empty() {
    if (has_value) {
      value.~T();
      has_value = false;
    }
  }

  // Since we have made sure that T is trivially copyable we can simply do an
  // std::forward and cover both the normal constructors and the copy one
  template <class... Args> void add_value_in_place(Args &&...args) {
    empty();
    new (static_cast<void *>(std::addressof(value)))
        T(std::forward<Args>(args)...);
    has_value = true;
  }

public:
  explicit optional() : e(), has_value(false) {}
  template <class... Args>
  explicit optional(Args &&...args)
      : value(std::forward<Args>(args)...), has_value(true) {}

  optional(optional &other) : optional() {
    if (other.has_value) {
      add_value_in_place(other.value);
    }
  }
  optional &operator=(optional &other) {
    if (other.has_value) {
      add_value_in_place(other.value);
    } else {
      empty();
    }
    return *this;
  }

  optional(optional &&other) : optional() {
    if (other.has_value) {
      add_value_in_place(std::move(other.value));
      other.has_value = false;
    }
  }
  optional &operator=(optional &&other) {
    if (other.has_value) {
      add_value_in_place(std::move(other.value));
      other.has_value = false;
    } else {
      empty();
    }
    return *this;
  }

  ~optional() { empty(); }

  T &get_or(T &&default_) {
    if (has_value) {
      return value;
    } else {
      return default_;
    }
  }

  T &get() {
    if (has_value) {
      return value;
    } else {
      throw std::runtime_error("Bad optional access");
    }
  }

  bool is_empty() { return !has_value; }
};

} // namespace qs
#endif
