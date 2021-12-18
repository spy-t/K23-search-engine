#ifndef QS_CORE_H
#define QS_CORE_H

#include <cstdint>

#ifndef QS_DEBUG
#define QS_FORCE_INLINE inline __attribute__((always_inline))
#else
#define QS_FORCE_INLINE inline
#endif

typedef uint8_t byte;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t ibyte;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#endif
