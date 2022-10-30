/*
 *  socket.c  --  remote libtw server implementation: supports unix and inet sockets
 *
 *  Copyright (C) 1999-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

/*
 * Life is tricky... under SunOS hstrerror() is in an obscure library, so it gets disabled,
 * yet <netdb.h> has its prototype, so the #define hstrerror() in "missing.h" breaks it.
 * Solution: include "twin.h" (pulls in "missing.h") late, but still include
 * "twconfig.h" and "osincludes.h" early to pull in TW_HAVE_* and system headers
 * necessary to include <sys/socket.h> under FreeBSD.
 */
#include <Tw/autoconf.h>
#include "osincludes.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <netdb.h>

#ifdef TW_HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef TW_HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#ifdef TW_HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef TW_HAVE_SYS_UTSNAME_H
#include <sys/utsname.h>
#endif
#ifdef TW_HAVE_ZLIB_H
#include <zlib.h>
#endif

#include "twin.h"
#include "algo.h"
#include "alloc.h"
#include "data.h"
#include "draw.h"
#include "main.h"
#include "methods.h"
#include "obj/id.h" // Obj2Id
#include "resize.h"

#include "common.h"
#include "extreg.h"
#include "fdlist.h"
#include "hw_multi.h"
#include "log.h"
#include "md5.h"
#include "printk.h" // printk_receive_fd(), RegisterPrintkFd()
#include "remote.h"
#include "socket.h"
#include "unaligned.h"
#include "util.h"
#include "version.h"

#include "stl/span.h"

#include <Tw/Tw.h>
#include <Tw/Twkeys.h>
#include <Tw/Twstat.h>
#include <Tw/Twstat_defs.h>
#include <Tutf/Tutf.h>

#define CONF_SOCKET_ALIEN

#ifdef CONF_SOCKET_ALIEN

static const byte *alienPop(const byte *src, uldat alien_len, void *dst, uldat len);
static byte *alienPush(const void *src, uldat len, byte *dst, uldat alien_len);
#define alienPOP(src, type, lval)                                                                  \
  (src = alienPop(src, AlienSizeof(type, Slot), &(lval), sizeof(type)))
#define alienPUSH(dst, type, lval)                                                                 \
  (dst = alienPush(&(lval), sizeof(type), (dst), AlienSizeof(type, Slot)))

static void alienSendMsg(Tmsgport MsgPort, Tmsg msg);
static void AlienIO(int fd, uldat slot);

#endif

/* First: private variables from remote.c */
extern fdlist *FdList;
extern uldat FdSize, FdTop, FdBottom, FdWQueued;
#define LS FdList[Slot]
#define ls FdList[slot]
#define ls_p FdList[ls.pairSlot]

/* some extra functions not included in remote.c */

#if 0  /* currently unused */
static uldat RemoteReadAddQueue(uldat Slot, uldat len, byte *data) {
    uldat nmax;

    if (len == 0 || Slot >= FdTop || LS.Fd == NOFD)
        return 0;

    /* append to queue */
    if (LS.RQstart + LS.RQlen + len > LS.RQmax) {
        if (LS.RQstart)
            MoveMem(LS.RQueue + LS.RQstart, LS.RQueue, LS.RQlen),
            LS.RQstart = 0;

        if (LS.RQlen + len > LS.RQmax) {
            LS.RQueue = (byte *)ReAllocMem(LS.RQueue, nmax = (LS.RQmax+len+40)*5/4);
            if (!LS.RQueue)
                return (LS.RQmax = LS.RQlen = 0);
            LS.RQmax = nmax;
        }
    }

    CopyMem(data, LS.RQueue + LS.RQstart + LS.RQlen, len);
    LS.RQlen += len;
    return len;
}
#endif /* 0 */

static byte *RemoteReadGrowQueue(uldat Slot, uldat len) {
  uldat nmax;

  if (len == 0 || Slot >= FdTop || LS.Fd == NOFD)
    return NULL;

  if (LS.RQstart + LS.RQlen + len > LS.RQmax) {
    /* grow the queue */
    if (LS.RQstart)
      MoveMem(LS.RQueue + LS.RQstart, LS.RQueue, LS.RQlen), LS.RQstart = 0;

    if (LS.RQlen + len > LS.RQmax) {
      LS.RQueue = (byte *)ReAllocMem(LS.RQueue, nmax = (LS.RQmax + len + 40) * 5 / 4);
      if (!LS.RQueue) {
        LS.RQmax = LS.RQlen = 0;
        return NULL;
      }
      LS.RQmax = nmax;
    }
  }
  nmax = LS.RQstart + LS.RQlen;
  LS.RQlen += len;

  return LS.RQueue + nmax;
}

static uldat RemoteReadShrinkQueue(uldat Slot, uldat len) {
  if (len && Slot < FdTop && LS.Fd != NOFD) {
    if (len < LS.RQlen) {
      LS.RQlen -= len;
    } else {
      len = LS.RQlen;
      LS.RQstart = LS.RQlen = 0;
    }
    return len;
  }
  return 0;
}

static uldat RemoteReadDeQueue(uldat Slot, uldat len) {
  if (len && Slot < FdTop && LS.Fd != NOFD) {
    if (len < LS.RQlen) {
      LS.RQstart += len;
      LS.RQlen -= len;
    } else {
      len = LS.RQlen;
      LS.RQstart = LS.RQlen = 0;
    }
    return len;
  }
  return 0;
}

static byte *RemoteReadGetQueue(uldat Slot, uldat *len) {
  if (Slot >= FdTop || LS.Fd == NOFD || !LS.RQlen) {
    if (len) {
      *len = 0;
    }
    return NULL;
  }
  if (len)
    *len = LS.RQlen;
  return LS.RQueue + LS.RQstart;
}

static byte *RemoteWriteGetQueue(uldat Slot, uldat *len) {
  if (Slot >= FdTop || LS.Fd == NOFD || !LS.WQlen) {
    if (len)
      *len = 0;
    return NULL;
  }
  if (len)
    *len = LS.WQlen;
  return LS.WQueue;
}

#ifdef CONF_SOCKET_GZ

static byte *RemoteReadFillQueue(uldat Slot, uldat *len) {
  uldat delta;
  if (Slot >= FdTop || LS.Fd == NOFD) {
    if (len) {
      *len = 0;
    }
    return NULL;
  }
  delta = LS.RQmax - LS.RQstart - LS.RQlen; /* still available */
  if (len) {
    *len = delta;
  }
  LS.RQlen += delta;                   /* alloc them */
  return LS.RQueue + LS.RQmax - delta; /* return the address of first one */
}

static byte *RemoteWriteFillQueue(uldat Slot, uldat *len) {
  uldat delta;
  if (Slot >= FdTop || LS.Fd == NOFD) {
    if (len) {
      *len = 0;
    }
    return NULL;
  }
  delta = LS.WQmax - LS.WQlen; /* still available */
  if (len) {
    *len = delta;
  }
  if (!LS.WQlen && LS.WQmax) {
    LS.WQlen = LS.WQmax; /* alloc them */
    FdWQueued++;
  }
  return LS.WQueue + LS.WQmax - delta; /* return the address of first one */
}

/* compress an uncompressed slot */
static byte RemoteGzip(uldat Slot) {
  uldat slot = LS.pairSlot, delta;
  z_streamp z = (z_streamp)ls.PrivateData;
  int zret = Z_OK;

  /* compress the queue */
  if (LS.WQlen) {
    z->next_in = LS.WQueue;
    z->avail_in = LS.WQlen;

    /* in case sizeof(z->avail_out) != sizeof(uldat) */
    z->next_out = RemoteWriteFillQueue(slot, &delta);
    z->avail_out = delta;

    while (z->avail_in && zret == Z_OK) {

      if (z->avail_out < (delta = z->avail_in + 12)) {
        if (RemoteWriteQueue(slot, delta - z->avail_out, NULL)) {
          if (!(ls.WQlen -= delta))
            FdWQueued--;
          z->next_out = RemoteWriteFillQueue(slot, &delta);
          z->avail_out = delta;
        } else {
          /* out of memory ! */
          break;
        }
      }

      zret = deflate(z, Z_SYNC_FLUSH);

      /* update the compressed queue */
      if (!(ls.WQlen -= z->avail_out))
        FdWQueued--;
    }
  }
  /* update the uncompressed queue */
  if (z->avail_in) {
    CopyMem(LS.WQueue + LS.WQlen - z->avail_in, LS.WQueue, z->avail_in);
  }
  slot = LS.WQlen;
  if (!(LS.WQlen = z->avail_in) && slot) {
    FdWQueued--;
  }
  return zret == Z_OK;
}

static byte RemoteGzipFlush(uldat Slot) {
  return RemoteGzip(Slot) && RemoteFlush(LS.pairSlot);
}

static byte RemoteGunzip(uldat Slot) {
  uldat slot = LS.pairSlot, delta;
  z_streamp z = (z_streamp)ls.PrivateData;
  int zret = Z_OK;

  /* uncompress the queue */
  if (LS.RQlen) {
    /* in case sizeof(z->avail_out) != sizeof(uldat) */
    z->next_in = RemoteReadGetQueue(Slot, &delta);
    z->avail_in = delta;
    z->next_out = RemoteReadFillQueue(slot, &delta);
    z->avail_out = delta;

    while (z->avail_in && zret == Z_OK) {

      /* approx. guess of uncompression ratio: 1 to 5 */
      if (z->avail_out < (delta = 5 * z->avail_in + 12)) {
        if (RemoteReadGrowQueue(slot, delta - z->avail_out)) {
          ls.RQlen -= delta;
          z->next_out = RemoteReadFillQueue(slot, &delta);
          z->avail_out = delta;
        } else { /* out of memory ! */
          break;
        }
      }

      zret = inflate(z, Z_SYNC_FLUSH);

      /* update the uncompressed queue */
      ls.RQlen -= z->avail_out;
    }
  }
  /* update the compressed queue */
  if (z->avail_in) {
    LS.RQstart += LS.RQlen - z->avail_in;
  }
  LS.RQlen = z->avail_in;
  return zret == Z_OK;
}

static void ShutdownGzip(uldat Slot);

#endif /* CONF_SOCKET_GZ */

static void SocketH(Tmsgport MsgPort);

static void sockShutDown(Tmsgport MsgPort) {
  if (MsgPort->RemoteData.FdSlot < FdTop) {
    UnRegisterMsgPort(MsgPort);
  }
}

/* prototypes of libtw back-end utility functions */

static uldat sockFindFunction(byte Len, const char *name, byte ProtoLen, const char *Proto);
static byte sockSyncSocket(void);
static byte sockServerSizeof(byte Type);
static byte sockCanCompress(void);
static byte sockDoCompress(byte on_off);

static void sockNeedResizeDisplay(void);
static void sockAttachHW(uldat len, const char *arg, byte flags);
static byte sockDetachHW(uldat len, const char *arg);
static void sockSetFontTranslation(const byte trans[0x80]);
static void sockSetTRuneTranslation(const trune trans[0x80]);

static void sockDeleteObj(void *V);

static Twidget sockCreateWidget(dat XWidth, dat YWidth, uldat Attr, uldat Flags, dat Left, dat Up,
                                tcell Fill);
