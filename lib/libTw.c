/*
 *  libTw.c  --  implementation of libTw functions
 *
 *  Copyright (C) 1999-2001 by Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA
 *
 */

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>

#ifdef CONF_SOCKET_GZ
# include <zlib.h>
#endif

#ifdef CONF_SOCKET_PTHREADS
# include <pthread.h>
#endif

#include "libTw.h"
#include "libTwerrno.h"
#include "mutex.h"

#include "unaligned.h"
#include "md5.h"
#include "version.h"


/* early check libTw.h against sockproto.h */
#include "sockparanoia.h"


#ifdef CONF_SOCKET_PTHREADS

typedef struct s_tw_errno {
    uldat E;
    uldat S;
    tw_self T;
} s_tw_errno;

typedef struct s_tw_errno_vec {
    s_tw_errno *vec;
    uldat max, last;
} s_tw_errno_vec;

#else

/* use libTwerrno.h types */
typedef tw_errno s_tw_errno;
typedef tw_errno s_tw_errno_vec;

#endif

typedef struct s_fn_list {
    void *Fn;
    byte *name;
    byte len;
} fn_list;

static fn_list Functions[] = {
#   define EL(funct) { Tw_##funct, "Tw_" #funct, 0 },
#   include "sockprotolist.h"
#   undef EL
    {NULL, NULL, 0 }
};

typedef uldat v_id_vec [ sizeof(Functions) / sizeof(Functions[0]) ];

/*
 * automagically get the symbols order_* to be the index
 * of corresponding element in Functions[] :
 * enums start at 0 and automatically increase...
 */
typedef enum e_fn_order {
#   define EL(funct) order_##funct,
#   include "sockprotolist.h"
#   undef EL
} fn_order;

#define order(funct) ((uldat)order_##funct - (uldat)order_FindFunction)

static void InitFunctions(void) {
    fn_list *f = Functions;
    while (f->name) {
	f->len = Tw_LenStr(3 + f->name);
	f++;
    }
}


#define QREAD    0
#define QWRITE   1
#define QMSG     2
#define QgzREAD  3
#define QgzWRITE 4
#define QMAX	 5

typedef struct s_tw_d {
#ifdef tw_mutex
    tw_mutex mutex;
#endif
    
    byte *Queue[5];
    uldat Qstart[5], Qlen[5], Qmax[5];
    /*
     * since DeQueue(Q[gz]WRITE) cancels bytes from end, not from start
     * 
     * Qstart[QWRITE] is used only if Tw_TimidFlush()
     * fails writing the whole queue,
     * while Qstart[QgzWRITE] is not used at all.
     */
    
    uldat *r;
    byte *s;

    int Fd;
    uldat RequestN;

    tlistener FirstListener, LastListener;
    tfn_default_listener DefaultListener;
    void *DefaultArg;

    s_tw_errno_vec rErrno_;

    byte ServProtocol[3];
    byte PanicFlag;

#ifdef CONF_SOCKET_GZ
    byte GzipFlag;
    z_streamp zR, zW;
#endif

    v_id_vec id_vec;
    
} *tw_d;

#define rErrno	(TwD->rErrno_)
#define mutex	(TwD->mutex)
#define id_Tw	(TwD->id_vec)
#define Queue	(TwD->Queue)
#define Qstart	(TwD->Qstart)
#define Qlen	(TwD->Qlen)
#define Qmax	(TwD->Qmax)
#define r	(TwD->r)
#define s	(TwD->s)
#define Fd	(TwD->Fd)
#define RequestN  (TwD->RequestN)
#define ServProtocol (TwD->ServProtocol)
#define PanicFlag (TwD->PanicFlag)
#define GzipFlag  (TwD->GzipFlag)
#define zR	(TwD->zR)
#define zW	(TwD->zW)



typedef struct s_reply *reply;
struct s_reply {
    uldat Len, Serial, Code, Data;
};


#define LOCK tw_mutex_lock(mutex)
#define UNLK tw_mutex_unlock(mutex)
#define NO_LOCK do { } while (0)
#define NO_UNLK do { } while (0)

static s_tw_errno rCommonErrno;
#define CommonErrno (rCommonErrno.E)

static uldat OpenCount;
#ifdef tw_mutex
static tw_mutex OpenCountMutex = TW_MUTEX_INITIALIZER;
#endif

/* and this is the 'default' display */
tw_d Tw_DefaultD;

#ifdef CONF_SOCKET_GZ
static uldat Gzip(tw_d TwD);
static uldat Gunzip(tw_d TwD);
#endif

static byte Sync(tw_d TwD);
static void Panic(tw_d TwD);
static void ParseReplies(tw_d TwD);

byte Tw_EnableGzip(tw_d TwD);

void *(*Tw_AllocMem)(size_t) = malloc;
void *(*Tw_ReAllocMem)(void *, size_t) = realloc;
void  (*Tw_FreeMem)(void *) = free;
byte *(*Tw_CloneStr)(TW_CONST byte *) = (byte *(*)(TW_CONST byte *))strdup;

static byte *clone_str(TW_CONST byte *S) {
    size_t len;
    byte *T;
    if (S) {
	len = 1 + Tw_LenStr(S);
	if ((T = Tw_AllocMem(len)))
	    return Tw_CopyMem(S, T, len);
    }
    return NULL;
}

void Tw_ConfigMalloc(void *(*my_malloc)(size_t),
		     void *(*my_realloc)(void *, size_t),
		     void  (*my_free)(void *)) {
    
    tw_mutex_lock(OpenCountMutex);
    if (!OpenCount) {
	if (my_malloc && my_realloc && my_free) {
	    Tw_AllocMem = my_malloc;
	    Tw_ReAllocMem = my_realloc;
	    Tw_FreeMem = my_free;
	    Tw_CloneStr = clone_str;
	} else {
	    Tw_AllocMem = malloc;
	    Tw_ReAllocMem = realloc;
	    Tw_FreeMem = free;
	    Tw_CloneStr = (byte *(*)(TW_CONST byte *))strdup;
	}
    }
    tw_mutex_unlock(OpenCountMutex);
}

#ifdef CONF_SOCKET_PTHREADS

INLINE byte GrowErrnoLocation(tw_d TwD) {
    s_tw_errno *vec;
    uldat newmax = rErrno.max <= 8 ? 16 : (rErrno.max<<1);
    
    if ((vec = (s_tw_errno *)Tw_ReAllocMem(rErrno.vec, newmax * sizeof(s_tw_errno)))) {

	/* assume (tw_self)-1 is _NOT_ a valid thread identifier */
	Tw_WriteMem(vec + rErrno.max, '\xFF', (newmax-rErrno.max) * sizeof(s_tw_errno));
	rErrno.vec = vec;
	rErrno.max = newmax;
	
	return TRUE;
    }
    /* out of memory! */
    return FALSE;
}

static s_tw_errno *GetErrnoLocation(tw_d TwD) {
    tw_self self;
    uldat i;
    if (TwD) {
	self = tw_self_get();
	
	/* we cache last thread that called GetErrnoLocation() */
	i = rErrno.last;
	if (i < rErrno.max && rErrno.vec[i].T == self)
	    return &rErrno.vec[i];
	
	for (i=0; i<rErrno.max; i++) {
	    if (rErrno.vec[i].T == self)
		break;
	    if (rErrno.vec[i].T == (tw_self)-1) {
		/* empty slot, initialize it */
		rErrno.vec[i].T = self;
		rErrno.vec[i].E = rErrno.vec[i].S = 0;
		break;
	    }
	}
	if (i < rErrno.max) {
	    rErrno.last = i;
	    return &rErrno.vec[i];
	}
	if (GrowErrnoLocation(TwD)) {
	    rErrno.vec[i].E = rErrno.vec[i].S = 0;
	    rErrno.vec[i].T = self;
	    rErrno.last = i; /* i is previous rErrno.max */
	    return &rErrno.vec[i];
	}
    }
    return &rCommonErrno;
}

static void FreeErrnoLocation(tw_d TwD) {
    if (rErrno.vec)
	Tw_FreeMem(rErrno.vec);
}

# define Errno (GetErrnoLocation(TwD)->E)

#else /* !CONF_SOCKET_PTHREADS */

# define GetErrnoLocation(TwD) ((TwD) ? &TwD->rErrno_ : &rCommonErrno)
# define Errno (rErrno.E)
# define FreeErrnoLocation(TwD) do { } while(0)

#endif /* CONF_SOCKET_PTHREADS */

static uldat AddQueue(tw_d TwD, byte i, uldat len, void *data) {
    uldat nmax;
    byte *t;
    
    if (len == 0)
	return len;
    
    /* append to queue */
    if (Qstart[i] + Qlen[i] + len > Qmax[i]) {
	if (Qstart[i])
	    Tw_MoveMem(Queue[i] + Qstart[i], Queue[i], Qlen[i]),
	    Qstart[i] = 0;

	if (Qlen[i] + len > Qmax[i]) {
	    t = (byte *)Tw_ReAllocMem(Queue[i], nmax = (Qmax[i]+len+40)*5/4);
	    if (!t)
		return 0;
	    Queue[i] = t;
	    Qmax[i] = nmax;
	}
    }
    
    Tw_CopyMem(data, Queue[i] + Qstart[i] + Qlen[i], len);
    Qlen[i] += len;
    return len;
}

#define QLeft(Q,len)	(Qlen[Q] + Qstart[Q] + (len) <= Qmax[Q] ? Qlen[Q] += (len) : Grow(TwD, Q, len))
#define RQLeft(len)     QLeft(QREAD,len)
#define WQLeft(len)     QLeft(QWRITE,len)


static uldat Grow(tw_d TwD, byte i, uldat len) {
    /* make enough space available in Queue[i] and mark it used */
    uldat nmax;
    byte *t;
    
    if ((i == QREAD || i == QgzREAD) && Qlen[i] + len < Qmax[i]) {
	Tw_MoveMem(Queue[i] + Qstart[i], Queue[i], Qlen[i]);
	Qstart[i] = 0;
    } else {
	t = (byte *)Tw_ReAllocMem(Queue[i], nmax = (Qmax[i]+len+40)*5/4);
	if (!t)
	    return 0;
	if (i == QWRITE) {
	    r = (uldat *)(t + ((byte *)r - Queue[i]));
	    s = t + (s - Queue[i]);
	}
	Queue[i] = t;
	Qmax[i] = nmax;
    }
    return Qlen[i] += len;
}

INLINE byte *GetQueue(tw_d TwD, byte i, uldat *len) {
    if (len) *len = Qlen[i];
    return Queue[i] + Qstart[i];
}

INLINE byte *FillQueue(tw_d TwD, byte i, uldat *len) {
    uldat delta = Qmax[i] - Qlen[i] - Qstart[i];
    Qlen[i] += delta;
    if (len) *len = delta;
    return Queue[i] + Qstart[i] + Qlen[i] - delta;
}

