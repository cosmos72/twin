
/* server internal stuff. define macros without any prefix */

#ifndef _TWIN_COMPILER_H
#define _TWIN_COMPILER_H




#if !defined(CAT)
# define _CAT(a,b) a##b
# define CAT(a,b) _CAT(a,b)

# define _CAT3(a,b,c,d) a##b##c
# define CAT3(a,b,c,d) _CAT3(a,b,c)

# define _CAT4(a,b,c,d) a##b##c##d
# define CAT4(a,b,c,d) _CAT4(a,b,c,d)

# define _CAT5(a,b,c,d,e) a##b##c##d##e
# define CAT5(a,b,c,d,e) _CAT5(a,b,c,d,e)

# define _CAT6(a,b,c,d,e,f) a##b##c##d##e##f
# define CAT6(a,b,c,d,e,f) _CAT6(a,b,c,d,e,f)

# define _CAT8(a,b,c,d,e,f,g,h) a##b##c##d##e##f##g##h
# define CAT8(a,b,c,d,e,f,g,h) _CAT8(a,b,c,d,e,f,g,h)

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
# if defined(HAVE_STATIC_INLINE)
#  define INLINE static inline
# elif defined(HAVE_INLINE) || defined(inline)
#  define INLINE inline
# endif
#else
# define INLINE static
#endif

#if !defined(VOLATILE)
# define VOLATILE volatile
#endif

#if !defined(FN_ATTR_CONST)
# if defined(HAVE_ATTRIBUTE_CONST)
#  define FN_ATTR_CONST __attribute__((const))
# else
#  define FN_ATTR_CONST
# endif
#endif

#if !defined(TYPE_ATTR_PACKED)
# if defined(HAVE_ATTRIBUTE_PACKED)
#  define TYPE_ATTR_PACKED __attribute__((packed))
# else
#  define TYPE_ATTR_PACKED
# endif
#endif




#endif /* _TWIN_COMPILER_H */

