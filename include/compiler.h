
/* server internal stuff. define macros without any prefix */

#ifndef _TWIN_COMPILER_H
#define _TWIN_COMPILER_H




#if !defined(CAT)
# define _CAT(a,b) a##b
# define CAT(a,b) _CAT(a,b)
#endif

#if !defined(STR)
# define _STR(s) #s
# define STR(s) _STR(s)
#endif


#if !defined(CONST)
# define CONST const
#endif

/* inline is NOT a standard C feature :( */
#if !defined(INLINE)
# if defined(__GNUC__)
#  define INLINE static __inline__
# elif defined(inline)
#  define INLINE inline
# else
#  define INLINE static
# endif
#endif

#if !defined(VOLATILE)
# define VOLATILE volatile
#endif

#if !defined(FNATTR_CONST)
# if defined(__GNUC__)
#  define FNATTR_CONST __attribute__((const))
# else
#  define FNATTR_CONST
# endif
#endif




#endif /* _TWIN_COMPILER_H */

