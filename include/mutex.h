/*
 *  mutex.h  --  mutex locking functions for libTw, libTT, ...
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef _TWIN_MUTEX_H
#define _TWIN_MUTEX_H


#ifdef CONF_SOCKET_PTHREADS

# define th_self                 pthread_t
# define th_self_none            ((th_self)-1)
# define th_self_get()           pthread_self()
# define th_mutex                pthread_mutex_t
# define TH_MUTEX_INITIALIZER    PTHREAD_MUTEX_INITIALIZER
# define th_mutex_init(__mx)     pthread_mutex_init(&(__mx), NULL)
# define th_mutex_destroy(__mx)  pthread_mutex_destroy(&(__mx))
# define th_mutex_lock(__mx)     pthread_mutex_lock(&(__mx))
# define th_mutex_unlock(__mx)   pthread_mutex_unlock(&(__mx))


/* implementation of recursive mutexes. use native ones if available, else emulate. */

# ifdef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP

#  define th_r_mutex                th_mutex
#  define TH_R_MUTEX_INITIALIZER    PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
#  define th_r_mutex_init(__mx)     th_mutex_init(__mx)
#  define th_r_mutex_destroy(__mx)  th_mutex_destroy(__mx)
#  define th_r_mutex_lock(__mx)     th_mutex_lock(__mx)
#  define th_r_mutex_unlock(__mx)   th_mutex_unlock(__mx)
#  define TH_R_MUTEX_HELPER_DEFS(attr) typedef th_r_mutex _th_r_mutex

#else

#  warning no recursive pthreads available, trying emulation

#define th_r_mutex th_r_mutex
typedef struct {
    unsigned __c;
    th_self __s;
    th_mutex __m1, __m2;
} th_r_mutex;
#  define TH_R_MUTEX_INITIALIZER    { 0, th_self_none, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER }
#  define th_r_mutex_init(__mx)    do { \
    (__mx).__c = 0; \
    (__mx).__s = th_self_none; \
    th_mutex_init((__mx).__m1); \
    th_mutex_init((__mx).__m2); \
} while (0)
#  define th_r_mutex_destroy(__mx) do { \
    th_mutex_destroy((__mx).__m2); \
    th_mutex_destroy((__mx).__m1); \
} while (0)
#  define th_r_mutex_lock(__mx)    _th_r_mutex_lock(&(__mx))
#  define th_r_mutex_unlock(__mx)  _th_r_mutex_unlock(&(__mx))

#  define TH_R_MUTEX_HELPER_DEFS(attr) \
    attr void _th_r_mutex_lock(th_r_mutex * __mx) { \
	th_self __self = th_self_get(); \
	\
	th_mutex_lock(__mx->__m1); \
	if (__mx->__s == th_self_none || __mx->__s == __self) { \
	    __mx->__s = __self; \
	    __mx->__c++; \
	} else { \
	    th_mutex_unlock(__mx->__m1); \
	    /* some other thread might steal us __mx->__m2 here, but that's semantically ok */ \
	    th_mutex_lock(__mx->__m2); \
	    th_mutex_lock(__mx->__m1); \
	    __mx->__s = __self; \
	    __mx->__c++; \
	} \
	th_mutex_unlock(__mx->__m1); \
    } \
    attr void _th_r_mutex_unlock(th_r_mutex * __mx) { \
	unsigned __c = 1; \
	th_mutex_lock(__mx->__m1); \
	if (__mx->__s == th_self_get()) \
	    if (!(__c = --__mx->__c)) \
		__mx->__s = th_self_none; \
	th_mutex_unlock(__mx->__m1); \
	if (!__c) \
	    th_mutex_unlock(__mx->__m2); \
    } \
    typedef th_r_mutex _th_r_mutex


# endif

#else /* !CONF_SOCKET_PTHREADS */


# undef  th_self
# define th_self_none            0
# define th_self_get()           0
# undef  th_mutex
# define TH_MUTEX_INITIALIZER    {}
# define th_mutex_init(__mx)    do { } while (0)
# define th_mutex_lock(__mx)    do { } while (0)
# define th_mutex_unlock(__mx)  do { } while (0)
# define th_mutex_destroy(__mx) do { } while (0)

# undef  th_r_mutex
# define TH_R_MUTEX_INITIALIZER    {}
# define th_r_mutex_init(__mx)    do { } while (0)
# define th_r_mutex_lock(__mx)    do { } while (0)
# define th_r_mutex_unlock(__mx)  do { } while (0)
# define th_r_mutex_destroy(__mx) do { } while (0)
# define TH_R_MUTEX_HELPER_DEFS(attr) typedef int _th_r_mutex




#endif /* CONF_SOCKET_PTHREADS */



#endif /* _TWIN_MUTEX_H */

