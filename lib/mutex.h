/*
 *  mutex.h  --  mutex locking functions for libTw
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef _TW_MUTEX_H
#define _TW_MUTEX_H


#ifdef CONF_SOCKET_PTHREADS

# define tw_self                 pthread_t
# define tw_mutex                pthread_mutex_t
# define TW_MUTEX_INITIALIZER    PTHREAD_MUTEX_INITIALIZER
# define tw_self_get()	         pthread_self()
# define tw_mutex_init(mutex)    pthread_mutex_init(&(mutex), NULL)
# define tw_mutex_lock(mutex)    pthread_mutex_lock(&(mutex))
# define tw_mutex_unlock(mutex)  pthread_mutex_unlock(&(mutex))
# define tw_mutex_destroy(mutex) pthread_mutex_destroy(&(mutex))

#else /* !CONF_SOCKET_PTHREADS */


# undef  tw_self
# undef  tw_mutex

# define tw_self_get()	         (0)
# define tw_mutex_init(mutex)    do { } while (0)
# define tw_mutex_lock(mutex)    do { } while (0)
# define tw_mutex_unlock(mutex)  do { } while (0)
# define tw_mutex_destroy(mutex) do { } while (0)


#endif /* CONF_SOCKET_PTHREADS */



#endif /* _TW_MUTEX_H */