static void sockRecursiveDeleteWidget(Twidget w);
static void sockSetXYWidget(Twidget w, dat x, dat y);
static void sockResizeWidget(Twidget w, dat XWidth, dat YWidth);
#define sockScrollWidget ScrollWidget
static void sockDrawWidget(Twidget w, dat XWidth, dat YWidth, dat Left, dat Up,
                           const char *utf8_bytes, const trune *runes, const tcell *cells);

#define sockSetVisibleWidget SetVisibleWidget
static void sockFocusSubWidget(Twidget w);

#define sockRestackChildrenWidget RestackWidgets
static void sockCirculateChildrenWidget(Twidget w, byte up_or_down);
#define TW_CIRCULATE_RAISE_LAST 0
#define TW_CIRCULATE_LOWER_FIRST 1

static Tgadget sockCreateGadget(Twidget Parent, dat XWidth, dat YWidth, const char *TextNormal,
                                uldat Attr, uldat Flags, udat Code, tcolor ColText,
                                tcolor ColTextSelect, tcolor ColTextDisabled,
                                tcolor ColTextSelectDisabled, dat Left, dat Up);

static Twindow sockCreateWindow(dat TitleLen, const char *Title, const tcolor *ColTitle, Tmenu Menu,
                                tcolor ColText, uldat CursorType, uldat Attr, uldat Flags,
                                dat XWidth, dat YWidth, dat ScrollBackLines);
static void sockWriteCharsetWindow(Twindow Window, uldat Len, const char *charset_bytes);
static void sockWriteUtf8Window(Twindow Window, uldat Len, const char *utf8_bytes);
static void sockWriteTRuneWindow(Twindow Window, uldat Len, const trune *runes);
static void sockWriteTCellWindow(Twindow Window, dat x, dat y, uldat Len, const tcell *cells);
static void sockSetTitleWindow(Twindow Window, dat titlelen, const char *title);

static Trow sockFindRowByCodeWindow(Twindow Window, dat Code);

static Tmenuitem sockCreate4MenuAny(Tobj Parent, Twindow Window, udat Code, byte Flags, ldat Len,
                                    const char *Name);

#define sockRestackChildrenRow RestackRows
static void sockCirculateChildrenRow(Tobj O, byte up_or_down);
#define TW_CIRCULATE_RAISE_LAST 0
#define TW_CIRCULATE_LOWER_FIRST 1

static Tmenu sockCreateMenu(tcolor ColItem, tcolor ColSelect, tcolor ColDisabled,
                            tcolor ColSelectDisabled, tcolor ColShtCut, tcolor ColSelShtCut,
                            byte FlagDefColInfo);

static Tmsgport sockCreateMsgPort(byte NameLen, const char *Name);
static Tmsgport sockFindMsgPort(Tmsgport Prev, byte NameLen, const char *Name);

static Tgroup sockCreateGroup(void);

static Tobj sockPrevObj(Tobj o);
static Tobj sockNextObj(Tobj o);
static Tobj sockParentObj(Tobj o);

static Tscreen sockFirstScreen(void);
static Twidget sockFirstWidget(Twidget w);
static Tmsgport sockFirstMsgPort(void);
static Tmenu sockFirstMenu(Tmsgport MsgPort);
static Twidget sockFirstW(Tmsgport MsgPort);
static Tgroup sockFirstGroup(Tmsgport MsgPort);
static Tmutex sockFirstMutex(Tmsgport MsgPort);
static Tmenuitem sockFirstMenuItem(Tmenu Menu);
static Tgadget sockFirstGadget(Tgroup group);

static byte sockSendToMsgPort(Tmsgport MsgPort, udat Len, const byte *Data);
static void sockBlindSendToMsgPort(Tmsgport MsgPort, udat Len, const byte *Data);

static Tobj sockGetOwnerSelection(void);
static void sockSetOwnerSelection(tany Time, tany Frac);
static void sockNotifySelection(Tobj Requestor, uldat ReqPrivate, uldat Magic,
                                const char MIME[MAX_MIMELEN], uldat Len, const char *Data);
static void sockRequestSelection(Tobj Owner, uldat ReqPrivate);

#define sockSetServerUid SetServerUid
#define sockGetDisplayWidth GetDisplayWidth
#define sockGetDisplayHeight GetDisplayHeight

static Tall sockGetAll(void);

/* Second: socket handling functions */

static uldat MaxFunct, Slot, RequestN;
static const byte *s, *end;
static int inetFd = NOFD, Fd;
static uldat inetSlot = NOSLOT;

static byte sockReply(uldat code, uldat len, const void *data);

static void SocketIO(int fd, uldat slot);

#define Left(size) (s + (size) <= end)

#include "socket_id.h"

static void sockStat(Tobj x, udat n, const byte *in);

typedef struct {
  byte Len, FormatLen;
  const char *Name, *Format;
} sockfn;

static sockfn sockF[] = {
#include "socket2_m4.h"
    {0, 0, "StatObj", "0S0x" obj_magic_STR "_" TWS_udat_STR "V" TWS_udat_STR}, {0, 0, NULL, NULL}};

/* convert a 2-byte string "v"TWS_void_STR or "_"* or "V"* into a tsfield->type */
inline udat proto_2_TWS(const char proto[2]) {
  udat tws_type = 0;
  switch (proto[0]) {
  case 'V':
    tws_type |= TWS_vec;
    /* FALLTHROUGH */
  case '_':
    if (proto[1] < TWS_highest) {
      tws_type |= proto[1];
      break;
    }
    /* FALLTHROUGH */
  case 'v':
    /* turn '\xFE' into TWS_void ('\0') */
    if ((byte)proto[1] == (byte)TWS_void_CHR)
      break;
    /* FALLTHROUGH */
  default:
    /* safe assumption */
    tws_type = TWS_tany;
    break;
  }
  return tws_type;
}

