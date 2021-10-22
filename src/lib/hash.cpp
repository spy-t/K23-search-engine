#include <qs/hash.h>

namespace qs {
uint64_t hash_i(const uint8_t *el, int i) {
  return djb2(el) + i * sdbm(el) + i * i;
}

uint64_t djb2(const uint8_t *str) {
  uint64_t hash = 5381;
  int c;
  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }
  return hash;
}

uint64_t sdbm(const uint8_t *str) {
  uint64_t hash = 0;
  int c;

  while ((c = *str++)) {
    hash = c + (hash << 6) + (hash << 16) - hash;
  }

  return hash;
}
} // namespace qs
