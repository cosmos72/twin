


#ifndef _TW_COMPILER_H
#define _TW_COMPILER_H

/* library stuff. define macros with TW_ prefix */






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









#if defined(_TWIN_H) && !defined(_TW_H)

/* server internal stuff. define macros without any prefix */

#ifndef _TWIN_COMPILER_H
#define _TWIN_COMPILER_H






#if !defined(CONST)
# define CONST const
#endif

/* inline is NOT a standard C feature :( */
#if !defined(INLINE)
# define INLINE static
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

#endif /* defined(_TWIN_H) && !defined(_TW_H) */