inline void TWS_2_proto(udat tws_type, char proto[2]) {
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

#define _obj .TWS_field_obj
#define _any .TWS_field_scalar
#define _vec .TWS_field_vecV
#define _cvec .TWS_field_vecCV
#define _len .TWS_field_vecL

#define _type .type

#define void_ TWS_void
#define obj_ TWS_obj
#define vec_ TWS_vec
#define vecW_ TWS_vecW

/* here a[0] is the first arg, N is the number of args */
static void sockMultiplex_S(uldat id, Span<s_tsfield> a) {
  switch (id) {
#include "socket1_m4.h"
  default:
    break;
  }
}

/* here a[1] is the first arg, a[0] is the return value, N is the number of args + 1 */
#define fullMultiplexS(Id, a)                                                                      \
  do {                                                                                             \
    if ((Id) == order_StatObj) {                                                                   \
      if ((a).size() > 3) {                                                                        \
        sockStat((Tobj)(a)[1] _obj, (udat)(a)[2] _any, (const byte *)(a)[3] _vec);                 \
      }                                                                                            \
    } else {                                                                                       \
      sockMultiplex_S(Id, a);                                                                      \
    }                                                                                              \
  } while (0)

/* here a[0] is the first arg, a.size is the number of args */
static void sockMultiplexS(uldat id, Span<s_tsfield> a) {
  sockMultiplex_S(id, a);
  if (id < MaxFunct)
    a[0] _type = proto_2_TWS(sockF[id].Format);
}

/* code to return array lengths V(expr) and W(expr) */
static uldat sockLengths(uldat id, View<s_tsfield> a) {
  uldat L = 0;
  uldat n = a.size();

  switch (id) {

#include "socket3_m4.h"

  case order_StatObj:
    switch (n) {
    case 3:
      L = a[2] _any;
      break;
    }
  default:
    break;
  }
  return L;
}

TW_DECL_MAGIC(TwinMagicData);

/***********/

#include "socketstat.h"

/***********/

/*
 * translate an array of IDs to an array of Tobj of type c;
 * *alloced is ttrue if needed to allocate a buffer, tfalse otherwise.
 * if success, return array of Tobj, else return NULL.
 */
static Tobj *AllocId2ObjVec(byte *alloced, byte c, uldat n, byte *VV) {
  const byte *S;
  Tobj *aX;
  Tobj *X;
  uldat i;

  if ((aX = X = (Tobj *)AllocMem(n * sizeof(Tobj)))) {
    S = (const byte *)VV;
    while (n--) {
      Pop(S, uldat, i);
      *X++ = Id2Obj(e_magic_byte(c), i);
    }
    *alloced = ttrue;
  } else
    *alloced = tfalse;
  return aX;
}

inline ldat sockDecodeArg(uldat id, const char *Format, uldat n, tsfield a, uldat mask[1],
                          byte flag[1], ldat fail) {
  const void *av;
  topaque nlen;
  byte c;

  switch ((c = (byte)*Format++)) {
  case '_':
    switch ((c = (byte)*Format)) {
#define CASE_(type)                                                                                \
  case CAT(TWS_, type):                                                                            \
    /* ensure type size WAS negotiated */                                                          \
    if (AlienSizeof(type, Slot) && Left(sizeof(type))) {                                           \
      type an;                                                                                     \
      Pop(s, type, an);                                                                            \
      a[n] _any = (tany)an;                                                                        \
      a[n] _type = c;                                                                              \
      break;                                                                                       \
    }                                                                                              \
    fail = -fail;                                                                                  \
    break

    case TWS_tcolor:
      /*FALLTHROUGH*/
      CASE_(byte);
      CASE_(dat);
      CASE_(ldat);
      CASE_(topaque);
      CASE_(tany);
      CASE_(trune);
      CASE_(tcell);
#undef CASE_
    default:
      fail = -fail;
      break;
    }
    break;
  case 'x':
    /* all kind of pointers */
    if (Left(sizeof(uldat))) {
      uldat a0;
      Pop(s, uldat, a0);
      c = (byte)*Format - base_magic_CHR;
      a[n] _obj = Id2Obj(e_magic_byte(c), a0);
      a[n] _type = obj_;
      break;
    }
    fail = -fail;
    break;
  case 'V':
    nlen = sockLengths(id, View<s_tsfield>(a, n));
    c = (byte)*Format;
    /* ensure type size WAS negotiated */
    if ((c <= TWS_tcolor || AlienMagic(Slot)[c])) {
      nlen *= AlienMagic(Slot)[c];
      if (Left(nlen)) {
        PopAddr(s, const byte, nlen, av);
        a[n] _len = nlen;
        a[n] _cvec = av;
        a[n] _type = vec_ | c;
        break;
      }
    }
    fail = -fail;
    break;
  case 'W':
    /* ensure 'topaque' size WAS negotiated */
    if (AlienSizeof(topaque, Slot) && Left(sizeof(topaque))) {
      Pop(s, topaque, nlen);

      c = (byte)*Format;
      /* ensure type size WAS negotiated */
      if ((c <= TWS_tcolor || AlienMagic(Slot)[c])) {
        if (!nlen ||
            (Left(nlen) && nlen == sockLengths(id, View<s_tsfield>(a, n)) * AlienMagic(Slot)[c])) {
          PopAddr(s, const byte, nlen, av);
          a[n] _len = nlen;
          a[n] _cvec = av;
          a[n] _type = vec_ | vecW_ | c;
          break;
        }
      }
    }
    fail = -fail;
    break;
  case 'X':
    nlen = sockLengths(id, View<s_tsfield>(a, n)) * sizeof(uldat);
    if (Left(nlen)) {
      c = (byte)*Format - base_magic_CHR;
      PopAddr(s, const byte, nlen, av);
      if ((a[n] _vec = AllocId2ObjVec(flag, c, nlen / sizeof(uldat), (byte *)RemoveConst(av)))) {
        a[n] _len = nlen;
        a[n] _type = vec_ | obj_;
        *mask |= *flag << n;
        break;
      }
    }
    fail = -fail;
    break;
  case 'Y':
    /* ensure 'topaque' size WAS negotiated */
    if (AlienSizeof(topaque, Slot) && Left(sizeof(topaque))) {
      Pop(s, topaque, nlen);

      nlen *= sizeof(uldat);
      if (Left(nlen)) {
        c = (byte)*Format - base_magic_CHR;
        PopAddr(s, const byte, nlen, av);
        if ((a[n] _vec = AllocId2ObjVec(flag, c, nlen / sizeof(uldat), (byte *)RemoveConst(av)))) {
          a[n] _len = nlen;
          a[n] _type = vec_ | obj_;
          *mask |= *flag << n;
          break;
        }
      }
    }
    fail = -fail;
    break;
  default:
    fail = -fail;
    break;
  }
  return fail;
}

static void sockMultiplexB(uldat id) {
  static struct s_tsfield a[TW_MAX_ARGS_N];
  static byte warned = tfalse;
  uldat mask = 0; /* at least 32 bits. we need TW_MAX_ARGS_N... */
  uldat nlen, n = 1;
  ldat fail = 1;
  const char *Format = sockF[id].Format;
  uldat a0;
  byte c, self, flag, retT[2];

  self = (byte)*Format++;
  retT[0] = (byte)*Format++;
  retT[1] = (byte)*Format++;

  while (fail > 0 && *Format) {
    if (n < TW_MAX_ARGS_N) {
      fail = sockDecodeArg(id, Format, n, a, &mask, &flag, fail);

    } else /* (n >= TW_MAX_ARGS_N) */ {
      if (!warned) {
        warned = ttrue;
        log(ERROR) << "twin: sockMultiplexB(): got a call with " << n << " args, only "
                   << TW_MAX_ARGS_N << " supported!\n";
      }
      fail = -fail;
    }

    if (fail > 0) {
      Format += 2;
      fail++;
      n++;
    } else
      break;
  }

  if ((flag = (fail > 0 && s == end && !*Format && (self != '2' || a[1] _obj)))) {

    if (retT[0] == 'O' && a[n - 1] _type == (TWS_vec | TWS_byte) &&
        a[n - 1] _len == 2 * sizeof(byte)) {
      /*
       * variable return type. store it in last arg,
       * and let function implementation overwrite it
       *
       * evil trick: only a[n-1]_vec will be passed to the function,
       * but it points to a[n-1] itself!
       */
      a[n - 1] _type = proto_2_TWS((const char *)(a[n - 1] _vec));
      if (mask & 1 << (n - 1))
        FreeMem(a[n - 1].TWS_field_vecV);

      a[n - 1] _vec = &a[n - 1];
      a[n - 1] _len = 0;
    }

    fullMultiplexS(id, Span<s_tsfield>(a, n));
  }

  for (nlen = 0; mask; mask >>= 1, nlen++) {
    if (mask & 1)
      FreeMem(a[nlen].TWS_field_vecV);
  }

  if (flag) {
    if (retT[0] == 'O') {
      /* variable return type. get it from last arg */
      /* FIXME: currently, only '_' (scalar) and 'v' (void) return types are supported */

      TWS_2_proto(a[n - 1] _type, (char *)retT);
    }

    switch (retT[0]) {
    case '_':
      switch (retT[1]) {
#define CASE_(type)                                                                                \
  case CAT(TWS_, type):                                                                            \
    /* ensure type size WAS negotiated */                                                          \
    if (CAT(TWS_, type) <= TWS_tcolor || AlienSizeof(type, Slot)) {                                \
      /* move to first bytes on MSB machines */                                                    \
      *(type *)&a[0] _any = (type)a[0] _any;                                                       \
      c = sizeof(type);                                                                            \
      break;                                                                                       \
    }                                                                                              \
    fail = 0;                                                                                      \
    break

      case TWS_tcolor:
        /*FALLTHROUGH*/
        CASE_(byte);
        CASE_(dat);
        CASE_(ldat);
        CASE_(topaque);
        CASE_(tany);
        CASE_(trune);
        CASE_(tcell);
#undef CASE_
      default:
        c = 0;
        break;
      }
      if (c && fail > 0) {
        sockReply(OK_MAGIC, c, &a[0] _any);
        return;
      }
      break;

    case 'x':
      a0 = a[0] _obj ? a[0] _obj->Id : NOID;
      sockReply(OK_MAGIC, sizeof(tobj), &a0);
      return;

    case 'S':
    case 'v':
      return;

    default:
      break;
    }
  }
  if (retT[0] != 'v') {
    if (fail > 0) {
      if (self != '2' || a[1] _obj)
        fail = FAIL_MAGIC;
      else
        fail = 1;
    }
    sockReply(fail, 0, NULL);
  }
}

#undef _obj
#undef _any
#undef _vec
#undef _len

#undef _type

#undef void_
#undef obj_
#undef vec_
#undef vecW_

/***********/

#ifdef CONF_SOCKET_ALIEN
#include "socketalien.h"
#endif

/***********/

/* actual libtw back-end utility functions */

static int CmpFormat(const char *F1, const char *F2, uldat Len) {
  for (; Len; F1++, F2++, Len--) {
    if (Len > 1 && *F1 == *F2 && (*F1 == 'x' || *F1 == 'X' || *F1 == 'Y')) {
      /*
       * pointer. must allow any, since libtw
       * generically asks for 'tobj', not specific ones
       */
      F1++, F2++, Len--;
      continue;
    }
    if (*F1 != *F2)
      break;
  }
  return Len ? *F1 - *F2 : 0;
}

static uldat sockFindFunction(byte Len, const char *Name, byte FormatLen, const char *Format) {
  sockfn *F = sockF;
  if (Name) {
    while (F->Name && (F->Len != Len || F->FormatLen - 1 != FormatLen ||
                       memcmp(F->Name, Name, Len) || CmpFormat(F->Format + 1, Format, FormatLen)))
      F++;
    if (F->Name)
      return (uldat)(F - sockF);
  }
  return NOID;
}

static byte sockSyncSocket(void) {
  return ttrue;
}

static byte sockServerSizeof(byte Type) {
  return TwinMagicData[Type < TWS_highest ? Type : TWS_void];
}

static void sockNeedResizeDisplay(void) {
  if (LS.MsgPort && LS.MsgPort->AttachHW)
    ResizeDisplayPrefer(LS.MsgPort->AttachHW);
}

/*
 * this does direct write() on the connecting socket,
 * so it bypasses any compression.
 */
static void sockAttachHW(uldat len, const char *arg, byte flags) {
  Tdisplay D_HW;
  fd_set set;
  struct timeval tv = {2, 0};
  int realFd;
  char buf[2] = "\0";
  bool verbose = (flags & TW_ATTACH_HW_REDIRECT) != 0;

  realFd = LS.Fd >= 0 ? LS.Fd : FdList[LS.pairSlot].Fd;

  if (!LS.MsgPort) {
    if (verbose)
      write(realFd, "twin: AttachHW(): client did not create a MsgPort\n\0", 52);
    else
      write(realFd, buf, 2);
    return;
  }

  if (verbose)
    verbose = RegisterPrintkFd(realFd);

  if ((D_HW = AttachDisplayHW(Chars(arg, len), Slot, flags & TW_ATTACH_HW_EXCLUSIVE))) {
    if (D_HW->NeedHW & NEEDPersistentSlot)
      LS.MsgPort->AttachHW = D_HW;
    else
      D_HW->AttachSlot = NOSLOT; /* we don't need it => forget it */
  }

  if (D_HW) {
    buf[1]++;
    if (D_HW->NeedHW & NEEDPersistentSlot)
      buf[1]++;
  }
  write(realFd, buf, 2);

  /* wait for twattach to confirm attach... */

  FD_ZERO(&set);
  FD_SET(realFd, &set);

  while (select(realFd + 1, &set, NULL, NULL, &tv) == -1 && errno == EINTR) {
  }
  read(realFd, buf, 1);

  if (verbose)
    UnRegisterPrintkFd();
}

static byte sockDetachHW(uldat len, const char *arg) {
  return DetachDisplayHW(Chars(arg, len), 0); /* cannot detach exclusive displays !! */
}

static void sockSetFontTranslation(const byte trans[0x80]) {
  if (trans) {
    int i;
    trune *G = All->Gtranslations[USER_MAP];

    for (i = 0; i < 0x80; i++)
      G[i] = i;
    if (sizeof(trune) == sizeof(byte))
      CopyMem(trans, G + 0x80, 0x80);
    else
      for (i = 0x0; i < 0x80; i++)
        G[0x80 | i] = trans[i];
  }
}

static void sockSetTRuneTranslation(const trune trans[0x80]) {
  if (trans) {
    int i;
    trune *G = All->Gtranslations[USER_MAP];

    for (i = 0; i < 0x80; i++)
      G[i] = i;
    CopyMem(trans, G + 0x80, sizeof(trune) * 0x80);
  }
}

static Tmsgport sockGetMsgPortObj(Tobj p) {
  TobjEntry e = (TobjEntry)p;
  while (e) {
    if (IS_MSGPORT(e)) {
      return (Tmsgport)e;
    }
    switch (e->Id >> magic_shift) {
    case Trow_magic_byte:
    case Tmenuitem_magic_byte:
    case Tmenu_magic_byte:
      e = (TobjEntry)e->Parent;
      break;
    case Tmutex_magic_byte:
      e = (TobjEntry)((Tmutex)e)->Owner;
      break;
    default:
      if (IS_WIDGET(e))
        e = (TobjEntry)((Twidget)e)->Owner;
      else
        e = NULL;
      break;
    }
  }
  return (Tmsgport)e;
}

static void sockDeleteObj(void *V) {
  Tobj O = (Tobj)V;
  Tmsgport MsgPort = RemoteGetMsgPort(Slot);

  if (MsgPort && MsgPort == sockGetMsgPortObj(O))
    O->Delete();
}

static Twidget sockCreateWidget(dat XWidth, dat YWidth, uldat Attr, uldat Flags, dat Left, dat Up,
                                tcell Fill) {
  Tmsgport Owner;
  if ((Owner = RemoteGetMsgPort(Slot)))
    return New(widget)(Owner, XWidth, YWidth, Attr, Flags, Left, Up, Fill);
  return (Twidget)0;
}
static void sockRecursiveDeleteWidget(Twidget w) {
  Tmsgport MsgPort = RemoteGetMsgPort(Slot);

  /* avoid too much visual activity... UnMap top-level Twidget immediately */
  Act(UnMap, w)(w);

  Act(RecursiveDelete, w)(w, MsgPort);
}
static void sockSetXYWidget(Twidget w, dat x, dat y) {
  if (w) {
    if (w->Parent && IS_SCREEN(w->Parent)) {
      x += w->Parent->XLogic;
      y += w->Parent->YLogic;
    }
    Act(SetXY, w)(w, x, y);
  }
}
static void sockDrawWidget(Twidget w, dat XWidth, dat YWidth, dat Left, dat Up, const char *Text,
                           const trune *Font, const tcell *Attr) {
  if (w) {
    Act(Expose, w)(w, XWidth, YWidth, Left, Up, XWidth, Text, Font, Attr);
  }
}

static void sockFocusSubWidget(Twidget w) {
  Twidget P;
  if (w && !IS_SCREEN(w) && w->Parent && !IS_SCREEN(w->Parent)) {
    w->SelectW = NULL;
    while ((P = w->Parent) && !IS_SCREEN(P)) {
      P->SelectW = w;
      w = P;
    }
    if (ContainsCursor((Twidget)WindowParent(w)))
      UpdateCursor();
  }
}

static void sockResizeWidget(Twidget w, dat X, dat Y) {
  if (w) {
    if (IS_WINDOW(w)) {
      if (!(w->Flags & WINDOWFL_BORDERLESS))
        X += 2, Y += 2;
      ResizeRelWindow((Twindow)w, X - w->XWidth, Y - w->YWidth);
    } else if (IS_GADGET(w)) {
      ResizeGadget((Tgadget)w, X, Y);
    } else {
      ResizeWidget((Twidget)w, X, Y);
    }
  }
}

static void sockCirculateChildrenWidget(Twidget w, byte up_or_down) {
  if (w) {
    if (up_or_down == TW_CIRCULATE_RAISE_LAST) {
      if ((w = w->LastW))
        Act(Raise, w)(w);
    } else if (up_or_down == TW_CIRCULATE_LOWER_FIRST) {
      if ((w = w->FirstW))
        Act(Lower, w)(w);
    }
  }
}

static void sockCirculateChildrenRow(Tobj O, byte up_or_down) {
  Trow R = (Trow)0;
  if (O) {
    if (IS_WINDOW(O) && W_USE((Twindow)O, USEROWS)) {

      if (up_or_down == TW_CIRCULATE_RAISE_LAST)
        R = ((Twindow)O)->USE.R.LastRow;
      else if (up_or_down == TW_CIRCULATE_LOWER_FIRST)
        R = ((Twindow)O)->USE.R.FirstRow;

    } else if (IS_MENU(O)) {

      if (up_or_down == TW_CIRCULATE_RAISE_LAST)
        R = (Trow)((Tmenu)O)->LastI;
      else if (up_or_down == TW_CIRCULATE_LOWER_FIRST)
        R = (Trow)((Tmenu)O)->FirstI;
    }

    if (R) {
      if (up_or_down == TW_CIRCULATE_RAISE_LAST)
        Act(Raise, R)(R);
      else if (up_or_down == TW_CIRCULATE_LOWER_FIRST)
        Act(Lower, R)(R);
    }
  }
}

static Tgadget sockCreateGadget(Twidget Parent, dat XWidth, dat YWidth, const char *TextNormal,
                                uldat Attr, uldat Flags, udat Code, tcolor ColText,
                                tcolor ColTextSelect, tcolor ColTextDisabled,
                                tcolor ColTextSelectDisabled, dat Left, dat Up) {
  Tmsgport Owner;
  if ((Owner = RemoteGetMsgPort(Slot)))
    return New(gadget)(Owner, Parent, XWidth, YWidth, TextNormal, Attr, Flags, Code, ColText,
                       ColTextSelect, ColTextDisabled, ColTextSelectDisabled, Left, Up);
  return (Tgadget)0;
}

static Twindow sockCreateWindow(dat TitleLen, const char *Title, const tcolor *ColTitle, Tmenu Menu,
                                tcolor ColText, uldat CursorType, uldat Attr, uldat Flags,
                                dat XWidth, dat YWidth, dat ScrollBackLines) {
  Tmsgport Owner;
  if ((Owner = RemoteGetMsgPort(Slot)))
    return New(window)(Owner, TitleLen, Title, ColTitle, Menu, ColText, CursorType, Attr, Flags,
                       XWidth, YWidth, ScrollBackLines);
  return (Twindow)0;
}

static void sockWriteCharsetWindow(Twindow Window, uldat Len, const char *charset_bytes) {
  if (Window) {
    if ((Window->Flags & WINDOWFL_USEANY) == WINDOWFL_USECONTENTS)
      Act(TtyWriteCharset, Window)(Window, Len, charset_bytes);
    else if ((Window->Flags & WINDOWFL_USEANY) == WINDOWFL_USEROWS)
      Act(RowWriteCharset, Window)(Window, Len, charset_bytes);
  }
}

static void sockWriteUtf8Window(Twindow Window, uldat Len, const char *utf8_bytes) {
  if (Window) {
    if ((Window->Flags & WINDOWFL_USEANY) == WINDOWFL_USECONTENTS)
      Act(TtyWriteUtf8, Window)(Window, Len, utf8_bytes);
    else if ((Window->Flags & WINDOWFL_USEANY) == WINDOWFL_USEROWS)
      Act(RowWriteUtf8, Window)(Window, Len, utf8_bytes);
  }
}

static void sockWriteTRuneWindow(Twindow Window, uldat Len, const trune *runes) {
  if (Window) {
    if ((Window->Flags & WINDOWFL_USEANY) == WINDOWFL_USECONTENTS)
      Act(TtyWriteTRune, Window)(Window, Len, runes);
    else if ((Window->Flags & WINDOWFL_USEANY) == WINDOWFL_USEROWS)
      Act(RowWriteTRune, Window)(Window, Len, runes);
  }
}

static void sockWriteTCellWindow(Twindow Window, dat x, dat y, uldat Len, const tcell *cells) {
  if (Window) {
    if ((Window->Flags & WINDOWFL_USEANY) == WINDOWFL_USECONTENTS)
      Act(TtyWriteTCell, Window)(Window, x, y, Len, cells);
    else if ((Window->Flags & WINDOWFL_USEANY) == WINDOWFL_USEROWS)
      Act(RowWriteTCell, Window)(Window, x, y, Len, cells);
  }
}

static void sockSetTitleWindow(Twindow Window, dat titlelen, const char *title) {
  char *_title = NULL;

  if (Window) {
    if (!titlelen || (_title = (char *)CloneMem(title, titlelen + 1)))
      Act(SetTitle, Window)(Window, titlelen, _title);
  }
}

static Trow sockFindRowByCodeWindow(Twindow Window, dat Code) {
  if (Window) {
    return Act(FindRowByCode, Window)(Window, Code, NULL);
  }
  return (Trow)0;
}

static Tmenuitem sockCreate4MenuAny(Tobj Parent, Twindow Window, udat Code, byte Flags, ldat Len,
                                    const char *Name) {
  return Do(Create4Menu, menuitem)(Parent, Window, Code, Flags, Len, Name);
}

static Tmenu sockCreateMenu(tcolor ColItem, tcolor ColSelect, tcolor ColDisabled,
                            tcolor ColSelectDisabled, tcolor ColShtCut, tcolor ColSelShtCut,
                            byte FlagDefColInfo) {
  Tmsgport Owner;
  if ((Owner = RemoteGetMsgPort(Slot)))
    return New(menu)(Owner, ColItem, ColSelect, ColDisabled, ColSelectDisabled, ColShtCut,
                     ColSelShtCut, FlagDefColInfo);
  return (Tmenu)0;
}

/* last 3 args are currently useless for remote clients */
static Tmsgport sockCreateMsgPort(byte NameLen, const char *Name) {
  Tmsgport MsgPort;

  if ((MsgPort = New(msgport)(NameLen, Name, 0, 0, 0, SocketH))) {
    RegisterMsgPort(MsgPort, Slot);
    MsgPort->ShutDownHook = sockShutDown;
  }
  return MsgPort;
}
static Tmsgport sockFindMsgPort(Tmsgport Prev, byte NameLen, const char *Name) {
  Tmsgport M;
  if (!(M = Prev))
    M = All->FirstMsgPort;
  while (M) {
    if (M->NameLen == NameLen && !memcmp(M->Name, Name, NameLen))
      break;
    M = M->Next;
  }
  return M;
}

static Tgroup sockCreateGroup(void) {
  Tmsgport Owner;
  if ((Owner = RemoteGetMsgPort(Slot)))
    return New(group)(Owner);
  return (Tgroup)0;
}

static Tobj sockPrevObj(Tobj o) {
  TobjEntry e = (TobjEntry)o;
  return (Tobj)(e ? e->Prev : e);
}
static Tobj sockNextObj(Tobj o) {
  TobjEntry e = (TobjEntry)o;
  return (Tobj)(e ? e->Next : e);
}
static Tobj sockParentObj(Tobj o) {
  TobjEntry e = (TobjEntry)o;
  return (Tobj)(e ? e->Parent : e);
}

static Tscreen sockFirstScreen(void) {
  return All->FirstScreen;
}
static Twidget sockFirstWidget(Twidget w) {
  return w ? w->FirstW : w;
}
static Tmsgport sockFirstMsgPort(void) {
  return All->FirstMsgPort;
}
static Tmenu sockFirstMenu(Tmsgport MsgPort) {
  return MsgPort ? MsgPort->FirstMenu : (Tmenu)0;
}
static Twidget sockFirstW(Tmsgport MsgPort) {
  return MsgPort ? MsgPort->FirstW : (Twidget)0;
}
static Tgroup sockFirstGroup(Tmsgport MsgPort) {
  return MsgPort ? MsgPort->FirstGroup : (Tgroup)0;
}
static Tmutex sockFirstMutex(Tmsgport MsgPort) {
  return MsgPort ? MsgPort->FirstMutex : (Tmutex)0;
}
static Tmenuitem sockFirstMenuItem(Tmenu Menu) {
  return Menu ? Menu->FirstI : (Tmenuitem)0;
}
static Tgadget sockFirstGadget(Tgroup group) {
  return group ? group->FirstG : (Tgadget)0;
}

static Tall sockGetAll(void) {
  return All;
}

/*
 * turn the (Tmsg) into a (tmsg) and write it on the MsgPort file descriptor.
 *
 * this can be called in nasty places like detaching non-exclusive displays
 * when an exclusive one is started. Must preserve Slot, Fd and other globals!
 */
static void sockSendMsg(Tmsgport MsgPort, Tmsg msg) {
  uldat Len, Tot;
  byte *t;
  uldat save_Slot;
  int save_Fd;

#ifdef CONF_SOCKET_ALIEN
  if (AlienXendian(Slot) != MagicNative) {
    alienSendMsg(MsgPort, msg);
    return;
  }
#endif

  Len = 0;
  save_Slot = Slot;
  save_Fd = Fd;

  RequestN = MSG_MAGIC;
  Fd = MsgPort->RemoteData.Fd;
  Slot = MsgPort->RemoteData.FdSlot;

  switch (msg->Type) {
  case msg_display:
    sockReply(msg->Type, Len = sizeof(twindow) + 4 * sizeof(udat) + msg->Event.EventDisplay.Len,
              NULL);

    if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
      t += Tot - Len;

      Push(t, twindow, NOID); /* not used here */
      Push(t, udat, msg->Event.EventDisplay.Code);
      Push(t, udat, msg->Event.EventDisplay.Len);
      Push(t, udat, msg->Event.EventDisplay.X);
      Push(t, udat, msg->Event.EventDisplay.Y);
      PushV(t, msg->Event.EventDisplay.Len, msg->Event.EventDisplay.Data);
    }
    break;
  case msg_widget_key:
    sockReply(msg->Type,
              Len = sizeof(twindow) + 3 * sizeof(udat) + 2 * sizeof(byte) +
                    msg->Event.EventKeyboard.SeqLen,
              NULL);
    if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
      t += Tot - Len;
      Push(t, twidget, Obj2Id(msg->Event.EventKeyboard.W));
      Push(t, udat, msg->Event.EventKeyboard.Code);
      Push(t, udat, msg->Event.EventKeyboard.ShiftFlags);
      Push(t, udat, msg->Event.EventKeyboard.SeqLen);
      Push(t, byte, msg->Event.EventKeyboard.pad);
      PushV(t, msg->Event.EventKeyboard.SeqLen + 1, /* the final '\0' */
            msg->Event.EventKeyboard.AsciiSeq);
    }
    break;
  case msg_widget_mouse:
    sockReply(msg->Type, Len = sizeof(twindow) + 4 * sizeof(udat), NULL);
    if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
      t += Tot - Len;
      Push(t, twidget, Obj2Id(msg->Event.EventMouse.W));
      Push(t, udat, msg->Event.EventMouse.Code);
      Push(t, udat, msg->Event.EventMouse.ShiftFlags);
      Push(t, dat, msg->Event.EventMouse.X);
      Push(t, dat, msg->Event.EventMouse.Y);
    }
    break;
  case msg_widget_change:
    sockReply(msg->Type, Len = sizeof(twindow) + 6 * sizeof(dat), NULL);
    if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
      t += Tot - Len;
      Push(t, twidget, Obj2Id(msg->Event.EventWidget.W));
      Push(t, udat, msg->Event.EventWidget.Code);
      Push(t, udat, msg->Event.EventWidget.Flags);
      Push(t, udat, msg->Event.EventWidget.XWidth);
      Push(t, udat, msg->Event.EventWidget.YWidth);
      Push(t, udat, msg->Event.EventWidget.X);
      Push(t, udat, msg->Event.EventWidget.Y);
    }
    break;
  case msg_widget_gadget:
    sockReply(msg->Type, Len = sizeof(twindow) + 2 * sizeof(dat), NULL);
    if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
      t += Tot - Len;
      Push(t, twidget, Obj2Id(msg->Event.EventGadget.W));
      Push(t, udat, msg->Event.EventGadget.Code);
      Push(t, udat, msg->Event.EventGadget.Flags);
    }
    break;
  case msg_menu_row:
    sockReply(msg->Type, Len = sizeof(twindow) + 2 * sizeof(dat) + sizeof(tmenu) + sizeof(trow),
              NULL);
    if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
      t += Tot - Len;
      Push(t, twindow, Obj2Id(msg->Event.EventMenu.W));
      Push(t, udat, msg->Event.EventMenu.Code);
      Push(t, udat, msg->Event.EventMenu.pad);
      Push(t, tmenu, Obj2Id(msg->Event.EventMenu.Menu));
      Push(t, trow, Obj2Id(msg->Event.EventMenu.Row));
    }
    break;
  case msg_selection:
    sockReply(msg->Type, Len = sizeof(twindow) + 4 * sizeof(dat), NULL);
    if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
      t += Tot - Len;
      Push(t, twidget, Obj2Id(msg->Event.EventSelection.W));
      Push(t, udat, msg->Event.EventSelection.Code);
      Push(t, udat, msg->Event.EventSelection.pad);
      Push(t, dat, msg->Event.EventSelection.X);
      Push(t, dat, msg->Event.EventSelection.Y);
    }
    break;
  case msg_selection_notify:
    sockReply(msg->Type,
              Len = sizeof(twindow) + 2 * sizeof(dat) + 3 * sizeof(ldat) + MAX_MIMELEN +
                    msg->Event.EventSelectionNotify.Len,
              NULL);
    if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
      t += Tot - Len;
      Push(t, twidget, Obj2Id(msg->Event.EventSelectionNotify.W));
      Push(t, udat, msg->Event.EventSelectionNotify.Code);
      Push(t, udat, msg->Event.EventSelectionNotify.pad);
      Push(t, uldat, msg->Event.EventSelectionNotify.ReqPrivate);
      Push(t, uldat, msg->Event.EventSelectionNotify.Magic);
      PushV(t, MAX_MIMELEN, msg->Event.EventSelectionNotify.MIME);
      Push(t, uldat, msg->Event.EventSelectionNotify.Len);
      PushV(t, msg->Event.EventSelectionNotify.Len, msg->Event.EventSelectionNotify.Data);
    }
    break;
  case msg_selection_request:
    sockReply(msg->Type, Len = sizeof(twindow) + 2 * sizeof(dat) + sizeof(tobj) + sizeof(ldat),
              NULL);
    if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
      t += Tot - Len;
      Push(t, twidget, Obj2Id(msg->Event.EventSelectionRequest.W));
      Push(t, udat, msg->Event.EventSelectionRequest.Code);
      Push(t, udat, msg->Event.EventSelectionRequest.pad);
      Push(t, tobj, Obj2Id(msg->Event.EventSelectionRequest.Requestor));
      Push(t, uldat, msg->Event.EventSelectionRequest.ReqPrivate);
    }
    break;

  case msg_user_control:
    sockReply(msg->Type, Len = sizeof(twindow) + 4 * sizeof(dat) + msg->Event.EventControl.Len,
              NULL);
    if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
      t += Tot - Len;
      Push(t, twidget, Obj2Id(msg->Event.EventControl.W));
      Push(t, udat, msg->Event.EventControl.Code);
      Push(t, udat, msg->Event.EventControl.Len);
      Push(t, dat, msg->Event.EventControl.X);
      Push(t, dat, msg->Event.EventControl.Y);
      PushV(t, msg->Event.EventControl.Len, msg->Event.EventControl.Data);
    }
    break;

  case msg_user_clientmsg:
    sockReply(msg->Type, Len = sizeof(twindow) + 2 * sizeof(dat) + msg->Event.EventClientMsg.Len,
              NULL);
    if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
      t += Tot - Len;
      Push(t, twidget, Obj2Id(msg->Event.EventClientMsg.W));
      Push(t, udat, msg->Event.EventClientMsg.Code);
      Push(t, udat, msg->Event.EventClientMsg.Format);
      Push(t, uldat, msg->Event.EventClientMsg.Len);
      PushV(t, msg->Event.EventClientMsg.Len, msg->Event.EventClientMsg.Data.b);
    }
    break;

  default:
    break;
  }
  Slot = save_Slot;
  Fd = save_Fd;
}

