


#ifndef _TW_COMPILER_H
#define _TW_COMPILER_H

/* library stuff. define macros with TW_ prefix */


#define TW__CAT(a,b)		a##b
#define TW__CAT3(a,b,c)		a##b##c
#define TW__CAT4(a,b,c,d)	a##b##c##d
#define TW_CAT(a,b)		TW__CAT(a,b)
#define TW_CAT3(a,b,c)		TW__CAT3(a,b,c)
#define TW_CAT4(a,b,c,d)	TW__CAT4(a,b,c,d)

#define TW__STR(s) #s
#define TW_STR(s) TW__STR(s)

#if defined(CONST)
# define TW_CONST CONST
#else
# define TW_CONST const
#endif

/* inline is NOT a standard C feature :( */
#if defined(INLINE)
# define TW_INLINE INLINE
#elif defined(TW_HAVE_STATIC_INLINE)
# define TW_INLINE static inline
#elif defined(TW_HAVE_INLINE) || defined(inline)
# define TW_INLINE inline
#else
# define TW_INLINE static
#endif

#if defined(TW_HAVE_STATIC_INLINE) || defined(TW_HAVE_INLINE) || defined(inline)
# define TW_CAN_INLINE 1
#endif

#if defined(VOLATILE)
# define TW_VOLATILE VOLATILE
#else
# define TW_VOLATILE volatile
#endif

#if defined(FN_ATTR_CONST)
# define TW_FN_ATTR_CONST FN_ATTR_CONST
#elif defined(TW_HAVE_ATTRIBUTE_CONST)
# define TW_FN_ATTR_CONST __attribute__((const))
#else
# define TW_FN_ATTR_CONST
#endif

#if defined(TYPE_ATTR_PACKED)
# define TW_TYPE_ATTR_PACKED TYPE_ATTR_PACKED
#elif defined(TW_HAVE_ATTRIBUTE_PACKED)
# define TW_TYPE_ATTR_PACKED __attribute__((packed))
#else
# define TW_TYPE_ATTR_PACKED
#endif




#endif /* _TW_COMPILER_H */



