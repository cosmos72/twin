
#ifndef _TT_ASSERT_H
#define	_TT_ASSERT_H

/* this prints an "assertion failed" message. */
void TTAssertFail (char *assertion, char *file, unsigned int line, char *function);

# define TTAssertAlways(expr)			\
    ((expr) ? 1 :				\
	 (TTAssertFail (#expr, __FILE__, __LINE__, TT_ASSERT_FUNCTION), 0))


#ifndef TT_ASSERT_LEVEL
# define TT_ASSERT_LEVEL 2
#endif

#if TT_ASSERT_LEVEL == 0
# define TTAssert(expr) TRUE
#elif TT_ASSERT_LEVEL == 1
# define TTAssert(expr) (expr)
#else
# define TTAssert(expr)				\
    ((expr) ? 1 :				\
	 (TTAssertFail (#expr, __FILE__, __LINE__, TT_ASSERT_FUNCTION), 0))
#endif


/* Version 2.4 and later of GCC define a magical variable `__PRETTY_FUNCTION__'
   which contains the name of the function currently being defined.
#  define __ASSERT_FUNCTION	__PRETTY_FUNCTION__
   This is broken in G++ before version 2.6.
   C9x has a similar variable called __func__, but prefer the GCC one since
   it demangles C++ function names.  */
# if defined(__GNUC__) && \
    (__GNUC__ > 2 || (__GNUC__ == 2 \
			  && __GNUC_MINOR__ >= (defined(__cplusplus) ? 6 : 4)))

#  define TT_ASSERT_FUNCTION	__PRETTY_FUNCTION__

# elif defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L

#  define TT_ASSERT_FUNCTION	__func__

# else

#  define TT_ASSERT_FUNCTION	""

# endif

#endif /* _TT_ASSERT_H */