#define tmsgEventDelta ((udat)(size_t) & (((tmsg)0)->Event))
#define tmsgEventOffset(x) ((udat)(size_t) & (((tmsg)0)->Event.x))

/* extract the (tmsg) from Data, turn it into a (Tmsg) and send it to MsgPort */
static byte sockSendToMsgPort(Tmsgport MsgPort, udat Len, const byte *Data) {
  tmsg tw_msg;
  Tmsgport Sender;
  Tmsg msg;
  uldat dstSlot;
  udat _Len, minType;
  byte ok = ttrue;

  /* FIXME: must code alienSendToMsgPort() and call it if AlienMagic(Slot) != MagicNative */

  /* be careful with alignment! */
  tw_msg = (tmsg)CloneMem(Data, Len);

  do
    if (MsgPort && Len && tw_msg && Len >= tmsgEventDelta && Len == tw_msg->Len &&
        tw_msg->Magic == msg_magic) {

      Sender = RemoteGetMsgPort(Slot);
      if (Sender && Sender->AttachHW && MsgPort->Handler != SocketH)
        minType = TW_MSG_DISPLAY;
      else
        minType = TW_MSG_USER_FIRST;
      if (tw_msg->Type < minType)
        /* not allowed! */
        break;

      _Len = 0;
      switch (tw_msg->Type) {
      case TW_MSG_WIDGET_KEY:
        if (Len >= tmsgEventOffset(EventKeyboard.AsciiSeq)) {
          if (tw_msg->Event.EventKeyboard.SeqLen + tmsgEventOffset(EventKeyboard.AsciiSeq) > Len)
            tw_msg->Event.EventKeyboard.SeqLen = Len - tmsgEventOffset(EventKeyboard.AsciiSeq);
          _Len += tw_msg->Event.EventKeyboard.SeqLen;
        } else
          /* (tmsg) too short */
          ok = tfalse;
        break;
      case TW_MSG_USER_CONTROL:
        if (Len >= tmsgEventOffset(EventControl.Data)) {
          if (tw_msg->Event.EventControl.Len + tmsgEventOffset(EventControl.Data) > Len)
            tw_msg->Event.EventControl.Len = Len - tmsgEventOffset(EventControl.Data);
          _Len += tw_msg->Event.EventControl.Len;
        } else
          /* (tmsg) too short */
          ok = tfalse;
        break;
      case TW_MSG_USER_CLIENTMSG:
        if (Len >= tmsgEventOffset(EventClientMsg.Data)) {
          if (tw_msg->Event.EventClientMsg.Len + tmsgEventOffset(EventClientMsg.Data) > Len)
            tw_msg->Event.EventClientMsg.Len = Len - tmsgEventOffset(EventClientMsg.Data);
          _Len += tw_msg->Event.EventClientMsg.Len;
        } else
          /* (tmsg) too short */
          ok = tfalse;
        break;
      default:
        break;
      }

      if (!ok)
        break;

      if (MsgPort->Handler == SocketH && MsgPort->RemoteData.Fd != NOFD &&
          (dstSlot = MsgPort->RemoteData.FdSlot) != NOSLOT &&
          !memcmp(FdList[dstSlot].AlienMagic, LS.AlienMagic, TWS_highest - 1)) {
        /*
         * shortcut: we have a (tmsg); instead of turning it into a (Tmsg)
         * then have SocketH() call sockSendMsg() to turn it back into a (tmsg),
         * we directly copy it --- only one condition must be met:
         * both clients must have the same AlienMagic()
         * (i.e. endianity and data sizes).
         */
        tw_msg->Len -= AlienSizeof(uldat, Slot);
        tw_msg->Magic = MSG_MAGIC;
        RemoteWriteQueue(dstSlot, Len, (const byte *)tw_msg);
        break;
      }

      if ((msg = New(msg)(tw_msg->Type, _Len))) {

        msg->Event.EventCommon.W = (Twidget)Id2Obj(Twidget_magic_byte, tw_msg->Event.EventCommon.W);

        switch (tw_msg->Type) {
        case TW_MSG_DISPLAY:
          if (sizeof(struct event_display) == sizeof(Twindow) + 4 * sizeof(dat) + sizeof(byte *) &&
              sizeof(struct s_tevent_display) ==
                  sizeof(twindow) + 4 * sizeof(dat) + sizeof(uldat)) {

            CopyMem(&tw_msg->Event.EventDisplay.Code, &msg->Event.EventDisplay.Code,
                    4 * sizeof(dat));
          } else {
            msg->Event.EventDisplay.Code = tw_msg->Event.EventDisplay.Code;
            msg->Event.EventDisplay.Len = tw_msg->Event.EventDisplay.Len;
            msg->Event.EventDisplay.X = tw_msg->Event.EventDisplay.X;
            msg->Event.EventDisplay.Y = tw_msg->Event.EventDisplay.Y;
          }
          if (!(msg->Event.EventDisplay.Data =
                    CloneMem(tw_msg->Event.EventDisplay.Data, tw_msg->Event.EventDisplay.Len)) &&
              tw_msg->Event.EventDisplay.Len)

            ok = tfalse;

          break;
        case TW_MSG_WIDGET_KEY:
          if (sizeof(struct event_keyboard) ==
                  sizeof(Twindow) + 3 * sizeof(dat) + 2 * sizeof(byte) &&
              sizeof(struct s_tevent_keyboard) ==
                  sizeof(twindow) + 3 * sizeof(dat) + 2 * sizeof(byte)) {

            CopyMem(&tw_msg->Event.EventKeyboard.Code, &msg->Event.EventKeyboard.Code,
                    3 * sizeof(dat) + sizeof(byte));
          } else {
            msg->Event.EventKeyboard.Code = tw_msg->Event.EventKeyboard.Code;
            msg->Event.EventKeyboard.ShiftFlags = tw_msg->Event.EventKeyboard.ShiftFlags;
            msg->Event.EventKeyboard.SeqLen = tw_msg->Event.EventKeyboard.SeqLen;
            msg->Event.EventKeyboard.pad = tw_msg->Event.EventKeyboard.pad;
          }
          CopyMem(tw_msg->Event.EventKeyboard.AsciiSeq, msg->Event.EventKeyboard.AsciiSeq,
                  tw_msg->Event.EventKeyboard.SeqLen);

          msg->Event.EventKeyboard.AsciiSeq[tw_msg->Event.EventKeyboard.SeqLen] = '\0';

          break;
        case TW_MSG_WIDGET_MOUSE:
          if (sizeof(struct event_mouse) == sizeof(Twindow) + 4 * sizeof(dat) &&
              sizeof(struct s_tevent_mouse) == sizeof(twindow) + 4 * sizeof(dat)) {

            CopyMem(&tw_msg->Event.EventMouse.Code, &msg->Event.EventMouse.Code, 4 * sizeof(dat));
          } else {
            msg->Event.EventMouse.Code = tw_msg->Event.EventMouse.Code;
            msg->Event.EventMouse.ShiftFlags = tw_msg->Event.EventMouse.ShiftFlags;
            msg->Event.EventMouse.X = tw_msg->Event.EventMouse.X;
            msg->Event.EventMouse.Y = tw_msg->Event.EventMouse.Y;
          }
          break;
        case TW_MSG_SELECTIONCLEAR:
          if (sizeof(struct event_common) == sizeof(Twindow) + 2 * sizeof(dat) &&
              sizeof(struct s_tevent_common) == sizeof(twindow) + 2 * sizeof(dat)) {

            CopyMem(&tw_msg->Event.EventCommon.Code, &msg->Event.EventCommon.Code, 2 * sizeof(dat));
          } else {
            msg->Event.EventCommon.Code = tw_msg->Event.EventCommon.Code;
            msg->Event.EventCommon.pad = tw_msg->Event.EventCommon.pad;
          }
          break;

/* TODO: is this needed? */
#if 0
              case TW_MSG_WIDGET_CHANGE:
                Push(t, udat,    msg->Event.EventWidget.Code);
                Push(t, udat,    msg->Event.EventWidget.XWidth);
                Push(t, udat,    msg->Event.EventWidget.YWidth);
                break;
              case TW_MSG_WIDGET_GADGET:
                Push(t, udat,    msg->Event.EventGadget.Code);
                Push(t, udat,    msg->Event.EventGadget.pad);
                break;
              case TW_MSG_MENU_ROW:
                Push(t, udat,    msg->Event.EventMenu.Code);
                Push(t, udat,    msg->Event.EventMenu.pad);
                Push(t, tmenu,   Obj2Id(msg->Event.EventMenu.Menu));
                Push(t, trow,    Obj2Id(msg->Event.EventMenu.Row));
                break;
#endif
        case TW_MSG_USER_CONTROL:
          if (sizeof(struct event_control) == sizeof(Twindow) + 4 * sizeof(dat) + sizeof(uldat) &&
              sizeof(struct s_tevent_control) ==
                  sizeof(twindow) + 4 * sizeof(dat) + sizeof(uldat)) {

            CopyMem(&tw_msg->Event.EventControl.Code, &msg->Event.EventControl.Code,
                    4 * sizeof(dat));
          } else {
            msg->Event.EventControl.Code = tw_msg->Event.EventControl.Code;
            msg->Event.EventControl.Len = tw_msg->Event.EventControl.Len;
            msg->Event.EventControl.X = tw_msg->Event.EventControl.X;
            msg->Event.EventControl.Y = tw_msg->Event.EventControl.Y;
          }
          CopyMem(tw_msg->Event.EventControl.Data, msg->Event.EventControl.Data,
                  tw_msg->Event.EventControl.Len);

          msg->Event.EventControl.Data[msg->Event.EventControl.Len] = '\0';
          break;
        case TW_MSG_USER_CLIENTMSG:
          msg->Event.EventClientMsg.Code = tw_msg->Event.EventClientMsg.Code;
          msg->Event.EventClientMsg.Format = tw_msg->Event.EventClientMsg.Format;
          msg->Event.EventClientMsg.Len = tw_msg->Event.EventClientMsg.Len;
#ifdef CONF_SOCKET_ALIEN
          /* FIXME: this must be replaced with a call to alienSendToMsgPort() above */
          if (AlienXendian(Slot) == MagicAlienXendian) {
            tevent_clientmsg tC = &tw_msg->Event.EventClientMsg;
            event_clientmsg *C = &msg->Event.EventClientMsg;

            alienReadVec(tC->Data.b, C->Data.b, tC->Len, tC->Format, tC->Format, ttrue);
          } else
#endif
            CopyMem(tw_msg->Event.EventClientMsg.Data.b, msg->Event.EventClientMsg.Data.b,
                    tw_msg->Event.EventClientMsg.Len);
          break;
        default:
          ok = tfalse;
          break;
        }

        if (ok)
          SendMsg(MsgPort, msg);
        else
          msg->Delete();
      }
    }
  while (0);

  FreeMem(tw_msg);
  return ok;
}

