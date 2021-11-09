#include <qs/hash_set.h>

namespace qs {

bool hash_set::contains(const qs::string &key) {
  qs::optional<char> val =
      this->table.lookup(reinterpret_cast<const uint8_t *>(key.get_buffer()));
  return !val.is_empty();
}

void hash_set::insert(const qs::string &key) {
  this->table.insert(reinterpret_cast<const uint8_t *>(key.get_buffer()), '\0');
}

void hash_set::remove(const qs::string &key) {
  this->table.remove(reinterpret_cast<const uint8_t *>(key.get_buffer()));
}

vector<qs::string> hash_set::get_all() {
  auto values = vector<qs::string>();
  // TODO(spyros)
  return values;
}
} // namespace qs
