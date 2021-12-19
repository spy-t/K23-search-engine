#ifndef QS_ITERATE_HPP
#define QS_ITERATE_HPP

namespace qs::functions {
template <typename Iterator, typename F>
void for_each(Iterator begin, Iterator end, F fn) {
  for (; begin != end; begin++) {
    fn(*begin);
  }
}

} // namespace qs::functions
#endif // QS_ITERATE_HPP
