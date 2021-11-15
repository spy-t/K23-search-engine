#ifndef QS_ENTRY_HPP
#define QS_ENTRY_HPP
#include <qs/string.h>
#include <memory>
namespace qs{
template <typename T> struct entry{
  qs::string word;
  T payload;
  entry(const qs::string& w,const T& payload): word(w), payload(payload){}
  entry(const qs::string& w,T&& payload): word(w), payload(std::move(payload)){}
  entry(qs::string&& w,const T& payload): word(std::move(w)), payload(payload){}
  entry(qs::string&& w,T&& payload): word(std::move(w)), payload(std::move(payload)){}

};
} // namespace qs

#endif // QS_ENTRY_HPP
