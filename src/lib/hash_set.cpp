#include <qs/hash_set.h>

namespace qs {

bool hash_set::contains(const qs::string &key) {
  return this->table.lookup(reinterpret_cast<const uint8_t *>(
             key.get_buffer())) != this->table.end();
}

void hash_set::insert(const qs::string &key) {
  this->table.insert(reinterpret_cast<const uint8_t *>(key.get_buffer()), '\0');
}

void hash_set::remove(const qs::string &key) {
  this->table.remove(reinterpret_cast<const uint8_t *>(key.get_buffer()));
}

vector<qs::string> hash_set::get_all() {
  auto values = vector<qs::string>(this->table.get_size());
  for (auto &n : this->table) {
    values.push(qs::string((const char *)n.get_key()));
  }
  return values;
}
} // namespace qs
