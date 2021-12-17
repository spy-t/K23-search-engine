#ifndef QS_ENTRY_HPP
#define QS_ENTRY_HPP
#include <qs/bk_tree.hpp>
#include <qs/distances.hpp>
#include <qs/string.h>
namespace qs {
template <typename T> struct entry {
  qs::string word;
  T payload;
  entry(qs::string w) : word(std::move(w)), payload(T{}) {}
  entry(qs::string w, const T &payload)
      : word(std::move(w)), payload(payload) {}
  entry(qs::string w, T &&payload)
      : word(std::move(w)), payload(std::move(payload)) {}
  entry(qs::string &&w, const T &payload)
      : word(std::move(w)), payload(payload) {}
  entry(qs::string &&w, T &&payload)
      : word(std::move(w)), payload(std::move(payload)) {}
};

template <typename T>
class edit_dist : public distance_func<entry<T>, qs::string> {
  int operator()(const entry<T> &a, const entry<T> &b) const override {
    return edit_distance(a.word, b.word);
  }
  int operator()(const entry<T> &a, const qs::string &b,
                 int max) const override {
    return edit_distance(a.word, b, max);
  }
};

template <typename T>
struct hamming_dist : public distance_func<entry<T>, qs::string> {
  int operator()(const entry<T> &a, const entry<T> &b) const override {
    return hamming_distance(a.word, b.word);
  }
  int operator()(const entry<T> &a, const qs::string &b,
                 int max) const override {
    return hamming_distance(a.word, b, max);
  }
};

} // namespace qs

#endif // QS_ENTRY_HPP