static uldat *InitRS(tw_d TwD) {
    uldat len;
    if (WQLeft(3*sizeof(uldat))) {
	s = GetQueue(TwD, QWRITE, &len);
	s += len;
	return r = (uldat *)s - 3;
    }
    Errno = TW_ENO_MEM;
    return (uldat *)0;
}

INLINE uldat DeQueue(tw_d TwD, byte i, uldat len) {
    if (!len)
	return len;
    
    switch (i) {
      case QREAD:
      case QMSG:
      case QgzREAD:
	/* QREAD, QMSG: DeQueue() from start (FIFO like) */
	if (len < Qlen[i]) {
	    Qstart[i] += len;
	    Qlen[i] -= len;
	} else {
	    len = Qlen[i];
	    Qstart[i] = Qlen[i] = 0;
	}
	return len;
      case QWRITE:
      case QgzWRITE:
	/* QWRITE: DeQueue() from end (stack like) */

	if (len < Qlen[i]) {
	    Qlen[i] -= len;
	} else {
	    len = Qlen[i];
	    Qlen[i] = 0;
	}
	return len;
      default:
	return (uldat)0;
    }
}

/* remove the given reply from QREAD */
static void KillReply(tw_d TwD, byte *rt, uldat rlen) {
    byte *t;
    uldat len;
    
    rlen += sizeof(uldat);
    t = GetQueue(TwD, QREAD, &len);
    if (rt >= t && rt + rlen <= t + len) {
	if (rt == t)
	    /* same as Qstart[QREAD] = Qlen[QREAD] = 0; */
	    DeQueue(TwD, QREAD, rlen);
	else {
	    if (rt + rlen < t + len) {
		uldat before = rt - t;
		uldat after = t + len - (rt + rlen);
		/* which Tw_MoveMem() copies less data ? */
		if (before <= after) {
		    Tw_MoveMem(t, t + rlen, before);
		    Qstart[QREAD] += rlen;
		} else
		    Tw_MoveMem(rt + rlen, rt, after);
	    }
	    Qlen[QREAD] -= rlen;
	}
    }
}

static void Panic(tw_d TwD) {
    uldat len;
    
    (void)GetQueue(TwD, QREAD, &len);
    DeQueue(TwD, QREAD, len);

    (void)GetQueue(TwD, QWRITE, &len);
    DeQueue(TwD, QWRITE, len);

    (void)GetQueue(TwD, QMSG, &len);
    DeQueue(TwD, QMSG, len);

#ifdef CONF_SOCKET_GZ
    (void)GetQueue(TwD, QgzREAD, &len);
    DeQueue(TwD, QgzREAD, len);

    (void)GetQueue(TwD, QgzWRITE, &len);
    DeQueue(TwD, QgzWRITE, len);
#endif
    
    if (Fd >= 0) {
	close(Fd);
	Fd = NOFD;
    }
    
    PanicFlag = TRUE;
}

byte Tw_InPanic(tw_d TwD) {
    return TwD && PanicFlag;
}

/* cancel the last request packet */
/* you can (must) call Fail() ONLY after a failed WQLeft() */
static void Fail(tw_d TwD) {
    DeQueue(TwD, QWRITE, s - (byte *)r);
}

static byte Flush(tw_d TwD, byte Wait) {
    fd_set fset;
    uldat chunk = 0, left;
    s_tw_errno *E;
    byte *t;
    byte Q;
    int fd;
    
    t = GetQueue(TwD, Q = QWRITE, &left);

    if (Fd != NOFD && left) {

#ifdef CONF_SOCKET_GZ
	if (GzipFlag) {
	    if (Gzip(TwD)) {
		t = GetQueue(TwD, Q = QgzWRITE, &left);
	    } else
		return FALSE; /* TwGzip() calls Panic() if needed */
	}
#endif
	FD_ZERO(&fset);
	
	while (left > 0) {
	    chunk = write(Fd, t, left);
	    if (chunk && chunk != (uldat)-1) {
		/* would be "if (chunk > 0)" but chunk is unsigned */
		t += chunk;
		left -= chunk;		
		if (chunk < Qlen[Q]) {
		    Qstart[Q] += chunk;
		    Qlen[Q] -= chunk;
		} else
		    Qstart[Q] = Qlen[Q] = 0;
	    }
	    else if (chunk == (uldat)-1 && errno == EINTR)
		; /*continue*/
	    else if (chunk == (uldat)-1 && errno == EWOULDBLOCK && Wait) {
		do {
		    fd = Fd;
		    /* release the lock before sleeping!!! */
		    UNLK;
		    FD_SET(fd, &fset);
		    chunk = select(fd+1, NULL, &fset, NULL, NULL);
		    FD_CLR(fd, &fset);
		    LOCK;
		    /* maybe another thread did our work while we slept? */
		    t = GetQueue(TwD, Q, &left);
		    if (!left)
			break;
		} while (chunk == (uldat)-1 && errno == EINTR);
		
		if (chunk == (uldat)-1)
		    break;
	    } else
		break;
	}
    } else
	left = 0;
    
    if (left && Wait) {
	E = GetErrnoLocation(TwD);
	E->E = TW_ECANT_WRITE;
	E->S = errno;
	Panic(TwD);
    }
    return (Fd != NOFD) + (Fd != NOFD && !Wait && left);
}

byte Tw_Flush(tw_d TwD) {
    byte b;
    LOCK; b = Flush(TwD, TRUE); UNLK;
    return b;
}

byte Tw_TimidFlush(tw_d TwD) {
    byte b;
    LOCK; b = Flush(TwD, FALSE); UNLK;
    return b;
}
    
/* return bytes read, or (uldat)-1 for errors */
static uldat TryRead(tw_d TwD, byte Wait) {
    fd_set fset;
    uldat got = 0, len;
    int sel, fd;
    byte *t, mayread;
    byte Q;
    
#ifdef CONF_SOCKET_GZ
    if (GzipFlag)
	Q = QgzREAD;
    else
#endif
	Q = QREAD;
    
    if (Wait) {
	FD_ZERO(&fset);
	do {
	    fd = Fd;
	    /* drop LOCK before sleeping! */
	    UNLK;
	    FD_SET(fd, &fset);
	    sel = select(fd+1, &fset, NULL, NULL, NULL); 
	    FD_CLR(fd, &fset);
	    LOCK;

	    /* maybe another thread received some data? */
	    (void)GetQueue(TwD, QREAD, &len);
	    if (len)
		break;
	} while (sel != 1);
    }
    
    mayread = ioctl(Fd, FIONREAD, &len) >= 0;
    if (!mayread || !len)
	len = SMALLBUFF;
    
    if (QLeft(Q,len)) {
	t = GetQueue(TwD, Q, &got);
	t += got - len;
	do {
	    got = read(Fd, t, len);
	} while (got == (uldat)-1 && errno == EINTR);
	
	Qlen[Q] -= len - (got == (uldat)-1 ? 0 : got);
	
	if (got == 0 || (got == (uldat)-1 && errno != EINTR && errno != EWOULDBLOCK)) {
	    Errno = TW_ELOST_CONN;
	    Panic(TwD);
	    return (uldat)-1;
	}
    }
    if (got == (uldat)-1)
	got = 0;
    
#ifdef CONF_SOCKET_GZ
    if (GzipFlag && got) {
	got = Gunzip(TwD);
    }
#endif
    return got;
}

INLINE reply FindReply(tw_d TwD, uldat Serial) {
    uldat left, rlen;
    reply rt;
    byte *t = GetQueue(TwD, QREAD, &left);
    
    while (left >= sizeof(uldat)) {
	rt = (reply)t;
	Pop(t,uldat,rlen);
	left -= sizeof(uldat);
	if (rt->Serial == Serial)
	    return rt;
	t += rlen;
	left -= rlen;
    }
    return (reply)0;
}


static reply Wait4Reply(tw_d TwD, uldat Serial) {
    uldat left;
    reply MyReply = (reply)0;
    if (Fd != NOFD && ((void)GetQueue(TwD, QWRITE, &left), left)) {
	if (Flush(TwD, TRUE)) while (Fd != NOFD && !(MyReply = FindReply(TwD, Serial)))
	    if (TryRead(TwD, TRUE) != (uldat)-1)
		ParseReplies(TwD);
    }
    return Fd != NOFD ? MyReply : (reply)0;
}

static uldat ReadUldat(tw_d TwD) {
    uldat l, chunk;
    byte *t;
    
    (void)GetQueue(TwD, QREAD, &l);
    while (Fd != NOFD && l < sizeof(uldat)) {
	if ((chunk = TryRead(TwD, TRUE)) != (uldat)-1)
	    l += chunk;
	else
	    return 0;
    }
	
    t = GetQueue(TwD, QREAD, NULL);
    l = *(uldat *)t;
    DeQueue(TwD, QREAD, sizeof(uldat));
    return l;
}

#define IS_DIGIT(c) ((c) >= '0' && (c) <= '9')

static void ExtractServProtocol(tw_d TwD, byte *servdata, uldat len) {
    uldat i = 0;
    byte c;
    
    ServProtocol[0] = ServProtocol[1] = ServProtocol[2] = 0;
    
    while (len) {
	c = *servdata;
	while (len && (c = *servdata) && IS_DIGIT(c)) {
	    ServProtocol[i] *= 10;
	    ServProtocol[i] += c - '0';
	    --len;
	    ++servdata;
	}
	while (len && (c = *servdata) && !IS_DIGIT(c)) {
	    --len;
	    ++servdata;
	}
	if (++i >= 3)
	    break;
    }
}

uldat Tw_LibraryVersion(tw_d TwD) {
    return TW_PROTOCOL_VERSION;
}

uldat Tw_ServerVersion(tw_d TwD) {
    uldat l;
    LOCK;
    l = TWVER_BUILD(ServProtocol[0],ServProtocol[1],ServProtocol[2]);
    UNLK;
    return l;
}


static byte ProtocolNumbers(tw_d TwD) {
    byte *servdata, *hostdata = " Twin-" toSTR(TW_PROTOCOL_VERSION_MAJOR) ".";
    uldat len = 0, chunk, _len = strlen(hostdata);
    
    while (Fd != NOFD && (!len || ((servdata = GetQueue(TwD, QREAD, NULL)), len < *servdata))) {
	if ((chunk = TryRead(TwD, TRUE)) != (uldat)-1)
	    len += chunk;
    }

    if (Fd != NOFD) {
	servdata = GetQueue(TwD, QREAD, &len);
    
	if (*servdata >= _len && !TwCmpMem(hostdata+1, servdata+1, _len-1)) {
	    ExtractServProtocol(TwD, servdata+6, *servdata-6);
	    DeQueue(TwD, QREAD, *servdata);
	    return TRUE;
	} else
	    Errno = TW_EX_PROTOCOL;
    }
    return FALSE;
}

