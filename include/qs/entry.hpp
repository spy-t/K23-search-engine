#ifndef QS_ENTRY_HPP
#define QS_ENTRY_HPP
#include <qs/bk_tree.hpp>
#include <qs/distances.hpp>
#include <qs/string.h>
#include <qs/string_view.h>

namespace qs {
template <typename T> struct entry {
  qs::string_view word;
  T payload;
  explicit entry(qs::string_view w) : word(w), payload(T{}) {}
  entry(qs::string_view w, const T &p) : word(w), payload(p) {}
  entry(qs::string_view w, T &&p) : word(w), payload(std::move(p)) {}
};

template <typename T>
class edit_dist : public distance_func<entry<T>, qs::string_view> {
  int operator()(const entry<T> &a, const entry<T> &b) const override {
    return edit_distance(a.word, b.word);
  }
  int operator()(const entry<T> &a, const qs::string_view &b,
                 int max) const override {
    return edit_distance(a.word, b, max);
  }
  int operator()(const entry<T> &a, const qs::string_view &b) const override {
    return edit_distance(a.word, b);
  }
};

template <typename T>
struct hamming_dist : public distance_func<entry<T>, qs::string_view> {
  int operator()(const entry<T> &a, const entry<T> &b) const override {
    return hamming_distance(a.word, b.word);
  }
  int operator()(const entry<T> &a, const qs::string_view &b,
                 int max) const override {
    return hamming_distance(a.word, b, max);
  }
  int operator()(const entry<T> &a, const qs::string_view &b) const override {
    return hamming_distance(a.word, b);
  }
};

} // namespace qs

#endif // QS_ENTRY_HPP
