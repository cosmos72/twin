/*
 *  libtw.c  --  implementation of libtw functions
 *
 *  Copyright (C) 1999-2001 by Massimiliano Ghilardi
 *
 * AVL (Adelson-Velskii and Landis) tree to speed up listener search
 * from O(n) to O(log n) based on code from linux kernel mm subsystem,
 * written by Bruno Haible <haible@ma2s2.mathematik.uni-karlsruhe.de>.
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

/*
 * Life is tricky... under SunOS hstrerror() is in an obscure library, so it gets disabled,
 * yet <netdb.h> has its prototype, so the #define hstrerror() in <Tw/missing.h> breaks it.
 * Solution: include <Tw/Tw.h> (pulls in <Tw/missing.h>) late, but still include
 * "twconfig.h" and <Tw/osincludes.h> early to pull in TW_HAVE_* and system headers
 * necessary to include <sys/socket.h> under FreeBSD.
 */
#include <Tw/autoconf.h> /* for TW_HAVE* macros */
#include "twconfig.h"    /* for CONF_* macros */
#include <Tw/osincludes.h>

#ifdef TW_HAVE_SIGNAL_H
#include <signal.h>
#endif
#ifdef TW_HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef TW_HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef TW_HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef TW_HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef TW_HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef TW_HAVE_SYS_UN_H
#include <sys/un.h>
#endif
#ifdef TW_HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef TW_HAVE_ZLIB_H
#include <zlib.h>
#endif
#ifdef TW_HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#include <Tw/Tw.h>

#include <Tw/Twavl.h>
#include <Tw/Twerrno.h>
#include <Tw/Twstat.h>
#include <Tw/Twstat_defs.h>

#include "mutex.h"

#ifdef CONF_SOCKET_PTHREADS
TH_R_MUTEX_HELPER_DEFS(static);
#endif

#include "md5.h"
#include "unaligned.h"
#include "util.h"
#include "version.h"

/* remove the `Obj' suffix from Tw_ChangeFieldObj() */
#define Tw_ChangeFieldObj Tw_ChangeField

#define Min2(a, b) ((a) < (b) ? (a) : (b))

#define N_OF(array) (sizeof(array) / sizeof((array)[0]))

#ifdef CONF_SOCKET_PTHREADS

typedef struct s_tw_errno {
  uldat E;
  uldat S;
  th_self T;
} s_tw_errno;

typedef struct s_tw_errno_vec {
  s_tw_errno *vec;
  uldat max, last;
} s_tw_errno_vec;

#else

/* use libtwerrno.h types */
typedef tw_errno s_tw_errno;
typedef tw_errno s_tw_errno_vec;

#endif

typedef struct s_fn_list {
  void *Fn;
  byte len, formatlen;
  char *name, *format;
} fn_list;

static fn_list Functions[] = {

#include "libtw1_m4.h"

    {Tw_Stat, 7, 8, "Tw_StatObj", "0S0x" magic_id_STR(obj) "_" TWS_udat_STR "V" TWS_udat_STR},

    {NULL, 0, 0, NULL, NULL}};

typedef uldat v_id_vec[N_OF(Functions) - 1];

/*
 * automagically get the symbols order_* to be the index
 * of corresponding element in Functions[] :
 */
typedef enum e_fn_order {
  order_DoesNotExist = -1,
#define EL(funct) order_##funct,
#include "socklist_m4.h"
#undef EL

  order_StatObj,

} fn_order;

struct s_tlistener {
  tlistener Left, Right, AVLParent;
  uldat AVLkey;
  byte AVLHeight;
  uldat Type;
  tevent_any Event;
  tfn_listener Listener;
  void *Arg;
  tdisplay TwD;
};

#define QREAD 0
#define QWRITE 1
#define QMSG 2
#define QgzREAD 3
#define QgzWRITE 4
#define QMAX 5

typedef struct s_tw_d {
#ifdef th_r_mutex
  th_r_mutex mutex;
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

  tlistener AVLRoot;
  tfn_default_listener DefaultListener;
  void *DefaultArg;

  s_tw_errno_vec rErrno_;

  byte ServProtocol[3];
  byte ExitMainLoop, PanicFlag, ServFlags;

#ifdef CONF_SOCKET_GZ
  byte GzipFlag;
  z_streamp zR, zW;
#endif

  v_id_vec id_vec;

} *tw_d;

#define rErrno (TwD->rErrno_)
#define mutex (TwD->mutex)
#define id_Tw (TwD->id_vec)
#define Queue (TwD->Queue)
#define Qstart (TwD->Qstart)
#define Qlen (TwD->Qlen)
#define Qmax (TwD->Qmax)
#define r (TwD->r)
#define s (TwD->s)
#define Fd (TwD->Fd)
#define RequestN (TwD->RequestN)
#define ServProtocol (TwD->ServProtocol)
#define PanicFlag (TwD->PanicFlag)
#define ServFlags (TwD->ServFlags)
#define GzipFlag (TwD->GzipFlag)
#define zR (TwD->zR)
#define zW (TwD->zW)

#define LOCK th_r_mutex_lock(mutex)
#define UNLK th_r_mutex_unlock(mutex)

enum {
  ServFlagTRune2 = 1,   /* server trune is two bytes, i.e. limited to 64k Unicode characters */
  ServFlagTCell47x = 2, /* server tcell has <= 4.7.x encoding {unicode64k_lo, color,
                            unicode64k_hi, extra}. implies ServFlagTRune2 */
};

static s_tw_errno rCommonErrno;
#define CommonErrno (rCommonErrno.E)

static uldat OpenCount;
#ifdef th_mutex
static th_mutex OpenCountMutex = TH_MUTEX_INITIALIZER;
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
static void DeleteAllListeners(tlistener);
static void RemoveListener(tw_d TwD, tlistener L);

byte Tw_EnableGzip(tw_d TwD);

void *(*Tw_AllocMem)(size_t) = malloc;
void *(*Tw_ReAllocMem)(void *, size_t) = realloc;
void (*Tw_FreeMem)(void *) = free;

void *Tw_AllocMem0(size_t len) {
  void *mem;
  if (Tw_AllocMem == malloc) {
    mem = calloc(1, len);
  } else if ((mem = Tw_AllocMem(len)) != NULL) {
    memset(mem, '\0', len);
  }
  return mem;
}

void *Tw_ReAllocMem0(void *mem, size_t old_len, size_t new_len) {
  void *newMem;
  if ((newMem = Tw_ReAllocMem(mem, new_len)) != NULL) {
    if (new_len > old_len) {
      memset((char *)newMem + old_len, '\0', new_len - old_len);
    }
  }
  return newMem;
}

/**
 * creates a copy of a chunk of memory
 */
void *Tw_CloneMem(const void *src, size_t len) {
  void *dst;
  if (src && (dst = Tw_AllocMem(len)))
    return Tw_CopyMem(src, dst, len);
  return NULL;
}
/**
 * creates a copy of a null-terminated string string
 */
char *Tw_CloneStr(const char *S) {
  if (S) {
    size_t len = 1 + strlen(S);
    char *T;
    if ((T = (char *)Tw_AllocMem(len)))
      return Tw_CopyMem(S, T, len);
  }
  return NULL;
}

/**
 * sets the function to call to allocate/realloc/free memory;
 * can be called only if no connections to server are open.
 */
void Tw_ConfigMalloc(void *(*my_malloc)(size_t), void *(*my_realloc)(void *, size_t),
                     void (*my_free)(void *)) {

  th_mutex_lock(OpenCountMutex);
  if (!OpenCount) {
    if (my_malloc && my_realloc && my_free) {
      Tw_AllocMem = my_malloc;
      Tw_ReAllocMem = my_realloc;
      Tw_FreeMem = my_free;
    } else {
      Tw_AllocMem = malloc;
      Tw_ReAllocMem = realloc;
      Tw_FreeMem = free;
    }
  }
  th_mutex_unlock(OpenCountMutex);
}

#ifdef CONF_SOCKET_PTHREADS

TW_INLINE byte GrowErrnoLocation(tw_d TwD) {
  s_tw_errno *vec;
  uldat newmax = rErrno.max <= 8 ? 16 : (rErrno.max << 1);

  if ((vec = (s_tw_errno *)Tw_ReAllocMem(rErrno.vec, newmax * sizeof(s_tw_errno)))) {

    /* assume (th_self)-1 is _NOT_ a valid thread identifier */
    memset(vec + rErrno.max, '\xFF', (newmax - rErrno.max) * sizeof(s_tw_errno));
    rErrno.vec = vec;
    rErrno.max = newmax;

    return ttrue;
  }
  /* out of memory! */
  return tfalse;
}

