#include <qs/hash_set.h>

namespace qs {

bool hash_set::contains(const qs::string &key) {
  return this->table.lookup(key) != this->table.end();
}

void hash_set::insert(const qs::string &key) {
  this->table.insert(qs::string(key), '\0');
}
void hash_set::insert(qs::string &&key) {
  this->table.insert(std::move(key), '\0');
}

void hash_set::remove(const qs::string &key) { this->table.remove(key); }
std::size_t hash_set::get_size() { return table.get_size(); }

} // namespace qs
