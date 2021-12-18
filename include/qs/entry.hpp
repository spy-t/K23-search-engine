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

  string_view get_string_view() const { return this->word; }
};

} // namespace qs

#endif // QS_ENTRY_HPP
