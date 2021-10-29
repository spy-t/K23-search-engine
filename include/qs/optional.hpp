#ifndef QS_OPTIONAL_H
#define QS_OPTIONAL_H
#include <stdexcept>
#include <type_traits>

namespace qs {

template <typename T, bool = std::is_pointer_v<T>> class optional {

  using value_type = typename std::remove_pointer<T>::type;

private:
  struct empty_marker {};
  union {
    empty_marker e;
    T value;
  };
  bool has_value;

public:
  explicit optional() : e(), has_value(false) {}
  explicit optional(T val) : value(val), has_value(true) {}

  optional(optional &other) = delete;
  optional &operator=(optional &other) = delete;

  optional(optional &&other) { *this = std::move(other); }
  optional &operator=(optional &&other) {
    if (this != &other) {
      other.has_value = false;
      this->value = std::move(other.value);
      this->has_value = true;
    }

    return *this;
  }

  ~optional() {}

  static optional<T> empty() { return optional(); }

  T get_or(value_type &&default_) {
    if (has_value) {
      has_value = false;
      return value;
    } else {
      T boxed_default = new value_type(default_);
      return boxed_default;
    }
  }

  T get() {
    if (has_value) {
      has_value = false;
      return value;
    } else {
      throw std::runtime_error("Bad optional access");
    }
  }

  bool is_empty() { return !has_value; }
};

template <typename T> class optional<T, false> {
private:
  struct empty_marker {};
  union {
    empty_marker e;
    T value;
  };
  bool has_value;

public:
  explicit optional() : e(), has_value(false) {}
  explicit optional(T val) : value(val), has_value(true) {}

  optional(optional &other) = delete;
  optional &operator=(optional &other) = delete;

  optional(optional &&other) { *this = std::move(other); }
  optional &operator=(optional &&other) {
    if (this != &other) {
      other.has_value = false;
      this->value = std::move(other.value);
      this->has_value = true;
    }

    return *this;
  }

  ~optional() {}

  static optional<T> empty() { return optional(); }

  T &&get_or(T &&default_) {
    if (has_value) {
      has_value = false;
      return std::move(value);
    } else {
      return std::move(default_);
    }
  }

  T &&get() {
    if (has_value) {
      has_value = false;
      return std::move(value);
    } else {
      throw std::runtime_error("Bad optional access");
    }
  }

  bool is_empty() { return !has_value; }
};

} // namespace qs
#endif
