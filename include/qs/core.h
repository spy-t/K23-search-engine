#ifndef QS_CORE_H
#define QS_CORE_H

#ifndef QS_DEBUG
#define QS_FORCE_INLINE inline __attribute__((always_inline))
#else
#define QS_FORCE_INLINE inline
#endif

#endif
