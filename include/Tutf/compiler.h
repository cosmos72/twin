

#ifndef _TUTF_COMPILER_H
#define _TUTF_COMPILER_H

/* library stuff. define macros with TUTF_ prefix */



#if !defined(TUTF_CAT)
# define TUTF__CAT(a,b) a##b
# define TUTF_CAT(a,b) TUTF__CAT(a,b)
# define T_CAT(a,b) TUTF__CAT(a,b)
#endif

#if !defined(TUTF_STR)
# define TUTF__STR(s) #s
# define TUTF_STR(s) TUTF__STR(s)
# define T_STR(s) TUTF__STR(s)
#endif

#if !defined(TUTF_CONST)
# if defined(CONST)
#  define TUTF_CONST CONST
# else
#  define TUTF_CONST const
# endif
#endif

/* inline is NOT a standard C feature :( */
#if !defined(TUTF_INLINE)
# if defined(INLINE)
#  define TUTF_INLINE INLINE
# elif defined(__GNUC__)
#  define TUTF_INLINE extern __inline__
# else
#  define TUTF_INLINE static
# endif
#endif

#if !defined(TUTF_VOLATILE)
# if defined(VOLATILE)
#  define TUTF_VOLATILE VOLATILE
# else
#  define TUTF_VOLATILE volatile
# endif
#endif

#if !defined(TUTF_FNATTR_CONST)
# if defined(FNATTR_CONST)
#  define TUTF_FNATTR_CONST FNATTR_CONST
# elif defined(__GNUC__)
#  define TUTF_FNATTR_CONST __attribute__((const))
# else
#  define TUTF_FNATTR_CONST
# endif
#endif


#endif /* _TUTF_COMPILER_H */