static s_tw_errno *GetErrnoLocation(tw_d TwD) {
  th_self self;
  uldat i;
  if (TwD) {
    self = th_self_get();

    /* we cache last thread that called GetErrnoLocation() */
    i = rErrno.last;
    if (i < rErrno.max && rErrno.vec[i].T == self)
      return &rErrno.vec[i];

    for (i = 0; i < rErrno.max; i++) {
      if (rErrno.vec[i].T == self)
        break;
      if (rErrno.vec[i].T == th_self_none) {
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

#define Errno (GetErrnoLocation(TwD)->E)

#else /* !CONF_SOCKET_PTHREADS */

#define GetErrnoLocation(TwD) ((TwD) ? &TwD->rErrno_ : &rCommonErrno)
#define Errno (GetErrnoLocation(TwD)->E)
#define FreeErrnoLocation(TwD)                                                                     \
  do {                                                                                             \
  } while (0)

#endif /* CONF_SOCKET_PTHREADS */

static uldat AddQueue(tw_d TwD, byte i, uldat len, void *data) {
  uldat nmax;
  byte *t;

  if (len == 0)
    return len;

  /* append to queue */
  if (Qstart[i] + Qlen[i] + len > Qmax[i]) {
    if (Qstart[i]) {
      Tw_MoveMem(Queue[i] + Qstart[i], Queue[i], Qlen[i]);
      Qstart[i] = 0;
    }
    if (Qlen[i] + len > Qmax[i]) {
      t = (byte *)Tw_ReAllocMem(Queue[i], nmax = (Qmax[i] + len + 40) * 5 / 4);
      if (!t)
        return 0;
      Queue[i] = t;
      Qmax[i] = nmax;
    }
  }
  if (data)
    Tw_CopyMem(data, Queue[i] + Qstart[i] + Qlen[i], len);
  Qlen[i] += len;
  return len;
}

TW_INLINE byte *GetQueue(tw_d TwD, byte i, uldat *len) {
  if (len)
    *len = Qlen[i];
  return Queue[i] + Qstart[i];
}

/* add data to QMSG queue keeping it aligned at 8 bytes (for tmsgs) */
TW_INLINE uldat ParanoidAddQueueQMSG(tw_d TwD, uldat len, void *data) {
  byte *t = (byte *)data + 2 * sizeof(uldat);
  uldat mtype, minlen, xlen;
  tmsg M;
  tevent_any E;

  /* we already checked (len >= 3 * sizeof(uldat)) */
  Pop(t, uldat, mtype);
  switch (mtype &= TW_MAXUDAT) {
  case TW_MSG_DISPLAY:
    minlen = sizeof(struct s_tevent_display) - sizeof(uldat);
    break;
  case TW_MSG_WIDGET_KEY:
    minlen = sizeof(struct s_tevent_keyboard);
    break;
  case TW_MSG_WIDGET_MOUSE:
    minlen = sizeof(struct s_tevent_mouse);
    break;
  case TW_MSG_WIDGET_CHANGE:
    minlen = sizeof(struct s_tevent_widget);
    break;
  case TW_MSG_WIDGET_GADGET:
    minlen = sizeof(struct s_tevent_gadget);
    break;
  case TW_MSG_MENU_ROW:
    /* servers < 0.4.4 do not have a ->Row field */
    minlen = sizeof(struct s_tevent_menu) - sizeof(tobj);
    break;
  case TW_MSG_SELECTION:
  case TW_MSG_SELECTIONCLEAR:
    minlen = sizeof(struct s_tevent_selection);
    break;
  case TW_MSG_SELECTIONNOTIFY:
    minlen = sizeof(struct s_tevent_selectionnotify) - sizeof(uldat);
    break;
  case TW_MSG_SELECTIONREQUEST:
    minlen = sizeof(struct s_tevent_selectionrequest);
    break;
  case TW_MSG_USER_CONTROL:
    minlen = sizeof(struct s_tevent_control) - sizeof(uldat);
    break;
  case TW_MSG_USER_CLIENTMSG:
    minlen = sizeof(struct s_tevent_clientmsg) - sizeof(uldat);
    break;
  default:
    return 0;
  }
  minlen += sizeof(struct s_tmsg) - sizeof(union s_tevent_any);

  if (len >= minlen && AddQueue(TwD, QMSG, len, data)) {
    if ((len & 7) && !AddQueue(TwD, QMSG, 8 - (len & 7), NULL)) {
      Qlen[QMSG] -= len;
      return 0;
    }
    /* check variable-length messages: */
    M = (tmsg)GetQueue(TwD, QMSG, NULL);
    E = &M->Event;

    switch (mtype &= TW_MAXUDAT) {
    case TW_MSG_DISPLAY:
      xlen = E->EventDisplay.Len;
      break;
    case TW_MSG_WIDGET_KEY:
      xlen = E->EventKeyboard.SeqLen;
      break;
    case TW_MSG_SELECTIONNOTIFY:
      xlen = E->EventSelectionNotify.Len;
      break;
    case TW_MSG_USER_CONTROL:
      xlen = E->EventControl.Len;
      break;
    case TW_MSG_USER_CLIENTMSG:
      xlen = E->EventClientMsg.Len;
      break;
    default:
      xlen = 0;
      break;
    }
    len = (len + 7) & ~7;
    if (M->Len >= xlen + minlen)
      return len;
    Qlen[QMSG] -= len;
  }
  return 0;
}

#define QLeft(Q, len)                                                                              \
  (Qlen[Q] + Qstart[Q] + (len) <= Qmax[Q] ? Qlen[Q] += (len) : Grow(TwD, Q, len))
#define RQLeft(len) QLeft(QREAD, len)
#define WQLeft(len) QLeft(QWRITE, len)

static uldat Grow(tw_d TwD, byte i, uldat len) {
  /* make enough space available in Queue[i] and mark it used */
  uldat nmax;
  byte *t;

  if ((i == QREAD || i == QgzREAD) && Qlen[i] + len < Qmax[i]) {
    Tw_MoveMem(Queue[i] + Qstart[i], Queue[i], Qlen[i]);
    Qstart[i] = 0;
  } else {
    t = (byte *)Tw_ReAllocMem(Queue[i], nmax = (Qmax[i] + len + 40) * 5 / 4);
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

static uldat *InitRS(tw_d TwD) {
  uldat len;
  if (WQLeft(3 * sizeof(uldat))) {
    s = GetQueue(TwD, QWRITE, &len);
    s += len;
    return r = (uldat *)s - 3;
  }
  Errno = TW_ESYS_NO_MEM;
  return (uldat *)0;
}

TW_INLINE uldat DeQueue(tw_d TwD, byte i, uldat len) {
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

#define DeQueueAligned(TwD, i, len) DeQueue(TwD, i, ((len) + 7) & ~7)

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
    Fd = TW_NOFD;
  }

  PanicFlag = ttrue;
}

/**
 * returns ttrue if a fatal error occurred, tfalse otherwise;
 * after a fatal error, the only useful thing to do is Tw_Close()
 */
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

  if (Fd != TW_NOFD && left) {

#ifdef CONF_SOCKET_GZ
    if (GzipFlag) {
      if (Gzip(TwD)) {
        t = GetQueue(TwD, Q = QgzWRITE, &left);
      } else
        return tfalse; /* TwGzip() calls Panic() if needed */
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
      } else if (chunk == (uldat)-1 && errno == EINTR)
        ; /*continue*/
      else if (chunk == (uldat)-1 && errno == EWOULDBLOCK && Wait) {
        do {
          fd = Fd;
          /* release the lock before sleeping!!! */
          UNLK;
          FD_SET(fd, &fset);
          chunk = select(fd + 1, NULL, &fset, NULL, NULL);
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
    E->E = TW_ESYS_CANNOT_WRITE;
    E->S = errno;
    Panic(TwD);
  }
  return (Fd != TW_NOFD) + (Fd != TW_NOFD && !Wait && left);
}

/**
 * sends all buffered data to server, blocking
 * if not all data can be immediately sent
 */
byte Tw_Flush(tw_d TwD) {
  byte b = tfalse;
  if (TwD) {
    LOCK;
    b = Flush(TwD, ttrue);
    UNLK;
  }
  return b;
}

/**
 * sends all buffered data to server, without blocking:
 * if not all data can be immediately sent, unsent data is kept in buffer
 */
byte Tw_TimidFlush(tw_d TwD) {
  byte b = tfalse;
  if (TwD) {
    LOCK;
    b = Flush(TwD, tfalse);
    UNLK;
  }
  return b;
}

static const timevalue TimeoutZero = {0, 0}, Timeout5Seconds = {5, 0};
#define TIMEOUT_INFINITE ((timevalue *)0)
#define TIMEOUT_ZERO (&TimeoutZero)
#define TIMEOUT_5_SECONDS (&Timeout5Seconds)

/* return bytes read, or (uldat)-1 for errors / timeout */
static uldat TryRead(tw_d TwD, const timevalue *Timeout) {
  uldat got = 0, len = 0;
  int sel, fd;
  byte *t, mayread;
  byte Q, timedout = tfalse;

  if (!TwD) {
    return (uldat)-1;
  }

#ifdef CONF_SOCKET_GZ
  if (GzipFlag)
    Q = QgzREAD;
  else
#endif
    Q = QREAD;

  if (!Timeout || Timeout->Seconds || Timeout->Fraction) {
    fd_set fset;
    struct timeval timeout;
    timevalue deadline;
    FD_ZERO(&fset);
    if (Timeout) {
      timeout.tv_sec = Timeout->Seconds;
      timeout.tv_usec = Timeout->Fraction / MicroSEC;
      InstantNow(&deadline);
      IncrTime(&deadline, Timeout);
    }
    for (;;) {
      fd = Fd;
      /* drop LOCK before sleeping! */
      UNLK;
      FD_SET(fd, &fset);
      sel = select(fd + 1, &fset, NULL, NULL, Timeout ? &timeout : NULL);
      FD_CLR(fd, &fset);
      LOCK;

      /* maybe another thread received some data? */
      (void)GetQueue(TwD, QREAD, &len);
      if (len != 0 || sel > 0)
        break;
      if (sel == -1 && errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)
        break;

      if (Timeout) {
        timevalue actual, to_sleep;
        InstantNow(&actual);
        if (CmpTime(&actual, &deadline) >= 0) {
          timedout = ttrue;
          break;
        }
        to_sleep = deadline; /* struct copy */
        DecrTime(&to_sleep, &actual);
        timeout.tv_sec = to_sleep.Seconds;
        timeout.tv_usec = to_sleep.Fraction / MicroSEC;
      }
    }
  }

  /* for ioctl(int, FIONREAD, int *) */
  sel = 0;
  mayread = ioctl(Fd, FIONREAD, &sel) >= 0;
  if (!mayread || !sel)
    len = TW_SMALLBUFF;
  else if (sel > TW_HUGEBUFF)
    len = TW_HUGEBUFF;
  else
    len = sel;

  if (QLeft(Q, len)) {
    t = GetQueue(TwD, Q, &got);
    t += got - len;
    do {
      got = read(Fd, t, len);
    } while (got == (uldat)-1 && errno == EINTR && !timedout);

    Qlen[Q] -= len - (got == (uldat)-1 ? 0 : got);

    if (got == 0 || (got == (uldat)-1 &&
                     (timedout || (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)))) {
      if (got == (uldat)-1 && timedout)
        Errno = TW_ESERVER_READ_TIMEOUT;
      else
        Errno = TW_ESERVER_LOST_CONNECT;
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

TW_INLINE byte *FindReply(tw_d TwD, uldat Serial) {
  uldat left, rlen, rSerial;
  byte *rt, *t = GetQueue(TwD, QREAD, &left);

  /* sequentially examine all received replies */
  while (left >= 3 * sizeof(uldat)) {
    rt = t;
    Pop(t, uldat, rlen);
    Pop(t, uldat, rSerial);
    /* it this reply complete? */
    if (left >= rlen + sizeof(uldat)) {
      if (rSerial == Serial)
        return rt;
      else {
        /* skip to next reply */
        t += rlen - sizeof(uldat);
        left -= rlen + sizeof(uldat);
      }
    } else
      /* last reply is incomplete, no more replies to examine */
      break;
  }
  return NULL;
}

static byte *Wait4Reply(tw_d TwD, uldat Serial) {
  uldat left;
  byte *MyReply = NULL;
  if (Fd != TW_NOFD && ((void)GetQueue(TwD, QWRITE, &left), left)) {
    if (Flush(TwD, ttrue))
      while (Fd != TW_NOFD && !(MyReply = FindReply(TwD, Serial)))
        if (TryRead(TwD, TIMEOUT_INFINITE) != (uldat)-1)
          ParseReplies(TwD);
  }
  return Fd != TW_NOFD ? MyReply : NULL;
}

static uldat ReadUldat(tw_d TwD) {
  uldat l, chunk;
  byte *t;

  (void)GetQueue(TwD, QREAD, &l);
  while (Fd != TW_NOFD && l < sizeof(uldat)) {
    if ((chunk = TryRead(TwD, TIMEOUT_INFINITE)) != (uldat)-1)
      l += chunk;
    else
      return 0;
  }

  t = GetQueue(TwD, QREAD, NULL);
  Pop(t, uldat, l);
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
  if (ServProtocol[0] < 4 || (ServProtocol[0] == 4 && ServProtocol[1] <= 7)) {
    /* server protocol is <= 4.7.x : has two-byte trune and old tcell enconding */
    ServFlags |= ServFlagTRune2 | ServFlagTCell47x;
  }
}

/**
 * returns library protocol version
 */
uldat Tw_LibraryVersion(tw_d TwD) {
  return TW_PROTOCOL_VERSION;
}

/**
 * returns server protocol version, or (uldat)0 on error
 */
uldat Tw_ServerVersion(tw_d TwD) {
  uldat l = 0;
  if (TwD) {
    LOCK;
    l = TW_VER_BUILD(ServProtocol[0], ServProtocol[1], ServProtocol[2]);
    UNLK;
  }
  return l;
}

static byte ProtocolNumbers(tw_d TwD) {
  byte *servdata;
  const char *hostdata = " Twin-" TW_STR(TW_PROTOCOL_VERSION_MAJOR) ".";
  uldat len = 0, chunk, _len = strlen(hostdata);

  while (Fd != TW_NOFD && (!len || ((servdata = GetQueue(TwD, QREAD, NULL)), len < *servdata))) {
    if ((chunk = TryRead(TwD, TIMEOUT_5_SECONDS)) != (uldat)-1)
      len += chunk;
  }

  if (Fd != TW_NOFD) {
    servdata = GetQueue(TwD, QREAD, &len);

    if (*servdata >= _len && !memcmp(hostdata + 1, servdata + 1, _len - 1)) {
      ExtractServProtocol(TwD, servdata + 6, *servdata - 6);
      DeQueue(TwD, QREAD, *servdata);
      return ttrue;
    } else
      Errno = TW_ESERVER_BAD_VERSION;
  } else if (len == 0) {
    /* server immediately closed the socket. socket module not running? */
    GetErrnoLocation(TwD)->S = TW_EDETAIL_NO_MODULE;
  }
  return tfalse;
}

TW_DECL_MAGIC(Tw_MagicData);

/**
 * returns ttrue if user-provided magic numbers
 * are compatible with library ones; used to avoid mixing
 * unicode clients with non-unicode library and vice-versa
 */
byte Tw_CheckMagic(const byte id[]) {
  if (memcmp(id + 1, Tw_MagicData + 1,
             (id[0] < Tw_MagicData[0] ? id[0] : Tw_MagicData[0]) - 2 - sizeof(uldat))) {
    CommonErrno = TW_EBAD_SIZES;
    return tfalse;
  }
  if (sizeof(struct s_tevent_display) != sizeof(tobj) + 4 * sizeof(udat) + sizeof(uldat) ||
      sizeof(struct s_tevent_keyboard) != sizeof(tobj) + 3 * sizeof(udat) + 2 ||
      sizeof(struct s_tevent_mouse) != sizeof(tobj) + 4 * sizeof(udat) ||
      sizeof(struct s_tevent_widget) != sizeof(tobj) + 6 * sizeof(udat) ||
      sizeof(struct s_tevent_gadget) != sizeof(tobj) + 2 * sizeof(udat) ||
      sizeof(struct s_tevent_menu) != 3 * sizeof(tobj) + 2 * sizeof(udat) ||
      sizeof(struct s_tevent_selection) != sizeof(tobj) + 4 * sizeof(udat) ||
      sizeof(struct s_tevent_selectionnotify) !=
          sizeof(tobj) + 2 * sizeof(udat) + 3 * sizeof(uldat) + TW_MAX_MIMELEN + 4 ||
      sizeof(struct s_tevent_selectionrequest) !=
          2 * sizeof(tobj) + 2 * sizeof(udat) + sizeof(uldat) ||
      sizeof(struct s_tevent_control) != sizeof(tobj) + 4 * sizeof(udat) + sizeof(uldat) ||
      sizeof(struct s_tevent_clientmsg) != sizeof(tobj) + 2 * sizeof(udat) + 2 * sizeof(uldat)) {

    CommonErrno = TW_EBAD_STRUCTS;
    return tfalse;
  }
  return ttrue;
}

static byte MagicNumbers(tw_d TwD) {
  uldat len = 0, chunk = TWIN_MAGIC;
  byte *hostdata;

  Tw_CopyMem(&chunk, Tw_MagicData + Tw_MagicData[0] - sizeof(uldat), sizeof(uldat));

  /* send our magic to server */
  if (!AddQueue(TwD, QWRITE, Tw_MagicData[0], Tw_MagicData) || !Flush(TwD, ttrue))
    return tfalse;

  /* wait for server magic */
  while (Fd != TW_NOFD && (!len || ((hostdata = GetQueue(TwD, QREAD, NULL)), len < *hostdata))) {
    if ((chunk = TryRead(TwD, TIMEOUT_5_SECONDS)) != (uldat)-1)
      len += chunk;
  }

  /*
   * at the moment, no client-side datasize or endianity translation is available...
   * so just check against our magic
   */
  if (Fd != TW_NOFD) {
    hostdata = GetQueue(TwD, QREAD, &len);

    if (*hostdata > TWS_tcolor + sizeof(uldat) + 1 &&
        /*
         * allow server to send us fewer or more types than we sent,
         * but ensure it agrees on the common ones.
         */
        !memcmp(Tw_MagicData + 1, hostdata + 1,
                Min2(*hostdata, Tw_MagicData[0]) - sizeof(uldat) - 2)) {
      if (!memcmp(Tw_MagicData + Tw_MagicData[0] - sizeof(uldat),
                  hostdata + *hostdata - sizeof(uldat), sizeof(uldat))) {
        DeQueue(TwD, QREAD, *hostdata);
        return ttrue;
      } else
        Errno = TW_ESERVER_BAD_ENDIAN;
    } else
      Errno = TW_ESERVER_BAD_SIZES;
  }
  return tfalse;
}

#define digestLen 16     /* hardcoded in MD5 routines */
#define hAuthLen 256     /* length of ~/.TwinAuth */
#define challengeLen 512 /* length of ~/.TwinAuth + random data */

static byte MagicChallenge(tw_d TwD) {
  struct MD5Context ctx;
  byte *t;
  char *data, *home;
  uldat len, got, challenge, chunk;
  int fd;

  challenge = ReadUldat(TwD);
  if (Fd == TW_NOFD)
    return tfalse;
  if (challenge == TW_GO_MAGIC)
    return ttrue;
  if (challenge != TW_WAIT_MAGIC) {
    Errno = TW_ESERVER_BAD_PROTOCOL;
    return tfalse;
  }
  if (!(home = getenv("HOME"))) {
    Errno = TW_ENO_AUTH;
    return tfalse;
  }
  if (!WQLeft(digestLen) || !(data = Tw_AllocMem(hAuthLen))) {
    Errno = TW_ESYS_NO_MEM;
    return tfalse;
  }

  len = strlen(home);
  if (len > hAuthLen - 11)
    len = hAuthLen - 11;

  Tw_CopyMem(home, data, len);
  Tw_CopyMem("/.TwinAuth", data + len, 11);
  if ((fd = open(data, O_RDONLY)) < 0) {
    Tw_FreeMem(data);
    Errno = TW_ENO_AUTH;
    return tfalse;
  }
  for (len = 0, got = 1; got && len < hAuthLen; len += got) {
    do {
      got = read(fd, data + len, hAuthLen - len);
    } while (got == (uldat)-1 && errno == EINTR);
    if (got == (uldat)-1)
      break;
  }
  close(fd);

  challenge = ReadUldat(TwD);
  if (Fd == TW_NOFD || got == (uldat)-1 || len + challenge != challengeLen) {
    Tw_FreeMem(data);
    if (Fd != TW_NOFD)
      Errno = TW_ENO_AUTH;
    return tfalse;
  }

  (void)GetQueue(TwD, QREAD, &got);
  while (Fd != TW_NOFD && got < challenge) {
    if ((chunk = TryRead(TwD, TIMEOUT_5_SECONDS)) != (uldat)-1)
      got += chunk;
  }

  if (Fd == TW_NOFD)
    return tfalse;

  MD5Init(&ctx);
  MD5Update(&ctx, data, len);

  t = GetQueue(TwD, QREAD, NULL);
  MD5Update(&ctx, t, challenge);

  t = GetQueue(TwD, QWRITE, NULL); /* we did WQLeft(digestLen) above */
  MD5Final(t, &ctx);

  DeQueue(TwD, QREAD, challenge);

  Flush(TwD, ttrue);
  challenge = ReadUldat(TwD);

  if (challenge == TW_GO_MAGIC)
    return ttrue;
  if (Fd != TW_NOFD)
    Errno = TW_ESERVER_DENIED_CONNECT;
  return tfalse;
}

static const char *tmpdir(void) {
  const char *tmp = getenv("TMPDIR");
  if (tmp == NULL)
    tmp = "/tmp";
  return tmp;
}

static udat copyToSockaddrUn(const char *src, struct sockaddr_un *addr, udat pos) {
  size_t len = strlen(src), max = sizeof(addr->sun_path) - 1; /* for final '\0' */
  if (pos < max) {
    if (len >= max - pos)
      len = max - pos - 1;
    Tw_CopyMem(src, addr->sun_path + pos, len);
    addr->sun_path[pos += len] = '\0';
  }
  return pos;
}

/**
 * opens a connection to server; TwDisplay is the server to contact;
 * if NULL the environment variable $TWDISPLAY is used
 */
tw_d Tw_Open(const char *TwDisplay) {
  tw_d TwD;
  char *options;
  int result = -1, fd = TW_NOFD;
  unsigned i;
  byte gzip = tfalse, handshake = tfalse;

  if (!TwDisplay && (!(TwDisplay = getenv("TWDISPLAY")) || !*TwDisplay)) {
    CommonErrno = TW_ENO_DISPLAY;
    return (tw_d)0;
  }

  if ((options = strchr(TwDisplay, ','))) {
    *options = '\0';
    if (!memcmp(options + 1, "gz", 2))
      gzip = ttrue;
  }

  CommonErrno = 0;

  if (*TwDisplay == ':')
    do {
      /* unix socket */
      struct sockaddr_un addr;
      udat len;

      if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        CommonErrno = TW_ESYS_NO_SOCKET;
        break;
      }
      memset(&addr, 0, sizeof(addr));
      addr.sun_family = AF_UNIX;

      len = copyToSockaddrUn(tmpdir(), &addr, 0);
      len = copyToSockaddrUn("/.Twin", &addr, len);
      len = copyToSockaddrUn(TwDisplay, &addr, len);

      result = connect(fd, (struct sockaddr *)&addr, sizeof(addr));

    } while (0);
  else
    do {
      /* inet socket */
      struct sockaddr_in addr;
      struct hostent *host_info;
      char *server = Tw_CloneStr(TwDisplay), *p;
      unsigned short port;

      if (!server) {
        CommonErrno = TW_ESYS_NO_MEM;
        break;
      }

      p = strchr(server, ':');

      if (!p) {
        CommonErrno = TW_EBAD_DISPLAY;
        Tw_FreeMem(server);
        break;
      }
      *p = '\0';
      /*
       * the following wraps around on 'unsigned short' overflow:
       * useful if we ever want to connect to ports < TW_INET_PORT i.e. < 7754
       */
      port = TW_INET_PORT + strtoul(p + 1, NULL, 16);

      memset(&addr, 0, sizeof(addr));
      addr.sin_family = AF_INET;

      /* check if the server is a numbers-and-dots host like "127.0.0.1" */
      addr.sin_port = htons(port);
      addr.sin_addr.s_addr = inet_addr(server);

      if (addr.sin_addr.s_addr == (uint32_t)-1) {
#ifdef TW_HAVE_GETHOSTBYNAME
        /* may be a FQDN host like "www.gnu.org" */
        host_info = gethostbyname(server);
        if (host_info) {
          Tw_CopyMem(host_info->h_addr, &addr.sin_addr, host_info->h_length);
          addr.sin_family = host_info->h_addrtype;
        } else
#endif
        {
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
        CommonErrno = TW_ESYS_NO_SOCKET;

    } while (0);

  if (options)
    *options = ',';

  if (result == -1) { /* some error occurred */
    if (fd != TW_NOFD) {
      close(fd);
      CommonErrno = TW_ESYS_CANNOT_CONNECT;
    }
    /* try to get a meaningful message for the error */
    if (CommonErrno == TW_ESYS_NO_SOCKET || CommonErrno == TW_ESYS_CANNOT_CONNECT)
      rCommonErrno.S = errno;
    return (tw_d)0;
  }

  if (!(TwD = (tw_d)Tw_AllocMem(sizeof(struct s_tw_d)))) {
    close(fd);
    CommonErrno = TW_ESYS_NO_MEM;
    return (tw_d)0;
  }

  memset(TwD, '\0', sizeof(struct s_tw_d) - sizeof(v_id_vec));
  for (i = 0; i < N_OF(id_Tw); i++)
    /* initialize function ids to 'unknown' */
    id_Tw[i] = TW_BADID;
  /* except for Tw_FindFunction */
  id_Tw[order_FindFunction] = FIND_MAGIC;

  th_r_mutex_init(mutex);
  Fd = fd;

  fcntl(fd, F_SETFD, FD_CLOEXEC);
  fcntl(fd, F_SETFL, O_NONBLOCK);

  th_mutex_lock(OpenCountMutex);
  OpenCount++;
  th_mutex_unlock(OpenCountMutex);

  LOCK;
  handshake = ProtocolNumbers(TwD) && MagicNumbers(TwD) && MagicChallenge(TwD);
  UNLK;

  if (handshake) {
    if (gzip)
      (void)Tw_EnableGzip(TwD);
    return TwD;
  }

  if (Fd != TW_NOFD) {
    close(Fd);
    Fd = TW_NOFD; /* to skip Flush() */
  }
  Tw_Close(TwD);
  return (tw_d)0;
}

/* just like all Tw_* functions, this requires LOCK not to be held */
/*
 * Tw_Close() is not completely thread-safe, but it would be useless anyway
 * for it to be thread safe, since after Tw_Close() the TwD pointer is invalid
 * and the only way for other threads to know that is to cooperate with the one
 * that issued Tw_Close().
 */
/**
 * closes a server connection
 */
void Tw_Close(tw_d TwD) {
  s_tw_errno *E;
  byte *q;
  int i;

  if (!TwD)
    return;

  LOCK;

  if (Fd != TW_NOFD) {
    Flush(TwD, ttrue);
    close(Fd);
    Fd = TW_NOFD;
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

  DeleteAllListeners(TwD->AVLRoot);

  /*PanicFlag = tfalse;*/
  UNLK;
  th_r_mutex_destroy(mutex);

  FreeErrnoLocation(TwD);
  Tw_FreeMem(TwD);

  th_mutex_lock(OpenCountMutex);
  OpenCount--;
  th_mutex_unlock(OpenCountMutex);
}

/*
 * Tw_AttachGetReply() returns 0 for failure, 1 for success,
 * else message string (len) bytes long.
 *
 * it bypasses any compression.
 */
/* this requires LOCK not to be held */
/**
 * returns one of the messages produced by server after Tw_Attach()
 */
const char *Tw_AttachGetReply(tw_d TwD, uldat *len) {
  uldat chunk;
  char *answ = (char *)-1, *nul;
#ifdef CONF_SOCKET_GZ
  byte wasGzipFlag;
#endif

  if (!TwD) {
    return NULL;
  }

  LOCK;

#ifdef CONF_SOCKET_GZ
  wasGzipFlag = GzipFlag;
  GzipFlag = tfalse;
#endif

  if (Fd != TW_NOFD)
    do {

      answ = (char *)GetQueue(TwD, QREAD, &chunk);
      if (!chunk) {
        (void)TryRead(TwD, TIMEOUT_INFINITE);
        answ = (char *)GetQueue(TwD, QREAD, &chunk);
      }
      if (chunk) {
        if ((nul = memchr(answ, '\0', chunk))) {
          if (nul == answ && nul + 1 < answ + chunk) {
            DeQueue(TwD, QREAD, 2);
            answ = (char *)(size_t)nul[1];
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
/**
 * tells the server to confirm the Tw_Attach() previously issued
 */
void Tw_AttachConfirm(tw_d TwD) {
  if (!TwD) {
    return;
  }
  LOCK;
  if (Fd != TW_NOFD) {
    write(Fd, "\1", 1);
  }
  UNLK;
}

/* this requires LOCK not to be held */
/**
 * returns a pointer to last error information (libtw equivaled of errno_location())
 */
tw_errno *Tw_ErrnoLocation(tw_d TwD) {
  s_tw_errno *t;
  if (TwD) {
    LOCK;
    t = GetErrnoLocation(TwD);
    UNLK;
  } else
    t = &rCommonErrno;
  return (tw_errno *)t;
}

/**
 * returns a string description of given error
 */
TW_ATTR_FN_CONST const char *Tw_StrError(const tw_d TwD, uldat e) {
  switch (e) {
  case 0:
    return "success";
  case TW_ESERVER_BAD_ENDIAN:
    return "server has reversed endianity, impossible to connect";
  case TW_ESERVER_BAD_SIZES:
    return "server has different data sizes, impossible to connect";
  case TW_EBAD_SIZES:
    return "compiled data sizes are incompatible with libtw now in use!";
  case TW_EBAD_STRUCTS:
    return "internal error: structs are not packed! Please contact the author.";
  case TW_ENO_DISPLAY:
    return "TWDISPLAY is not set";
  case TW_EBAD_DISPLAY:
    return "badly formed TWDISPLAY";
  case TW_ENO_AUTH:
    return "bad or missing authorization file ~/.TwinAuth, cannot connect";
  case TW_ESYS_CANNOT_CONNECT:
    return "failed to connect: ";
  case TW_ESYS_NO_MEM:
    return "out of memory!";
  case TW_ESYS_CANNOT_WRITE:
    return "failed to send data to server: ";
  case TW_ESYS_NO_SOCKET:
    return "failed to create socket: ";
  case TW_ESERVER_LOST_CONNECT:
    return "connection lost ";
  case TW_ESERVER_ALREADY_CONNECT:
    return "already connected";
  case TW_ESERVER_BAD_PROTOCOL:
    return "got invalid data from server, protocol violated";
  case TW_ESERVER_NO_FUNCTION:
    return "function not supported by server: ";
  case TW_ESERVER_BAD_FUNCTION:
    return "function is not a possible server function";
  case TW_ESERVER_DENIED_CONNECT:
    return "server denied permission to connect, file ~/.TwinAuth may be wrong";
  case TW_EGZIP_BAD_PROTOCOL:
    return "got invalid data from server, gzip format violated";
  case TW_EGZIP_INTERNAL:
    return "internal gzip error, panic!";
  case TW_ENO_HOST:
    return "unknown host in TWDISPLAY: ";
  case TW_ESERVER_BAD_VERSION:
    return "server has incompatible protocol version, impossible to connect";
  case TW_ESERVER_BAD_RETURN:
    return "server function call returned strange data, wrong data sizes? : ";
  case TW_ECALL_BAD:
    return "function call rejected by server, wrong data sizes? : ";
  case TW_ECALL_BAD_ARG:
    return "function call rejected by server, invalid arguments? : ";
  case TW_ESERVER_READ_TIMEOUT:
    return "failed to receive data from server: read timeout";
  default:
    return "unknown error";
  }
}

/**
 * returns a string description of given error detail
 */
TW_ATTR_FN_CONST const char *Tw_StrErrorDetail(const tw_d TwD, uldat E, uldat S) {
  switch (E) {
  case TW_ESERVER_LOST_CONNECT:
    switch (S) {
    case TW_EDETAIL_NO_MODULE:
      return "(socket module may be not running on server)";
    default:
      break;
    }
    return "(explicit kill or server shutdown)";
  case TW_ESYS_CANNOT_CONNECT:
  case TW_ESYS_CANNOT_WRITE:
  case TW_ESYS_NO_SOCKET:
    return strerror(S);
  case TW_ENO_HOST:
    return hstrerror(S);
  case TW_ESERVER_NO_FUNCTION:
  case TW_ESERVER_BAD_RETURN:
  case TW_ECALL_BAD:
  case TW_ECALL_BAD_ARG:
    if (Functions[S].name)
      return Functions[S].name;
    break;
  }
  return "";
}

/* this requires LOCK not to be held */
/**
 * returns the file descriptor used by the connection
 */
int Tw_ConnectionFd(tw_d TwD) {
  int f = -1;
  if (TwD) {
    LOCK;
    f = Fd;
    UNLK;
  }
  return f;
}

/* hack:
 * TwReadMsg() returns an already DeQueued tmsg.
 * TwPeekMsg() returns a tmsg without DeQueueing it.
 * They work because DeQueue() doesn't touch the data, just sets
 * some variables to mark it free.
 * But if you AddQueueAligned(QMSG) you may junk your precious tmsg.
 * There is no easy solution to this, except for Tw_CloneReadMsg() it,
 * as both DeQueued and still queued tmsg may be moved around
 * by the TwReallocMem() in AddQueueAligned(QMSG) in ParseReplies().
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
 * and in general any Tw() function of libtw.h returning non-void
 * so that it sends something to the server and waits for the server
 * to send back the return value.
 */

/* this requires LOCK to be held */
static tmsg ReadMsg(tw_d TwD, byte Wait, byte deQueue) {
  tmsg msg = (tmsg)0;
  uldat len;

  if (Fd != TW_NOFD) {
    msg = (tmsg)GetQueue(TwD, QMSG, &len);

    if (!len) {
      Flush(TwD, Wait);
      do {
        if (TryRead(TwD, Wait ? TIMEOUT_INFINITE : TIMEOUT_ZERO) != (uldat)-1) {
          ParseReplies(TwD);
          msg = (tmsg)GetQueue(TwD, QMSG, &len);
        }
      } while (Wait && Fd != TW_NOFD && !len);
    }

    if (Fd != TW_NOFD && len) {
      if (deQueue)
        DeQueueAligned(TwD, QMSG, msg->Len);
    } else
      msg = (tmsg)0;
  }
  return msg;
}

/* this requires LOCK not to be held */
/**
 * returns the first tmsg already received, or NULL if no events
 * are available in the queue; it never blocks nor reads from the connection
 */
tmsg Tw_PendingMsg(tw_d TwD) {
  tmsg msg;
  uldat len = 0;
  if (TwD) {
    LOCK;
    msg = (tmsg)GetQueue(TwD, QMSG, &len);
    UNLK;
  }
  if (!len)
    msg = (tmsg)0;
  return msg;
}

/* non-blocking check if there are messages available,
 * reading non-blocking from the socket if necessary */
/* this requires LOCK not to be held */
/**
 * returns the first tmsg already received, or tries to read non-blocking
 * more messages from the connection; if no messages are available,
 * returns NULL
 */
tmsg Tw_PeekMsg(tw_d TwD) {
  tmsg msg = (tmsg)0;
  if (TwD) {
    LOCK;
    msg = ReadMsg(TwD, tfalse, tfalse);
    UNLK;
  }
  return msg;
}

/**
 * returns the first tmsg already received, or tries to read non-blocking
 * more messages from the connection; if Wait is ttrue and no messages are
 * immediately available, blocks until a message is received
 * and returns NULL only in case of a fatal error (panic);
 *
 * in any case, the tmsg returned is removed from the queue.
 */
tmsg Tw_ReadMsg(tw_d TwD, byte Wait) {
  tmsg msg = (tmsg)0;
  if (TwD) {
    LOCK;
    msg = ReadMsg(TwD, Wait, ttrue);
    UNLK;
  }
  return msg;
}

/**
 * behaves exactly like Tw_ReadMsg(), but a Tw_AllocMem()ed copy of the message
 * is returned to avoid concurrency problems with other threads;
 * you must Tw_FreeMem() it when done!
 */
tmsg Tw_CloneReadMsg(tw_d TwD, byte Wait) {
  tmsg msg, ClonedMsg = (tmsg)0;
  if (TwD) {
    LOCK;
    if ((msg = ReadMsg(TwD, Wait, ttrue)) && (ClonedMsg = (tmsg)Tw_AllocMem(msg->Len))) {
      Tw_CopyMem(msg, ClonedMsg, msg->Len);
    }
    UNLK;
  }
  return ClonedMsg;
}

/* AVL listeners tree handling functions */

TW_INLINE uldat TwAVLgetkey(uldat Type, tevent_common Event) {
  /* only (udat)Type is significative, not whole Type. */
  return (Type << 5) ^ Event->W ^ ((uldat)Event->Code << ((sizeof(uldat) - sizeof(udat)) * 8));
}

#define TwAVLgetkeyMsg(msg) TwAVLgetkey((msg)->Type, &(msg)->Event.EventCommon)

#define TwAVLgetkeyListener(L) TwAVLgetkey((L)->Type, &(L)->Event->EventCommon)

/* this assumes L1->AVLkey == L2->AVLkey */
static int CompareListeners(tlistener L1, tlistener L2) {
  tevent_any L1A = L1->Event, L2A = L2->Event;

  if (L1->Type == L2->Type) {
    if (L1A->EventCommon.W == L2A->EventCommon.W) {
      if (L1A->EventCommon.Code == L2A->EventCommon.Code) {
        /* common part matches. check details. */
        switch (L1->Type) {
          /*
           * WARNING: I am assuming all fields of a union
           * have the same address as the union itself
           */
        case TW_MSG_WIDGET_KEY:
        case TW_MSG_WIDGET_MOUSE:
          return L1A->EventKeyboard.ShiftFlags - L2A->EventKeyboard.ShiftFlags;
        case TW_MSG_MENU_ROW:
          return L1A->EventMenu.Menu - L2A->EventMenu.Menu;
        case TW_MSG_WIDGET_CHANGE:
        case TW_MSG_WIDGET_GADGET:
        case TW_MSG_SELECTION:
        case TW_MSG_SELECTIONNOTIFY:
        case TW_MSG_SELECTIONREQUEST:
        case TW_MSG_SELECTIONCLEAR:
        case TW_MSG_USER_CONTROL:
          /* no extra checks needed */
        default:
          break;
        }
        return 0;
      }
      return L1A->EventCommon.Code - L2A->EventCommon.Code;
    }
    return L1A->EventCommon.W - L2A->EventCommon.W;
  }
  return L1->Type - L2->Type;
}

static tlistener FindListener(tw_d TwD, tmsg msg) {
  struct s_tlistener key;

  key.Type = msg->Type;
  key.Event = &msg->Event;
  key.AVLkey = TwAVLgetkeyMsg(msg);

  return (tlistener)AVLFind((tavl)&key, (tavl)TwD->AVLRoot, (tavl_compare)CompareListeners);
}

static void InsertListener(tw_d TwD, tlistener L) {
  if (L && !L->TwD) {
    L->AVLkey = TwAVLgetkeyListener(L);
    L->TwD = TwD;
    AVLInsert((tavl)L, (tavl)TwD->AVLRoot, (tavl_compare)CompareListeners, (tavl *)&TwD->AVLRoot);
  }
}

static void RemoveListener(tw_d TwD, tlistener L) {
  if (L && L->TwD == TwD) {
    AVLRemove((tavl)L, (tavl_compare)CompareListeners, (tavl *)&TwD->AVLRoot);
    L->TwD = NULL;
  }
}

static void DeleteAllListeners(tlistener L) {
  if (L) {
    DeleteAllListeners(L->Left);
    DeleteAllListeners(L->Right);
    if (L->Event)
      Tw_FreeMem(L->Event);
    Tw_FreeMem(L);
  }
}

/**
 * adds an event listener to connection; event listeners are used only
 * if you call Tw_MainLoop() or Tw_DispatchMsg()
 */
void Tw_InsertListener(tw_d TwD, tlistener L) {
  if (!TwD) {
    return;
  }
  LOCK;
  InsertListener(TwD, L);
  UNLK;
}

/**
 * removes an event listener from connection
 */
void Tw_RemoveListener(tw_d TwD, tlistener L) {
  if (!TwD) {
    return;
  }
  LOCK;
  RemoveListener(TwD, L);
  UNLK;
}

/**
 * deletes an event listener
 */
void Tw_DeleteListener(tw_d TwD, tlistener L) {
  if (!TwD) {
    return;
  }
  LOCK;
  if (L->TwD == TwD) {
    RemoveListener(TwD, L);
    if (L->Event)
      Tw_FreeMem(L->Event);
    Tw_FreeMem(L);
  }
  UNLK;
}

/**
 * sets the fallback event listener, to be called when no other listeners match
 */
void Tw_SetDefaultListener(tw_d TwD, tfn_default_listener Listener, void *Arg) {
  if (!TwD) {
    return;
  }
  LOCK;
  TwD->DefaultListener = Listener;
  TwD->DefaultArg = Arg;
  UNLK;
}

static tlistener CreateListener(tw_d TwD, udat Type, tevent_any E, tfn_listener Listener,
                                void *Arg) {
  tlistener L = (tlistener)0;
  if (TwD && (L = (tlistener)Tw_AllocMem(sizeof(struct s_tlistener)))) {
    L->AVLParent = NULL;
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

/* this does NOT add the created tlistener to the display! */
/**
 * creates an event listener; you must manually Tw_InsertListener() it
 */
tlistener Tw_CreateListener(tw_d TwD, udat Type, tevent_any E, tfn_listener Listener, void *Arg) {
  tlistener L = (tlistener)0;
  if (TwD && (L = (tlistener)Tw_AllocMem(sizeof(struct s_tlistener)))) {
    L->AVLParent = NULL;
    L->Type = Type;
    L->Left = L->Right = NULL;
    L->Event = E;
    L->Listener = Listener;
    L->Arg = Arg;
    L->TwD = NULL;
  }
  return L;
}

/**
 * changes the already inserted event listener to listen for
 * the new Type and event
 */
void Tw_SetTEListener(tw_d TwD, tlistener L, udat Type, tevent_any E) {
  if (!TwD || !L) {
    return;
  }
  LOCK;
  if (L->TwD && L->TwD == TwD) {
    RemoveListener(TwD, L);
    L->Type = Type;
    L->Event = E;
    InsertListener(TwD, L);
  } else if (!L->TwD) {
    L->Type = Type;
    L->Event = E;
  }
  UNLK;
}

static tevent_any CreateEvent(twidget W, udat Code, udat ShiftFlags) {

  tevent_common E;

  if ((E = (tevent_common)Tw_AllocMem(sizeof(struct s_tevent_common)))) {
    E->W = W;
    E->Code = Code;
    E->pad = ShiftFlags;
    /* warning: we RELY on tevent_keyboard->ShiftFlags
     * to be the same as tevent_common->pad */
  }
  return (tevent_any)E;
}

static tevent_any CreateMenuEvent(twidget W, tmenu Menu, udat Code) {

  tevent_menu E;

  if ((E = (tevent_menu)Tw_AllocMem(sizeof(struct s_tevent_menu)))) {
    E->W = W;
    E->Code = Code;
    E->Menu = Menu;
    E->Row = (trow)0;
  }
  return (tevent_any)E;
}

static tlistener AddListener(tw_d TwD, udat Type, twidget W, udat Code, udat ShiftFlags,
                             tfn_listener Listener, void *Arg) {

  tevent_any E;
  if (TwD && (E = CreateEvent(W, Code, ShiftFlags))) {
    tlistener L;
    if ((L = CreateListener(TwD, Type, E, Listener, Arg)))
      return L;
    Tw_FreeMem(E);
  }
  return NULL;
}

static tlistener AddMenuListener(tw_d TwD, udat Type, twidget W, tmenu Menu, udat Code,
                                 tfn_listener Listener, void *Arg) {
  tevent_any E;
  if (TwD && (E = CreateMenuEvent(W, Menu, Code))) {
    tlistener L;
    if ((L = CreateListener(TwD, Type, E, Listener, Arg)))
      return L;
    Tw_FreeMem(E);
  }
  return NULL;
}

/**
 * creates and adds a keyboard event listener
 */
tlistener Tw_AddKeyboardListener(tw_d TwD, twidget W, udat Code, udat ShiftFlags,
                                 tfn_listener Listener, void *Arg) {
  return AddListener(TwD, TW_MSG_WIDGET_KEY, W, Code, ShiftFlags, Listener, Arg);
}

/**
 * creates and adds a mouse event listener
 */
tlistener Tw_AddMouseListener(tw_d TwD, twidget W, udat Code, udat ShiftFlags,
                              tfn_listener Listener, void *Arg) {
  return AddListener(TwD, TW_MSG_WIDGET_MOUSE, W, Code, ShiftFlags, Listener, Arg);
}

/**
 * creates and adds a control message listener
 */
tlistener Tw_AddControlListener(tw_d TwD, twidget W, udat Code, tfn_listener Listener, void *Arg) {
  return AddListener(TwD, TW_MSG_USER_CONTROL, W, Code, 0, Listener, Arg);
}

/**
 * creates and adds a client message listener
 */
tlistener Tw_AddClientMsgListener(tw_d TwD, twidget W, udat Code, tfn_listener Listener,
                                  void *Arg) {
  return AddListener(TwD, TW_MSG_USER_CLIENTMSG, W, Code, 0, Listener, Arg);
}

/**
 * creates and adds a `display' message listener
 * (the ones used by twdisplay to implement remote display hw)
 */
tlistener Tw_AddDisplayListener(tw_d TwD, udat Code, tfn_listener Listener, void *Arg) {
  return AddListener(TwD, TW_MSG_DISPLAY, TW_NOID, Code, 0, Listener, Arg);
}

/**
 * creates and adds a widget change message listener
 */
tlistener Tw_AddWidgetListener(tw_d TwD, twidget W, udat Code, tfn_listener Listener, void *Arg) {
  return AddListener(TwD, TW_MSG_WIDGET_CHANGE, W, Code, 0, Listener, Arg);
}

/**
 * creates and adds a gadget message listener
 */
tlistener Tw_AddGadgetListener(tw_d TwD, twidget W, udat Code, tfn_listener Listener, void *Arg) {
  return AddListener(TwD, TW_MSG_WIDGET_GADGET, W, Code, 0, Listener, Arg);
}

/**
 * creates and adds a menu activation message listener
 */
tlistener Tw_AddMenuListener(tw_d TwD, twidget W, tmenu Menu, udat Code, tfn_listener Listener,
                             void *Arg) {
  return AddMenuListener(TwD, TW_MSG_MENU_ROW, W, Menu, Code, Listener, Arg);
}

/**
 * creates and adds a selection message listener
 */
tlistener Tw_AddSelectionListener(tw_d TwD, twidget W, tfn_listener Listener, void *Arg) {
  return AddListener(TwD, TW_MSG_SELECTION, W, 0, 0, Listener, Arg);
}

/**
 * creates and adds a selection-notify message listener
 */
tlistener Tw_AddSelectionNotifyListener(tw_d TwD, tfn_listener Listener, void *Arg) {
  return AddListener(TwD, TW_MSG_SELECTIONNOTIFY, TW_NOID, 0, 0, Listener, Arg);
}

/**
 * creates and adds a selection-request message listener
 */
tlistener Tw_AddSelectionRequestListener(tw_d TwD, tfn_listener Listener, void *Arg) {
  return AddListener(TwD, TW_MSG_SELECTIONREQUEST, TW_NOID, 0, 0, Listener, Arg);
}

static byte DispatchMsg(tdisplay TwD, tmsg msg, byte mustClone) {
  tlistener L;
  tfn_listener Listener;
  tfn_default_listener DefaultListener;
  void *Arg;
  tmsg ClonedMsg;

  if (!TwD) {
    return tfalse;
  } else if ((L = FindListener(TwD, msg))) {
    Listener = L->Listener;
    Arg = L->Arg;
    DefaultListener = NULL;
  } else if (TwD->DefaultListener) {
    DefaultListener = TwD->DefaultListener;
    Arg = TwD->DefaultArg;
    Listener = NULL;
  } else
    return tfalse;

  /*
   * Tw_MainLoop calls us with a msg still inside Queue[].
   * Clone it before calling Listener to avoid problems.
   *
   * If the user calls Tw_DispatchMsg, it's up to him to ensure
   * there are no problems (possibly using Tw_CloneReadMsg() instead
   * of Tw_ReadMsg() ).
   */
  if (mustClone) {
    if ((ClonedMsg = (tmsg)Tw_AllocMem(msg->Len)))
      Tw_CopyMem(msg, ClonedMsg, msg->Len);
  } else
    ClonedMsg = msg;

  if (ClonedMsg) {
    UNLK;
    if (Listener)
      Listener(&ClonedMsg->Event, Arg);
    else
      DefaultListener(ClonedMsg, Arg);
    LOCK;
    if (mustClone)
      Tw_FreeMem(ClonedMsg);
    return ttrue;
  }
  return tfalse;
}

/**
 * calls the appropriate event listener for given tmsg
 */
byte Tw_DispatchMsg(tdisplay TwD, tmsg msg) {
  byte ret = tfalse;
  if (TwD) {
    LOCK;
    ret = DispatchMsg(TwD, msg, tfalse);
    UNLK;
  }
  return ret;
}

/**
 * utility function: sits in a loop, waiting for messages and calling
 * the appropriate event listener for them, until a fatal error occurs
 */
byte Tw_MainLoop(tw_d TwD) {
  byte ret = tfalse;
  tmsg msg;
  if (TwD) {
    LOCK;
    Errno = 0;
    while (!TwD->ExitMainLoop && (msg = ReadMsg(TwD, ttrue, ttrue)))
      (void)DispatchMsg(TwD, msg, ttrue);
    ret = TwD->ExitMainLoop || Errno == 0;
    UNLK;
  }
  return ret;
}

void Tw_ExitMainLoop(tw_d TwD) {
  if (TwD) {
    TwD->ExitMainLoop = ttrue;
  }
}

static void FailedCall(tw_d TwD, uldat err, uldat order) {
  s_tw_errno *vec = GetErrnoLocation(TwD);
  vec->E = err;
  vec->S = order;
}

TW_INLINE uldat NextSerial(tw_d TwD) {
  if (++RequestN == tmsg_magic)
    ++RequestN;
  return RequestN;
}

TW_INLINE void Send(tw_d TwD, uldat Serial, uldat idFN) {
  /* be careful with aligmnent!! */
  byte *R = (byte *)r;
  Push(R, uldat, s - (byte *)(r + 1));
  Push(R, uldat, Serial);
  Push(R, uldat, idFN);
}

/***********/

#include "encode.h"

static uldat FindFunctionId(tw_d TwD, uldat order);

#define DECL_MyReply                                                                               \
  byte *MyReply, *MyData;                                                                          \
  uldat MyLen, MyCode;
#define INIT_MyReply                                                                               \
  (Pop(MyReply, uldat, MyLen), /*skip Serial:*/ Pop(MyReply, uldat, MyCode),                       \
   Pop(MyReply, uldat, MyCode), MyData = MyReply, MyReply -= 3 * sizeof(uldat)),

#define ENCODE_FL_NOLOCK 1
#define ENCODE_FL_LOCK 1
#define ENCODE_FL_VOID 2
#define ENCODE_FL_RETURN 2

static tany _Tw_EncodeCall(byte flags, uldat o, tw_d TwD, ...) {
  struct s_tsfield a[TW_MAX_ARGS_N];
  tsfield b;
  va_list va;
  DECL_MyReply uldat space, myId;
  udat N;

  if (!TwD) {
    return (tany)0;
  }
  a->TWS_field_scalar = TW_NOID;

  flags ^= (ENCODE_FL_NOLOCK | ENCODE_FL_VOID);

  if (flags & ENCODE_FL_LOCK)
    LOCK;
  if (Fd != TW_NOFD && (myId = id_Tw[o]) != TW_NOID &&
      (myId != TW_BADID || (myId = FindFunctionId(TwD, o)) != TW_NOID)) {

    va_start(va, (void *)TwD);
    N = EncodeArgs(o, &space, va, a);
    va_end(va);

    if (N != (udat)-1) {
      if (InitRS(TwD) && WQLeft(space)) {
        /* skip over a[0], that will hold return value */
        for (b = a + 1; N; b++, N--)
          s = PushArg(s, b);

        Send(TwD, (myId = NextSerial(TwD)), id_Tw[o]);
        if (flags & ENCODE_FL_RETURN) {
          if ((MyReply = (void *)Wait4Reply(TwD, myId)) && (INIT_MyReply MyCode == OK_MAGIC)) {
            if (MyLen == 2 * sizeof(uldat) + a[0].label)
              DecodeReply((byte *)MyData, a);
            else
              FailedCall(TwD, TW_ESERVER_BAD_PROTOCOL, o);
          } else {
            FailedCall(TwD, MyReply && MyCode != (uldat)-1 ? TW_ECALL_BAD_ARG : TW_ECALL_BAD, o);
          }
          if (MyReply)
            KillReply(TwD, (byte *)MyReply, MyLen);
        }
      } else {
        /* still here? must be out of memory! */
        Errno = TW_ESYS_NO_MEM;
        Fail(TwD);
      }
    } else {
      /* N == (udat)-1, EncodeArgs() failed ! */
      FailedCall(TwD, TW_ECALL_BAD, o);
    }
  } else if (Fd != TW_NOFD)
    FailedCall(TwD, TW_ESERVER_NO_FUNCTION, o);
  if (flags & ENCODE_FL_LOCK)
    UNLK;
  return a->TWS_field_scalar;
}

#include "libtw2_m4.h"

static byte Sync(tw_d TwD) {
  uldat left;
  return Fd != TW_NOFD ? (GetQueue(TwD, QWRITE, &left), left) ? _Tw_SyncSocket(TwD) : ttrue
                       : tfalse;
}

/**
 * sends all buffered data to connection and waits for server to process it
 */
byte Tw_Sync(tw_d TwD) {
  byte ok = tfalse;
  if (TwD) {
    LOCK;
    ok = Sync(TwD);
    UNLK;
  }
  return ok;
}

/* wrap _Tw_FindFunction() with LOCKs */
/**
 * returns the server-dependant order number of a function (specified as string),
 * or NOID if server is missing that function
 */
uldat Tw_FindFunction(tw_d TwD, byte Len, const char *Name, byte FormatLen, const char *Format) {
  uldat myId = 0;
  if (TwD) {
    LOCK;
    myId = _Tw_FindFunction(TwD, Len, Name, FormatLen, Format);
    UNLK;
  }
  return myId;
}

static uldat FindFunctionId(tw_d TwD, uldat order) {
  uldat myId;
  if ((myId = id_Tw[order]) == TW_BADID) {
    myId = id_Tw[order] = _Tw_FindFunction(TwD, Functions[order].len, 3 + Functions[order].name,
                                           Functions[order].formatlen, Functions[order].format + 1);
  }
  return Fd != TW_NOFD ? myId : TW_NOID;
}

/* convert tcell to protocol <= 4.7.x i.e. {unicode64k_lo, color, unicode64k_hi, extra} */
static tcell ConvertTCellTo47x(tcell cell) {
  trune f = TRUNE(cell);
  tcolor col = TCOLOR(cell);
  if (f > 0xFFFF) /* maximum supported by protocol <= 4.7.x is 64k */
    f = 0xFFFD;   /* use replacement char instead */
  return (f & 0xFF) | ((tcell)col << 8) | (((tcell)f & 0xFF00) << 8);
}

TW_INLINE tcell MaybeConvertTCell(tw_d TwD, tcell cell) {
  if (ServFlags & ServFlagTCell47x)
    cell = ConvertTCellTo47x(cell);
  return cell;
}

#define PushVMaybeConvertTCell(TwD, dst, len, vec)                                                 \
  ((dst) = VecMaybeConvertTCell(TwD, dst, len, vec))

static byte *VecMaybeConvertTCell(tw_d TwD, byte *dst, uldat len, const tcell *cell) {
  if (ServFlags & ServFlagTCell47x) {
    uldat i;
    tcell h;
    len /= sizeof(tcell);
    for (i = 0; i < len; i++) {
      h = ConvertTCellTo47x(cell[i]);
      Push(dst, tcell, h);
    }
  } else {
    PushV(dst, len, cell);
  }
  return dst;
}

/**
 * sends all buffered data to connection and waits for server to process it
 */
byte Tw_SyncSocket(tw_d TwD) {
  return Tw_Sync(TwD);
}

/* handy special cases (also for compatibility) */

/**
 * return the owner of this gadget
 */
tmsgport Tw_OwnerWidget(tw_d TwD, twidget a1) {
  return Tw_Stat(TwD, a1, TWS_widget_Owner);
}

/**
 * return prev widget with same owner
 */
tgadget Tw_O_PrevWidget(tw_d TwD, twidget a1) {
  return Tw_Stat(TwD, a1, TWS_widget_O_Prev);
}

/**
 * return next widget with same owner
 */
tgadget Tw_O_NextWidget(tw_d TwD, twidget a1) {
  return Tw_Stat(TwD, a1, TWS_widget_O_Next);
}

/**
 * change the fill patter of given widget
 */
void Tw_SetFillWidget(tw_d TwD, twidget a1, tcell a2) {
  a2 = MaybeConvertTCell(TwD, a2);
  Tw_ChangeField(TwD, a1, TWS_widget_Fill, ~(tcell)0, a2);
}

/**
 * draws given portion of a widget; usually called after a TW_WIDGET_EXPOSE message
 */
void Tw_Draw2Widget(tw_d TwD, twidget a1, dat a2, dat a3, dat a4, dat a5, dat pitch, const char *a6,
                    const trune *a7, const tcell *a8) {
  uldat len6, len7, len8, myId;
  if (!TwD) {
    return;
  }
  LOCK;
  if (Fd != TW_NOFD && (myId = id_Tw[order_DrawWidget]) != TW_NOID &&
      (myId != TW_BADID || (myId = FindFunctionId(TwD, order_DrawWidget)) != TW_NOID)) {

    if (InitRS(TwD)) {
      myId = (0 + sizeof(uldat) + sizeof(dat) + sizeof(dat) + sizeof(dat) + sizeof(dat) +
              (len6 = a6 ? (uldat)a2 * a3 * sizeof(char) : 0, sizeof(uldat) + len6) +
              (len7 = a7 ? (uldat)a2 * a3 * sizeof(trune) : 0, sizeof(uldat) + len7) +
              (len8 = a8 ? (uldat)a2 * a3 * sizeof(tcell) : 0, sizeof(uldat) + len8));
      if (WQLeft(myId)) {
        Push(s, uldat, a1);
        Push(s, dat, a2);
        Push(s, dat, a3);
        Push(s, dat, a4);
        Push(s, dat, a5);
        Push(s, uldat, len6);
        while (len6) {
          PushV(s, (uldat)a2 * sizeof(char), a6);
          a6 += pitch;
          len6 -= (uldat)a2 * sizeof(char);
        }
        Push(s, uldat, len7);
        while (len7) {
          PushV(s, (uldat)a2 * sizeof(trune), a7);
          a7 += pitch;
          len7 -= (uldat)a2 * sizeof(trune);
        }
        Push(s, uldat, len8);
        while (len8) {
          PushVMaybeConvertTCell(TwD, s, (uldat)a2 * sizeof(tcell), a8);
          a8 += pitch;
          len8 -= (uldat)a2 * sizeof(tcell);
        }
        Send(TwD, (myId = NextSerial(TwD)), id_Tw[order_DrawWidget]);
        UNLK;
        return;
      }
    }
    /* still here? must be out of memory! */
    Errno = TW_ESYS_NO_MEM;
    Fail(TwD);
  } else if (Fd != TW_NOFD)
    FailedCall(TwD, TW_ESERVER_NO_FUNCTION, order_DrawWidget);

  UNLK;
}

/**
 * draws given portion of a widget; usually called after a TW_WIDGET_EXPOSE message
 */
void Tw_DrawTextWidget(tw_d TwD, twidget W, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch,
                       const char *Text) {
  Tw_Draw2Widget(TwD, W, XWidth, YWidth, Left, Up, Pitch, Text, NULL, NULL);
}

/**
 * draws given portion of a widget; usually called after a TW_WIDGET_EXPOSE message
 */
void Tw_DrawTRuneWidget(tw_d TwD, twidget W, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch,
                        const trune *Font) {
  Tw_Draw2Widget(TwD, W, XWidth, YWidth, Left, Up, Pitch, NULL, Font, NULL);
}

/**
 * draws given portion of a widget; usually called after a TW_WIDGET_EXPOSE message
 */
void Tw_DrawTCellWidget(tw_d TwD, twidget W, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch,
                        const tcell *Attr) {
  Tw_Draw2Widget(TwD, W, XWidth, YWidth, Left, Up, Pitch, NULL, NULL, Attr);
}

/**
 * sets a gadget to pressed or unpressed
 */
void Tw_SetPressedGadget(tw_d TwD, tgadget a1, byte a2) {
  Tw_ChangeField(TwD, a1, TWS_gadget_Flags, TW_GADGETFL_PRESSED, a2 ? TW_GADGETFL_PRESSED : 0);
}

/**
 * returns wether a gadget is pressed or not
 */
byte Tw_IsPressedGadget(tw_d TwD, tgadget a1) {
  return Tw_Stat(TwD, a1, TWS_gadget_Flags) & TW_GADGETFL_PRESSED ? ttrue : tfalse;
}

/**
 * sets a gadget to toggle-type or to normal type
 */
void Tw_SetToggleGadget(tw_d TwD, tgadget a1, byte a2) {
  Tw_ChangeField(TwD, a1, TWS_gadget_Flags, TW_GADGETFL_TOGGLE, a2 ? TW_GADGETFL_TOGGLE : 0);
}

/**
 * return wether a gadget is toggle-type or not
 */
byte Tw_IsToggleGadget(tw_d TwD, tgadget a1) {
  return Tw_Stat(TwD, a1, TWS_gadget_Flags) & TW_GADGETFL_TOGGLE ? ttrue : tfalse;
}

/**
 * return the group of this gadget
 */
tgroup Tw_GroupGadget(tw_d TwD, tgadget a1) {
  return Tw_Stat(TwD, a1, TWS_gadget_Group);
}

/**
 * return prev gadget in same group
 */
tgadget Tw_G_PrevGadget(tw_d TwD, tgadget a1) {
  return Tw_Stat(TwD, a1, TWS_gadget_G_Prev);
}

/**
 * return next gadget in same group
 */
tgadget Tw_G_NextGadget(tw_d TwD, tgadget a1) {
  return Tw_Stat(TwD, a1, TWS_gadget_G_Next);
}

/**
 * sets given portion of gadget's contents
 */
void Tw_WriteTextGadget(tw_d TwD, tgadget G, dat Width, dat Height, const char *Text, dat Left,
                        dat Up) {
  Tw_WriteTextsGadget(TwD, G, 1, Width, Height, Text, Left, Up);
}

/**
 * clears whole gadget contents, then sets given portion of gadget's contents
 */
void Tw_SetTextGadget(tw_d TwD, tgadget G, dat Width, dat Height, const char *Text, dat Left,
                      dat Up) {
  /* clear the whole gadget */
  Tw_WriteTextsGadget(TwD, G, 1, TW_MAXDAT, TW_MAXDAT, NULL, 0, 0);
  /* write the specified text */
  Tw_WriteTextsGadget(TwD, G, 1, Width, Height, Text, Left, Up);
}

/**
 * clears whole gadget contents, then sets given portion of gadget's contents
 */
void Tw_SetTextsGadget(tw_d TwD, tgadget G, byte bitmap, dat Width, dat Height, const char *Text,
                       dat Left, dat Up) {
  /* clear the whole gadget */
  Tw_WriteTextsGadget(TwD, G, bitmap, TW_MAXDAT, TW_MAXDAT, NULL, 0, 0);
  /* write the specified text */
  Tw_WriteTextsGadget(TwD, G, bitmap, Width, Height, Text, Left, Up);
}

/**
 * sets given portion of gadget's contents
 */
void Tw_WriteTRuneGadget(tw_d TwD, tgadget G, dat Width, dat Height, const trune *TRune, dat Left,
                         dat Up) {
  Tw_WriteTRunesGadget(TwD, G, 1, Width, Height, TRune, Left, Up);
}

/**
 * clears whole gadget contents, then sets given portion of gadget's contents
 */
void Tw_SetTRuneGadget(tw_d TwD, tgadget G, dat Width, dat Height, const trune *TRune, dat Left,
                       dat Up) {
  /* clear the whole gadget */
  Tw_WriteTRunesGadget(TwD, G, 1, TW_MAXDAT, TW_MAXDAT, NULL, 0, 0);
  /* write the specified text */
  Tw_WriteTRunesGadget(TwD, G, 1, Width, Height, TRune, Left, Up);
}

/**
 * clears whole gadget contents, then sets given portion of gadget's contents
 */
void Tw_SetTRunesGadget(tw_d TwD, tgadget G, byte bitmap, dat Width, dat Height, const trune *TRune,
                        dat Left, dat Up) {
  /* clear the whole gadget */
  Tw_WriteTRunesGadget(TwD, G, bitmap, TW_MAXDAT, TW_MAXDAT, NULL, 0, 0);
  /* write the specified text */
  Tw_WriteTRunesGadget(TwD, G, bitmap, Width, Height, TRune, Left, Up);
}

/**
 * creates a row for a menu
 */
trow Tw_Create4MenuRow(tw_d TwD, twindow Window, udat Code, byte Flags, ldat Len,
                       const char *Text) {
  return Tw_Create4MenuAny(TwD, (tobj)Window, (twindow)0, Code, Flags, Len, Text);
}

/**
 * creates a menuitem for a menu
 */
tmenuitem Tw_Create4MenuMenuItem(tdisplay TwD, tobj Parent, twindow Window, byte Flags, dat Len,
                                 const char *Name) {
  return Tw_Create4MenuAny(TwD, Parent, Window, (udat)0, Flags, Len, Name);
}

/***********/

/* tslist flags: */
#define TWS_CLONE_MEM 1
#define TWS_SORTED 2
#define TWS_SCALAR 4

static tslist StatA(tw_d TwD, tobj Id, udat flags, uldat hN, const udat *h, tslist f);

/**
 * returns information about given object
 */
tany Tw_Stat(tw_d TwD, tobj Id, udat h) {
  struct s_tslist f;
  if (StatA(TwD, Id, TWS_SCALAR, 1, &h, &f))
    return f.TSF[0].TWS_field_scalar;
  return TW_NOID;
}
/**
 * returns information about given object
 */
tslist Tw_StatL(tw_d TwD, tobj Id, uldat hN, ...) {
  tslist TS;
  va_list ap;

  va_start(ap, hN);
  TS = Tw_StatV(TwD, Id, hN, ap);
  va_end(ap);

  return TS;
}
/**
 * returns information about given object
 */
tslist Tw_StatA(tw_d TwD, tobj Id, uldat hN, const udat *h) {
  return StatA(TwD, Id, 0, hN, h, NULL);
}
/**
 * returns information about given object
 */
tslist Tw_StatV(tw_d TwD, tobj Id, uldat hN, va_list ap) {
  tslist TS = NULL;
  udat i, *h;

  if (hN && (h = Tw_AllocMem(hN * sizeof(udat)))) {
    for (i = 0; i < hN; i++)
      h[i] = va_arg(ap, int);
    TS = StatA(TwD, Id, 0, hN, h, NULL);
    Tw_FreeMem(h);
  }
  return TS;
}
/**
 * returns information about given object
 */
tslist Tw_CloneStatL(tw_d TwD, tobj Id, uldat hN, ...) {
  tslist TS;
  va_list ap;

  va_start(ap, hN);
  TS = Tw_CloneStatV(TwD, Id, hN, ap);
  va_end(ap);

  return TS;
}
/**
 * returns information about given object
 */
tslist Tw_CloneStatA(tw_d TwD, tobj Id, uldat hN, const udat *h) {
  return StatA(TwD, Id, TWS_CLONE_MEM, hN, h, NULL);
}
/**
 * returns information about given object
 */
tslist Tw_CloneStatV(tw_d TwD, tobj Id, uldat hN, va_list ap) {
  tslist TS = NULL;
  udat i, *h;

  if (hN && (h = Tw_AllocMem(hN * sizeof(udat)))) {
    for (i = 0; i < hN; i++)
      h[i] = va_arg(ap, int);
    TS = StatA(TwD, Id, TWS_CLONE_MEM, hN, h, NULL);
    Tw_FreeMem(h);
  }
  return TS;
}

/**
 * returns information about given object
 */
tslist TwStatL(tobj Id, uldat hN, ...) {
  tslist TS;
  va_list ap;

  va_start(ap, hN);
  TS = Tw_StatV(Tw_DefaultD, Id, hN, ap);
  va_end(ap);

  return TS;
}
/**
 * returns information about given object
 */
tslist TwCloneStatL(tobj Id, uldat hN, ...) {
  tslist TS;
  va_list ap;

  va_start(ap, hN);
  TS = Tw_CloneStatV(Tw_DefaultD, Id, hN, ap);
  va_end(ap);

  return TS;
}

/**
 * free information about an object that was returned
 * with one of the Tw_*Stat() functions
 */
void Tw_DeleteStat(tw_d TwD, tslist TSL) {
  if (TSL) {
    if (TSL->flags & TWS_CLONE_MEM) {
      udat i;
      tsfield f;
      for (i = 0; i < TSL->N; i++) {
        f = TSL->TSF;
        if (f->type >= TWS_vec && (f->type & ~TWS_vec) < TWS_last && f->TWS_field_vecV)
          Tw_FreeMem(f->TWS_field_vecV);
      }
    }
    Tw_FreeMem(TSL);
  }
}

static int CompareTSF(const tsfield f1, const tsfield f2) {
  return (int)f1->label - f2->label;
}

/**
 * searches for a specific field in object information
 */
tsfield Tw_FindStat(tw_d TwD, tslist TSL, udat label) {
  struct s_tsfield f;

  f.label = label;

  return (tsfield)bsearch(&f, TSL->TSF, TSL->N, sizeof(struct s_tsfield),
                          (int (*)(const void *, const void *))CompareTSF);
}

static void SortTSL(tslist TSL) {
  tsfield f, end;
  udat label;

  if (!(TSL->flags & TWS_SORTED)) {
    TSL->flags |= TWS_SORTED;
    for (label = 0, f = TSL->TSF, end = f + TSL->N; f < end; f++) {
      if (label > f->label)
        /* not sorted */
        break;
      label = f->label;
    }
    if (f < end)
      qsort(TSL->TSF, TSL->N, sizeof(struct s_tsfield),
            (int (*)(const void *, const void *))CompareTSF);
  }
}

static tslist StatScalar(tslist f, byte *data, byte *end) {
  udat Type, N;

  Pop(data, udat, N);
  Pop(data, udat, Type); /* pad */

  if (N == 1 && data + 2 * sizeof(udat) <= end) {
    Pop(data, udat, Type); /* label */
    Pop(data, udat, Type);
    switch (Type) {
#define Popcase(type)                                                                              \
  case TWS_CAT(TWS_, type):                                                                        \
    if (data + sizeof(type) <= end) {                                                              \
      /* avoid padding problems */                                                                 \
      type tmp;                                                                                    \
      Pop(data, type, tmp);                                                                        \
      f->TSF[0].TWS_field_scalar = tmp;                                                            \
    }                                                                                              \
    break

      Popcase(byte);
      Popcase(dat);
      Popcase(ldat);
      Popcase(tcolor);
      Popcase(topaque);
      Popcase(tany);
      Popcase(trune);
      Popcase(tcell);
      Popcase(tobj);
#undef Popcase
    default:
      return (tslist)0;
    }
    return f;
  }
  return (tslist)0;
}

static tslist StatTSL(tw_d TwD, udat flags, byte *data, byte *end) {
  tslist TSL;
  tsfield TSF;
  udat i, N;
  byte ok = ttrue;

  Pop(data, udat, N);
  Pop(data, udat, i); /* pad */

  if (N && (TSL = Tw_AllocMem(sizeof(struct s_tslist) + (N - 1) * sizeof(struct s_tsfield)))) {
    TSL->N = N;
    TSL->flags = flags;
    TSF = TSL->TSF;
    for (i = 0; i < N && ok && data + 2 * sizeof(udat) <= end; i++) {
      Pop(data, udat, TSF[i].label);
      Pop(data, udat, TSF[i].type);
      switch (TSF[i].type) {
#define Popcase(type)                                                                              \
  case TWS_CAT(TWS_, type):                                                                        \
    if (data + sizeof(type) <= end) {                                                              \
      /* avoid padding problems */                                                                 \
      type tmp;                                                                                    \
      Pop(data, type, tmp);                                                                        \
      TSF[i].TWS_field_scalar = tmp;                                                               \
    } else                                                                                         \
      ok = tfalse;                                                                                 \
    break

        Popcase(byte);
        Popcase(dat);
        Popcase(ldat);
        Popcase(tcolor);
        Popcase(topaque);
        Popcase(tany);
        Popcase(trune);
        Popcase(tcell);
        Popcase(tobj);
#undef Popcase
      default:
        if (TSF[i].type >= TWS_vec && (TSF[i].type & ~TWS_vec) <= TWS_last &&
            data + sizeof(uldat) <= end) {

          Pop(data, uldat, TSF[i].TWS_field_vecL);
          if (TSF[i].TWS_field_vecL) {
            if (data + TSF[i].TWS_field_vecL <= end) {
              if (flags & TWS_CLONE_MEM) {
                if ((TSF[i].TWS_field_vecV = Tw_AllocMem(TSF[i].TWS_field_vecL)))
                  PopV(data, TSF[i].TWS_field_vecL, TSF[i].TWS_field_vecV);
                else
                  ok = tfalse;
              } else
                PopAddr(data, byte, TSF[i].TWS_field_vecL, TSF[i].TWS_field_vecV);
            } else
              ok = tfalse;
          } else
            TSF[i].TWS_field_vecV = NULL;
        } else
          ok = tfalse;
        break;
      }
    }
    if (ok && data == end /* paranoia */) {
      SortTSL(TSL);
      return TSL;
    }
    FailedCall(TwD, TW_ESERVER_BAD_RETURN, order_StatObj);
    Tw_DeleteStat(TwD, TSL);
  }
  return (tslist)0;
}

static tslist StatA(tw_d TwD, tobj Id, udat flags, uldat hN, const udat *h, tslist f) {
  tslist a0 = (tslist)TW_NOID;
  DECL_MyReply uldat myId;
  if (!TwD) {
    return a0;
  }
  LOCK;
  if (Fd != TW_NOFD && (myId = id_Tw[order_StatObj]) != TW_NOID &&
      (myId != TW_BADID || (myId = FindFunctionId(TwD, order_StatObj)) != TW_NOID)) {

    if (InitRS(TwD)) {
      myId = (sizeof(tobj) + sizeof(udat) + hN * sizeof(udat));
      if (WQLeft(myId)) {
        Push(s, tobj, Id);
        Push(s, udat, hN);
        PushV(s, hN * sizeof(udat), h);

        Send(TwD, (myId = NextSerial(TwD)), id_Tw[order_StatObj]);
        if ((MyReply = (void *)Wait4Reply(TwD, myId)) && (INIT_MyReply MyCode == OK_MAGIC)) {
          if (flags & TWS_SCALAR)
            a0 = StatScalar(f, (byte *)MyData, (byte *)MyReply + MyLen + sizeof(uldat));
          else
            a0 = StatTSL(TwD, flags, (byte *)MyData, (byte *)MyReply + MyLen + sizeof(uldat));
        } else {
          FailedCall(TwD, MyReply && MyCode != (uldat)-1 ? TW_ECALL_BAD_ARG : TW_ECALL_BAD,
                     order_StatObj);
          a0 = (tslist)TW_NOID;
        }
        if (MyReply)
          KillReply(TwD, (byte *)MyReply, MyLen);
        UNLK;
        return a0;
      }
    }
    /* still here? must be out of memory! */
    Errno = TW_ESYS_NO_MEM;
    Fail(TwD);
  } else if (Fd != TW_NOFD)
    FailedCall(TwD, TW_ESERVER_NO_FUNCTION, order_StatObj);
  UNLK;
  return (tslist)TW_NOID;
}

/***********/

static void ParseReplies(tw_d TwD) {
  uldat left, len, rlen, serial;
  byte *t;
  byte *rt;

  t = GetQueue(TwD, QREAD, &len);
  left = len;

  /* parse all replies, move messages to QMSG queue, delete malformed replies */
  while (left >= sizeof(uldat)) {
    rt = t;
    Pop(t, uldat, rlen);
    left -= sizeof(uldat);
    if (left >= rlen) {
      if (rlen < 2 * sizeof(uldat) ||
          (Pop(t, uldat, serial), t -= sizeof(uldat), serial == MSG_MAGIC)) {

        /* either a MSG, or a malformed reply. In both cases, it will be removed */
        if (rlen >= 2 * sizeof(uldat) && serial == MSG_MAGIC) {
          /* it's a msg, copy it in its own queue */
          /* we no longer need `t', clobber it */
          t -= sizeof(uldat);
          Push(t, uldat, rlen + sizeof(uldat));
          ParanoidAddQueueQMSG(TwD, rlen + sizeof(uldat), rt);
        }

        /* delete the reply and recalculate t, left, len */
        KillReply(TwD, rt, rlen);
        (void)GetQueue(TwD, QREAD, &rlen);
        if (rlen) {
          /* t -= sizeof(uldat); */
          left += sizeof(uldat);
          t = GetQueue(TwD, QREAD, NULL) + (len - left);
          left -= len - rlen;
          len = rlen;
          continue;
        }
        return;
      }
      t += rlen;
      left -= rlen;
    } else {
      /* the last reply is still incomplete, do not touch it and bail out */
      /* left += sizeof(uldat); */
      break;
    }
  }
}

/* nothing to lock here... */
/**
 * creates an event message
 */
tmsg Tw_CreateMsg(tw_d TwD, uldat Type, uldat len) {
  tmsg msg;
  const uldat delta = (uldat)(size_t)&(((tmsg)NULL)->Event);

  if ((msg = (tmsg)Tw_AllocMem(len += delta))) {
    msg->Len = len;
    msg->Magic = tmsg_magic;
    msg->Type = Type;
  }
  return msg;
}

/**
 * frees an event message: should be called only on messages
 * created with Tw_CreateMsg(), and only if they are not sent with
 * Tw_SendMsg() or Tw_BlindSendMsg()
 */
void Tw_DeleteMsg(tw_d TwD, tmsg msg) {
  if (msg && msg->Magic == tmsg_magic)
    Tw_FreeMem(msg);
}

/**
 * sends message to given client, blocking to see if it could be delivered
 */
byte Tw_SendMsg(tw_d TwD, tmsgport MsgPort, tmsg msg) {
  byte ret = tfalse;
  if (msg && msg->Magic == tmsg_magic) {
    ret = Tw_SendToMsgPort(TwD, MsgPort, msg->Len, (void *)msg);
    Tw_FreeMem(msg);
  }
  return ret;
}

/**
 * sends message to given client, without blocking
 */
void Tw_BlindSendMsg(tw_d TwD, tmsgport MsgPort, tmsg msg) {
  if (msg && msg->Magic == tmsg_magic) {
    Tw_BlindSendToMsgPort(TwD, MsgPort, msg->Len, (void *)msg);
    Tw_FreeMem(msg);
  }
}

/**
 * returns ttrue if server implements all given functions, tfalse otherwise
 */
byte Tw_FindVFunction(tw_d TwD, va_list V) {
  void *F, *tryF;
  uldat i, *id;
  s_tw_errno *E;

  while ((F = va_arg(V, void *))) {

    for (i = 0; (tryF = Functions[i].Fn) && tryF != F; i++) {
    }
    if (tryF == F) {
      id = &id_Tw[i];
      if (*id != TW_NOID && (*id != TW_BADID || (*id = FindFunctionId(TwD, i)) != TW_NOID))

        continue;
      E = GetErrnoLocation(TwD);
      E->E = TW_ESERVER_NO_FUNCTION;
      E->S = i;
    } else {
      Errno = TW_ESERVER_BAD_FUNCTION;
    }
    return tfalse;
  }

  return ttrue;
}

byte Tw_FindLFunction(tw_d TwD, ...) {
  byte ret;
  va_list V;

  va_start(V, TwD);
  ret = Tw_FindVFunction(TwD, V);
  va_end(V);

  return ret;
}

#ifndef __GNUC__
byte TwFindLFunction(void *F, ...) {
  byte ret;
  va_list V;

  if (F) {
    va_start(V, F);
    ret = Tw_FindVFunction(Tw_DefaultD, F, V);
    va_end(V);

    return ret;
  }
  return ttrue;
}
#endif

void Tw_MergeHyphensArgv(int argc, char **argv) {
  char *S;
  while (argc) {
    if ((S = *argv) && S[0] == '-' && S[1] == '-' && S[2] && S[3])
      (*argv)++;
    argv++, argc--;
  }
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

TW_INLINE byte *FillQueue(tw_d TwD, byte i, uldat *len) {
  uldat delta = Qmax[i] - Qlen[i] - Qstart[i];
  Qlen[i] += delta;
  if (len)
    *len = delta;
  return Queue[i] + Qstart[i] + Qlen[i] - delta;
}

/* compress data before sending */
static uldat Gzip(tw_d TwD) {
  uldat oldQWRITE = Qlen[QWRITE], delta, tmp;
  z_streamp z = zW;
  int zret = Z_OK;

  /* compress the queue */
  if (Qlen[QWRITE]) {
    z->next_in = GetQueue(TwD, QWRITE, &tmp);
    z->avail_in = tmp;
    z->next_out = FillQueue(TwD, QgzWRITE, &tmp);
    z->avail_out = tmp;

    while (z->avail_in && zret == Z_OK) {

      if (z->avail_out < (delta = z->avail_in + 12)) {
        if (Grow(TwD, QgzWRITE, delta - z->avail_out)) {
          Qlen[QgzWRITE] -= delta;
          z->next_out = FillQueue(TwD, QgzWRITE, &tmp);
          z->avail_out = tmp;
        } else {
          /* out of memory ! */
          Errno = TW_ESYS_NO_MEM;
          Panic(TwD);
          break;
        }
      }

      zret = deflate(z, Z_SYNC_FLUSH);

      /* update the compressed queue */
      Qlen[QgzWRITE] -= z->avail_out;
    }
  }

  if (Fd != TW_NOFD) {
    /* update the uncompressed queue */
    DeQueue(TwD, QWRITE, Qlen[QWRITE] - z->avail_in);

    if (zret == Z_OK)
      return oldQWRITE;
    else {
      Errno = TW_EGZIP_INTERNAL;
      Panic(TwD);
    }
  }
  return tfalse;
}

static uldat Gunzip(tw_d TwD) {
  uldat oldQRead = Qlen[QREAD], delta, tmp;
  int zret = Z_OK;
  z_streamp z = zR;

  /* uncompress the queue */
  if (Qlen[QgzREAD]) {
    z->next_in = GetQueue(TwD, QgzREAD, &tmp);
    z->avail_in = tmp;
    z->next_out = FillQueue(TwD, QREAD, &tmp);
    z->avail_out = tmp;

    while (z->avail_in && zret == Z_OK) {

      /* approx. guess of uncompression ratio: 1 to 5 */
      /* in case we guess wrong, inflate() will tell us to make more space */
      if (z->avail_out < (delta = 5 * z->avail_in + 12)) {
        if (Grow(TwD, QREAD, delta - z->avail_out)) {
          Qlen[QREAD] -= delta;
          z->next_out = FillQueue(TwD, QREAD, &tmp);
          z->avail_out = tmp;
        } else {
          /* out of memory ! */
          Errno = TW_ESYS_NO_MEM;
          Panic(TwD);
          break;
        }
      }

      zret = inflate(z, Z_SYNC_FLUSH);

      /* update the uncompressed queue */
      Qlen[QREAD] -= z->avail_out;
    }
  }

  if (Fd != TW_NOFD) {
    /* update the compressed queue */
    DeQueue(TwD, QgzREAD, Qlen[QgzREAD] - z->avail_in);

    if (zret == Z_OK)
      return Qlen[QREAD] - oldQRead;
    else {
      Errno = TW_EGZIP_BAD_PROTOCOL;
      Panic(TwD);
    }
  }
  return tfalse;
}

/**
 * tries to enable compression on the connection; returns ttrue if succeeded
 */
byte Tw_EnableGzip(tw_d TwD) {
  if (!GzipFlag && Tw_CanCompress(TwD)) {
    if ((zW = Tw_AllocMem(sizeof(*zW))) && (zR = Tw_AllocMem(sizeof(*zR)))) {

      if (Tw_AllocMem == malloc) {
        zW->zalloc = zR->zalloc = Z_NULL;
        zW->zfree = zR->zfree = Z_NULL;
      } else {
        zW->zalloc = zR->zalloc = Tw_ZAlloc;
        zW->zfree = zR->zfree = Tw_ZFree;
      }
      zW->opaque = zR->opaque = NULL;

      if (deflateInit(zW, Z_BEST_COMPRESSION) == Z_OK) {
        if (inflateInit(zR) == Z_OK) {
          if (Tw_DoCompress(TwD, ttrue))
            return GzipFlag = ttrue;
          inflateEnd(zR);
        }
        deflateEnd(zW);
      }
    }
    if (zR)
      Tw_FreeMem(zR);
    if (zW)
      Tw_FreeMem(zW);
  }
  return tfalse;
}

/**
 * tries to disable compression on the connection; returns ttrue if succeeded
 */
byte Tw_DisableGzip(tw_d TwD) {
  if (GzipFlag && (Fd == TW_NOFD || Tw_DoCompress(TwD, tfalse) || Fd == TW_NOFD)) {
    inflateEnd(zR);
    deflateEnd(zW);
    Tw_FreeMem(zR);
    Tw_FreeMem(zW);
    GzipFlag = tfalse;
    return ttrue;
  }
  return tfalse;
}

#else /* !CONF_SOCKET_GZ */

byte Tw_EnableGzip(tw_d TwD) {
  return tfalse;
}

byte Tw_DisableGzip(tw_d TwD) {
  return tfalse;
}

#endif /* CONF_SOCKET_GZ */

TW_INLINE void TWS_2_proto(udat tws_type, byte proto[2]) {
  if (tws_type & TWS_vec) {
    proto[0] = 'V';
  } else if (tws_type == TWS_void) {
    proto[0] = 'v';
  } else
    proto[0] = '_';

  if ((tws_type &= TWS_last) == TWS_void) {
    /* turn TWS_void ('\0') into '\xFE' */
    proto[1] = TWS_void_CHR;
  } else if (tws_type < TWS_highest) {
    proto[1] = tws_type;
  } else {
    /* safe assumption */
    proto[0] = '_';
    proto[1] = TWS_tany;
  }
}