static byte MagicNumbers(tw_d TwD) {
    uldat len = 0, chunk;
    byte *hostdata, data[8+sizeof(uldat)] = { 8+sizeof(uldat), sizeof(byte), sizeof(udat), sizeof(uldat), sizeof(hwcol), sizeof(time_t), sizeof(frac_t), 0};
    *(uldat *)(data+8) = TWIN_MAGIC;
    
    /* send our magic to server */
    if (!AddQueue(TwD, QWRITE, 8+sizeof(uldat), data) || !Flush(TwD, TRUE))
	return FALSE;

    /*
     * at the moment, no client-side datasize or endianity translation is available...
     * so just check against our Magic
     */
    while (Fd != NOFD && (!len || ((hostdata = GetQueue(TwD, QREAD, NULL)), len < *hostdata))) {
	if ((chunk = TryRead(TwD, TRUE)) != (uldat)-1)
	    len += chunk;
    }

    if (Fd != NOFD) {
	hostdata = GetQueue(TwD, QREAD, &len);
    
	if (len >= 8+sizeof(uldat) && *hostdata == 8+sizeof(uldat)
	    && !TwCmpMem(data, hostdata, 8)) {
	    if (!TwCmpMem(data+8, hostdata+8, sizeof(uldat))) {
		DeQueue(TwD, QREAD, 8+sizeof(uldat));
		return TRUE;
	    } else
		Errno = TW_EX_ENDIAN;
	} else
	    Errno = TW_EX_SIZES;
    }
    return FALSE;
}

#define digestLen       16  /* hardcoded in MD5 routines */
#define hAuthLen	256 /* length of ~/.TwinAuth */
#define challengeLen	512 /* length of ~/.TwinAuth + random data */

static byte MagicChallenge(tw_d TwD) {
    struct MD5Context ctx;
    byte *t, *data, *home;
    uldat len, got, challenge, chunk;
    int fd;
    
    challenge = ReadUldat(TwD);
    if (Fd == NOFD)
	return FALSE;
    if (challenge == TW_GO_MAGIC)
	return TRUE;
    if (challenge != TW_WAIT_MAGIC) {
	Errno = TW_EWEIRD;
	return FALSE;
    }
    if (!(home = getenv("HOME"))) {
	Errno = TW_ENO_AUTH;
	return FALSE;
    }
    if (!WQLeft(digestLen) || !(data = Tw_AllocMem(hAuthLen))) {
	Errno = TW_ENO_MEM;
	return FALSE;
    }
	
    
    len = TwLenStr(home);
    if (len > hAuthLen - 11)
	len = hAuthLen - 11;
    
    Tw_CopyMem(home, data, len);
    Tw_CopyMem("/.TwinAuth", data+len, 11);
    if ((fd = open(data, O_RDONLY)) < 0) {
	Tw_FreeMem(data);
	Errno = TW_ENO_AUTH;
	return FALSE;
    }
    for (len = 0, got = 1; got && len < hAuthLen; len += got) {
	do {
	    got = read(fd, data + len, hAuthLen - len);
	} while (got == (uldat)-1 && errno == EINTR);
	if (got < 0)
	    break;
    }
    close(fd);
    
    challenge = ReadUldat(TwD);
    if (Fd == NOFD || got == (uldat)-1 || len + challenge != challengeLen) {
	Tw_FreeMem(data);
	if (Fd != NOFD)
	    Errno = TW_ENO_AUTH;
	return FALSE;
    }
    
    (void)GetQueue(TwD, QREAD, &got);
    while (Fd != NOFD && got < challenge) {
	if ((chunk = TryRead(TwD, TRUE)) != (uldat)-1)
	    got += chunk;
    }
    
    if (Fd == NOFD)
	return FALSE;
    
    MD5Init(&ctx);
    MD5Update(&ctx, data, len);
    
    t = GetQueue(TwD, QREAD, NULL);
    MD5Update(&ctx, t, challenge);

    t = GetQueue(TwD, QWRITE, NULL); /* we did WQLeft(digestLen) above */
    MD5Final(t, &ctx);

    DeQueue(TwD, QREAD, challenge);
    
    Flush(TwD, TRUE);
    challenge = ReadUldat(TwD);
    
    if (challenge == TW_GO_MAGIC)
	return TRUE;
    if (Fd != NOFD)
	Errno = TW_EDENIED;
    return FALSE;
}

