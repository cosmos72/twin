/*
 *  libTw.c  --  implementation of libTw functions
 *
 *  Copyright (C) 1999-2001 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
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

#include "libTw.h"
#include "unaligned.h"
#include "md5.h"
#include "version.h"

#ifdef CONF_SOCKET_GZ
# include <zlib.h>
#endif

typedef struct id_list {
#   define EL(funct) uldat id_Tw_##funct;
#   include "sockprotolist.h"
#   undef EL
} id_list;

typedef struct reply reply;
struct reply {
    uldat Len, ReplyN, Code, Data;
};

typedef struct s_tw_d {
    uldat rErrno;

    byte *Queue[5];
    uldat Qstart[5], Qlen[5], Qmax[5];
    /*
     * since DeQueue(Q[gz]WRITE) cancels bytes from end, not from start
     * 
     * Qstart[QWRITE] is used only if Tw_TimidFlush()
     * fails writing the whole queue,
     * while Qstart[QgzWRITE] is not used at all.
     */
    
    reply *LastReply;
    /*
     * LastReply points to last reply from server.
     * it sits inside Queue[QREAD], so it becomes invalid
     * after an AddQueue(QREAD) or a DeQueue(QREAD)
     */

    uldat *r;
    byte *s;

    int Fd;
    uldat RequestN;

    id_list id;
    
    byte ServProtocol[3];
    byte PanicFlag;

    
#ifdef CONF_SOCKET_GZ
    byte GzipFlag;
    z_streamp zR, zW;
#endif

} *tw_d;

#define Errno	(TwD->rErrno)
#define id_Tw	(TwD->id)
#define Queue	(TwD->Queue)
#define Qstart	(TwD->Qstart)
#define Qlen	(TwD->Qlen)
#define Qmax	(TwD->Qmax)
#define LastReply (TwD->LastReply)
#define r	(TwD->r)
#define s	(TwD->s)
#define Fd	(TwD->Fd)
#define RequestN  (TwD->RequestN)
#define ServProtocol (TwD->ServProtocol)
#define PanicFlag (TwD->PanicFlag)
#define GzipFlag  (TwD->GzipFlag)
#define zR	(TwD->zR)
#define zW	(TwD->zW)

#define QREAD    0
#define QWRITE   1
#define QMSG     2
#define QgzREAD  3
#define QgzWRITE 4
#define QMAX	 5

static uldat CommonErrno, OpenCount;

/* and this is the 'default' display */
tw_d Tw_DefaultD;

#ifdef CONF_SOCKET_GZ
static uldat Gzip(tw_d TwD);
static uldat Gunzip(tw_d TwD);
#endif

static void Panic(tw_d TwD);
static uldat ParseReplies(tw_d TwD);

byte Tw_EnableGzip(tw_d TwD);

void *(*Tw_AllocMem)(size_t) = malloc;
void *(*Tw_ReAllocMem)(void *, size_t) = realloc;
void  (*Tw_FreeMem)(void *) = free;
byte *(*Tw_CloneStr)(TW_CONST byte *) = (byte *(*)(TW_CONST byte *))strdup;

static fd_set fset;

static byte *clone_str(TW_CONST byte *S) {
    size_t len;
    byte *T;
    if (S) {
	len = 1 + Tw_LenStr(S);
	if ((T = Tw_AllocMem(len)))
	    return TwCopyMem(S, T, len);
    }
    return NULL;
}

void Tw_ConfigMalloc(void *(*my_malloc)(size_t),
		     void *(*my_realloc)(void *, size_t),
		     void  (*my_free)(void *)) {
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
}


