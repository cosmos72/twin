/*
 *  shm.c  --  a custom version of malloc (not realloc/free) routines
 *             which exclusively uses a shared mmapped file to get memory.
 *             this implementation is really dumb, but does what is needed.
 *
 *  Copyright (C) 2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */


#include <sys/stat.h>

#ifdef CONF_WM_RC_SHMMAP
# include <sys/mman.h>
#endif

#include "twin.h"
#include "main.h"
#include "data.h"
#include "printk.h"

#include "wm.h"
#include "rctypes.h"
#include "rcproto.h"
#include "rcrun.h"

#if !defined(CONF_WM_RC_SHMMAP) && defined(CONF_WM_RC_SHRINK)
static byte may_shrink = TRUE;
#endif


static byte *M;  /* the memory pool */
static size_t L; /* its length */
static byte *S;  /* the first available address */ 
static byte *E;  /* 1 + the last available address */

static byte *TSR_M; /* the _previous_ memory pool */
static size_t TSR_L; /* its length */

#define ALIGN 15

/* look at WriteGlobals() to find the reason of this size */
#define _GL_SIZE (sizeof(Globals) + sizeof(All->ButtonVec) + sizeof(GlobalFlags) + sizeof(GlobalShadows))
#define GL_SIZE ((_GL_SIZE & ~ALIGN) + ALIGN + 1)


static size_t full_write(int fd, CONST byte *data, size_t len) {
    size_t left = len;
    int r;
    
    do {
	r = write(fd, data, left);
	if (r > 0) {
	    data += r;
	    left -= r;
	}
    } while (left && (r > 0 || (r == -1 && errno == EINTR)));
    
    return len - left;
}

static size_t full_read(int fd, byte *data, size_t len) {
    size_t left = len;
    int r;
    
    do {
	r = read(fd, data, left);
	if (r > 0) {
	    data += r;
	    left -= r;
	}
    } while (left && (r > 0 || (r == -1 && errno == EINTR)));

    return len - left;
}

#if !defined(CONF_WM_RC_SHMMAP) && defined(CONF_WM_RC_SHRINK)
static void shm_shrink_error(void) {
    
    may_shrink = FALSE;
    printk("twin: shm_shrink(): "
# ifdef CONF__ALLOC
	  "ReAllocMem"
# else
	  "realloc"
# endif
	  "() relocated memory while shrinking! \n"
# ifdef CONF__ALLOC
	  "      This should not happen! Please report.\n"
# endif
	  "      CONF_WM_RC_SHRINK disabled.\n"
	  "      Reconfigure and recompile to disable it permanently.\n");
}
#endif /* !defined(CONF_WM_RC_SHMMAP) && defined(CONF_WM_RC_SHRINK) */




#ifdef CONF_WM_RC_SHMMAP




/* value returned on mmap() failure */
#define NOCORE ((void *)-1)

static byte shmfile[]="/tmp/.Twin_shm\0\0\0\0";

#define TW_PAGE_ALIGN_DOWN(l) ((l) & ~(size_t)(TW_PAGE_SIZE - 1))
#define TW_PAGE_ALIGN_UP(l) (((l) + (TW_PAGE_SIZE - 1)) & ~(size_t)(TW_PAGE_SIZE - 1))

byte shm_init(size_t len) {
    int fd = NOFD;
    
    CopyMem(TWDisplay, shmfile+14, lenTWDisplay);
    unlink(shmfile);

    len += GL_SIZE;
    
    len = TW_PAGE_ALIGN_UP(len + GL_SIZE);

    if ((fd = open(shmfile, O_RDWR|O_CREAT|O_TRUNC|O_EXCL, 0600)) >= 0) {
	if (((L = len), lseek(fd, L-1, SEEK_SET) == L-1) &&
	    write(fd, "", 1) == 1) {

#ifndef MAP_FILE
# define MAP_FILE 0
#endif
	    
#ifdef CONF__ALLOC
	    if ((M = S = (void *)mmap((byte *)AllocStatHighest() + (1l<<24), L, PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED, fd, 0)) == NOCORE)
#endif		
		M = S = (void *)mmap(0, L, PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED, fd, 0);

	    if (M != NOCORE) {
		S += GL_SIZE; /* reserve the first part for globals */
		E = M + L;
		
		close(fd);
		unlink(shmfile);
		return TRUE;
	    }
	}
	close(fd);
	unlink(shmfile);
    }
    return FALSE;
}


/*
 * shrink (M, M+L) to (M, S)
 */
byte shm_shrink(void) {
    size_t new_L = TW_PAGE_ALIGN_UP((size_t)(S - M));
   
    if (new_L < L) {
	munmap(M + new_L, L - new_L);
	L = new_L;
    }
    
    return TRUE;
}

void shm_abort(void) {
    munmap(M, L);
    M = NULL;
}

void shm_TSR_abort(void) {
    if (TSR_M)
	(void)munmap(TSR_M, TSR_L);
    TSR_M = NULL;
}

/*
 * Terminate Stay Resident :
 *
 * free (TSR_M, TSR_M+TSR_L),
 * install (M, S) as the new (TSR_M, TSR_M+TSR_L)
 *
 */
