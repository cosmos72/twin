/*  Copyright (C) 2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */



/* server internal stuff. define macros without any prefix */

#ifndef _TWIN_COMPILER_H
#define _TWIN_COMPILER_H




#if !defined(CAT)
# define _CAT(a,b) a##b
# define CAT(a,b) _CAT(a,b)

# define _CAT3(a,b,c) a##b##c
# define CAT3(a,b,c) _CAT3(a,b,c)

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


#ifndef CONST
# define CONST const
#endif

#ifndef VOLATILE
# if defined(HAVE_VOLATILE) || defined(volatile)
#  define VOLATILE volatile
# else
#  define VOLATILE
# endif
#endif


/* inline is NOT a standard C89 feature :( */
#ifndef INLINE
# if defined(TW_HAVE_STATIC_INLINE)
#  define INLINE static inline
# elif defined(TW_HAVE_INLINE) || defined(inline)
#  define INLINE inline
# else
#  define INLINE static
# endif
#endif

#endif /* _TWIN_COMPILER_H */

