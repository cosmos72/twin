/*
 *  libTw.c  --  implementation of libTw functions
 *
 *  Copyright (C) 1999-2000 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#include <stdlib.h>
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

#ifdef CONF_SOCKET_GZ
# include <zlib.h>
  static byte TwGzipFlag;
  static z_streamp zR, zW;

  static uldat TwGzip(void);
  static uldat TwGunzip(void);
#endif


typedef struct id_list {
#   define EL(funct) uldat id_Tw##funct;
#   include "sockprotolist.h"
#   undef EL
} id_list;

static id_list id_Tw;

static byte TwPanicFlag;
uldat TwErrno;


#define QREAD    0
#define QWRITE   1
#define QMSG     2
#define QgzREAD  3
#define QgzWRITE 4

static byte *Queue[5];
static uldat Qstart[5], Qlen[5], Qmax[5], *r;
/* Qstart[QWRITE] and Qstart[QgzWRITE] are not used,
 * as DeQueue(Q[gz]WRITE) cancels bytes from end, not from start
 */
static byte *s;

static int Fd = NOFD;
static uldat RequestN;

static fd_set fset;

typedef struct reply reply;
struct reply {
    uldat Len, ReplyN, Code, Data;
};
/*
 * LastReply points to last reply from server.
 * it sits inside Queue[QREAD], so it becomes invalid
 * after an AddQueue(QREAD) or a DeQueue(QREAD)
 */
static reply *LastReply;

static void TwDoPanic(void);
static uldat ParseReplies(void);
byte TwEnableGzip(void);