#undef tmsgEventDelta
#undef tmsgEventOffset

static void sockBlindSendToMsgPort(Tmsgport MsgPort, udat Len, const byte *Data) {
  (void)sockSendToMsgPort(MsgPort, Len, Data);
}

static Tobj sockGetOwnerSelection(void) {
  return TwinSelectionGetOwner();
}

static void sockSetOwnerSelection(tany Time, tany Frac) {
  if (LS.MsgPort)
    TwinSelectionSetOwner((Tobj)LS.MsgPort, Time, Frac);
}

static void sockNotifySelection(Tobj Requestor, uldat ReqPrivate, uldat Magic,
                                const char MIME[MAX_MIMELEN], uldat Len, const char *Data) {
  TwinSelectionNotify(Requestor, ReqPrivate, e_id(Magic), MIME, Chars(Data, Len));
}

static void sockRequestSelection(Tobj Owner, uldat ReqPrivate) {
  if (LS.MsgPort)
    TwinSelectionRequest((Tobj)LS.MsgPort, ReqPrivate, Owner);
}

#ifdef CONF_SOCKET_GZ

static byte sockCanCompress(void) {
  return ttrue;
}

static voidpf sockZAlloc(voidpf opaque, uInt items, uInt size) {
  void *ret = AllocMem(items * (size_t)size);
  return ret ? (voidpf)ret : Z_NULL;
}

