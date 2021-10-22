#ifndef QS_OPTIONAL_H
#define QS_OPTIONAL_H
namespace qs {

template <typename T> class optional {
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
  static optional<T> empty() { return optional(); }

  T get_or(T default_) {
    if (has_value) {
      return value;
    } else {
      return default_;
    }
  }

  bool is_empty() { return !has_value; }
};

} // namespace qs
#endif