tw_d Tw_Open(TW_CONST byte *TwDisplay) {
    tw_d TwD;
    int result = -1, fd = NOFD;
    byte *options, gzip = FALSE;

    if (Functions[order(FindFunction)].len == 0)
	InitFunctions();
    
    if (!TwDisplay && (!(TwDisplay = getenv("TWDISPLAY")) || !*TwDisplay)) {
	CommonErrno = TW_ENO_DISPLAY;
	return (tw_d)0;
    }
    
    if ((options = strchr(TwDisplay, ','))) {
	*options = '\0';
	if (!TwCmpMem(options+1, "gz", 2))
	    gzip = TRUE;
    }

    CommonErrno = 0;

    if (*TwDisplay == ':') do {
	/* unix socket */
	struct sockaddr_un addr;
	
	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
	    CommonErrno = TW_ENO_SOCKET;
	    break;
	}
	Tw_WriteMem(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;

	Tw_CopyStr("/tmp/.Twin", addr.sun_path);
	Tw_CopyStr(TwDisplay, addr.sun_path + 10);
	
	result = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
	
    } while(0); else do {
	/* inet socket */
	struct sockaddr_in addr;
	struct hostent *host_info;
	byte *server = Tw_CloneStr(TwDisplay), *p;
	unsigned short port;
	
	if (!server) {
	    CommonErrno = TW_ENO_MEM;
	    break;
	}
	    
	p = strchr(server, ':');
	
	if (!p) {
	    CommonErrno = TW_EBAD_DISPLAY;
	    Tw_FreeMem(server);
	    break;
	}
	*p = '\0';
	port = TW_INET_PORT + strtoul(p+1, NULL, 16);

	Tw_WriteMem(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	
	/* check if the server is a numbers-and-dots host like "127.0.0.1" */
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(server);
	
	if (addr.sin_addr.s_addr == (unsigned long)-1) {
	    /* may be a FQDN host like "www.gnu.org" */
	    host_info = gethostbyname(server);
	    if (host_info) {
		Tw_CopyMem(host_info->h_addr, &addr.sin_addr, host_info->h_length);
		addr.sin_family = host_info->h_addrtype;
	    } else {
		/* unknown hostname */
		rCommonErrno.E = TW_ENO_HOST;
		rCommonErrno.S = h_errno;
		Tw_FreeMem(server);
		break;
	    }
	}

	Tw_FreeMem(server);

	if ((fd = socket(addr.sin_family, SOCK_STREAM, 0)) >= 0)
	    result = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
	else
	    CommonErrno = TW_ENO_SOCKET;
	    
    } while (0);

    if (options)
	*options = ',';

    if (result == -1) { /* some error occurred */
	if (fd != NOFD) {
	    close(fd);
	    CommonErrno = TW_ECANT_CONN;
	}
	/* try to get a meaningful message for the error */
	if (CommonErrno == TW_ENO_SOCKET || CommonErrno == TW_ECANT_CONN)
	    rCommonErrno.S = errno;
	return (tw_d)0;
    }

    if (!(TwD = (tw_d)Tw_AllocMem(sizeof(struct s_tw_d)))) {
	close(fd);
	CommonErrno = TW_ENO_MEM;
	return (tw_d)0;
    }

    Tw_WriteMem(TwD, 0, sizeof(struct s_tw_d));
    tw_mutex_init(mutex);
    Fd = fd;

    fcntl(Fd, F_SETFD, FD_CLOEXEC);
    fcntl(Fd, F_SETFL, O_NONBLOCK);
    
    tw_mutex_lock(OpenCountMutex);
    OpenCount++;
    tw_mutex_unlock(OpenCountMutex);

    LOCK;
    if (ProtocolNumbers(TwD) && MagicNumbers(TwD) && MagicChallenge(TwD)) {
	UNLK;
	if (gzip)
	    (void)Tw_EnableGzip(TwD);
	return TwD;
    }
    UNLK;
    
    close(Fd); Fd = NOFD; /* to skip Sync() */
    Tw_Close(TwD);
    return (tw_d)0;
}

/* just like all Tw_* functions, this requires LOCK not to be held */
void Tw_Close(tw_d TwD) {
    s_tw_errno *E;
    byte *q;
    int i;
    
    if (!TwD)
	return;

    LOCK;
    
    if (Fd != NOFD) {
	Sync(TwD);
	close(Fd);
    }
#ifdef CONF_SOCKET_GZ
    if (GzipFlag)
	Tw_DisableGzip(TwD);
#endif
    for (i = 0; i < QMAX; i++) {
	if ((q = Queue[i]))
	    Tw_FreeMem(q);
    }

    /* save Errno in CommonErrno */
    E = GetErrnoLocation(TwD);
    rCommonErrno.E = E->E;
    rCommonErrno.S = E->S;
    
    /*PanicFlag = FALSE;*/
    UNLK;
    tw_mutex_destroy(mutex);
    
    FreeErrnoLocation(TwD);
    Tw_FreeMem(TwD);

    tw_mutex_lock(OpenCountMutex);
    OpenCount--;
    tw_mutex_unlock(OpenCountMutex);
}


/*
 * Tw_AttachGetReply() returns 0 for failure, 1 for success,
 * else message string (len) bytes long.
 * 
 * it bypasses any compression.
 */
/* this requires LOCK not to be held */
TW_CONST byte *Tw_AttachGetReply(tw_d TwD, uldat *len) {
    uldat chunk;
    byte *answ = (byte *)-1, *nul;
#ifdef CONF_SOCKET_GZ
    byte wasGzipFlag;
#endif
    
    LOCK;
    
#ifdef CONF_SOCKET_GZ
    wasGzipFlag = GzipFlag;
    GzipFlag = FALSE;
#endif
    
    if (Fd != NOFD) do {
	
	answ = GetQueue(TwD, QREAD, &chunk);
	if (!chunk) {
	    (void)TryRead(TwD, TRUE);
	    answ = GetQueue(TwD, QREAD, &chunk);
	}
	if (chunk) {
	    if ((nul = memchr(answ, '\0', chunk))) {
		if (nul == answ && nul + 1 < answ + chunk) {
		    DeQueue(TwD, QREAD, 2);
		    answ = (byte *)(size_t)nul[1];
		    break;
		}
		chunk = nul - answ;
	    }
	    DeQueue(TwD, QREAD, chunk);
	    *len = chunk;
	    break;
	}
    } while (0);
#ifdef CONF_SOCKET_GZ
    GzipFlag = wasGzipFlag;
#endif

    UNLK;
    return answ;
}

/* this requires LOCK not to be held */
void Tw_AttachConfirm(tw_d TwD) {
    LOCK;
    if (Fd != NOFD) {
	write(Fd, "\1", 1);
    }
    UNLK;
}


/* warning: Tw_Errno() is a macro in libTw.h ! */
#undef Tw_Errno
/* this requires LOCK not to be held */
uldat Tw_Errno(tw_d TwD) {
    uldat e;
    if (TwD) {
	LOCK; e = Errno; UNLK;
    } else
	e = CommonErrno;
    return e;
}

/* this requires LOCK not to be held */
tw_errno *Tw_ErrnoLocation(tw_d TwD) {
    s_tw_errno *t;
    if (TwD) {
	LOCK; t = GetErrnoLocation(TwD); UNLK;
    } else
	t = &rCommonErrno;
    return (tw_errno *)t;
}

TW_FNATTR_CONST TW_CONST byte *Tw_StrError(TW_CONST tw_d TwD, uldat e) {
    switch (e) {
      case 0:
	return "success";
      case TW_EX_ENDIAN:
	return "server has reversed endianity, impossible to connect";
      case TW_EX_SIZES:
	return "server has different data sizes, impossible to connect";
      case TW_ELOST_CONN:
	return "connection lost (explicit kill or server shutdown)";
      case TW_EALREADY_CONN:
	return "already connected";
      case TW_ENO_DISPLAY:
	return "TWDISPLAY is not set";
      case TW_EBAD_DISPLAY:
	return "badly formed TWDISPLAY";
      case TW_ECANT_CONN:
	return "failed to connect: ";
      case TW_ENO_MEM:
	return "out of memory";
      case TW_ECANT_WRITE:
	return "failed to send data to server: ";
      case TW_ENO_FUNCTION:
	return "function not supported by server: ";
      case TW_EWEIRD:
	return "got weird data from server, protocol violated";
      case TW_ENO_AUTH:
	return "bad or missing authorization file ~/.TwinAuth, cannot connect";
      case TW_EDENIED:
	return "server denied permission to connect, file ~/.TwinAuth may be wrong";
      case TW_EBAD_GZIP:
	return "got invalid data from server, gzip format violated";
      case TW_EINTERNAL_GZIP:
	return "internal gzip error, panic!";
      case TW_ENO_HOST:
	return "unknown host in TWDISPLAY: ";
      case TW_EBAD_FUNCTION:
	return "function is not a possible server function";
      case TW_EX_PROTOCOL:
	return "server has incompatible protocol version, impossible to connect";
      case TW_ENO_SOCKET:
	return "failed to create socket: ";
      default:
	return "unknown error";
    }
}

TW_FNATTR_CONST TW_CONST byte *Tw_StrErrorDetail(TW_CONST tw_d TwD, uldat E, uldat S) {
    switch (E) {
      case TW_ECANT_CONN:
      case TW_ECANT_WRITE:
      case TW_ENO_SOCKET:
	return strerror(S);
      case TW_ENO_HOST:
	return hstrerror(S);
      case TW_ENO_FUNCTION:
	return Functions[S].name;
    }
    return "";
}

/* this requires LOCK not to be held */
int Tw_ConnectionFd(tw_d TwD) {
    int f;
    LOCK; f = Fd; UNLK;
    return f;
}

/* hack:
 * TwReadMsg() returns an already DeQueued tmsg.
 * TwPeekMsg() returns a tmsg without DeQueueing it.
 * They work because DeQueue() doesn't touch the data, just sets
 * some variables to mark it free.
 * But if you AddQueue(QMSG) you may junk your precious tmsg.
 * There is no easy solution to this, except for Tw_CloneReadMsg() it,
 * as both DeQueued and still queued tmsg may be moved around
 * by the TwReallocMem() in AddQueue(QMSG) in ParseReplies().
 * 
 * So: the contents of the tmsg returned by TwReadMsg()
 * becomes undefined after a call to a function that internally
 * calls ParseReplies(). These "bad" functions are:
 * 
 * TwSync();
 * TwPeekMsg();
 * TwReadMsg();
 * TwCreateGadget();
 * TwSearchGadget();
 * TwCreateWindow();
 * TwCreate4MenuWindow();
 * TwCreate4MenuMenuItem();
 * TwCreateMsgPort();
 * 
 * and in general any Tw() function of libTw.h returning non-void
 * so that it sends something to the server and waits for the server
 * to send back the return value.
 */

/* this requires LOCK to be held */
static tmsg ReadMsg(tw_d TwD, byte Wait, byte deQueue) {
    tmsg Msg = (tmsg)0;
    uldat len;

    if (Fd != NOFD) {
	Msg = (tmsg)GetQueue(TwD, QMSG, &len);
    
	if (!len) {
	    Flush(TwD, Wait);
	    do {
		if (TryRead(TwD, Wait) != (uldat)-1) {
		    ParseReplies(TwD);
		    Msg = (tmsg)GetQueue(TwD, QMSG, &len);
		}
	    } while (Wait && Fd != NOFD && !len);
	}
    
	if (Fd != NOFD && len) {
	    if (deQueue)
		DeQueue(TwD, QMSG, Msg->Len);
	} else
	    Msg = (tmsg)0;
    }
    return Msg;
}

/* non-blocking check if there are messages ALREADY RECEIVED */
/* this never tries to read from the socket */
tmsg Tw_PendingMsg(tw_d TwD) {
    tmsg Msg;
    uldat len;
    LOCK; Msg = (tmsg)GetQueue(TwD, QMSG, &len); UNLK;
    if (!len)
	Msg = (tmsg)0;
    return Msg;
}

/* non-blocking check if there are messages available,
 * reading non-blocking from the socket if necessary */
/* this requires LOCK not to be held */
tmsg Tw_PeekMsg(tw_d TwD) {
    tmsg Msg;
    LOCK; Msg = ReadMsg(TwD, FALSE, FALSE); UNLK;
    return Msg;
}

/*
 * get the first available message. If Wait is true and no messages
 * are immediately available, block until we receive one
 */
tmsg Tw_ReadMsg(tw_d TwD, byte Wait) {
    tmsg Msg;
    LOCK; Msg = ReadMsg(TwD, Wait, TRUE); UNLK;
    return Msg;
}

/*
 * get the first available message. If Wait is true and no messages
 * are immediately available, block until we receive one.
 * 
 * This returns a Tw_AllocMem()ed copy of the message, to avoid concurrency
 * problems with other threads. You must Tw_FreeMem() it when done!
 */
tmsg Tw_CloneReadMsg(tw_d TwD, byte Wait) {
    tmsg Msg, ClonedMsg = (tmsg)0;
    LOCK;
    if ((Msg = ReadMsg(TwD, Wait, TRUE)) &&
	(ClonedMsg = (tmsg)Tw_AllocMem(Msg->Len))) {
	
	Tw_CopyMem(Msg, ClonedMsg, Msg->Len);
    }
    UNLK;
    return ClonedMsg;
}

/* find and return the appropriate Listener for Msg->Event */
/* this requires LOCK to be held */
static tlistener FindListener(tw_d TwD, tmsg Msg) {
    tlistener L;
    tevent_common EC, LC;
    
    for (L = TwD->FirstListener; L; L = L->Next) {
	
	EC = &Msg->Event.EventCommon;
	LC = &L->Event->EventCommon;
	if (L->Type == Msg->Type && LC->Code == EC->Code && LC->Window == EC->Window) {
	    
	    /* common part matches. check details. */
	    switch (L->Type) {
		/*
		 * HACK: I am assuming all fields of a union
		 * have the same address of the union itself
		 */
	      case TW_MSG_WINDOW_KEY:
	      case TW_MSG_WINDOW_MOUSE:
		if (L->Event->EventKeyboard.ShiftFlags == Msg->Event.EventKeyboard.ShiftFlags)
		    return L;
		break;
	      case TW_MSG_MENU_ROW:
		if (L->Event->EventMenu.Menu == Msg->Event.EventMenu.Menu)
		    return L;
		break;
	      case TW_MSG_WINDOW_CHANGE:
	      case TW_MSG_WINDOW_GADGET:
	      case TW_MSG_SELECTION:
	      case TW_MSG_SELECTIONNOTIFY:
	      case TW_MSG_SELECTIONREQUEST:
	      case TW_MSG_SELECTIONCLEAR:
	      case TW_MSG_USER_CONTROL:
		/* no extra checks needed */
		return L;
	      default:
		if (L->Type >= TW_MSG_USER_FIRST)
		    return L;
		break;
	    }
	}
    }
    return NULL;
}

static void InsertListener(tw_d TwD, tlistener L) {
    if (!L->TwD) {
	if ((L->Next = TwD->FirstListener))
	    L->Next->Prev = L;
	else
	    TwD->LastListener = L;
	L->Prev = NULL;
	TwD->FirstListener = L;
	L->TwD = TwD;
    }
}

static void RemoveListener(tw_d TwD, tlistener L) {
    if (L->TwD == TwD) {
	if (L->Prev)
	    L->Prev->Next = L->Next;
	else
	    TwD->FirstListener = L->Next;
	if (L->Next)
	    L->Next->Prev = L->Prev;
	else
	    TwD->LastListener = L->Prev;
	L->Prev = L->Next = NULL;
	L->TwD = NULL;
    }
}


void Tw_InsertListener(tw_d TwD, tlistener L) {
    LOCK;
    InsertListener(TwD, L);
    UNLK;
}

void Tw_RemoveListener(tw_d TwD, tlistener L) {
    LOCK;
    if (TwD == L->TwD)
	RemoveListener(TwD, L);
    UNLK;
}



void Tw_DeleteListener(tw_d TwD, tlistener L) {
    LOCK;
    if (TwD == L->TwD) {
	RemoveListener(TwD, L);
	if (L->Event)
	    Tw_FreeMem(L->Event);
	Tw_FreeMem(L);
    }
    UNLK;
}


void Tw_SetDefaultListener(tw_d TwD, tfn_default_listener listener, void *arg) {
    LOCK;
    TwD->DefaultListener = listener;
    TwD->DefaultArg = arg;
    UNLK;
}


static tlistener CreateListener(tw_d TwD, udat Type, tevent_any E,
				tfn_listener Listener, void *Arg) {
    tlistener L;
    if ((L = (tlistener)Tw_AllocMem(sizeof(struct s_tlistener)))) {
	L->Type = Type;
	L->Event = E;
	L->Listener = Listener;
	L->Arg = Arg;
	L->TwD = NULL;
	LOCK;
	InsertListener(TwD, L);
	UNLK;
    }
    return L;
}

static tevent_any CreateEvent(twindow Window, udat Code, udat ShiftFlags) {
    
    tevent_common E;
    
    if ((E = (tevent_common)Tw_AllocMem(sizeof(struct s_tevent_common)))) {
	E->Window = Window;
	E->Code = Code;
	E->pad = ShiftFlags;
	/* warning: we RELY on tevent_keyboard->ShiftFlags
	 * to be the same as tevent_common->pad */
    }
    return (tevent_any)E;
}

static tevent_any CreateMenuEvent(twindow Window, tmenu Menu, udat Code) {
    
    tevent_menu E;
    
    if ((E = (tevent_menu)Tw_AllocMem(sizeof(struct s_tevent_menu)))) {
	E->Window = Window;
	E->Code = Code;
	E->Menu = Menu;
    }
    return (tevent_any)E;
}

static tlistener AddListener(tw_d TwD, udat Type, twindow Window, udat Code, udat ShiftFlags,
			tfn_listener Listener, void *Arg) {
    
    tlistener L;
    tevent_any E;
    
    if ((E = CreateEvent(Window, Code, ShiftFlags))) {
	if ((L = CreateListener(TwD, Type, E, Listener, Arg)))
	    return L;
	Tw_FreeMem(E);
    }
    return NULL;
}

static tlistener AddMenuListener(tw_d TwD, udat Type, twindow Window, tmenu Menu, udat Code,
				 tfn_listener Listener, void *Arg) {
    
    tlistener L;
    tevent_any E;
    
    if ((E = CreateMenuEvent(Window, Menu, Code))) {
	if ((L = CreateListener(TwD, Type, E, Listener, Arg)))
	    return L;
	Tw_FreeMem(E);
    }
    return NULL;
}

tlistener Tw_AddKeyboardListener(tw_d TwD, twindow Window, udat Code, udat ShiftFlags,
				 tfn_listener Listener, void *Arg) {
    return AddListener(TwD, TW_MSG_WINDOW_KEY, Window, Code, ShiftFlags, Listener, Arg);
}

tlistener Tw_AddMouseListener(tw_d TwD, twindow Window, udat Code, udat ShiftFlags,
			      tfn_listener Listener, void *Arg) {
    return AddListener(TwD, TW_MSG_WINDOW_MOUSE, Window, Code, ShiftFlags, Listener, Arg);
}

tlistener Tw_AddControlListener(tw_d TwD, twindow Window, udat Code,
				tfn_listener Listener, void *Arg) {
    return AddListener(TwD, TW_MSG_USER_CONTROL, Window, Code, 0, Listener, Arg);
}

tlistener Tw_AddClientMsgListener(tw_d TwD, twindow Window, udat Code,
				  tfn_listener Listener, void *Arg) {
    return AddListener(TwD, TW_MSG_USER_CLIENTMSG, Window, Code, 0, Listener, Arg);
}

tlistener Tw_AddDisplayListener(tw_d TwD, udat Code,
				tfn_listener Listener, void *Arg) {
    return AddListener(TwD, TW_MSG_DISPLAY, NOID, Code, 0, Listener, Arg);
}

tlistener Tw_AddWindowListener(tw_d TwD, twindow Window, udat Code,
			       tfn_listener Listener, void *Arg) {
    return AddListener(TwD, TW_MSG_WINDOW_CHANGE, Window, Code, 0, Listener, Arg);
}

tlistener Tw_AddGadgetListener(tw_d TwD, twindow Window, udat Code,
			       tfn_listener Listener, void *Arg) {
    return AddListener(TwD, TW_MSG_WINDOW_GADGET, Window, Code, 0, Listener, Arg);
}

tlistener Tw_AddMenuListener(tw_d TwD, twindow Window, tmenu Menu, udat Code,
			     tfn_listener Listener, void *Arg) {
    return AddMenuListener(TwD, TW_MSG_MENU_ROW, Window, Menu, Code, Listener, Arg);
}

tlistener Tw_AddSelectionListener(tw_d TwD, twindow Window,
				  tfn_listener Listener, void *Arg) {
    return AddListener(TwD, TW_MSG_SELECTION, Window, 0, 0, Listener, Arg);
}

tlistener Tw_AddSelectionNotifyListener(tw_d TwD, tfn_listener Listener, void *Arg) {
    return AddListener(TwD, TW_MSG_SELECTIONNOTIFY, NOID, 0, 0, Listener, Arg);
}

tlistener Tw_AddSelectionRequestListener(tw_d TwD, tfn_listener Listener, void *Arg) {
    return AddListener(TwD, TW_MSG_SELECTIONREQUEST, NOID, 0, 0, Listener, Arg);
}


static byte DispatchMsg(tdisplay TwD, tmsg Msg, byte mustClone) {
    tlistener L;
    tfn_listener Listener;
    tfn_default_listener DefaultListener;
    void *Arg;
    tmsg ClonedMsg;

    if ((L = FindListener(TwD, Msg))) {
	/* for performance, move search hits at list beginning */
	RemoveListener(TwD, L);
	InsertListener(TwD, L);
	Listener = L->Listener;
	Arg = L->Arg;
	DefaultListener = NULL;
    } else if (TwD->DefaultListener) {
	DefaultListener = TwD->DefaultListener;
	Arg = TwD->DefaultArg;
	Listener = NULL;
    } else
	return FALSE;

    /*
     * Tw_MainLoop calls us with a Msg still inside Queue[].
     * Clone it before calling Listener to avoid problems.
     * 
     * If the user calls Tw_DispatchMsg, it's up to him to ensure
     * there are no problems (possibly using Tw_CloneReadMsg() instead
     * of Tw_ReadMsg() ).
     */
    if (mustClone) {
	if ((ClonedMsg = (tmsg)Tw_AllocMem(Msg->Len)))
	    Tw_CopyMem(Msg, ClonedMsg, Msg->Len);
    } else
	ClonedMsg = Msg;
    
    if (ClonedMsg) {
	UNLK;
	if (Listener)
	    Listener(&ClonedMsg->Event, Arg);
	else
	    DefaultListener(ClonedMsg, Arg);
	LOCK;
	if (mustClone)
	    Tw_FreeMem(ClonedMsg);
	return TRUE;
    }
    return FALSE;
}

byte Tw_DispatchMsg(tdisplay TwD, tmsg Msg) {
    byte ret;
    LOCK;
    ret = DispatchMsg(TwD, Msg, FALSE);
    UNLK;
    return ret;
}

uldat Tw_MainLoop(tw_d TwD) {
    tmsg Msg;
    
    LOCK;
    while ((Msg = ReadMsg(TwD, TRUE, TRUE)))
	(void)DispatchMsg(TwD, Msg, TRUE);	
    UNLK;
    return Errno;
}









INLINE uldat NextSerial(tw_d TwD) {
    if (++RequestN == msg_magic)
	++RequestN;
    return RequestN;
}

INLINE void Send(tw_d TwD, uldat Serial, uldat idFN) {
    r[0] = s - (byte *)(r+1);
    r[1] = Serial;
    r[2] = idFN;
}

/***********/

#define A(n)		a##n
#define AL(n)		len##n

/*
 * a function cannot return a vector V(...) as
 * there is no way to notify the vector size,
 * but it can return W(...) once the RETW()
 * macro is implemented (seems tricky though)
 */
#define tv(arg)		void
#define t_(arg)		arg
#define tx(arg)		uldat
#define tV(arg)		TW_CONST arg *
#define tW(arg)		TW_CONST arg *
#define t(arg,f)	t##f(arg)
#define a(n,arg,f)	t(arg,f) A(n)

#define V(check)	V
#define iV(check)	check
#define W(check)	W
#define iW(check)	check

#define DECLv(n,arg)
#define DECL_(n,arg)	
#define DECLx(n,arg)	
#define DECLV(n,arg)	uldat AL(n);
#define DECLW(n,arg)	uldat AL(n);
#define D(n,arg,f)	DECL##f(n,arg)

#define DECL0v(arg)
#define DECL0_(arg)     t_(arg) A(0); reply MyReply;
#define DECL0x(arg)	tx(arg) A(0); reply MyReply;
#define D_0(arg,f)	DECL0##f(arg)

#define NEXT()		(My = NextSerial(TwD))

#define RETv(ret,f0,u)	u; return;
#define RET_(ret,f0,u)	if ((MyReply = Wait4Reply(TwD, My)) && \
			    MyReply->Code == OK_MAGIC) { \
			    A(0) = *(t(ret,f0) *)&MyReply->Data; \
			    KillReply(TwD, (byte *)MyReply, MyReply->Len); \
			} else \
			    A(0) = (t(ret,f0))NOID; \
			u; return A(0);