static void sockZFree(voidpf opaque, voidpf address) {
  if (address != Z_NULL)
    FreeMem(address);
}

/* fixup the uncompressed slot for on-the-fly compression */
static void FixupGzip(uldat slot) {
  ls_p.Fd = ls.Fd;
  ls.Fd = specFD;
  ls.PrivateFlush = RemoteGzipFlush;
  ls.PrivateAfterFlush = NULL;
}

/* finish shutting down compression (called on the uncompressed slot) */
static void ShutdownGzip(uldat slot) {
  inflateEnd((z_streamp)ls.PrivateData);
  deflateEnd((z_streamp)ls_p.PrivateData);

  FreeMem(ls.PrivateData);
  FreeMem(ls_p.PrivateData);

  ls.Fd = ls_p.Fd;
  ls_p.Fd = specFD;
  UnRegisterRemote(ls.pairSlot);

  ls.pairSlot = NOSLOT;
  ls.PrivateFlush = NULL;
  ls.PrivateAfterFlush = NULL;
}

/*
 * this is not perfectly clean, but works well:
 * we enable (disable) compression and give an uncompressed (compressed) answer,
 * but we also send in the same way any answer pending or collected before the RemoteFlush().
 *
 * So the library must wait for the answer immediately after calling TwDoCompress();
 * this is what it normally does anyway since it has to wait for the return value...
 * no problem.
 */
static byte sockDoCompress(byte on_off) {
  uldat slot = NOSLOT;
  z_streamp z1 = NULL, z2 = NULL;

  if (on_off) {
    if ((slot = RegisterRemoteFd(specFD, LS.HandlerIO.S)) != NOSLOT &&
        (z1 = (z_streamp)AllocMem(sizeof(*z1))) && (z2 = (z_streamp)AllocMem(sizeof(*z2)))) {

      z1->zalloc = z2->zalloc = sockZAlloc;
      z1->zfree = z2->zfree = sockZFree;
      z1->opaque = z2->opaque = NULL;

      if (deflateInit(z1, Z_BEST_COMPRESSION) == Z_OK) {
        if (inflateInit(z2) == Z_OK) {

          /* ok, start pairing the two slots */
          ls.pairSlot = Slot;
          ls.PrivateData = static_cast<void *>(z1);

          LS.pairSlot = slot;
          LS.PrivateData = static_cast<void *>(z2);
          LS.PrivateAfterFlush = FixupGzip;

          /* we have ls.Fd == specFD. it will be fixed by LS.PrivateAfterFlush() */

          return ttrue;
        }
        deflateEnd(z1);
      }
    }
    if (z2)
      FreeMem(z2);
    if (z1)
      FreeMem(z1);
    if (slot != NOSLOT)
      UnRegisterRemote(slot);
    return tfalse;
  } else {
    /* inform RemoteFlush() we are shutting down the compression... */
    LS.PrivateAfterFlush = ShutdownGzip;

    return ttrue;
  }
}

#else

static byte sockCanCompress(void) {
  return tfalse;
}

static byte sockDoCompress(byte on_off) {
  return tfalse;
}

#endif /* CONF_SOCKET_GZ */

/* socket initialization functions */

static uldat SendTwinProtocol(void) {
  static char buf[] = " Twin-" TW_PROTOCOL_VERSION_STR "-" TWIN_VERSION_STR "\n";
  buf[0] = strlen(buf);
  return RemoteWriteQueue(Slot, buf[0], (const byte *)buf);
}