void shm_TSR(void) {
    shm_TSR_abort();
    TSR_M = M;
    TSR_L = L;
    M = NULL;
}

void shm_quit(void) {
    shm_TSR_abort();
    shm_abort();
}

/*
 * send all data structures through the fd
 * (not needed in this case, we have a real shared memory
 * so we just send the value of S to mark end of meaningful data)
 */
byte shm_send(int fd) {
    size_t len = S - M;

    return full_write(fd, (CONST byte *)&len, sizeof(size_t)) == sizeof(size_t);
}

byte shm_receive(int fd) {
    size_t len;
    
    if (full_read(fd, (byte *)&len, sizeof(size_t)) == sizeof(size_t)) {
	S = M + len;
	return TRUE;
    }
    return FALSE;
}


#else /* CONF_WM_RC_SHMMAP */






byte shm_init(size_t len) {
    
    M = S = AllocMem(L = len + 4096);
    if (M) {
	S += GL_SIZE; /* reserve the first part for globals */
	E = M + L;
    }
    return !!M;
}

/*
 * shrink (M, M+L) to (M, S)
 */
byte shm_shrink(void) {
#ifdef CONF_WM_RC_SHRINK
    if (may_shrink) {
	size_t new_L = (size_t)(S - M);
	
	if (new_L < L) {
	    void *new_M = ReAllocMem(M, new_L);
	
	    if (new_M == M) {
		L = new_L;
		return TRUE;
	    }
	    if (new_M) {
		/* ReAllocMem relocated ! inform the user and disable may_shrink ! */
		shm_shrink_error();
		M = new_M;
		L = new_L;
	    }
	    /* ReAllocMem failed or relocated... the only thing to do here is shm_abort() */
	    return FALSE;
	}
    }
#endif /* CONF_WM_RC_SHRINK */
    return TRUE;
}


void shm_abort(void) {
    if (M)
	FreeMem(M);
    M = NULL;
}

void shm_TSR_abort(void) {
    if (TSR_M)
	FreeMem(TSR_M);
    TSR_M = NULL;
}

/*
 * Terminate Stay Resident :
 * 
 * free (TSR_M, TSR_M+TSR_L),
 * install (M, S) as the new (TSR_M, TSR_M+TSR_L)
 * 
 */
void shm_TSR(void) {
    shm_TSR_abort();
    TSR_M = M;
    TSR_L = L;
    M = NULL;
}

void shm_TSR_quit(void) {
    shm_TSR_abort();
    shm_abort();
}

/*
 * send all data structures through the fd
 * (we don't have a real shared memory now,
 * so we must really send all data)
 */
byte shm_send(int fd) {
    size_t len = S - M;

    *(size_t *)M = len;
    
    return full_write(fd, M, len) == len;
}

static byte full_read0(int fd, byte *data, size_t *dlen) {
    size_t len, first = full_read(fd, data, sizeof(size_t));

    if (first == sizeof(size_t)) {
	len = *(size_t *)data - first;
	data += first;
	*dlen = first + full_read(fd, data, len);
	return *dlen == first + len;
    }
    *dlen = first;
    return FALSE;
}

byte shm_receive(int fd) {
    size_t len;
    
    if (full_read0(fd, M, &len)) {
	/*
	 * also update S, it allows strict FAILc() checks
	 */
	S = M + len;
	return TRUE;
    }
    return FALSE;
}




#endif /* CONF_WM_RC_SHMMAP */





/* important: memory returned by shm_malloc() must be full of zeros! */

void *shm_malloc(size_t len) {
    byte *ret, *retE;
    int delta;

    if (!len)
	return NULL;
    
    /*      if allocating a multiple of 16 bytes, align return value at 16 bytes */
    /* else if allocating a multiple of  8 bytes, align return value at  8 bytes */
    /* else if allocating a multiple of  4 bytes, align return value at  4 bytes */
    /* else if allocating a multiple of  2 bytes, align return value at  2 bytes */
    /* else don't align return value */
    if ((delta = ffs(len & ALIGN)) > 1)
	delta = (2<<(delta-2)) - 1;
    else
	delta = 0;
    
    ret = S;
    if ((size_t)ret & delta)
	ret += delta + 1 - ((size_t)ret & delta);
    
    retE = ret + len;

    if (retE <= E) {
	S = retE;
#ifdef DEBUG_SHM
	printk("%.8X  ", (size_t)ret);
#endif
#ifndef CONF_WM_RC_SHMMAP
	/* important: memory returned by shm_malloc() must be full of zeros! */
	WriteMem(ret, '\0', len);
#endif
	return (void *)ret;
    }
    return NULL;
}


void *shm_malloc_or_die(size_t len) {
    void *m = shm_malloc(len);
    if (m || !len)
	return m;
    printk("twin: RC: Out of shared memory!\n");
    exit(1);
}

byte *shm_strdup_or_die(CONST byte *s) {
    size_t len = LenStr(s) + 1;
    byte *d = shm_malloc_or_die(len);
    CopyMem(s, d, len);
    return d;
}

void *shm_getbase(void) {
    return M + sizeof(size_t); /* skip space for placing shm length */
}

void *shm_getend(void) {
    return S;
}

