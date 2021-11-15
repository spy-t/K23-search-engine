#ifndef QS_ENTRY_HPP
#define QS_ENTRY_HPP
#include <memory>
#include <qs/distances.hpp>
#include <qs/string.h>
namespace qs {
template <typename T> struct entry {
  qs::string word;
  T payload;
  entry(const qs::string &w, const T &payload) : word(w), payload(payload) {}
  entry(const qs::string &w, T &&payload)
      : word(w), payload(std::move(payload)) {}
  entry(qs::string &&w, const T &payload)
      : word(std::move(w)), payload(payload) {}
  entry(qs::string &&w, T &&payload)
      : word(std::move(w)), payload(std::move(payload)) {}

  static int edit_distance(entry<T> &a, entry<T> &b) {
    return qs::edit_distance(a.word, b.word);
  }
  static int hamming_distance(entry<T> &a, entry<T> &b) {
    return qs::hamming_distance(a.word, b.word);
  }
};

} // namespace qs

#endif // QS_ENTRY_HPP