static byte SendTwinMagic(byte len, const byte *magic) {
  return RemoteWriteQueue(Slot, len, magic) == len;
}

static byte sockReply(uldat code, uldat len, const void *data) {
  uldat buf[3];
  buf[0] = 2 * sizeof(uldat) + len;
  buf[1] = RequestN;
  buf[2] = code;
  if (RemoteWriteQueue(Slot, 3 * sizeof(uldat), buf) == 3 * sizeof(uldat) && len)
    return RemoteWriteQueue(Slot, len, data) == len;
  return tfalse;
}

static byte SendUldat(uldat data) {

#ifdef CONF_SOCKET_ALIEN

  if (AlienXendian(Slot) == MagicNative)
    return RemoteWriteQueue(Slot, sizeof(uldat), &data) == sizeof(uldat);

  {
    uldat len;
    byte *t, AlienSizeofUldat = AlienSizeof(uldat, Slot);

    if (RemoteWriteQueue(Slot, AlienSizeofUldat, NULL) == AlienSizeofUldat) {
      t = RemoteWriteGetQueue(Slot, &len);
      t += len - AlienSizeofUldat;
      alienPUSH(t, uldat, data);
      return ttrue;
    }
    return tfalse;
  }
#else /* !CONF_SOCKET_ALIEN */

  return RemoteWriteQueue(Slot, sizeof(uldat), &data) == sizeof(uldat);

#endif /* CONF_SOCKET_ALIEN */
}

#define digestLen 16 /* hardcoded in MD5 routines */
#define AuthLen 256  /* length of ~/.TwinAuth */
#define ChallLen 256 /* length of random data */
#define TotalLen (AuthLen + ChallLen)

static char AuthData[TotalLen];

static uldat GetRandomData(void) {
  int ufd, got = -1;
  unsigned long res;
  uldat len = 0;

  if ((ufd = open("/dev/urandom", O_RDONLY)) >= 0) {
    for (; len < AuthLen; len += got) {
      got = read(ufd, AuthData + len, (size_t)(AuthLen - len));
      if (got < 0) {
        if (errno == EINTR || errno == EWOULDBLOCK)
          got = 0;
        else
          break;
      }
    }
    close(ufd);
  }
  if (len < AuthLen) {
    /* /dev/urandom unavailable ? use mrand48... */
    srand48(time(NULL) + getpid());
    got = 0;

    while (len < AuthLen) {
      if (!got) {
        got = 4;
        res = (unsigned long)mrand48();
      }
      AuthData[len++] = res & TW_MAXBYTE;
      res >>= 8;
      got--;
    }
  }
  return len;
}

static byte CreateAuth(char *path) {
  int fd, got = -1;
  uldat len = 0;

  if ((fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0600)) >= 0 && chmod(path, 0600) == 0) {

    len = GetRandomData();

    if (len == AuthLen)
      for (len = 0; len < AuthLen; len += got) {
        got = write(fd, AuthData + len, AuthLen - len);
        if (got < 0) {
          if (errno == EINTR || errno == EWOULDBLOCK)
            got = 0;
          else
            break;
        }
      }
    close(fd);
  }

  return len == AuthLen ? ttrue : Error(SYSERROR);
}

static byte sockInitAuth(void) {
  int fd, got;
  uldat len;

  if (!HOME)
    return tfalse;

  len = HOME.size();
  len = Min2(len, TotalLen - 11);
  CopyMem(HOME.data(), AuthData, len);
  CopyMem("/.TwinAuth", AuthData + len, 11);

  if ((fd = open(AuthData, O_RDONLY)) < 0)
    return CreateAuth(AuthData);

  for (len = 0; len < AuthLen; len += got) {
    got = read(fd, AuthData + len, AuthLen - len);
    if (got < 0) {
      if (errno == EINTR || errno == EWOULDBLOCK)
        got = 0;
      else {
        close(fd);
        return Error(SYSERROR);
      }
    }
  }
  close(fd);
  if (len < AuthLen)
    return CreateAuth(AuthData);

  return ttrue;
}

static byte SendChallenge(void) {
  struct MD5Context ctx;
  int fd, got;
  uldat len, queue;
  byte *t;

  if ((fd = open("/dev/urandom", O_RDONLY)) < 0)
    return tfalse;
  len = AuthLen;
  for (got = 1; len < TotalLen && got; len += got) {
    do {
      got = read(fd, AuthData + len, TotalLen - len);
    } while (got < 0 && errno == EINTR);
    if (got <= 0)
      break;
  }
  close(fd);
  if (got > 0 && len == TotalLen && (len -= AuthLen) && RemoteReadGrowQueue(Slot, digestLen) &&
      /* make space for digest, and also check there's no pending data */
      (t = RemoteReadGetQueue(Slot, &queue)) && queue == digestLen && SendUldat(len) &&
      RemoteWriteQueue(Slot, len, AuthData + AuthLen)) {

    MD5Init(&ctx);
    MD5Update(&ctx, AuthData, TotalLen);
    MD5Final(t, &ctx); /* write digest into t */

    return ttrue;
  }
  return tfalse;
}

/*
 * try to read (len) bytes from socket.
 * return # of bytes received or -1 for errors
 */
static int TryRead(int fd, uldat slot, uldat len) {
  byte *t;
  int got;

  if ((t = RemoteReadGrowQueue(slot, len))) {
    got = read(fd, t, len);

    RemoteReadShrinkQueue(slot, len - Max2(got, 0));

    return got;
  }
  return -1;
}

/*
 * try to ensure (len) bytes are in FdList[slot].RQueue.
 * return # of bytes available or -1 for errors
 */
static int EnsureRead(int fd, uldat slot, uldat len) {
  uldat max;
  int got;

  (void)RemoteReadGetQueue(slot, &max);

  if (max < len) {
    got = TryRead(fd, slot, len - max);

    if (got == 0 || (got < 0 && errno != EINTR && errno != EWOULDBLOCK))
      return -1;

    if (got > 0)
      max += got;
  }
  return max;
}

static handler_io_s GetHandlerIO() {
#ifdef CONF_SOCKET_ALIEN
  if (AlienXendian(Slot) != MagicNative) {

    return AlienIO;
  }
#endif /* CONF_SOCKET_ALIEN */

  return SocketIO;
}

static void Wait4AuthIO(int fd, uldat slot) {
  byte *t;
  int got;

  /* macro LS uses global variable Slot: set it */
  got = EnsureRead(fd, Slot = slot, digestLen * 2);

  if (got < 0)
    ;
  else if (got < digestLen * 2)
    /* not yet ready to check */
    return;
  else { /* (got >= digestLen*2) */
    t = RemoteReadGetQueue(Slot, NULL);
    if (!memcmp(t, t + digestLen, digestLen)) {
      /* OK! */
      if ((LS.HandlerIO.S = GetHandlerIO())) {
        RemoteReadDeQueue(Slot, digestLen * 2);
        SendUldat(TW_GO_MAGIC);
        return;
      }
    }
  }

  /* I/O error or Auth error */
  SendUldat(TW_STOP_MAGIC);
  RemoteFlush(Slot);
  UnRegisterRemote(Slot);
  close(fd);
}

static byte Check4MagicTranslation(uldat slot, const byte *magic, byte len) {
#ifdef CONF_SOCKET_ALIEN
  static byte warn_count = 0;
#endif

  const char *zero = (const char *)memchr(magic, '\0', len);
  byte len1 = zero ? (byte)(zero - (const char *)magic) : 0;

  if (len1 > TWS_tcolor && len == magic[0] && len == len1 + 1 + sizeof(uldat) &&
      /*check negotiated size to match ours*/
      !memcmp(magic + 1, TwinMagicData + 1, Min2(len1, TWS_highest) - 1) &&
      /*pre-0.3.9 compatibility: if tcell is not negotiated, assume 2 bytes*/
      (len1 > TWS_tcell || sizeof(tcell) == 2) &&
      /*check endianity*/
      !memcmp(magic + len1 + 1, TwinMagicData + TWS_highest + 1, sizeof(uldat))) {

    /* store client magic numbers */
    CopyMem(magic, AlienMagic(slot), Min2(len1, TWS_highest));
    if (len1 < TWS_highest)
      /* version mismatch compatibility: zero out unnegotiated sizes */
      memset(AlienMagic(slot) + len1, '\0', TWS_highest - len1);

    return MagicNative;
  }

#ifdef CONF_SOCKET_ALIEN

  if (len1 > TWS_tcell && len == magic[0] &&
      magic[TWS_byte] == 1 && /* sizeof(byte) MUST be 1, or passing byte[] vectors would fail */
      magic[TWS_udat] >= 2 && magic[TWS_uldat] >= 4 && len - len1 == magic[TWS_uldat] + 1 &&
      magic[TWS_tcolor] ==
          1 && /* sizeof(tcolor) MUST be 1, or passing tcolor[] vectors would fail */
      magic[TWS_topaque] >= 4 &&
      magic[TWS_tany] >= 4 && magic[TWS_trune] >= 1 && magic[TWS_tcell] >= 2) {

    /* store client magic numbers */
    CopyMem(magic, AlienMagic(slot), Min2(len1, TWS_highest));
    if (len1 < TWS_highest)
      /* version mismatch compatibility: zero out unnegotiated sizes */
      memset(AlienMagic(slot) + len1, '\0', TWS_highest - len1);

    if (warn_count < 6) {
      zero = NULL;
      if (AlienMagic(slot)[TWS_tcell] < sizeof(tcell))
        zero = "tcell";
      else if (AlienMagic(slot)[TWS_trune] < sizeof(trune))
        zero = "trune";

      if (zero) {
        if (warn_count == 5)
          log(WARNING) << "twin: warning: many clients with different sizes, suppressing further "
                          "messages.\n";
        else
          log(WARNING) << "twin: warning: client has different `" << Chars::from_c(zero),
              "' size, it may not be Unicode aware.\n";
        warn_count++;
      }
    }

    /*
     * now check endianity.
     *
     * non-trivial trick: TWIN_MAGIC significant bits are 0-31,
     * so on little endian machines (magic+len1+1) is always "Twin"...
     * no matter what value has magic[TWS_uldat].
     * On big endian machines, (magic+len1+1) is ..."niwT", with
     * (magic[TWS_uldat] - 4) zeroed bytes at start.
     */
    if (!memcmp(magic + len1 + 1, "Twin", 4))
      /* little endian client. and us? */
      return TW_IS_LITTLE_ENDIAN ? MagicAlien : MagicAlienXendian;
    if (!memcmp(magic + len1 + 1 + (magic[TWS_uldat] - 4), "niwT", 4))
      /* big endian client. and us? */
      return TW_IS_BIG_ENDIAN ? MagicAlien : MagicAlienXendian;
  }

#endif /* CONF_SOCKET_ALIEN */

  return MagicUnknown;
}