static uldat AddQueue(tw_d TwD, byte i, uldat len, void *data) {
    uldat nmax;
    byte *t;
    
    if (len == 0)
	return len;
    
    /* append to queue */
    if (Qstart[i] + Qlen[i] + len > Qmax[i]) {
	if (Qstart[i])
	    TwMoveMem(Queue[i] + Qstart[i], Queue[i], Qlen[i]),
	    Qstart[i] = 0;

	if (Qlen[i] + len > Qmax[i]) {
	    t = (byte *)TwReAllocMem(Queue[i], nmax = (Qmax[i]+len+40)*5/4);
	    if (!t)
		return 0;
	    Queue[i] = t;
	    Qmax[i] = nmax;
	}
    }
    
    TwCopyMem(data, Queue[i] + Qstart[i] + Qlen[i], len);
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
	TwMoveMem(Queue[i] + Qstart[i], Queue[i], Qlen[i]);
	if ((byte *)LastReply >= Queue[i] + Qstart[i] && (byte *)LastReply < Queue[i] + Qmax[i])
	    LastReply = (reply *)((byte *)LastReply - Qstart[i]);
	else
	    LastReply = (reply *)0;
	Qstart[i] = 0;
    } else {
	t = (byte *)TwReAllocMem(Queue[i], nmax = (Qmax[i]+len+40)*5/4);
	if (!t)
	    return 0;
	if (i == QWRITE) {
	    r = (uldat *)(t + ((byte *)r - Queue[i]));
	    s = t + (s - Queue[i]);
	} else {
	    if ((byte *)LastReply >= Queue[i] && (byte *)LastReply < Queue[i] + Qmax[i])
		LastReply = (reply *)(t + ((byte *)LastReply - Queue[i]));
	    else
		LastReply = (reply *)0;
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

static byte Flush(tw_d TwD, byte timid) {
    uldat chunk = 0, left, len;
    byte *t;
    byte Q;
    
    if (!timid)
	FD_ZERO(&fset);
    
    t = GetQueue(TwD, Q = QWRITE, &left);
    len = left;

    if (Fd != NOFD && left) {

#ifdef CONF_SOCKET_GZ
	if (GzipFlag) {
	    if (Gzip(TwD)) {
		t = GetQueue(TwD, Q = QgzWRITE, &left);
		len = left;
	    } else
		return FALSE; /* TwGzip() calls Panic() if needed */
	}
#endif
    
	while (left > 0) {
	    chunk = write(Fd, t, left);
	    if (chunk && chunk != (uldat)-1) {
		/*would be "if (chunk > 0)" but chunk is unsigned */
		left -= chunk;
		t += chunk;
	    }
	    else if (chunk == (uldat)-1 && errno == EINTR)
		continue;
	    else if (chunk == (uldat)-1 && errno == EAGAIN && !timid) {
		do {
		    FD_SET(Fd, &fset);
		    chunk = select(Fd+1, &fset, NULL, NULL, NULL);
		} while (chunk == (uldat)-1 && errno == EINTR);
		if (chunk == (uldat)-1)
		    break;
		else
		    continue;
	    } else
		break;
	}
    } else
	left = 0;
    
    if (!left)
	DeQueue(TwD, Q, len);
    else if (timid && chunk == (uldat)-1 && (errno == EINTR || errno == EAGAIN)) {
	/* manually DeQueue the first (len - left) bytes */
	len -= left;
	
	if (len < Qlen[Q]) {
	    Qstart[Q] += len;
	    Qlen[Q] -= len;
	} else {
	    len = Qlen[Q];
	    Qstart[Q] = Qlen[Q] = 0;
	}
    } else {
	Errno = TW_ECANT_WRITE;
	Panic(TwD);
    }
    return (Fd != NOFD) + (Fd != NOFD && timid && left);
}

byte Tw_Flush(tw_d TwD) {
    return Flush(TwD, FALSE);
}

byte Tw_TimidFlush(tw_d TwD) {
    return Flush(TwD, TRUE);
}
    
static uldat TryRead(tw_d TwD, byte Wait) {
    uldat got = 0, len = 0;
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
	    FD_SET(Fd, &fset);
	} while (select(Fd+1, &fset, NULL, NULL, NULL) != 1);
    }
    
    mayread = ioctl(Fd, FIONREAD, &len) >= 0;
    if (!mayread || !len)
	len = SMALLBUFF;
    
    if (QLeft(Q,len)) {
	t = GetQueue(TwD, Q, &got);
	t += got - len;
	do {
	    got = read(Fd, t, len);
	} while (got < 0 && errno == EINTR);
	
	Qlen[Q] -= len - (got == (uldat)-1 ? 0 : got);
	
	if (got == 0 || (got == (uldat)-1 && errno != EINTR && errno != EAGAIN)) {
	    Errno = TW_ELOST_CONN;
	    Panic(TwD);
	    got = 0;
	}
    }
    
#ifdef CONF_SOCKET_GZ
    if (GzipFlag && got)
	return Gunzip(TwD);
    else
#endif
	return got;
}