#define RETx(ret,f0,u)	RET_(ret,f0,u)

#define RET(functname,ret,f0)	Send(TwD, NEXT(), id_Tw[order(functname)]); \
				RET##f0(ret,f0,UNLK)

#define NOUNLK_RET(functname,ret,f0) \
				Send(TwD, NEXT(), id_Tw[order(functname)]); \
				RET##f0(ret,f0,NO_UNLK)

#define SPACE_(n,arg,len)	sizeof(arg)
#define PARSE_(n,arg,len)	Push(s,arg,A(n));

#define SPACEx(n,arg,len)	sizeof(uldat)
#define PARSEx(n,arg,len)	Push(s,uldat,A(n));

#define SPACEV(n,arg,len)	(AL(n) = (len) * sizeof(arg))
#define PARSEV(n,arg,len)	PushV(s,AL(n),A(n));

#define SPACEW(n,arg,len)	(AL(n) = A(n) ? (len) * sizeof(arg) : 0, sizeof(uldat) + AL(n))
#define PARSEW(n,arg,len)	Push(s,uldat,AL(n)); PushV(s,AL(n),A(n));

#define S(n,arg,f,len)		SPACE##f(n,arg,len)
#define P(n,arg,f,len)		PARSE##f(n,arg,len)

#define GROW(len, pass)		My = (len); if (WQLeft(My)) { pass }

#define FAILv(ret)		return;
#define FAIL_(ret)		return (ret)0;
#define FAILx(ret)		return NOID;
#define FAIL(ret,f0)		FAIL##f0(ret)

#define NAME(ret,f0,funct,name) t(ret,f0) Tw_##funct##name

#define lockSELF(functname,pass,lock,unlk) \
				uldat My; \
				lock; \
				if (Fd != NOFD) { \
				    if ((My = id_Tw[order(functname)]) == NOID) { \
					My = id_Tw[order(functname)] = Tw_FindFunction_ \
					(TwD, Functions[order(functname)].len, 3 + Functions[order(functname)].name); \
				    } \
				    if (My != NOID && Fd != NOFD) { \
					if (InitRS(TwD)) { \
					    pass \
					} \
					/* still here? must be out of memory! */ \
					Errno = TW_ENO_MEM; \
					Fail(TwD); \
				    } else if (Fd != NOFD) { \
					s_tw_errno *vec = GetErrnoLocation(TwD); \
					vec->E = TW_ENO_FUNCTION; \
					vec->S = order(functname); \
					Panic(TwD); \
				    } \
				} \
				unlk;