static uldat AddQueue(byte i, uldat len, void *data) {
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

#define QLeft(Q,len)	(Qlen[Q] + Qstart[Q] + (len) <= Qmax[Q] ? Qlen[Q] += (len) : Grow(Q, len))
#define RQLeft(len)	QLeft(QREAD,len)

#define WQLeft(len)	(Qlen[QWRITE] + (len) <= Qmax[QWRITE] ? Qlen[QWRITE] += (len) : Grow(QWRITE, len))

static uldat Grow(byte i, uldat len) {
    /* make enough space available in Queue[i] */
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

INLINE byte *GetQueue(byte i, uldat *len) {
    if (len) *len = Qlen[i];
    return Queue[i] + Qstart[i];
}

INLINE byte *FillQueue(byte i, uldat *len) {
    uldat delta = Qmax[i] - Qlen[i] - Qstart[i];
    Qlen[i] += delta;
    if (len) *len = delta;
    return Queue[i] + Qstart[i] + Qlen[i] - delta;
}

static uldat *InitRS(void) {
    uldat len;
    if (WQLeft(3*sizeof(uldat))) {
	s = GetQueue(QWRITE, &len);
	s += len;
	return r = (uldat *)s - 3;
    }
    TwErrno = TW_ENO_MEM;
    return (uldat *)0;
}

INLINE uldat DeQueue(byte i, uldat len) {
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

static void TwDoPanic(void) {
    uldat len;
    
    (void)GetQueue(QREAD, &len);
    DeQueue(QREAD, len);

    (void)GetQueue(QWRITE, &len);
    DeQueue(QWRITE, len);

    (void)GetQueue(QMSG, &len);
    DeQueue(QMSG, len);

#ifdef CONF_SOCKET_GZ
    (void)GetQueue(QgzREAD, &len);
    DeQueue(QgzREAD, len);

    (void)GetQueue(QgzWRITE, &len);
    DeQueue(QgzWRITE, len);
#endif
    
    if (Fd >= 0) {
	FD_CLR(Fd, &fset);
	close(Fd);
	Fd = NOFD;
    }
    
    TwPanicFlag = TRUE;
}

byte TwInPanic(void) {
    return TwPanicFlag;
}

/* cancel the last request packet */
/* you can (must) call DoFail() ONLY after a failed WQLeft() */
static void DoFail(void) {
    DeQueue(QWRITE, s - (byte *)r);
}

byte TwFlush(void) {
    uldat chunk, left, len;
    byte *t;
    byte Q;
    
    (void)GetQueue(QWRITE, &left);

    if (Fd != NOFD && left) {

#ifdef CONF_SOCKET_GZ
	if (TwGzipFlag) {
	    if (TwGzip())
		Q = QgzWRITE;
	    else
		return FALSE; /* TwGzip() calls TwDoPanic() if needed */
	} else
#endif
	    Q = QWRITE;
	
	t = GetQueue(Q, &left);
	len = left;
    
	while (left > 0) {
	    chunk = write(Fd, t, left);
	    if (chunk > 0) {
		left -= chunk;
		t += chunk;
	    } else if (chunk < 0 && errno == EINTR)
		continue;
	    else
		break;
	}
    } else
	left = 0;
    
    
    if (!left)
	DeQueue(Q, len);
    else {
	TwErrno = TW_ECANT_WRITE;
	TwDoPanic();
    }
    return Fd != NOFD;
}

static uldat TryRead(byte Wait) {
    uldat got = 0, len = 0;
    byte *t, mayread;
    byte Q;
    
#ifdef CONF_SOCKET_GZ
    if (TwGzipFlag)
	Q = QgzREAD;
    else
#endif
	Q = QREAD;
    
    if (Wait) do {
	FD_SET(Fd, &fset);
    } while (select(Fd+1, &fset, NULL, NULL, NULL) != 1);

    mayread = ioctl(Fd, FIONREAD, &len) >= 0;
    if (!mayread || !len)
	len = SMALLBUFF;
    
    if (QLeft(Q,len)) {
	t = GetQueue(Q, &got);
	t += got - len;
	do {
	    got = read(Fd, t, len);
	} while (got < 0 && errno == EINTR);
	
	Qlen[Q] -= len - (got == (uldat)-1 ? 0 : got);
	
	if (got == 0 || (got == (uldat)-1 && errno != EINTR && errno != EAGAIN)) {
	    TwErrno = TW_ELOST_CONN;
	    TwDoPanic();
	    got = 0;
	}
    }
    
#ifdef CONF_SOCKET_GZ
    if (TwGzipFlag && got)
	return TwGunzip();
    else
#endif
	return got;
}


byte TwDrain(void) {
    uldat left;
    if (Fd != NOFD && ((void)GetQueue(QWRITE, &left), left)) {
	if (TwFlush()) while (Fd != NOFD && (!LastReply || LastReply->ReplyN != RequestN))
	    if (TryRead(TRUE))
		ParseReplies();
    }
    return Fd != NOFD;
}

static uldat TwinReadUldat(void) {
    uldat l;
    byte *t;
    
    (void)GetQueue(QREAD, &l);
    while (Fd != NOFD && l < sizeof(uldat))
	l += TryRead(TRUE);
    t = GetQueue(QREAD, NULL);
    l = *(uldat *)t;
    DeQueue(QREAD, sizeof(uldat));
    return l;
}

static byte TwinMagicNumbers(void) {
    uldat len = 0;
    byte *hostdata, data[8+sizeof(uldat)] = { sizeof(byte), sizeof(udat), sizeof(uldat), sizeof(hwcol), sizeof(time_t), 0, sizeof(frac_t), 0};
    *(uldat *)(data+8) = TWIN_MAGIC;
    
    while (Fd != NOFD && len < 8+sizeof(uldat))
	len += TryRead(TRUE);

    if (Fd != NOFD) {
	hostdata = GetQueue(QREAD, &len);
    
	if (!TwCmpMem(data, hostdata, 8)) {
	    if (!TwCmpMem(data+8, hostdata+8, sizeof(uldat))) {
		DeQueue(QREAD, 8+sizeof(uldat));
		return TRUE;
	    } else
		TwErrno = TW_EX_ENDIAN;
	} else
	    TwErrno = TW_EX_SIZES;
    }
    return FALSE;
}

#define digestLen       16  /* hardcoded in MD5 routines */
#define hAuthLen	256 /* max length of ~/.TwinAuth */
#define challengeLen	512 /* length of ~/.TwinAuth + random data */

static byte TwinMagicChallenge(void) {
    struct MD5Context ctx;
    byte *t, *data, *home;
    int fd, len, got, challenge;
    
    challenge = TwinReadUldat();
    if (Fd == NOFD)
	return FALSE;
    if (challenge == TW_GO_MAGIC)
	return TRUE;
    if (challenge != TW_WAIT_MAGIC) {
	TwErrno = TW_EWEIRD;
	return FALSE;
    }
    if (!(home = getenv("HOME"))) {
	TwErrno = TW_ENO_AUTH;
	return FALSE;
    }
    if (!WQLeft(digestLen) || !(data = TwAllocMem(hAuthLen))) {
	TwErrno = TW_ENO_MEM;
	return FALSE;
    }
	
    
    len = TwLenStr(home);
    if (len > hAuthLen - 11)
	len = hAuthLen - 11;
    
    TwCopyMem(home, data, len);
    TwCopyMem("/.TwinAuth", data+len, 11);
    if ((fd = open(data, O_RDONLY)) < 0) {
	TwFreeMem(data);
	TwErrno = TW_ENO_AUTH;
	return FALSE;
    }
    for (len = 0, got = 1; got && len < hAuthLen; len += got) {
	do {
	    got = read(fd, data + len, hAuthLen - len);
	} while (got < 0 && errno == EINTR);
	if (got < 0)
	    break;
    }

    challenge = TwinReadUldat();
    if (Fd == NOFD || got < 0 || len + challenge != challengeLen) {
	TwFreeMem(data);
	if (Fd != NOFD)
	    TwErrno = TW_ENO_AUTH;
	return FALSE;
    }
    
    (void)GetQueue(QREAD, &got);
    while (Fd != NOFD && got < challenge)
	got += TryRead(TRUE);
    
    if (Fd == NOFD)
	return FALSE;
    
    MD5Init(&ctx);
    MD5Update(&ctx, data, len);
    
    t = GetQueue(QREAD, NULL);
    MD5Update(&ctx, t, challenge);

    t = GetQueue(QWRITE, NULL); /* we did WQLeft(digestLen) above */
    MD5Final(t, &ctx);

    DeQueue(QREAD, challenge);
    
    TwFlush();
    challenge = TwinReadUldat();
    
    if (challenge == TW_GO_MAGIC)
	return TRUE;
    if (Fd != NOFD)
	TwErrno = TW_EDENIED;
    return FALSE;
}

byte TwOpen(byte *TwDisplay) {
    int result;
    byte *options, gzip = FALSE;
    
    if (Fd != NOFD) {
	TwErrno = TW_EALREADY_CONN;
	return FALSE;
    }
    if (!TwDisplay)
	if (!(TwDisplay = getenv("TWDISPLAY")) || !*TwDisplay) {
	    TwErrno = TW_ENO_DISPLAY;
	    return FALSE;
	}

    if ((options = strchr(TwDisplay, ','))) {
	*options = '\0';
	if (!TwCmpStr(options+1, "gz"))
	    gzip = TRUE;
    }
    
    
    if (*TwDisplay == ':') {
	/* unix socket */
	struct sockaddr_un addr;
	
	if ((Fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
	    TwErrno = TW_ECANT_CONN;
	    return FALSE;
	}
	TwWriteMem(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;

	TwCopyStr("/tmp/.Twin", addr.sun_path);
	TwCopyStr(TwDisplay, addr.sun_path + 10);
	
	result = connect(Fd, (struct sockaddr *)&addr, sizeof(addr));
	
    } else {
	
	struct sockaddr_in addr;
	struct hostent *host_info;
	byte *server = TwDisplay, *p = TwDisplay;
	unsigned short port;
	
	while (p && *p != ':')
	    p++;
	
	if (*p == ':')
	    port = TW_INET_PORT + strtoul(p+1, NULL, 16);
	else {
	    TwErrno = TW_ECANT_CONN;
	    return FALSE;
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
	    *p = ':';
	    if (host_info == 0) {
		/* unknown hostname */
		TwErrno = TW_ECANT_CONN;
		return FALSE;
	    }
	    TwCopyMem(host_info->h_addr, &addr.sin_addr, host_info->h_length);
	    addr.sin_family = host_info->h_addrtype;
	} else
	    *p = ':';

	
	if ((Fd = socket(addr.sin_family, SOCK_STREAM, 0)) < 0) {
	    TwErrno = TW_ECANT_CONN;
	    return FALSE;
	}
	result = connect(Fd, (struct sockaddr *)&addr, sizeof(addr));
    }

    if (options)
	*options = ',';
    
    if (result == -1) { /* some error occurred */
	if (Fd != NOFD)
	    close(Fd), Fd = NOFD;
	TwErrno = TW_ECANT_CONN;
	return FALSE;
    }
    
    fcntl(Fd, F_SETFD, FD_CLOEXEC);
    fcntl(Fd, F_SETFL, O_NONBLOCK);
    
    if (TwinMagicNumbers() && TwinMagicChallenge()) {
	/* reset all Tw* function ids */
	/* HACK: we exploit the fact that NOID == 0xFFFFFFFF with all bytes equal */
	memset(&id_Tw, (int)(byte)NOID, sizeof(id_Tw));
	
	if (gzip)
	    (void)TwEnableGzip();
	return TRUE;
    }
    TwDoPanic();
    return FALSE;
}

void TwClose(void) {
    uldat len;
    
    if (Fd != NOFD) {
	TwSync();

	(void)GetQueue(QREAD, &len);
	DeQueue(QREAD, len);

	(void)GetQueue(QWRITE, &len);
	DeQueue(QWRITE, len);

	FD_CLR(Fd, &fset);
	close(Fd);
	Fd = NOFD;
    }
    TwPanicFlag = FALSE;
}

/*
 * TwAttachGetReply() returns 0 for failure, 1 for success,
 * else message string (len) bytes long.
 */
byte *TwAttachGetReply(uldat *len) {
    uldat chunk;
    byte *answ, *nul;
    
    if (Fd != NOFD) {
	answ = GetQueue(QREAD, &chunk);
	if (!chunk) {
	    (void)TryRead(TRUE);
	    answ = GetQueue(QREAD, &chunk);
	}
	if (chunk) {
	    if ((nul = memchr(answ, '\0', chunk))) {
		if (nul == answ && nul + 1 < answ + chunk) {
		    DeQueue(QREAD, 2);
		    return (byte *)!!nul[1];
		}
		chunk = nul - answ;
	    }
	    DeQueue(QREAD, chunk);
	    *len = chunk;
	    return answ;
	}
    }
    return (byte *)-1;
}

byte *TwStrError(uldat e) {
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
      default:
	return "unknown error";
    }
}

int TwConnectionFd(void) {
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
 * and in general any Tw() function of libTw.h with the <real> flag set
 * so that it sends something to the server and waits for the server
 * to send the return value.
 */
tmsg TwPeekMsg(void) {
    tmsg Msg;
    uldat len;
    
    if (Fd == NOFD)
	return (tmsg)0;
    
    Msg = (tmsg)GetQueue(QMSG, &len);
    
    if (!len) {
	TwFlush();
	if (TryRead(FALSE)) {
	    ParseReplies();
	    Msg = (tmsg)GetQueue(QMSG, &len);
	}
    }
    if (len)
	return Msg;
    return (tmsg)0;
}

tmsg TwReadMsg(byte Wait) {
    tmsg Msg;
    uldat len;

    if (Fd == NOFD)
	return (tmsg)0;
    
    Msg = (tmsg)GetQueue(QMSG, &len);
    
    if (!len) {
	TwFlush();
	do {
	    if (TryRead(Wait)) {
		ParseReplies();
		Msg = (tmsg)GetQueue(QMSG, &len);
	    }		
	} while (Wait && Fd != NOFD && !len);
    }
    
    if (Fd != NOFD && len) {
	DeQueue(QMSG, Msg->Len);
	return Msg;
    }
    return (tmsg)0;
}
    
static void Send(uldat idFN) {
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
 * there is no way to notify the vector size
 */
#define tv(arg)		void
#define t_(arg)		arg
#define tx(arg)		uldat
#define tV(arg)		arg *
#define t(arg,f)	t##f(arg)
#define a(n,arg,f)	t(arg,f) A(n)

#define V(check)	V
#define iV(check)	check

#define DECLv(n,arg)
#define DECL_(n,arg)	
#define DECLx(n,arg)	
#define DECLV(n,arg)	uldat AL(n);
#define D(n,arg,f)	DECL##f(n,arg)

#define RETv(ret,f0)	return;
#define RET_(ret,f0)	TwDrain(); return OkLastReply() ? *(t(ret,f0) *)&LastReply->Data : (t(ret,f0))NOID;
#define RETx(ret,f0)	RET_(ret,f0)
#define RET(functname,ret,f0)	Send(id_Tw.id_Tw##functname); RET##f0(ret,f0)

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
#define P(n,arg,f,len,pass)	PARSE##f(n,arg,len,pass)

#define FAILv(ret)		return;
#define FAIL_(ret)		return (ret)0;
#define FAILx(ret)		return NOID;
#define FAIL(ret,f0)		DoFail(); FAIL##f0(ret)

#define NAME(ret,f0,funct,name)	t(ret,f0) Tw##funct##name

#define SELF(functname,pass)	if (id_Tw.id_Tw##functname == NOID) \
				    id_Tw.id_Tw##functname = TwFindFunction(TwLenStr(#functname), #functname); \
				if (id_Tw.id_Tw##functname != NOID) { \
				    if (InitRS()) { pass } \
				} else { \
				    TwErrno = TW_ENO_FUNCTION; \
				    TwDoPanic(); \
				}
			    
#define NOSELF(functname, pass)	id_Tw.id_Tw##functname = FIND_MAGIC; if (InitRS()) { pass }


#define PROTO0(ret,f0, funct,name,fn) \
NAME(ret,f0,funct,name) (void) \
{ \
  SELF(funct##name, \
  RET(funct##name,ret,f0)) \
  FAIL(ret,f0) \
}

#define PROTO0Abs	PROTO0

#define PROTO1(ret,f0, funct,name,fn, arg1,f1) \
NAME(ret,f0,funct,name) (a(1,arg1,f1)) \
{ \
  D(1,arg1,f1) \
  SELF(funct##name, \
  P(1,arg1,f1,i##f1, \
  RET(funct##name,ret,f0))) \
  FAIL(ret,f0) \
}

#define PROTO1Abs PROTO1

#define PROTO2(ret,f0, funct,name,fn, arg1,f1, arg2,f2) \
NAME(ret,f0,funct,name) (a(1,arg1,f1), a(2,arg2,f2)) \
{ \
  D(1,arg1,f1) D(2,arg2,f2) \
  SELF(funct##name, \
  P(1,arg1,f1,i##f1, P(2,arg2,f2,i##f2, \
  RET(funct##name,ret,f0)))) \
  FAIL(ret,f0) \
}

#define PROTO2Abs	PROTO2

#define PROTO2FindFunction(ret,f0, funct,name,fn, arg1,f1, arg2,f2) \
NAME(ret,f0,funct,name) (a(1,arg1,f1), a(2,arg2,f2)) \
{ \
  D(1,arg1,f1) D(2,arg2,f2) \
  NOSELF(funct##name, \
  P(1,arg1,f1,i##f1, P(2,arg2,f2,i##f2, \
  RET(funct##name,ret,f0)))) \
  FAIL(ret,f0) \
}

#define PROTO3(ret,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3) \
NAME(ret,f0,funct,name) (a(1,arg1,f1), a(2,arg2,f2), a(3,arg3,f3)) \
{ \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) \
  SELF(funct##name, \
  P(1,arg1,f1,i##f1, P(2,arg2,f2,i##f2, P(3,arg3,f3,i##f3, \
  RET(funct##name,ret,f0))))) \
  FAIL(ret,f0) \
}

#define PROTO3Abs	PROTO3

#define PROTO4(ret,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3, arg4,f4) \
NAME(ret,f0,funct,name) (a(1,arg1,f1), a(2,arg2,f2), a(3,arg3,f3), a(4,arg4,f4)) \
{ \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4,f4) \
  SELF(funct##name, \
  P(1,arg1,f1,i##f1, P(2,arg2,f2,i##f2, P(3,arg3,f3,i##f3, P(4,arg4,f4,i##f4, \
  RET(funct##name,ret,f0)))))) \
  FAIL(ret,f0) \
}

#define PROTO5(ret,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5) \
NAME(ret,f0,funct,name) (a(1,arg1,f1), a(2,arg2,f2), a(3,arg3,f3), a(4,arg4,f4), a(5,arg5,f5)) \
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
NAME(ret,f0,funct,name) (a(1,arg1,f1), a(2,arg2,f2), a(3,arg3,f3), a(4,arg4,f4), a(5,arg5,f5),\
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
NAME(ret,f0,funct,name) (a(1,arg1,f1), a(2,arg2,f2), a(3,arg3,f3), a(4,arg4,f4), a(5,arg5,f5),\
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
NAME(ret,f0,funct,name) (a(1,arg1,f1), a(2,arg2,f2), a(3,arg3,f3), a(4,arg4,f4), a(5,arg5,f5),\
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
NAME(ret,f0,funct,name) (a(1 ,arg1 ,f1 ), a(2 ,arg2 ,f2 ), a(3 ,arg3 ,f3 ), a(4 ,arg4 ,f4 ), \
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

#define PROTO11Abs PROTO11

#define PROTO18(ret0,f0, funct,name,fn, arg1 ,f1 , arg2 ,f2 , arg3 ,f3 , arg4 ,f4 , arg5 ,f5 , \
       arg6 ,f6 , arg7 ,f7 , arg8 ,f8 , arg9 ,f9 , arg10,f10, arg11,f11, arg12,f12, arg13,f13, \
       arg14,f14, arg15,f15, arg16,f16, arg17,f17, arg18,f18) \
NAME(ret,f0,funct,name) (a(1 ,arg1 ,f1 ), a(2 ,arg2 ,f2 ), a(3 ,arg3 ,f3 ), a(4 ,arg4 ,f4 ), \
			 a(5 ,arg5 ,f5 ), a(6 ,arg6 ,f6 ), a(7 ,arg7 ,f7 ), a(8 ,arg8 ,f8 ), \
			 a(9 ,arg9 ,f9 ), a(10,arg10,f10), a(11,arg11,f11), a(12,arg12,f12), \
			 a(13,arg13,f13), a(14,arg14,f14), a(15,arg15,f15), a(16,arg16,f16), \
			 a(17,arg17,f17), a(18,arg18,f18)) \
{ \
  D(1 ,arg1 ,f1 ) D(2 ,arg2 ,f2 ) D(3 ,arg3 ,f3 ) D(4 ,arg4, f4 ) \
  D(5 ,arg5 ,f5 ) D(6 ,arg6 ,f6 ) D(7 ,arg7 ,f7 ) D(8 ,arg8 ,f8 ) \
  D(9 ,arg9, f9 ) D(10,arg10,f10) D(11,arg11,f11) D(12,arg12,f12) \
  D(13,arg13,f13) D(14,arg14,f14) D(15,arg15,f15) D(16,arg16,f16) \
  D(17,arg17,f17) D(18,arg18,f18) \
  SELF(funct##name, \
  P(1 ,arg1 ,f1 ,i##f1 , P(2 ,arg2 ,f2 ,i##f2 , P(3 ,arg3 ,f3 ,i##f3 , P(4 ,arg4 ,f4 ,i##f4 , \
  P(5 ,arg5 ,f5 ,i##f5 , P(6 ,arg6 ,f6 ,i##f6 , P(7 ,arg7 ,f7 ,i##f7 , P(8 ,arg8 ,f8 ,i##f8 , \
  P(9 ,arg9 ,f9 ,i##f9 , P(10,arg10,f10,i##f10, P(11,arg11,f11,i##f11, P(12,arg12,f12,i##f12, \
  P(13,arg13,f13,i##f13, P(14,arg14,f14,i##f14, P(15,arg15,f15,i##f15, P(16,arg16,f16,i##f16, \
  P(17,arg17,f17,i##f17, P(18,arg18,f18,i##f18, \
  RET(funct##name,ret,f0)))))))))))))))))))) \
  FAIL(ret,f0) \
}

#define PROTO19(ret0,f0, funct,name,fn, arg1 ,f1 , arg2 ,f2 , arg3 ,f3 , arg4 ,f4 , arg5 ,f5 , \
       arg6 ,f6 , arg7 ,f7 , arg8 ,f8 , arg9 ,f9 , arg10,f10, arg11,f11, arg12,f12, arg13,f13, \
       arg14,f14, arg15,f15, arg16,f16, arg17,f17, arg18,f18, arg19,f19) \
NAME(ret,f0,funct,name) (a(1 ,arg1 ,f1 ), a(2 ,arg2 ,f2 ), a(3 ,arg3 ,f3 ), a(4 ,arg4 ,f4 ), \
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

#define PROTO20(ret0,f0, funct,name,fn, arg1 ,f1 , arg2 ,f2 , arg3 ,f3 , arg4 ,f4 , arg5 ,f5 , \
       arg6 ,f6 , arg7 ,f7 , arg8 ,f8 , arg9 ,f9 , arg10,f10, arg11,f11, arg12,f12, arg13,f13, \
       arg14,f14, arg15,f15, arg16,f16, arg17,f17, arg18,f18, arg19,f19, arg20,f20) \
NAME(ret,f0,funct,name) (a(1 ,arg1 ,f1 ), a(2 ,arg2 ,f2 ), a(3 ,arg3 ,f3 ), a(4 ,arg4 ,f4 ), \
			 a(5 ,arg5 ,f5 ), a(6 ,arg6 ,f6 ), a(7 ,arg7 ,f7 ), a(8 ,arg8 ,f8 ), \
			 a(9 ,arg9 ,f9 ), a(10,arg10,f10), a(11,arg11,f11), a(12,arg12,f12), \
			 a(13,arg13,f13), a(14,arg14,f14), a(15,arg15,f15), a(16,arg16,f16), \
			 a(17,arg17,f17), a(18,arg18,f18), a(19,arg19,f19), a(20,arg20,f20)) \
{ \
  D(1 ,arg1 ,f1 ) D(2 ,arg2 ,f2 ) D(3 ,arg3 ,f3 ) D(4 ,arg4, f4 ) \
  D(5 ,arg5 ,f5 ) D(6 ,arg6 ,f6 ) D(7 ,arg7 ,f7 ) D(8 ,arg8 ,f8 ) \
  D(9 ,arg9, f9 ) D(10,arg10,f10) D(11,arg11,f11) D(12,arg12,f12) \
  D(13,arg13,f13) D(14,arg14,f14) D(15,arg15,f15) D(16,arg16,f16) \
  D(17,arg17,f17) D(18,arg18,f18) D(19,arg19,f19) D(20,arg20,f20) \
  SELF(funct##name, \
  P(1 ,arg1 ,f1 ,i##f1 , P(2 ,arg2 ,f2 ,i##f2 , P(3 ,arg3 ,f3 ,i##f3 , P(4 ,arg4 ,f4 ,i##f4 , \
  P(5 ,arg5 ,f5 ,i##f5 , P(6 ,arg6 ,f6 ,i##f6 , P(7 ,arg7 ,f7 ,i##f7 , P(8 ,arg8 ,f8 ,i##f8 , \
  P(9 ,arg9 ,f9 ,i##f9 , P(10,arg10,f10,i##f10, P(11,arg11,f11,i##f11, P(12,arg12,f12,i##f12, \
  P(13,arg13,f13,i##f13, P(14,arg14,f14,i##f14, P(15,arg15,f15,i##f15, P(16,arg16,f16,i##f16, \
  P(17,arg17,f17,i##f17, P(18,arg18,f18,i##f18, P(19,arg19,f19,i##f19, P(20,arg20,f20,i##f20, \
  RET(funct##name,ret,f0)))))))))))))))))))))) \
  FAIL(ret,f0) \
}

#define PROTO25(ret0,f0, funct,name,fn, arg1 ,f1 , arg2 ,f2 , arg3 ,f3 , arg4 ,f4 , arg5 ,f5 , \
       arg6 ,f6 , arg7 ,f7 , arg8 ,f8 , arg9 ,f9 , arg10,f10, arg11,f11, arg12,f12, arg13,f13, \
       arg14,f14, arg15,f15, arg16,f16, arg17,f17, arg18,f18, arg19,f19, arg20,f20, arg21,f21, \
       arg22,f22, arg23,f23, arg24,f24, arg25,f25) \
NAME(ret,f0,funct,name) (a(1 ,arg1 ,f1 ), a(2 ,arg2 ,f2 ), a(3 ,arg3 ,f3 ), a(4 ,arg4 ,f4 ), \
			 a(5 ,arg5 ,f5 ), a(6 ,arg6 ,f6 ), a(7 ,arg7 ,f7 ), a(8 ,arg8 ,f8 ), \
			 a(9 ,arg9 ,f9 ), a(10,arg10,f10), a(11,arg11,f11), a(12,arg12,f12), \
			 a(13,arg13,f13), a(14,arg14,f14), a(15,arg15,f15), a(16,arg16,f16), \
			 a(17,arg17,f17), a(18,arg18,f18), a(19,arg19,f19), a(20,arg20,f20), \
			 a(21,arg21,f21), a(22,arg22,f22), a(23,arg23,f23), a(24,arg24,f24), \
			 a(25,arg25,f25)) \
{ \
  D(1 ,arg1 ,f1 ) D(2 ,arg2 ,f2 ) D(3 ,arg3 ,f3 ) D(4 ,arg4, f4 ) \
  D(5 ,arg5 ,f5 ) D(6 ,arg6 ,f6 ) D(7 ,arg7 ,f7 ) D(8 ,arg8 ,f8 ) \
  D(9 ,arg9, f9 ) D(10,arg10,f10) D(11,arg11,f11) D(12,arg12,f12) \
  D(13,arg13,f13) D(14,arg14,f14) D(15,arg15,f15) D(16,arg16,f16) \
  D(17,arg17,f17) D(18,arg18,f18) D(19,arg19,f19) D(20,arg20,f20) \
  D(21,arg21,f21) D(22,arg22,f22) D(23,arg23,f23) D(24,arg24,f24) \
  D(25,arg25,f25) \
  SELF(funct##name, \
  P(1 ,arg1 ,f1 ,i##f1 , P(2 ,arg2 ,f2 ,i##f2 , P(3 ,arg3 ,f3 ,i##f3 , P(4 ,arg4 ,f4 ,i##f4 , \
  P(5 ,arg5 ,f5 ,i##f5 , P(6 ,arg6 ,f6 ,i##f6 , P(7 ,arg7 ,f7 ,i##f7 , P(8 ,arg8 ,f8 ,i##f8 , \
  P(9 ,arg9 ,f9 ,i##f9 , P(10,arg10,f10,i##f10, P(11,arg11,f11,i##f11, P(12,arg12,f12,i##f12, \
  P(13,arg13,f13,i##f13, P(14,arg14,f14,i##f14, P(15,arg15,f15,i##f15, P(16,arg16,f16,i##f16, \
  P(17,arg17,f17,i##f17, P(18,arg18,f18,i##f18, P(19,arg19,f19,i##f19, P(20,arg20,f20,i##f20, \
  P(21,arg21,f21,i##f21, P(22,arg22,f22,i##f22, P(23,arg23,f23,i##f23, P(24,arg24,f24,i##f24, \
  P(25,arg25,f25,i##f25, \
  RET(funct##name,ret,f0))))))))))))))))))))))))))) \
  FAIL(ret,f0) \
}

#include "sockproto.h"


/***********/

static uldat ParseReplies(void) {
    uldat left, len, rlen;
    byte *t;
    reply *rt;
    
    t = GetQueue(QREAD, &left);
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
		AddQueue(QMSG, rt->Len += sizeof(uldat), rt);
	    else
		LastReply = rt;
	} else {
	    left += sizeof(uldat);
	    break;
	}
    }
    DeQueue(QREAD, len - left);
    return len - left;
}

byte TwSync(void) {
    uldat left;
    return Fd != NOFD ? (GetQueue(QWRITE, &left), left) ? TwSyncSocket() : TRUE : FALSE;
}
    




#ifdef CONF_SOCKET_GZ

/* compress data before sending */
static uldat TwGzip(void) {
    uldat oldQWRITE = Qlen[QWRITE], delta;
    z_streamp z = zW;
    int zret = Z_OK;
    
    /* compress the queue */
    if (Qlen[QWRITE]) {
	z->next_in = GetQueue(QWRITE, &z->avail_in);
	z->next_out = FillQueue(QgzWRITE, &z->avail_out);
	    
	while (z->avail_in && zret == Z_OK) {
	    
	    if (z->avail_out < (delta = z->avail_in + 12)) {
		if (Grow(QgzWRITE, delta - z->avail_out)) {
		    Qlen[QgzWRITE] -= delta;
		    z->next_out = FillQueue(QgzWRITE, &z->avail_out);
		} else {
		    /* out of memory ! */
		    TwErrno = TW_ENO_MEM;
		    TwDoPanic();
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
	DeQueue(QWRITE, Qlen[QWRITE] - z->avail_in);
	
	if (zret == Z_OK)
	    return oldQWRITE;
	else {
	    TwErrno = TW_EINTERNAL_GZIP;
	    TwDoPanic();
	}
    }
    return FALSE;
}

static uldat TwGunzip(void) {
    uldat oldQRead = Qlen[QREAD], delta;
    int zret = Z_OK;
    z_streamp z = zR;
    
    /* uncompress the queue */
    if (Qlen[QgzREAD]) {
	z->next_in = GetQueue(QgzREAD, &z->avail_in);
	z->next_out = FillQueue(QREAD, &z->avail_out);
	
	while (z->avail_in && zret == Z_OK) {
	    
	    /* approx. guess of uncompression ratio: 1 to 5 */
	    if (z->avail_out < (delta = 5 * z->avail_in + 12)) {
		if (Grow(QREAD, delta - z->avail_out)) {
		    Qlen[QREAD] -= delta;
		    z->next_out = FillQueue(QREAD, &z->avail_out);
		} else {
		    /* out of memory ! */
		    TwErrno = TW_ENO_MEM;
		    TwDoPanic();
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
	DeQueue(QgzREAD, Qlen[QgzREAD] - z->avail_in);
	
	if (zret == Z_OK)
	    return Qlen[QREAD] - oldQRead;
	else {
	    TwErrno = TW_EBAD_GZIP;
	    TwDoPanic();
	}
    }
    return FALSE;
}

byte TwEnableGzip(void) {
    if (!TwGzipFlag && TwCanCompress()) {
	if ((zW = TwAllocMem(sizeof(*zW))) &&
	    (zR = TwAllocMem(sizeof(*zR)))) {
	    
	    zW->zalloc = zR->zalloc = Z_NULL;
	    zW->zfree  = zR->zfree  = Z_NULL;
	    zW->opaque = zR->opaque = NULL;

	    if (deflateInit(zW, Z_DEFAULT_COMPRESSION) == Z_OK) {
		if (inflateInit(zR) == Z_OK) {
		    if (TwDoCompress(TRUE))
			return TwGzipFlag = TRUE;
		    inflateEnd(zR);
		}
		deflateEnd(zW);
	    }
	}
	if (zR) TwFreeMem(zR);
	if (zW) TwFreeMem(zW);
    }
    return FALSE;
}

byte TwDisableGzip(void) {
    if (TwGzipFlag && TwDoCompress(FALSE)) {
	inflateEnd(zR);
	deflateEnd(zW);
	TwFreeMem(zR);
	TwFreeMem(zW);
	TwGzipFlag = FALSE;
	return TRUE;
    }
    return FALSE;
}

#else /* !CONF_SOCKET_GZ */

byte TwEnableGzip(void) {
    return FALSE;
}

byte TwDisableGzip(void) {
    return FALSE;
}

#endif /* CONF_SOCKET_GZ */