static void Wait4Magic(int fd, uldat slot) {
  byte *t;
  uldat max;
  int got;

  t = RemoteReadGetQueue(Slot = slot, &max);
  if (max == 0)
    max = TW_MAXBYTE;
  else
    max = t[0];

  got = EnsureRead(fd, Slot, max);

  t = RemoteReadGetQueue(Slot, NULL);
  if (got > 0)
    max = t[0];

  if (got < 0) {
    ;
  } else if ((uldat)got < max) {
    /* not yet ready to check */
    return;
  } else { /* (got >= max) */
    /*
     * check whether the client has our same sizes and endianity
     * or one of the available translations is needed.
     */
    if ((AlienXendian(Slot) = Check4MagicTranslation(Slot, t, max)) == MagicUnknown) {
      /*
       * no suitable translation available. use our native magic,
       * in case client's library can handle it.
       */
      CopyMem(TwinMagicData, AlienMagic(Slot), TWS_highest);
      AlienXendian(Slot) = MagicNative;
      got = SendTwinMagic(TwinMagicData[0], TwinMagicData);
    } else {
      /*
       * we have a translation for client's magic.
       * answer its same magic.
       */
      got = SendTwinMagic((byte)max, t);
    }
    RemoteReadDeQueue(Slot, max);

    if (got) {
      LS.HandlerIO.S = Wait4AuthIO;
      if (SendUldat(TW_WAIT_MAGIC) && SendChallenge()) {
        return;
      }
    }
  }

  /* problem... out of memory or I/O error. */
  UnRegisterRemote(Slot);
  close(fd);
}

static void unixSocketIO(int fd, uldat slot) {
  struct sockaddr_un un_addr = {};
  socklen_t len = sizeof(un_addr);
  if ((fd = accept(unixFd, (struct sockaddr *)&un_addr, &len)) >= 0) {
    /*
     * twin < 0.9.1 automatically trusted clients connected via the unix socket
     * i.e. accepted them without TwinAuth challenge.
     * For increased security, twin >= 0.9.1 sends TwinAuth challenge also to them.
     *
     * Reason: on non-Linux systems, file system permissions on unix domain socket
     * may be ignored => different users may be able to connect through it.
     */
    if ((Slot = RegisterRemoteFd(fd, Wait4Magic)) != NOSLOT) {
      fcntl(fd, F_SETFL, O_NONBLOCK);
      fcntl(fd, F_SETFD, FD_CLOEXEC);
      if (SendTwinProtocol()) {
        return;
      }
      UnRegisterRemote(Slot);
    }
    close(fd);
  }
}

static void inetSocketIO(int fd, uldat slot) {
  struct sockaddr_in in_addr = {};
  socklen_t len = sizeof(in_addr);
  if ((fd = accept(inetFd, (struct sockaddr *)&in_addr, &len)) >= 0) {
    if ((Slot = RegisterRemoteFd(fd, Wait4Magic)) != NOSLOT) {
      fcntl(fd, F_SETFL, O_NONBLOCK);
      fcntl(fd, F_SETFD, FD_CLOEXEC);
      if (SendTwinProtocol()) {
        return;
      }
      UnRegisterRemote(Slot);
    }
    close(fd);
  }
}

static void sockKillSlot(uldat slot) {
  Tmsgport MsgPort;
  Tdisplay D_HW;

  if (slot != NOSLOT) {

#ifdef CONF_SOCKET_GZ
    if (ls.pairSlot != NOSLOT) {
      if (ls.Fd != specFD)
        /* compressed socket, use the other one */
        slot = ls.pairSlot;

      /* uncompressed socket: shutdown compression then close */
      ShutdownGzip(slot);

      UnRegisterRemote(ls.pairSlot);
    }
#endif

    if ((MsgPort = RemoteGetMsgPort(slot))) {
      /*
       * no infinite recursion between KillSlot and DeleteMsgPort...
       * DeleteMsgPort doesn't kill the slot, only unregisters from it
       */

      if ((D_HW = MsgPort->AttachHW)) {
        /* avoid KillSlot <-> DeleteDisplayHW infinite recursion */
        D_HW->AttachSlot = NOSLOT;
        D_HW->Delete();
      }

      MsgPort->Delete(); /* and all its children ! */
    }

    if (ls.Fd >= 0)
      close(ls.Fd);
    UnRegisterRemote(slot);
  }
}

static void SocketIO(int fd, uldat slot) {
  uldat len, Funct;
  byte *t;
  const byte *tend;
  int tot = 0;
#ifdef CONF_SOCKET_GZ
  uldat gzSlot;
#endif

  Fd = fd;
  Slot = slot;

  if (ioctl(Fd, FIONREAD, &tot) != 0 || tot <= 0)
    tot = TW_SMALLBUFF;
  else if (tot > TW_HUGEBUFF)
    tot = TW_HUGEBUFF;

  if (!(t = RemoteReadGrowQueue(Slot, tot)))
    return;

  if ((len = read(Fd, t, tot)) && len && len != (uldat)-1) {
    if (len < (uldat)tot)
      RemoteReadShrinkQueue(Slot, (uldat)tot - len);

      /* ok, now process the data */

#ifdef CONF_SOCKET_GZ
    if ((gzSlot = LS.pairSlot) != NOSLOT) {
      /* hmmm, a compressed socket. */
      if (RemoteGunzip(Slot))
        Slot = gzSlot;
      else {
        Ext(Remote, KillSlot)(Slot);
        return;
      }
    }
#endif

    s = t = RemoteReadGetQueue(Slot, &len);
    tend = s + len;

    while (s + 3 * sizeof(uldat) <= tend) {
      Pop(s, uldat, len);
      if (len < 2 * sizeof(uldat)) {
        s += len;
        continue;
      }
      if (s + len > s && s + len <= tend) {
        end = s + len;
        Pop(s, uldat, RequestN);
        Pop(s, uldat, Funct);
        if (Funct < MaxFunct) {
          slot = Slot;
          sockMultiplexB(Funct); /* Slot is the uncompressed socket here ! */
          Slot = slot;           /*
                                  * restore, in case sockF[Funct].F() changed it;
                                  * without this, tw* clients can freeze
                                  * if twdisplay is in use
                                  */
        } else if (Funct == FIND_MAGIC)
          sockMultiplexB(0);
        s = end;
      } else if (s + len < s) {
        s = tend;
        break;
      } else { /* if (s + len > tend) */
        /* must wait for rest of packet... unpop len */
        s -= sizeof(uldat);
        break;
      }
    }
    RemoteReadDeQueue(Slot, (uldat)(s - t));

#ifdef CONF_SOCKET_GZ
    if (gzSlot != NOSLOT)
      /* compressed socket, restore Slot */
      Slot = gzSlot;
#endif

  } else if (!len || (len == (uldat)-1 && errno != EINTR && errno != EWOULDBLOCK)) {
    /* let's close this sucker */
    Ext(Remote, KillSlot)(Slot);
  }
}

static void SocketH(Tmsgport MsgPort) {
  Tmsg msg;
  Twidget w;
  char buf[10];
  byte len;

  while ((msg = MsgPort->FirstMsg)) {
    msg->Remove();

    if (msg->Type == msg_widget_mouse && (w = msg->Event.EventMouse.W) && IS_WINDOW(w) &&
        (w->Flags & WINDOWFL_USECONTENTS) && ((Twindow)w)->USE.C.TtyData &&
        ((Twindow)w)->USE.C.TtyData->Flags & (TTY_REPORTMOUSE_TWTERM | TTY_REPORTMOUSE_XTERM)) {

      len = CreateXTermMouseEvent(&msg->Event.EventMouse, 10, buf);
      /*
       * SyntheticKey() will send an appropriate keyboard message to this MsgPort
       * and we will sockSendMsg() it later in the while() loop.
       */
      if (len)
        SyntheticKey(msg->Event.EventMouse.W, TW_XTermMouse, 0, len, buf);
    } else
      sockSendMsg(MsgPort, msg);

    msg->Delete();
  }
}

static void (*save_unixSocketIO)(int fd, uldat slot);

EXTERN_C byte InitModule(Tmodule Module) {
  uldat m;
  struct sockaddr_in addr;
  char opt[TW_SIZEOF_SIZE_T] = {
      0,
  };

  if (!sockInitAuth()) {
    log(ERROR) << "twin: failed to create ~/.TwinAuth: " << Errstr << "\n";
    return tfalse;
  }

  memset(&addr, 0, sizeof(addr));

  addr.sin_family = AF_INET;
  /*
   * the following wraps around on 'unsigned short' overflow:
   * useful if we ever want to listen on ports < TW_INET_PORT i.e. < 7754
   */
  addr.sin_port = htons(TW_INET_PORT + strtoul(TWDisplay + 1, NULL, 16));

  if ((inetFd = socket(AF_INET, SOCK_STREAM, 0)) >= 0 &&
      setsockopt(inetFd, SOL_SOCKET, SO_REUSEADDR, static_cast<void *>(opt), sizeof(opt)) >= 0 &&
      bind(inetFd, (struct sockaddr *)&addr, sizeof(addr)) >= 0 && listen(inetFd, 1) >= 0 &&
      fcntl(inetFd, F_SETFD, FD_CLOEXEC) >= 0) {

    if ((inetSlot = RegisterRemoteFd(inetFd, inetSocketIO)) != NOSLOT) {
      // nothing to do
    } else {
      close(inetFd);
    }
  } else {
    Error(SYSERROR);
    if (inetFd >= 0) {
      close(inetFd);
    }
  }

  if (unixSlot != NOSLOT || inetSlot != NOSLOT) {
    for (MaxFunct = 0; sockF[MaxFunct].Name; MaxFunct++) {
      sockF[MaxFunct].Len = strlen(sockF[MaxFunct].Name);
      sockF[MaxFunct].FormatLen = strlen(sockF[MaxFunct].Format);
    }

    RegisterExt(Remote, KillSlot, sockKillSlot);
    RegisterExt(Socket, SendMsg, sockSendMsg);
    RegisterExt(Socket, InitAuth, sockInitAuth);
    RegisterExt(Socket, MultiplexS, sockMultiplexS);

    m = TWIN_MAGIC;
    CopyMem(&m, TwinMagicData + TwinMagicData[0] - sizeof(uldat), sizeof(uldat));

    if (unixSlot != NOSLOT) {
      save_unixSocketIO = FdList[unixSlot].HandlerIO.S;
      FdList[unixSlot].HandlerIO.S = unixSocketIO;
    }

    return ttrue;
  }
  log(ERROR) << "twin: failed to create sockets: " << Errstr << "\n";
  return tfalse;
}

EXTERN_C void QuitModule(Tmodule Module) {
  if (unixSlot != NOSLOT)
    FdList[unixSlot].HandlerIO.S = save_unixSocketIO;

  if (inetSlot != NOSLOT) {
    UnRegisterRemote(inetSlot);
    close(inetFd);
  }
  for (Slot = 0; Slot < FdTop; Slot++) {
    if (LS.Fd != NOFD && (LS.HandlerIO.S == Wait4Magic || LS.HandlerIO.S == Wait4AuthIO ||
#ifdef CONF_SOCKET_ALIEN
                          LS.HandlerIO.S == AlienIO ||
#endif
                          LS.HandlerIO.S == SocketIO)) {
      Ext(Remote, KillSlot)(Slot);
    }
  }
  UnRegisterExt(Remote, KillSlot, sockKillSlot);
  UnRegisterExt(Socket, SendMsg, sockSendMsg);
  UnRegisterExt(Socket, InitAuth, sockInitAuth);
  UnRegisterExt(Socket, MultiplexS, sockMultiplexS);
}
