

#ifndef TW_COMPILER_H
#define TW_COMPILER_H

/* library stuff. define macros with TW_ prefix */

#ifndef TW_STR
#define TW__STR(s) #s
#define TW_STR(s) TW__STR(s)
#endif

#ifndef TW_CAT
#define TW__CAT(a, b) a##b
#define TW_CAT(a, b) TW__CAT(a, b)
#endif

#ifndef TW_CAT3
#define TW__CAT3(a, b, c) a##b##c
#define TW_CAT3(a, b, c) TW__CAT3(a, b, c)
#endif

#ifndef TW_CAT3
#define TW__CAT4(a, b, c, d) a##b##c##d
#define TW_CAT4(a, b, c, d) TW__CAT4(a, b, c, d)
#endif

#ifndef TW_VOLATILE
#if defined(TW_HAVE_VOLATILE) || defined(volatile)
#define TW_VOLATILE volatile
#else
#define TW_VOLATILE
#endif
#endif

/* inline is NOT a standard C89 feature :( */
#ifndef TW_INLINE
#if defined(TW_HAVE_STATIC_INLINE)
#define TW_INLINE static inline
#elif defined(TW_HAVE_INLINE) || defined(inline)
#define TW_INLINE inline
#else
#define TW_INLINE static
#endif
#endif

#endif /* TW_COMPILER_H */
