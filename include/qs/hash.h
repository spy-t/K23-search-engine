#ifndef QS_HASH_H
#define QS_HASH_H

#include <cstdint>

namespace qs {
uint64_t hash_i(const uint8_t *el, int i);

uint64_t djb2(const uint8_t *str);
uint64_t sdbm(const uint8_t *str);

} // namespace qs
#endif
