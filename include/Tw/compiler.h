


#ifndef _TW_COMPILER_H
#define _TW_COMPILER_H

/* library stuff. define macros with TW_ prefix */



#if !defined(TW_CAT)
# define TW__CAT(a,b) a##b
# define TW_CAT(a,b) TW__CAT(a,b)
#endif

#if !defined(TW_STR)
# define TW__STR(s) #s
# define TW_STR(s) TW__STR(s)
#endif

#if !defined(TW_CONST)
# if defined(CONST)
#  define TW_CONST CONST
# else
#  define TW_CONST const
# endif
#endif

/* inline is NOT a standard C feature :( */
#if !defined(TW_INLINE)
# if defined(INLINE)
#  define TW_INLINE INLINE
# elif defined(__GNUC__)
#  define TW_INLINE extern __inline__
# elif defined(inline)
#  define TW_INLINE inline
# else
#  define TW_INLINE static
# endif
#endif

#if !defined(TW_VOLATILE)
# if defined(VOLATILE)
#  define TW_VOLATILE VOLATILE
# else
#  define TW_VOLATILE volatile
# endif
#endif

#if !defined(TW_FNATTR_CONST)
# if defined(FNATTR_CONST)
#  define TW_FNATTR_CONST FNATTR_CONST
# elif defined(__GNUC__)
#  define TW_FNATTR_CONST __attribute__((const))
# else
#  define TW_FNATTR_CONST
# endif
#endif




#endif /* _TW_COMPILER_H */