#define SELF(functname,pass)	    lockSELF(functname,pass,LOCK,UNLK)
#define NOLOCK_SELF(functname,pass) lockSELF(functname,pass,NO_LOCK,NO_UNLK)

/* exception: Tw_FindFunction_() must be called WITH LOCK HELD! */
#define NOSELF(functname, pass)	uldat My; \
			        id_Tw[order(functname)] = FIND_MAGIC; \
				if (Fd != NOFD) { \
				    if (InitRS(TwD)) { \
					pass \
				    } \
				    /* still here? must be out of memory! */ \
				    Errno = TW_ENO_MEM; \
				    Fail(TwD); \
				}

#define PROTO0(ret,f0, funct,name,fn) \
NAME(ret,f0,funct,name) (tw_d TwD) \
{ \
  D_0(ret ,f0) \
  SELF(funct##name, \
  RET(funct##name,ret,f0)) \
  FAIL(ret,f0) \
}

/* turn Tw_SyncSocket into Tw_SyncSocket_, to be able to wrap it */
#define PROTO0SyncSocket(ret,f0, funct,name,fn) \
static NAME(ret,f0,funct,name##_) (tw_d TwD) \
{ \
  D_0(ret ,f0) \
  NOLOCK_SELF(funct##name, \
  NOUNLK_RET(funct##name,ret,f0)) \
  FAIL(ret,f0) \
}

#define PROTO1(ret,f0, funct,name,fn, arg1,f1) \
NAME(ret,f0,funct,name) (tw_d TwD, a(1,arg1,f1)) \
{ \
  D_0( ret,f0) \
  D(1,arg1,f1) \
  SELF(funct##name, \
  GROW(S(1,arg1,f1,i##f1), \
  P(1,arg1,f1,i##f1) \
  RET(funct##name,ret,f0))) \
  FAIL(ret,f0) \
}

#define PROTO2(ret,f0, funct,name,fn, arg1,f1, arg2,f2) \
NAME(ret,f0,funct,name) (tw_d TwD, \
			 a(1,arg1,f1), a(2,arg2,f2)) \
{ \
  D_0( ret,f0) \
  D(1,arg1,f1) D(2,arg2,f2) \
  SELF(funct##name, \
  GROW(S(1,arg1,f1,i##f1) + S(2,arg2,f2,i##f2), \
  P(1,arg1,f1,i##f1) P(2,arg2,f2,i##f2) \
  RET(funct##name,ret,f0))) \
  FAIL(ret,f0) \
}

/* turn Tw_FindFunction into Tw_FindFunction_, to be able to wrap it */
#define PROTO2FindFunction(ret,f0, funct,name,fn, arg1,f1, arg2,f2) \
static NAME(ret,f0,funct,name##_) (tw_d TwD, \
				   a(1,arg1,f1), a(2,arg2,f2)) \
{ \
  D_0( ret,f0) \
  D(1,arg1,f1) D(2,arg2,f2) \
  NOSELF(funct##name, \
  GROW(S(1,arg1,f1,i##f1) + S(2,arg2,f2,i##f2), \
  P(1,arg1,f1,i##f1) P(2,arg2,f2,i##f2) \
  NOUNLK_RET(funct##name,ret,f0))) \
  FAIL(ret,f0) \
}

#define PROTO3(ret,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3) \
NAME(ret,f0,funct,name) (tw_d TwD, \
			 a(1,arg1,f1), a(2,arg2,f2), a(3,arg3,f3)) \
{ \
  D_0( ret,f0) \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) \
  SELF(funct##name, \
  GROW(S(1,arg1,f1,i##f1) + S(2,arg2,f2,i##f2) + S(3,arg3,f3,i##f3), \
  P(1,arg1,f1,i##f1) P(2,arg2,f2,i##f2) P(3,arg3,f3,i##f3) \
  RET(funct##name,ret,f0))) \
  FAIL(ret,f0) \
}
    
#define PROTO4(ret,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3, arg4,f4) \
NAME(ret,f0,funct,name) (tw_d TwD, \
			 a(1,arg1,f1), a(2,arg2,f2), a(3,arg3,f3), a(4,arg4,f4)) \
{ \
  D_0( ret,f0) \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4,f4) \
  SELF(funct##name, \
  GROW(S(1,arg1,f1,i##f1) + S(2,arg2,f2,i##f2) + S(3,arg3,f3,i##f3) + S(4,arg4,f4,i##f4), \
  P(1,arg1,f1,i##f1) P(2,arg2,f2,i##f2) P(3,arg3,f3,i##f3) P(4,arg4,f4,i##f4) \
  RET(funct##name,ret,f0))) \
  FAIL(ret,f0) \
}

#define PROTO5(ret,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5) \
NAME(ret,f0,funct,name) (tw_d TwD, \
			 a(1,arg1,f1), a(2,arg2,f2), a(3,arg3,f3), a(4,arg4,f4), a(5,arg5,f5)) \
{ \
  D_0( ret,f0) \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4,f4) D(5,arg5,f5) \
  SELF(funct##name, \
  GROW(S(1,arg1,f1,i##f1) + S(2,arg2,f2,i##f2) + S(3,arg3,f3,i##f3) + S(4,arg4,f4,i##f4) + \
       S(5,arg5,f5,i##f5), \
  P(1,arg1,f1,i##f1) P(2,arg2,f2,i##f2) P(3,arg3,f3,i##f3) P(4,arg4,f4,i##f4) \
  P(5,arg5,f5,i##f5) \
  RET(funct##name,ret,f0))) \
  FAIL(ret,f0) \
}

#define PROTO6(ret0,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5, \
	arg6,f6) \
NAME(ret,f0,funct,name) (tw_d TwD, \
			 a(1,arg1,f1), a(2,arg2,f2), a(3,arg3,f3), a(4,arg4,f4), a(5,arg5,f5),\
			 a(6,arg6,f6)) \
{ \
  D_0( ret,f0) \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4,f4) D(5,arg5,f5) \
  D(6,arg6,f6) \
  SELF(funct##name, \
  GROW(S(1,arg1,f1,i##f1) + S(2,arg2,f2,i##f2) + S(3,arg3,f3,i##f3) + S(4,arg4,f4,i##f4) + \
       S(5,arg5,f5,i##f5) + S(6,arg6,f6,i##f6), \
  P(1,arg1,f1,i##f1) P(2,arg2,f2,i##f2) P(3,arg3,f3,i##f3) P(4,arg4,f4,i##f4) \
  P(5,arg5,f5,i##f5) P(6,arg6,f6,i##f6) \
  RET(funct##name,ret,f0))) \
  FAIL(ret,f0) \
}

#define PROTO7(ret0,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5, \
	arg6,f6, arg7,f7) \
NAME(ret,f0,funct,name) (tw_d TwD, \
			 a(1,arg1,f1), a(2,arg2,f2), a(3,arg3,f3), a(4,arg4,f4), a(5,arg5,f5),\
			 a(6,arg6,f6), a(7,arg7,f7)) \
{ \
  D_0( ret,f0) \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4,f4) D(5,arg5,f5) \
  D(6,arg6,f6) D(7,arg7,f7) \
  SELF(funct##name, \
  GROW(S(1,arg1,f1,i##f1) + S(2,arg2,f2,i##f2) + S(3,arg3,f3,i##f3) + S(4,arg4,f4,i##f4) + \
       S(5,arg5,f5,i##f5) + S(6,arg6,f6,i##f6) + S(7,arg7,f7,i##f7), \
  P(1,arg1,f1,i##f1) P(2,arg2,f2,i##f2) P(3,arg3,f3,i##f3) P(4,arg4,f4,i##f4) \
  P(5,arg5,f5,i##f5) P(6,arg6,f6,i##f6) P(7,arg7,f7,i##f7) \
  RET(funct##name,ret,f0))) \
  FAIL(ret,f0) \
}
    

#define PROTO8(ret0,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5, \
	arg6,f6, arg7,f7, arg8,f8) \
NAME(ret,f0,funct,name) (tw_d TwD, \
			 a(1,arg1,f1), a(2,arg2,f2), a(3,arg3,f3), a(4,arg4,f4), a(5,arg5,f5),\
			 a(6,arg6,f6), a(7,arg7,f7), a(8,arg8,f8)) \
{ \
  D_0( ret,f0) \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4,f4) D(5,arg5,f5) \
  D(6,arg6,f6) D(7,arg7,f7) D(8,arg8,f8) \
  SELF(funct##name, \
  GROW(S(1,arg1,f1,i##f1) + S(2,arg2,f2,i##f2) + S(3,arg3,f3,i##f3) + S(4,arg4,f4,i##f4) + \
       S(5,arg5,f5,i##f5) + S(6,arg6,f6,i##f6) + S(7,arg7,f7,i##f7) + S(8,arg8,f8,i##f8), \
  P(1,arg1,f1,i##f1) P(2,arg2,f2,i##f2) P(3,arg3,f3,i##f3) P(4,arg4,f4,i##f4) \
  P(5,arg5,f5,i##f5) P(6,arg6,f6,i##f6) P(7,arg7,f7,i##f7) P(8,arg8,f8,i##f8) \
  RET(funct##name,ret,f0))) \
  FAIL(ret,f0) \
}

#define PROTO11(ret0,f0, funct,name,fn, arg1 ,f1 , arg2 ,f2 , arg3 ,f3 , arg4 ,f4 , arg5 ,f5 , \
       arg6 ,f6 , arg7 ,f7 , arg8 ,f8 , arg9 ,f9 , arg10,f10, arg11,f11) \
NAME(ret,f0,funct,name) (tw_d TwD, \
			 a(1 ,arg1 ,f1 ), a(2 ,arg2 ,f2 ), a(3 ,arg3 ,f3 ), a(4 ,arg4 ,f4 ), \
			 a(5 ,arg5 ,f5 ), a(6 ,arg6 ,f6 ), a(7 ,arg7 ,f7 ), a(8 ,arg8 ,f8 ), \
			 a(9 ,arg9 ,f9 ), a(10,arg10,f10), a(11,arg11,f11)) \
{ \
  D_0(  ret ,f0 ) \
  D(1 ,arg1 ,f1 ) D(2 ,arg2 ,f2 ) D(3 ,arg3 ,f3 ) D(4 ,arg4, f4 ) \
  D(5 ,arg5 ,f5 ) D(6 ,arg6 ,f6 ) D(7 ,arg7 ,f7 ) D(8 ,arg8 ,f8 ) \
  D(9 ,arg9, f9 ) D(10,arg10,f10) D(11,arg11,f11) \
  SELF(funct##name, \
  GROW(S(1 ,arg1 ,f1 ,i##f1 ) + S(2 ,arg2 ,f2 ,i##f2 ) + S(3 ,arg3 ,f3 ,i##f3 ) + S(4 ,arg4 ,f4 ,i##f4 ) + \
       S(5 ,arg5 ,f5 ,i##f5 ) + S(6 ,arg6 ,f6 ,i##f6 ) + S(7 ,arg7 ,f7 ,i##f7 ) + S(8 ,arg8 ,f8 ,i##f8 ) + \
       S(9 ,arg9 ,f9 ,i##f9 ) + S(10,arg10,f10,i##f10) + S(11,arg11,f11,i##f11), \
  P(1 ,arg1 ,f1 ,i##f1 ) P(2 ,arg2 ,f2 ,i##f2 ) P(3 ,arg3 ,f3 ,i##f3 ) P(4 ,arg4 ,f4 ,i##f4 ) \
  P(5 ,arg5 ,f5 ,i##f5 ) P(6 ,arg6 ,f6 ,i##f6 ) P(7 ,arg7 ,f7 ,i##f7 ) P(8 ,arg8 ,f8 ,i##f8 ) \
  P(9 ,arg9 ,f9 ,i##f9 ) P(10,arg10,f10,i##f10) P(11,arg11,f11,i##f11) \
  RET(funct##name,ret,f0))) \
  FAIL(ret,f0) \
}

#define PROTO19(ret0,f0, funct,name,fn, arg1 ,f1 , arg2 ,f2 , arg3 ,f3 , arg4 ,f4 , arg5 ,f5 , \
       arg6 ,f6 , arg7 ,f7 , arg8 ,f8 , arg9 ,f9 , arg10,f10, arg11,f11, arg12,f12, arg13,f13, \
       arg14,f14, arg15,f15, arg16,f16, arg17,f17, arg18,f18, arg19,f19) \
NAME(ret,f0,funct,name) (tw_d TwD, \
			 a(1 ,arg1 ,f1 ), a(2 ,arg2 ,f2 ), a(3 ,arg3 ,f3 ), a(4 ,arg4 ,f4 ), \
			 a(5 ,arg5 ,f5 ), a(6 ,arg6 ,f6 ), a(7 ,arg7 ,f7 ), a(8 ,arg8 ,f8 ), \
			 a(9 ,arg9 ,f9 ), a(10,arg10,f10), a(11,arg11,f11), a(12,arg12,f12), \
			 a(13,arg13,f13), a(14,arg14,f14), a(15,arg15,f15), a(16,arg16,f16), \
			 a(17,arg17,f17), a(18,arg18,f18), a(19,arg19,f19)) \
{ \
  D_0(  ret ,f0 ) \
  D(1 ,arg1 ,f1 ) D(2 ,arg2 ,f2 ) D(3 ,arg3 ,f3 ) D(4 ,arg4, f4 ) \
  D(5 ,arg5 ,f5 ) D(6 ,arg6 ,f6 ) D(7 ,arg7 ,f7 ) D(8 ,arg8 ,f8 ) \
  D(9 ,arg9, f9 ) D(10,arg10,f10) D(11,arg11,f11) D(12,arg12,f12) \
  D(13,arg13,f13) D(14,arg14,f14) D(15,arg15,f15) D(16,arg16,f16) \
  D(17,arg17,f17) D(18,arg18,f18) D(19,arg19,f19) \
  SELF(funct##name, \
  GROW(S(1 ,arg1 ,f1 ,i##f1 ) + S(2 ,arg2 ,f2 ,i##f2 ) + S(3 ,arg3 ,f3 ,i##f3 ) + S(4 ,arg4 ,f4 ,i##f4 ) + \
       S(5 ,arg5 ,f5 ,i##f5 ) + S(6 ,arg6 ,f6 ,i##f6 ) + S(7 ,arg7 ,f7 ,i##f7 ) + S(8 ,arg8 ,f8 ,i##f8 ) + \
       S(9 ,arg9 ,f9 ,i##f9 ) + S(10,arg10,f10,i##f10) + S(11,arg11,f11,i##f11) + S(12,arg12,f12,i##f12) + \
       S(13,arg13,f13,i##f13) + S(14,arg14,f14,i##f14) + S(15,arg15,f15,i##f15) + S(16,arg16,f16,i##f16) + \
       S(17,arg17,f17,i##f17) + S(18,arg18,f18,i##f18) + S(19,arg19,f19,i##f19), \
  P(1 ,arg1 ,f1 ,i##f1 ) P(2 ,arg2 ,f2 ,i##f2 ) P(3 ,arg3 ,f3 ,i##f3 ) P(4 ,arg4 ,f4 ,i##f4 ) \
  P(5 ,arg5 ,f5 ,i##f5 ) P(6 ,arg6 ,f6 ,i##f6 ) P(7 ,arg7 ,f7 ,i##f7 ) P(8 ,arg8 ,f8 ,i##f8 ) \
  P(9 ,arg9 ,f9 ,i##f9 ) P(10,arg10,f10,i##f10) P(11,arg11,f11,i##f11) P(12,arg12,f12,i##f12) \
  P(13,arg13,f13,i##f13) P(14,arg14,f14,i##f14) P(15,arg15,f15,i##f15) P(16,arg16,f16,i##f16) \
  P(17,arg17,f17,i##f17) P(18,arg18,f18,i##f18) P(19,arg19,f19,i##f19) \
  RET(funct##name,ret,f0))) \
  FAIL(ret,f0) \
}
    
#include "sockproto.h"

#undef A
#undef AL
#undef tv
#undef t_
#undef tx
#undef tV
#undef tW
#undef t
#undef a
#undef V
#undef iV
#undef W
#undef iW
#undef DECLv
#undef DECL_
#undef DECLx
#undef DECLV
#undef DECLW
#undef D
#undef DECL0v
#undef DECL0_
#undef DECL0x
#undef D_0
#undef NEXT
#undef RETv
#undef RET_
#undef RETx
#undef RET
#undef NOUNLK_RET
#undef SPACE_
#undef PARSE_
#undef SPACEx
#undef PARSEx
#undef SPACEV
#undef PARSEV
#undef SPACEW
#undef PARSEW
#undef S
#undef P
#undef GROW
#undef FAILv
#undef FAIL_
#undef FAILx
#undef FAIL
#undef NAME
#undef lockSELF
#undef SELF
#undef NOLOCK_SELF
#undef NOSELF
#undef PROTO0
#undef PROTO0SyncSocket
#undef PROTO1
#undef PROTO2
#undef PROTO2FindFunction
#undef PROTO3
#undef PROTO4
#undef PROTO5
#undef PROTO6
#undef PROTO7
#undef PROTO8
#undef PROTO11
#undef PROTO19

/* handy aliases (also for compatibility) */
#ifdef __GNUC__
void Tw_DeleteWidget(tw_d TwD, tobj a1)			__attribute__ ((alias ("Tw_DeleteObj")));
void Tw_DeleteGadget(tw_d TwD, tobj a1)			__attribute__ ((alias ("Tw_DeleteObj")));
void Tw_MapGadget(tw_d TwD, twidget a1, twidget a2)	__attribute__ ((alias ("Tw_MapWidget")));
void Tw_UnMapGadget(tw_d TwD, twidget a1)		__attribute__ ((alias ("Tw_UnMapWidget")));
void Tw_SetXYGadget(tw_d TwD, twidget a1, dat x, dat y)	__attribute__ ((alias ("Tw_SetXYWidget")));
void Tw_RecursiveDeleteGadget(tw_d TwD, twidget a1)	__attribute__ ((alias ("Tw_RecursiveDeleteWidget")));
tmsgport Tw_GetOwnerGadget(tw_d TwD, twidget a1)	__attribute__ ((alias ("Tw_GetOwnerWidget")));
void Tw_DeleteWindow(tw_d TwD, tobj a1)			__attribute__ ((alias ("Tw_DeleteObj")));
void Tw_MapWindow(tw_d TwD, twidget a1, twidget a2)	__attribute__ ((alias ("Tw_MapWidget")));
void Tw_UnMapWindow(tw_d TwD, twidget a1)		__attribute__ ((alias ("Tw_UnMapWidget")));
void Tw_SetXYWindow(tw_d TwD, twidget a1, dat x, dat y)	__attribute__ ((alias ("Tw_SetXYWidget")));
void Tw_RecursiveDeleteWindow(tw_d TwD, twidget a1)	__attribute__ ((alias ("Tw_RecursiveDeleteWidget")));
tmsgport Tw_GetOwnerWindow(tw_d TwD, twidget a1)	__attribute__ ((alias ("Tw_GetOwnerWidget")));
void Tw_DeleteMenuItem(tw_d TwD, tobj a1)		__attribute__ ((alias ("Tw_DeleteObj")));
void Tw_DeleteMenu(tw_d TwD, tobj a1)			__attribute__ ((alias ("Tw_DeleteObj")));
void Tw_DeleteMsgPort(tw_d TwD, tobj a1)		__attribute__ ((alias ("Tw_DeleteObj")));
byte Tw_SyncSocket(tw_d TwD)				__attribute__ ((alias ("Tw_Sync")));
#else
void Tw_DeleteWidget(tw_d TwD, tobj a1)			{ Tw_DeleteObj(TwD, a1); }
void Tw_DeleteGadget(tw_d TwD, tobj a1)			{ Tw_DeleteObj(TwD, a1); }
void Tw_MapGadget(tw_d TwD, twidget a1,	twidget a2)	{ Tw_MapWidget(TwD, a1, a2); }
void Tw_UnMapGadget(tw_d TwD, twidget a1)		{ Tw_UnMapWidget(TwD, a1); }
void Tw_SetXYGadget(tw_d TwD, twidget a1, dat x, dat y) { Tw_SetXYWidget(TwD, a1, x, y); }
void Tw_RecursiveDeleteGadget(tw_d TwD, twidget a1)	{ Tw_RecursiveDeleteWidget(TwD, a1); }
tmsgport Tw_GetOwnerGadget(tw_d TwD, twidget a1) { return Tw_GetOwnerWidget(TwD, a1); }
void Tw_DeleteWindow(tw_d TwD, tobj a1)			{ Tw_DeleteObj(TwD, a1); }
void Tw_MapWindow(tw_d TwD, twidget a1,	twidget a2)	{ Tw_MapWidget(TwD, a1, a2); }
void Tw_UnMapWindow(tw_d TwD, twidget a1)		{ Tw_UnMapWidget(TwD, a1); }
void Tw_SetXYWindow(tw_d TwD, twidget a1, dat x, dat y) { Tw_SetXYWidget(TwD, a1, x, y); }
void Tw_RecursiveDeleteWindow(tw_d TwD, twidget a1)	{ Tw_RecursiveDeleteWidget(a1); }
tmsgport Tw_GetOwnerWindow(tw_d TwD, twidget a1) { return Tw_GetOwnerWidget(TwD, a1); }
void Tw_DeleteMenuItem(tw_d TwD, tobj a1)		{ Tw_DeleteObj(TwD, a1); }
void Tw_DeleteMenu(tw_d TwD, tobj a1)			{ Tw_DeleteObj(TwD, a1); }
void Tw_DeleteMsgPort(tw_d TwD, tobj a1)		{ Tw_DeleteObj(TwD, a1); }
byte Tw_SyncSocket(tw_d TwD)			 { return Tw_Sync(TwD); }
#endif

void Tw_SetTextGadget(tw_d TwD, tgadget G, dat Width, dat Height, TW_CONST byte *Text, dat Left, dat Up) {
    /* clear the whole gadget */
    Tw_WriteTextGadget(TwD, G, MAXDAT, MAXDAT, NULL, 0, 0);
    /* write the specified text */
    Tw_WriteTextGadget(TwD, G, Width, Height, Text, Left, Up);
}
    

static byte Sync(tw_d TwD) {
    uldat left;
    return Fd != NOFD ? (GetQueue(TwD, QWRITE, &left), left) ? Tw_SyncSocket_(TwD) : TRUE : FALSE;
}

byte Tw_Sync(tw_d TwD) {
    byte left;
    LOCK; left = Sync(TwD); UNLK;
    return left;
}


/***********/

/* wrap Tw_FindFunction_() with LOCKs */
uldat Tw_FindFunction(tw_d TwD, byte Len, TW_CONST byte *Name) {
    uldat MyId;
    LOCK; MyId = Tw_FindFunction_(TwD, Len, Name); UNLK;
    return MyId;
}

static void ParseReplies(tw_d TwD) {
    uldat left, len, rlen;
    byte *t;
    reply rt;
    
    t = GetQueue(TwD, QREAD, &len);
    left = len;
    
    while (left >= sizeof(uldat)) {
	rt = (reply)t;
	Pop(t,uldat,rlen);
	left -= sizeof(uldat);
	if (left >= rlen) {
	    if (rlen < 2*sizeof(uldat) || rt->Serial == MSG_MAGIC) {
		if (rlen >= 2*sizeof(uldat) && rt->Serial == MSG_MAGIC)
		    /* it's a Msg, copy it in its own queue */
		    AddQueue(TwD, QMSG, rt->Len += sizeof(uldat), rt);
		    
		/* delete the reply and recalculate t, left, len */
		KillReply(TwD, (byte *)rt, rlen);
		(void)GetQueue(TwD, QREAD, &rlen);
		if (rlen) {
		    /* t -= sizeof(uldat); */
		    left += sizeof(uldat);
		    t = GetQueue(TwD, QREAD, NULL) + (len - left);
		    left += rlen - len;
		    len = rlen;
		    continue;
		}
		return;
	    }
	    t += rlen;
	    left -= rlen;
	} else {
	    /* the last reply is still incomplete, do not touch it and bail out */
	    left += sizeof(uldat);
	    break;
	}
    }
}

#define Delta ((uldat)(size_t)&(((tmsg)0)->Event))

/* nothing to lock here... */
tmsg Tw_CreateMsg(tw_d TwD, uldat Type, uldat EventLen) {
    tmsg Msg;

    if ((Msg = (tmsg)Tw_AllocMem(EventLen += Delta))) {
	Msg->Len = EventLen;
	Msg->Magic = msg_magic;
	Msg->Type = Type;
    }
    return Msg;
}

void Tw_DeleteMsg(tw_d TwD, tmsg Msg) {
    if (Msg && Msg->Magic == msg_magic)
	Tw_FreeMem(Msg);
}

byte Tw_SendMsg(tw_d TwD, tmsgport MsgPort, tmsg Msg) {
    byte ret = FALSE;
    if (Msg && Msg->Magic == msg_magic) {
	ret = Tw_SendToMsgPort(TwD, MsgPort, Msg->Len, (void *)Msg);
	Tw_FreeMem(Msg);
    }
    return ret;
}

void Tw_BlindSendMsg(tw_d TwD, tmsgport MsgPort, tmsg Msg) {
    if (Msg && Msg->Magic == msg_magic) {
	Tw_BlindSendToMsgPort(TwD, MsgPort, Msg->Len, (void *)Msg);
	Tw_FreeMem(Msg);
    }
}


    

byte Tw_FindFunctions(tw_d TwD, void *F, ...) {
    va_list L;
    void *tryF;
    uldat i, *id;
    s_tw_errno *E;

    if (F) {
	va_start(L, F);
	do {
	    for (i = 0; (tryF = Functions[i].Fn) && tryF != F; i++)
		;
	    if (tryF == F) {
		id = &id_Tw[i];
		if (*id != NOID ||
		    (*id = Tw_FindFunction_
		     (TwD, Functions[i].len, 3 + Functions[i].name)) != NOID)
		    
		    continue;
		E = GetErrnoLocation(TwD);
		E->E = TW_ENO_FUNCTION;
		E->S = i;
	    } else {
		Errno = TW_EBAD_FUNCTION;
	    }
	    va_end(L);
	    return FALSE;
	
	} while ((F = va_arg(L, void *)));

	va_end(L);
    }
    return TRUE;
}

#ifdef CONF_SOCKET_GZ

static voidpf Tw_ZAlloc(voidpf opaque, uInt items, uInt size) {
    void *ret = Tw_AllocMem(items * (size_t)size);
    return ret ? (voidpf)ret : Z_NULL;
}

static void Tw_ZFree(voidpf opaque, voidpf address) {
    if (address != Z_NULL)
	Tw_FreeMem((void *)address);
}

/* compress data before sending */
static uldat Gzip(tw_d TwD) {
    uldat oldQWRITE = Qlen[QWRITE], delta;
    z_streamp z = zW;
    int zret = Z_OK;
    
    /* compress the queue */
    if (Qlen[QWRITE]) {
	z->next_in = GetQueue(TwD, QWRITE, &z->avail_in);
	z->next_out = FillQueue(TwD, QgzWRITE, &z->avail_out);
	    
	while (z->avail_in && zret == Z_OK) {
	    
	    if (z->avail_out < (delta = z->avail_in + 12)) {
		if (Grow(TwD, QgzWRITE, delta - z->avail_out)) {
		    Qlen[QgzWRITE] -= delta;
		    z->next_out = FillQueue(TwD, QgzWRITE, &z->avail_out);
		} else {
		    /* out of memory ! */
		    Errno = TW_ENO_MEM;
		    Panic(TwD);
		    break;
		}
	    }
	    
	    zret = deflate(z, Z_SYNC_FLUSH);

	    /* update the compressed queue */
	    Qlen[QgzWRITE] -= z->avail_out;
	}
    }
    
    if (Fd != NOFD) {
	/* update the uncompressed queue */
	DeQueue(TwD, QWRITE, Qlen[QWRITE] - z->avail_in);
	
	if (zret == Z_OK)
	    return oldQWRITE;
	else {
	    Errno = TW_EINTERNAL_GZIP;
	    Panic(TwD);
	}
    }
    return FALSE;
}

static uldat Gunzip(tw_d TwD) {
    uldat oldQRead = Qlen[QREAD], delta;
    int zret = Z_OK;
    z_streamp z = zR;
    
    /* uncompress the queue */
    if (Qlen[QgzREAD]) {
	z->next_in = GetQueue(TwD, QgzREAD, &z->avail_in);
	z->next_out = FillQueue(TwD, QREAD, &z->avail_out);
	
	while (z->avail_in && zret == Z_OK) {
	    
	    /* approx. guess of uncompression ratio: 1 to 5 */
	    /* in case we guess wrong, inflate() will tell us to make more space */
	    if (z->avail_out < (delta = 5 * z->avail_in + 12)) {
		if (Grow(TwD, QREAD, delta - z->avail_out)) {
		    Qlen[QREAD] -= delta;
		    z->next_out = FillQueue(TwD, QREAD, &z->avail_out);
		} else {
		    /* out of memory ! */
		    Errno = TW_ENO_MEM;
		    Panic(TwD);
		    break;
		}
	    }
	    
	    zret = inflate(z, Z_SYNC_FLUSH);

	    /* update the uncompressed queue */
	    Qlen[QREAD] -= z->avail_out;
	}
    }
    
    if (Fd != NOFD) {
	/* update the compressed queue */
	DeQueue(TwD, QgzREAD, Qlen[QgzREAD] - z->avail_in);
	
	if (zret == Z_OK)
	    return Qlen[QREAD] - oldQRead;
	else {
	    Errno = TW_EBAD_GZIP;
	    Panic(TwD);
	}
    }
    return FALSE;
}

byte Tw_EnableGzip(tw_d TwD) {
    if (!GzipFlag && Tw_CanCompress(TwD)) {
	if ((zW = Tw_AllocMem(sizeof(*zW))) &&
	    (zR = Tw_AllocMem(sizeof(*zR)))) {
	    
	    if (Tw_AllocMem == malloc) {
		zW->zalloc = zR->zalloc = Z_NULL;
		zW->zfree  = zR->zfree  = Z_NULL;
	    } else {
		zW->zalloc = zR->zalloc = Tw_ZAlloc;
		zW->zfree  = zR->zfree  = Tw_ZFree;
	    }
	    zW->opaque = zR->opaque = NULL;

	    if (deflateInit(zW, Z_BEST_COMPRESSION) == Z_OK) {
		if (inflateInit(zR) == Z_OK) {
		    if (Tw_DoCompress(TwD, TRUE))
			return GzipFlag = TRUE;
		    inflateEnd(zR);
		}
		deflateEnd(zW);
	    }
	}
	if (zR) Tw_FreeMem(zR);
	if (zW) Tw_FreeMem(zW);
    }
    return FALSE;
}

byte Tw_DisableGzip(tw_d TwD) {
    if (GzipFlag && (Fd == NOFD || Tw_DoCompress(TwD, FALSE) || Fd == NOFD)) {
	inflateEnd(zR);
	deflateEnd(zW);
	Tw_FreeMem(zR);
	Tw_FreeMem(zW);
	GzipFlag = FALSE;
	return TRUE;
    }
    return FALSE;
}

#else /* !CONF_SOCKET_GZ */

byte Tw_EnableGzip(tw_d TwD) {
    return FALSE;
}

byte Tw_DisableGzip(tw_d TwD) {
    return FALSE;
}

#endif /* CONF_SOCKET_GZ */