static byte Drain(tw_d TwD) {
    uldat left;
    if (Fd != NOFD && ((void)GetQueue(TwD, QWRITE, &left), left)) {
	if (Tw_Flush(TwD)) while (Fd != NOFD && (!LastReply || LastReply->ReplyN != RequestN))
	    if (TryRead(TwD, TRUE))
		ParseReplies(TwD);
    }
    return Fd != NOFD;
}

static uldat ReadUldat(tw_d TwD) {
    uldat l;
    byte *t;
    
    (void)GetQueue(TwD, QREAD, &l);
    while (Fd != NOFD && l < sizeof(uldat))
	l += TryRead(TwD, TRUE);
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
    return TWVER_BUILD(ServProtocol[0],ServProtocol[1],ServProtocol[2]);
}


static byte ProtocolNumbers(tw_d TwD) {
    byte *servdata, *hostdata = " Twin-" toSTR(TW_PROTOCOL_VERSION_MAJOR) ".";
    uldat len = 0, _len = strlen(hostdata);
    
    while (Fd != NOFD && (!len || ((servdata = GetQueue(TwD, QREAD, NULL)), len < *servdata)))
	len += TryRead(TwD, TRUE);

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
    uldat len = 0;
    byte *hostdata, data[8+sizeof(uldat)] = { 8+sizeof(uldat), sizeof(byte), sizeof(udat), sizeof(uldat), sizeof(hwcol), sizeof(time_t), sizeof(frac_t), 0};
    *(uldat *)(data+8) = TWIN_MAGIC;
    
    /* send our magic to server */
    if (!AddQueue(TwD, QWRITE, 8+sizeof(uldat), data) || !Tw_Flush(TwD))
	return FALSE;

    /*
     * at the moment, no client-side datasize or endianity translation is available...
     * so just check against our Magic
     */
    while (Fd != NOFD && (!len || ((hostdata = GetQueue(TwD, QREAD, NULL)), len < *hostdata)))
	len += TryRead(TwD, TRUE);

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
    int fd, len, got, challenge;
    
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
    
    TwCopyMem(home, data, len);
    TwCopyMem("/.TwinAuth", data+len, 11);
    if ((fd = open(data, O_RDONLY)) < 0) {
	Tw_FreeMem(data);
	Errno = TW_ENO_AUTH;
	return FALSE;
    }
    for (len = 0, got = 1; got && len < hAuthLen; len += got) {
	do {
	    got = read(fd, data + len, hAuthLen - len);
	} while (got < 0 && errno == EINTR);
	if (got < 0)
	    break;
    }
    close(fd);
    
    challenge = ReadUldat(TwD);
    if (Fd == NOFD || got < 0 || len + challenge != challengeLen) {
	Tw_FreeMem(data);
	if (Fd != NOFD)
	    Errno = TW_ENO_AUTH;
	return FALSE;
    }
    
    (void)GetQueue(TwD, QREAD, &got);
    while (Fd != NOFD && got < challenge)
	got += TryRead(TwD, TRUE);
    
    if (Fd == NOFD)
	return FALSE;
    
    MD5Init(&ctx);
    MD5Update(&ctx, data, len);
    
    t = GetQueue(TwD, QREAD, NULL);
    MD5Update(&ctx, t, challenge);

    t = GetQueue(TwD, QWRITE, NULL); /* we did WQLeft(digestLen) above */
    MD5Final(t, &ctx);

    DeQueue(TwD, QREAD, challenge);
    
    Tw_Flush(TwD);
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
    uldat _Errno;

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

    if (*TwDisplay == ':') {
	/* unix socket */
	struct sockaddr_un addr;
	
	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
	    CommonErrno = TW_ECANT_CONN;
	    return FALSE;
	}
	TwWriteMem(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;

	TwCopyStr("/tmp/.Twin", addr.sun_path);
	TwCopyStr(TwDisplay, addr.sun_path + 10);
	
	result = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
	
    } else do {
	
	struct sockaddr_in addr;
	struct hostent *host_info;
	byte *server = Tw_CloneStr(TwDisplay), *p = server;
	unsigned short port;
	
	if (!server) {
	    CommonErrno = TW_ENO_MEM;
	    break;
	}
	    
	while (p && *p && *p != ':')
	    p++;
	
	if (*p == ':')
	    port = TW_INET_PORT + strtoul(p+1, NULL, 16);
	else {
	    CommonErrno = TW_EBAD_DISPLAY;
	    Tw_FreeMem(server);
	    break;
	}
	*p = '\0';

	/* check if the server is a numbers-and-dots host like "127.0.0.1" */
	TwWriteMem(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(server);
	
	if (addr.sin_addr.s_addr == (unsigned long)-1) {
	    /* may be a FQDN host like "www.gnu.org" */
	    host_info = gethostbyname(server);
	    if (host_info) {
		TwCopyMem(host_info->h_addr, &addr.sin_addr, host_info->h_length);
		addr.sin_family = host_info->h_addrtype;
	    } else {
		/* unknown hostname */
		CommonErrno = TW_ENO_HOST;
		Tw_FreeMem(server);
		break;
	    }
	}

	Tw_FreeMem(server);
	
	if ((fd = socket(addr.sin_family, SOCK_STREAM, 0)) >= 0)
	    result = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
	else
	    CommonErrno = TW_ECANT_CONN;

    } while (0);

    if (options)
	*options = ',';

    if (result == -1) { /* some error occurred */
	if (fd != NOFD)
	    close(fd);
	if (!CommonErrno)
	    CommonErrno = TW_ECANT_CONN;
	return (tw_d)0;
    }

    if ((TwD = (tw_d)Tw_AllocMem(sizeof(struct s_tw_d)))) {
	TwWriteMem(TwD, 0, sizeof(struct s_tw_d));
	Fd = fd;
    } else {
	CommonErrno = TW_ENO_MEM;
	return (tw_d)0;
    }
    
    fcntl(Fd, F_SETFD, FD_CLOEXEC);
    fcntl(Fd, F_SETFL, O_NONBLOCK);
    
    OpenCount++;
    if (ProtocolNumbers(TwD) && MagicNumbers(TwD) && MagicChallenge(TwD)) {
	if (gzip)
	    (void)Tw_EnableGzip(TwD);
	return TwD;
    }
    _Errno = Errno;
    close(Fd); Fd = NOFD; /* to skip Tw_Sync() */
    Tw_Close(TwD);
    CommonErrno = _Errno;
    return (tw_d)0;
}

void Tw_Close(tw_d TwD) {
    byte *q;
    int i;
    
    if (!TwD)
	return;
    
    if (Fd != NOFD) {
	Tw_Sync(TwD);
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
    OpenCount--;
    CommonErrno = 0;
    /*PanicFlag = FALSE;*/
    Tw_FreeMem(TwD);
}


/*
 * Tw_AttachGetReply() returns 0 for failure, 1 for success,
 * else message string (len) bytes long.
 * 
 * it bypasses any compression.
 */
byte *Tw_AttachGetReply(tw_d TwD, uldat *len) {
    uldat chunk;
    byte *answ = (byte *)-1, *nul;
#ifdef CONF_SOCKET_GZ
    byte wasGzipFlag = GzipFlag;
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
    return answ;
}

void Tw_AttachConfirm(tw_d TwD) {
    if (Fd != NOFD) {
	write(Fd, "\1", 1);
    }
}

uldat Tw_Errno(tw_d TwD) {
    return TwD ? Errno : CommonErrno;
}

byte *Tw_StrError(tw_d TwD, uldat e) {
    switch (e) {
      case 0:
	return "success";
      case TW_EX_ENDIAN:
	return "server has reversed endianity, impossible to connect";
      case TW_EX_SIZES:
	return "server has different data sizes, impossible to connect";
      case TW_ELOST_CONN:
	return "lost connection (explicit kill or server shutdown)";
      case TW_EALREADY_CONN:
	return "already connected";
      case TW_ENO_DISPLAY:
	return "TWDISPLAY is not set";
      case TW_EBAD_DISPLAY:
	return "badly formed TWDISPLAY";
      case TW_ECANT_CONN:
	return "failed to connect";
      case TW_ENO_MEM:
	return "out of memory";
      case TW_ECANT_WRITE:
	return "failed to send data to server";
      case TW_ENO_FUNCTION:
	return "function not supported by server";
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
	return "unknown host in TWDISPLAY";
      case TW_EBAD_FUNCTION:
	return "function is not a possible server function";
      case TW_EX_PROTOCOL:
	return "server has incompatible protocol version, impossible to connect";
      default:
	return "unknown error";
    }
}


int Tw_ConnectionFd(tw_d TwD) {
    return Fd;
}

/* hack:
 * TwReadMsg() returns an already DeQueued tmsg.
 * TwPeekMsg() returns a tmsg without DeQueueing it.
 * They work because DeQueue() doesn't touch the data, just sets
 * some variables to mark it free.
 * But if you AddQueue(QMSG) you may junk your precious tmsg.
 * There is no easy solution to this, except for TwCloneMem() it,
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
tmsg Tw_PeekMsg(tw_d TwD) {
    tmsg Msg;
    uldat len;
    
    if (Fd == NOFD)
	return (tmsg)0;
    
    Msg = (tmsg)GetQueue(TwD, QMSG, &len);
    
    if (!len) {
	Tw_Flush(TwD);
	if (TryRead(TwD, FALSE)) {
	    ParseReplies(TwD);
	    Msg = (tmsg)GetQueue(TwD, QMSG, &len);
	}
    }
    if (len)
	return Msg;
    return (tmsg)0;
}

tmsg Tw_ReadMsg(tw_d TwD, byte Wait) {
    tmsg Msg;
    uldat len;

    if (Fd == NOFD)
	return (tmsg)0;
    
    Msg = (tmsg)GetQueue(TwD, QMSG, &len);
    
    if (!len) {
	Tw_Flush(TwD);
	do {
	    if (TryRead(TwD, Wait)) {
		ParseReplies(TwD);
		Msg = (tmsg)GetQueue(TwD, QMSG, &len);
	    }		
	} while (Wait && Fd != NOFD && !len);
    }
    
    if (Fd != NOFD && len) {
	DeQueue(TwD, QMSG, Msg->Len);
	return Msg;
    }
    return (tmsg)0;
}

static void Send(tw_d TwD, uldat idFN) {
    if (++RequestN == msg_magic)
	++RequestN;
    r[0] = s - (byte *)(r+1);
    r[1] = RequestN;
    r[2] = idFN;
}

/***********/

#define OkLastReply() (LastReply && LastReply->ReplyN==RequestN && LastReply->Code==OK_MAGIC)

#define A(n)		a##n
#define AL(n)		len##n

/*
 * a function cannot return a vector V(...) as
 * there is no way to notify the vector size,
 * but it can return W(...)
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

#define RETv(ret,f0)	return;
#define RET_(ret,f0)	Drain(TwD); return OkLastReply() ? *(t(ret,f0) *)&LastReply->Data : (t(ret,f0))NOID;
#define RETx(ret,f0)	RET_(ret,f0)
#define RET(functname,ret,f0)	Send(TwD, id_Tw.id_Tw_##functname); RET##f0(ret,f0)

#define PARSE_(n,arg,len,pass)	if (WQLeft(sizeof(arg))) { \
				    Push(s,arg,A(n)); \
				    pass \
				}
#define PARSEx(n,arg,len,pass)	if (WQLeft(sizeof(uldat))) { \
				    Push(s,uldat,A(n)); \
				    pass \
				}
#define PARSEV(n,arg,len,pass)	if (AL(n) = (len) * sizeof(arg), WQLeft(AL(n))) { \
				    PushV(s,AL(n),A(n)); \
				    pass \
				}
#define PARSEW(n,arg,len,pass)	if (AL(n) = A(n) ? (len) * sizeof(arg) : 0, WQLeft(sizeof(uldat) + AL(n))) { \
				    Push(s,uldat,AL(n)); \
				    PushV(s,AL(n),A(n)); \
				    pass \
				}
#define P(n,arg,f,len,pass)	PARSE##f(n,arg,len,pass)

#define FAILv(ret)		return;
#define FAIL_(ret)		return (ret)0;
#define FAILx(ret)		return NOID;
#define FAIL(ret,f0)		Fail(TwD); FAIL##f0(ret)

#define NAME(ret,f0,funct,name) t(ret,f0) Tw_##funct##name

#define SELF(functname,pass)	if (id_Tw.id_Tw_##functname == NOID) \
				    id_Tw.id_Tw_##functname = Tw_FindFunction(TwD, TwLenStr(#functname), #functname); \
				if (id_Tw.id_Tw_##functname != NOID) { \
				    if (InitRS(TwD)) { pass } \
				} else { \
				    Errno = TW_ENO_FUNCTION; \
				    Panic(TwD); \
				}
			    
#define NOSELF(functname, pass)	id_Tw.id_Tw_##functname = FIND_MAGIC; if (InitRS(TwD)) { pass }


#define PROTO0(ret,f0, funct,name,fn) \
NAME(ret,f0,funct,name) (tw_d TwD) \
{ \
  SELF(funct##name, \
  RET(funct##name,ret,f0)) \
  FAIL(ret,f0) \
}

#define PROTO1(ret,f0, funct,name,fn, arg1,f1) \
NAME(ret,f0,funct,name) (tw_d TwD, a(1,arg1,f1)) \
{ \
  D(1,arg1,f1) \
  SELF(funct##name, \
  P(1,arg1,f1,i##f1, \
  RET(funct##name,ret,f0))) \
  FAIL(ret,f0) \
}

#define PROTO2(ret,f0, funct,name,fn, arg1,f1, arg2,f2) \
NAME(ret,f0,funct,name) (tw_d TwD, \
			 a(1,arg1,f1), a(2,arg2,f2)) \
{ \
  D(1,arg1,f1) D(2,arg2,f2) \
  SELF(funct##name, \
  P(1,arg1,f1,i##f1, P(2,arg2,f2,i##f2, \
  RET(funct##name,ret,f0)))) \
  FAIL(ret,f0) \
}

#define PROTO2FindFunction(ret,f0, funct,name,fn, arg1,f1, arg2,f2) \
NAME(ret,f0,funct,name) (tw_d TwD, \
			 a(1,arg1,f1), a(2,arg2,f2)) \
{ \
  D(1,arg1,f1) D(2,arg2,f2) \
  NOSELF(funct##name, \
  P(1,arg1,f1,i##f1, P(2,arg2,f2,i##f2, \
  RET(funct##name,ret,f0)))) \
  FAIL(ret,f0) \
}


#define PROTO3(ret,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3) \
NAME(ret,f0,funct,name) (tw_d TwD, \
			 a(1,arg1,f1), a(2,arg2,f2), a(3,arg3,f3)) \
{ \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) \
  SELF(funct##name, \
  P(1,arg1,f1,i##f1, P(2,arg2,f2,i##f2, P(3,arg3,f3,i##f3, \
  RET(funct##name,ret,f0))))) \
  FAIL(ret,f0) \
}
    
#define PROTO4(ret,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3, arg4,f4) \
NAME(ret,f0,funct,name) (tw_d TwD, \
			 a(1,arg1,f1), a(2,arg2,f2), a(3,arg3,f3), a(4,arg4,f4)) \
{ \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4,f4) \
  SELF(funct##name, \
  P(1,arg1,f1,i##f1, P(2,arg2,f2,i##f2, P(3,arg3,f3,i##f3, P(4,arg4,f4,i##f4, \
  RET(funct##name,ret,f0)))))) \
  FAIL(ret,f0) \
}

#define PROTO5(ret,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5) \
NAME(ret,f0,funct,name) (tw_d TwD, \
			 a(1,arg1,f1), a(2,arg2,f2), a(3,arg3,f3), a(4,arg4,f4), a(5,arg5,f5)) \
{ \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4,f4) D(5,arg5,f5) \
  SELF(funct##name, \
  P(1,arg1,f1,i##f1, P(2,arg2,f2,i##f2, P(3,arg3,f3,i##f3, P(4,arg4,f4,i##f4, P(5,arg5,f5,i##f5, \
  RET(funct##name,ret,f0))))))) \
  FAIL(ret,f0) \
}

#define PROTO5CreateMsgPort PROTO5

#define PROTO6(ret0,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5, \
	arg6,f6) \
NAME(ret,f0,funct,name) (tw_d TwD, \
			 a(1,arg1,f1), a(2,arg2,f2), a(3,arg3,f3), a(4,arg4,f4), a(5,arg5,f5),\
			 a(6,arg6,f6)) \
{ \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4,f4) D(5,arg5,f5) \
  D(6,arg6,f6) \
  SELF(funct##name, \
  P(1,arg1,f1,i##f1, P(2,arg2,f2,i##f2, P(3,arg3,f3,i##f3, P(4,arg4,f4,i##f4, P(5,arg5,f5,i##f5, \
  P(6,arg6,f6,i##f6, \
  RET(funct##name,ret,f0)))))))) \
  FAIL(ret,f0) \
}

#define PROTO7(ret0,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5, \
	arg6,f6, arg7,f7) \
NAME(ret,f0,funct,name) (tw_d TwD, \
			 a(1,arg1,f1), a(2,arg2,f2), a(3,arg3,f3), a(4,arg4,f4), a(5,arg5,f5),\
			 a(6,arg6,f6), a(7,arg7,f7)) \
{ \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4,f4) D(5,arg5,f5) \
  D(6,arg6,f6) D(7,arg7,f7) \
  SELF(funct##name, \
  P(1,arg1,f1,i##f1, P(2,arg2,f2,i##f2, P(3,arg3,f3,i##f3, P(4,arg4,f4,i##f4, P(5,arg5,f5,i##f5, \
  P(6,arg6,f6,i##f6, P(7,arg7,f7,i##f7, \
  RET(funct##name,ret,f0))))))))) \
  FAIL(ret,f0) \
}
    

#define PROTO8(ret0,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5, \
	arg6,f6, arg7,f7, arg8,f8) \
NAME(ret,f0,funct,name) (tw_d TwD, \
			 a(1,arg1,f1), a(2,arg2,f2), a(3,arg3,f3), a(4,arg4,f4), a(5,arg5,f5),\
			 a(6,arg6,f6), a(7,arg7,f7), a(8,arg8,f8)) \
{ \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4,f4) D(5,arg5,f5) \
  D(6,arg6,f6) D(7,arg7,f7) D(8,arg8,f8) \
  SELF(funct##name, \
  P(1,arg1,f1,i##f1, P(2,arg2,f2,i##f2, P(3,arg3,f3,i##f3, P(4,arg4,f4,i##f4, P(5,arg5,f5,i##f5, \
  P(6,arg6,f6,i##f6, P(7,arg7,f7,i##f7, P(8,arg8,f8,i##f8, \
  RET(funct##name,ret,f0)))))))))) \
  FAIL(ret,f0) \
}

#define PROTO11(ret0,f0, funct,name,fn, arg1 ,f1 , arg2 ,f2 , arg3 ,f3 , arg4 ,f4 , arg5 ,f5 , \
       arg6 ,f6 , arg7 ,f7 , arg8 ,f8 , arg9 ,f9 , arg10,f10, arg11,f11) \
NAME(ret,f0,funct,name) (tw_d TwD, \
			 a(1 ,arg1 ,f1 ), a(2 ,arg2 ,f2 ), a(3 ,arg3 ,f3 ), a(4 ,arg4 ,f4 ), \
			 a(5 ,arg5 ,f5 ), a(6 ,arg6 ,f6 ), a(7 ,arg7 ,f7 ), a(8 ,arg8 ,f8 ), \
			 a(9 ,arg9 ,f9 ), a(10,arg10,f10), a(11,arg11,f11)) \
{ \
  D(1 ,arg1 ,f1 ) D(2 ,arg2 ,f2 ) D(3 ,arg3 ,f3 ) D(4 ,arg4, f4 ) \
  D(5 ,arg5 ,f5 ) D(6 ,arg6 ,f6 ) D(7 ,arg7 ,f7 ) D(8 ,arg8 ,f8 ) \
  D(9 ,arg9, f9 ) D(10,arg10,f10) D(11,arg11,f11) \
  SELF(funct##name, \
  P(1 ,arg1 ,f1 ,i##f1 , P(2 ,arg2 ,f2 ,i##f2 , P(3 ,arg3 ,f3 ,i##f3 , P(4 ,arg4 ,f4 ,i##f4 , \
  P(5 ,arg5 ,f5 ,i##f5 , P(6 ,arg6 ,f6 ,i##f6 , P(7 ,arg7 ,f7 ,i##f7 , P(8 ,arg8 ,f8 ,i##f8 , \
  P(9 ,arg9 ,f9 ,i##f9 , P(10,arg10,f10,i##f10, P(11,arg11,f11,i##f11, \
  RET(funct##name,ret,f0))))))))))))) \
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
  D(1 ,arg1 ,f1 ) D(2 ,arg2 ,f2 ) D(3 ,arg3 ,f3 ) D(4 ,arg4, f4 ) \
  D(5 ,arg5 ,f5 ) D(6 ,arg6 ,f6 ) D(7 ,arg7 ,f7 ) D(8 ,arg8 ,f8 ) \
  D(9 ,arg9, f9 ) D(10,arg10,f10) D(11,arg11,f11) D(12,arg12,f12) \
  D(13,arg13,f13) D(14,arg14,f14) D(15,arg15,f15) D(16,arg16,f16) \
  D(17,arg17,f17) D(18,arg18,f18) D(19,arg19,f19) \
  SELF(funct##name, \
  P(1 ,arg1 ,f1 ,i##f1 , P(2 ,arg2 ,f2 ,i##f2 , P(3 ,arg3 ,f3 ,i##f3 , P(4 ,arg4 ,f4 ,i##f4 , \
  P(5 ,arg5 ,f5 ,i##f5 , P(6 ,arg6 ,f6 ,i##f6 , P(7 ,arg7 ,f7 ,i##f7 , P(8 ,arg8 ,f8 ,i##f8 , \
  P(9 ,arg9 ,f9 ,i##f9 , P(10,arg10,f10,i##f10, P(11,arg11,f11,i##f11, P(12,arg12,f12,i##f12, \
  P(13,arg13,f13,i##f13, P(14,arg14,f14,i##f14, P(15,arg15,f15,i##f15, P(16,arg16,f16,i##f16, \
  P(17,arg17,f17,i##f17, P(18,arg18,f18,i##f18, P(19,arg19,f19,i##f19, \
  RET(funct##name,ret,f0))))))))))))))))))))) \
  FAIL(ret,f0) \
}

#include "sockproto.h"


/***********/

static uldat ParseReplies(tw_d TwD) {
    uldat left, len, rlen;
    byte *t;
    reply *rt;
    
    t = GetQueue(TwD, QREAD, &left);
    len = left;
    
    while (left >= sizeof(uldat)) {
	/* we cannot set LastReply here, as it would catch Msgs too */
	rt = (reply *)t;
	Pop(t,uldat,rlen);
	left -= sizeof(uldat);
	if (left >= rlen) {
	    t += rlen;
	    left -= rlen;
	    if (rt->ReplyN == MSG_MAGIC)
		/* it's a Msg */
		AddQueue(TwD, QMSG, rt->Len += sizeof(uldat), rt);
	    else
		LastReply = rt;
	} else {
	    left += sizeof(uldat);
	    break;
	}
    }
    DeQueue(TwD, QREAD, len - left);
    return len - left;
}

byte Tw_Sync(tw_d TwD) {
    uldat left;
    return Fd != NOFD ? (GetQueue(TwD, QWRITE, &left), left) ? Tw_SyncSocket(TwD) : TRUE : FALSE;
}
    
#define Delta ((udat)(size_t)&(((tmsg)0)->Event))

tmsg Tw_CreateMsg(tw_d TwD, udat Type, udat EventLen) {
    tmsg Msg;
    
    if ((Msg = (tmsg)Tw_AllocMem(EventLen += Delta))) {
	Msg->Magic = msg_magic;
	Msg->Type = Type;
	Msg->Len = EventLen;
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


    

typedef struct fn_list {
    void *Fn;
    byte *name;
} fn_list;

static fn_list Functions[] = {
#   define EL(funct) {Tw_##funct, #funct},
#   include "sockprotolist.h"
#   undef EL
    {NULL, NULL}
};

byte Tw_FindFunctions(tw_d TwD, void *F, ...) {
    va_list L;
    void *tryF;
    uldat i, *id;
    

    if (F) {
	va_start(L, F);
	do {
	    for (i = 0; (tryF = Functions[i].Fn) && tryF != F; i++)
		;
	    if (tryF == F) {
		id = (uldat *)&TwD->id + i;
		if (*id != NOID ||
		    (*id = Tw_FindFunction
		     (TwD, TwLenStr(Functions[i].name), Functions[i].name)) != NOID)
		    
		    continue;
		Errno = TW_ENO_FUNCTION;
	    } else
		Errno = TW_EBAD_FUNCTION;
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

