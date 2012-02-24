/*
 *  socket.c  --  remote libTw server implementation: supports unix and inet sockets
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
 * "autoconf.h" and "osincludes.h" early to pull in HAVE_* and system headers
 * necessary to include <sys/socket.h> under FreeBSD.
 */
#include "autoconf.h"
#include "osincludes.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <netdb.h>
#include <netinet/in.h>

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_UTSNAME_H
# include <sys/utsname.h>
#endif

#ifdef CONF_SOCKET_GZ
# include <zlib.h>
#endif

#include "twin.h"
#include "methods.h"
#include "data.h"
#include "draw.h"
#include "main.h"
#include "printk.h"
#include "util.h"
#include "resize.h"
#include "extreg.h"

#include "fdlist.h"
#include "remote.h"
#include "socket.h"
#include "md5.h"
#include "hw_multi.h"
#include "common.h"
#include "unaligned.h"
#include "version.h"

#include <Tw/Tw.h>
#include <Tw/Twkeys.h>
#include <Tw/Twstat.h>
#include <Tw/Twstat_defs.h>

#ifdef CONF__UNICODE
# include <Tutf/Tutf.h>
#endif





#ifdef CONF_SOCKET_ALIEN

static void alienPop(CONST byte ** src, uldat alien_len, byte *dst, uldat len);
static void alienPush(CONST byte *src, uldat len, byte **dst, uldat alien_len);
# define alienPOP(s,type,lval)		alienPop((CONST byte **)&(s),AlienSizeof(type,Slot),(byte *)&(lval),sizeof(type))
# define alienPUSH(s,type,lval)		alienPush((CONST byte *)&(lval),sizeof(type),&(s),AlienSizeof(type,Slot))

static void alienSendMsg(msgport MsgPort, msg Msg);
static void AlienIO(int fd, uldat slot);

#endif


/* First: private variables from remote.c */
extern fdlist *FdList;
extern uldat FdSize, FdTop, FdBottom, FdWQueued;
#define LS	FdList[Slot]
#define ls      FdList[slot]
#define ls_p    FdList[ls.pairSlot]

/* some extra functions not included in remote.c */

#if 0 /* currently unused */
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
	    MoveMem(LS.RQueue + LS.RQstart, LS.RQueue, LS.RQlen),
	    LS.RQstart = 0;
	
	if (LS.RQlen + len > LS.RQmax) {
	    LS.RQueue = (byte *)ReAllocMem(LS.RQueue, nmax = (LS.RQmax+len+40)*5/4);
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
	if (len < LS.RQlen)
	    LS.RQlen -= len;
	else {
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
	if (len) *len = 0;
	return NULL;
    }
    if (len)
	*len = LS.RQlen;
    return LS.RQueue + LS.RQstart;
}

static byte *RemoteWriteGetQueue(uldat Slot, uldat *len) {
    if (Slot >= FdTop || LS.Fd == NOFD || !LS.WQlen) {
	if (len) *len = 0;
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
	if (len) *len = 0;
	return NULL;
    }
    delta = LS.RQmax - LS.RQstart - LS.RQlen; /* still available */
    if (len)
	*len = delta;
    LS.RQlen += delta; /* alloc them */
    return LS.RQueue + LS.RQmax - delta; /* return the address of first one */
}

static byte *RemoteWriteFillQueue(uldat Slot, uldat *len) {
    uldat delta;
    if (Slot >= FdTop || LS.Fd == NOFD) {
	if (len) *len = 0;
	return NULL;
    }
    delta = LS.WQmax - LS.WQlen; /* yet available */
    if (len)
	*len = delta;
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
	z->next_in = LS.WQueue;	z->avail_in = LS.WQlen;
	
	/* in case sizeof(z->avail_out) != sizeof(uldat) */
	z->next_out = RemoteWriteFillQueue(slot, &delta); z->avail_out = delta;

	while (z->avail_in && zret == Z_OK) {
	    
	    if (z->avail_out < (delta = z->avail_in + 12)) {
		if (RemoteWriteQueue(slot, delta - z->avail_out, NULL)) {
		    if (!(ls.WQlen -= delta))
			FdWQueued--;
		    z->next_out = RemoteWriteFillQueue(slot, &delta); z->avail_out = delta;
		} else /* out of memory ! */
		    break;
	    }
	    
	    zret = deflate(z, Z_SYNC_FLUSH);

	    /* update the compressed queue */
	    if (!(ls.WQlen -= z->avail_out))
		FdWQueued--;
	}
    }
    /* update the uncompressed queue */
    if (z->avail_in)
	CopyMem(LS.WQueue + LS.WQlen - z->avail_in, LS.WQueue, z->avail_in);

    slot = LS.WQlen;
    if (!(LS.WQlen = z->avail_in) && slot)
	FdWQueued--;
    
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
	z->next_in = RemoteReadGetQueue(Slot, &delta); z->avail_in = delta;
	z->next_out = RemoteReadFillQueue(slot, &delta); z->avail_out = delta;
	
	while (z->avail_in && zret == Z_OK) {
	    
	    /* approx. guess of uncompression ratio: 1 to 5 */
	    if (z->avail_out < (delta = 5 * z->avail_in + 12)) {
		if (RemoteReadGrowQueue(slot, delta - z->avail_out)) {
		    ls.RQlen -= delta;
		    z->next_out = RemoteReadFillQueue(slot, &delta); z->avail_out = delta;
		} else /* out of memory ! */
		    break;
	    }
	    
	    zret = inflate(z, Z_SYNC_FLUSH);

	    /* update the uncompressed queue */
	    ls.RQlen -= z->avail_out;
	}
    }
    /* update the compressed queue */
    if (z->avail_in)
	LS.RQstart += LS.RQlen - z->avail_in;
    LS.RQlen = z->avail_in;
    return zret == Z_OK;
}

static void ShutdownGzip(uldat Slot);

#endif /* CONF_SOCKET_GZ */


static void SocketH(msgport MsgPort);

static void sockShutDown(msgport MsgPort) {
    if (MsgPort->RemoteData.FdSlot < FdTop)
	UnRegisterMsgPort(MsgPort);
}



/* prototypes of libTw back-end utility functions */

static uldat sockFindFunction(byte Len, CONST byte *name, byte ProtoLen, CONST byte *Proto);
static byte sockSyncSocket(void);
static byte sockServerSizeof(byte Type);
static byte sockCanCompress(void);
static byte sockDoCompress(byte on_off);

static void sockNeedResizeDisplay(void);
static void sockAttachHW(uldat len, CONST byte *arg, byte flags);
static byte sockDetachHW(uldat len, CONST byte *arg);
static void sockSetFontTranslation(CONST byte trans[0x80]);
static void sockSetHWFontTranslation(CONST hwfont trans[0x80]);

static void sockDeleteObj(void *V);

static widget sockCreateWidget(dat XWidth, dat YWidth, uldat Attrib, uldat Flags, dat Left, dat Up, hwattr Fill);
static void sockRecursiveDeleteWidget(widget W);
static void sockSetXYWidget(widget W, dat x, dat y);
static void sockResizeWidget(widget W, dat XWidth, dat YWidth);
#define sockScrollWidget ScrollWidget
static void sockDrawWidget(widget W, dat XWidth, dat YWidth, dat Left, dat Up, CONST byte *Text, CONST hwfont *Font, CONST hwattr *Attr);

#define sockSetVisibleWidget SetVisibleWidget
static void sockFocusSubWidget(widget W);

#define sockRestackChildrenWidget RestackWidgets
static void sockCirculateChildrenWidget(widget W, byte up_or_down);
#define TW_CIRCULATE_RAISE_LAST 0
#define TW_CIRCULATE_LOWER_FIRST 1

static gadget sockCreateGadget
    (widget Parent, dat XWidth, dat YWidth, CONST byte *TextNormal, uldat Attrib, uldat Flags, udat Code,
     hwcol ColText, hwcol ColTextSelect, hwcol ColTextDisabled, hwcol ColTextSelectDisabled,
     dat Left, dat Up);

static window sockCreateWindow(dat TitleLen, CONST byte *Title, CONST hwcol *ColTitle, menu Menu,
			       hwcol ColText, uldat CursorType, uldat Attrib, uldat Flags,
			       dat XWidth, dat YWidth, dat ScrollBackLines);
static void sockWriteAsciiWindow(window Window, ldat Len, CONST byte *Ascii);
static void sockWriteStringWindow(window Window, ldat Len, CONST byte *String);
static void sockWriteHWFontWindow(window Window, ldat Len, CONST hwfont *HWFont);
static void sockWriteHWAttrWindow(window Window, dat x, dat y, ldat Len, CONST hwattr *Attr);
static void sockSetTitleWindow(window Window, dat titlelen, CONST byte *title);

static row  sockFindRowByCodeWindow(window Window, dat Code);

static menuitem sockCreate4MenuAny(obj Parent, window Window, udat Code, byte Flags, ldat Len, byte CONST *Name);

#define sockRestackChildrenRow RestackRows
static void sockCirculateChildrenRow(obj O, byte up_or_down);
#define TW_CIRCULATE_RAISE_LAST 0
#define TW_CIRCULATE_LOWER_FIRST 1

static menu sockCreateMenu
    (hwcol ColItem, hwcol ColSelect, hwcol ColDisabled, hwcol ColSelectDisabled,
     hwcol ColShtCut, hwcol ColSelShtCut, byte FlagDefColInfo);

static msgport sockCreateMsgPort(byte NameLen, CONST byte *Name);
static msgport sockFindMsgPort(msgport Prev, byte NameLen, CONST byte *Name);

static group sockCreateGroup(void);

static obj sockPrevObj(obj Obj);
static obj sockNextObj(obj Obj);
static obj sockParentObj(obj Obj);

static screen sockFirstScreen(void);
static widget sockFirstWidget(widget W);
static msgport sockFirstMsgPort(void);
static menu sockFirstMenu(msgport MsgPort);
static widget sockFirstW(msgport MsgPort);
static group sockFirstGroup(msgport MsgPort);
static mutex sockFirstMutex(msgport MsgPort);
static menuitem sockFirstMenuItem(menu Menu);
static gadget sockFirstGadget(group Group);

static byte sockSendToMsgPort(msgport MsgPort, udat Len, CONST byte *Data);
static void sockBlindSendToMsgPort(msgport MsgPort, udat Len, CONST byte *Data);

static obj  sockGetOwnerSelection(void);
static void sockSetOwnerSelection(tany Time, tany Frac);
static void sockNotifySelection(obj Requestor, uldat ReqPrivate,
				 uldat Magic, CONST byte MIME[MAX_MIMELEN], uldat Len, CONST byte *Data);
static void sockRequestSelection(obj Owner, uldat ReqPrivate);

#define sockSetServerUid SetServerUid
#define sockGetDisplayWidth GetDisplayWidth
#define sockGetDisplayHeight GetDisplayHeight

static all sockGetAll(void);

static byte sockDecodeExtension(topaque *Len, CONST byte **Data, topaque *Args_n, tsfield args);

static extension sockOpenExtension(byte namelen, CONST byte *name);
static tany sockCallBExtension(extension e, topaque len, CONST byte *args, CONST byte *return_type);
static void sockCloseExtension(extension e);


/* Second: socket handling functions */


static uldat MaxFunct, Slot, RequestN;
static byte *s, *end;
static int inetFd = NOFD, Fd;
static uldat inetSlot = NOSLOT;

static byte sockReply(uldat code, uldat len, CONST void *data);

static void SocketIO(int fd, uldat slot);


#define Left(size)	(s + (size) <= end)




#include "socket_id.h"


static void sockStat(obj x, udat n, CONST byte *in);




typedef struct {
    byte Len, FormatLen;
    CONST byte *Name, *Format;
} sockfn;

static sockfn sockF[] = {
#include "socket2_m4.h"
    { 0, 0, "StatObj", "0S0x"obj_magic_STR"_"TWS_udat_STR"V"TWS_udat_STR },
    { 0, 0, NULL, NULL }
};


/* convert a 2-byte string "v"TWS_void_STR or "_"* or "V"* into a tsfield->type */
TW_INLINE udat proto_2_TWS(CONST byte proto[2]) {
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
	/* else FALLTHROUGH */
      case 'v':
	/* turn '\xFE' into TWS_void ('\0') */
	if (proto[1] == (byte)TWS_void_CHR)
	    break;
	/* else FALLTHROUGH */
      default:
	/* safe assumption */
	tws_type = TWS_tany;
	break;
    }
    return tws_type;
}

TW_INLINE void TWS_2_proto(udat tws_type, byte proto [2]) {
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
#define _len .TWS_field_vecL

#define _type .type

#define void_ TWS_void
#define obj_  TWS_obj
#define vec_  TWS_vec
#define vecW_ TWS_vecW





/* here a[0] is the first arg, N is the number of args */
static void sockMultiplex_S(uldat id, topaque N, tsfield a) {
    switch (id) {
#include "socket1_m4.h"
      default:
	break;
    }
}

/* here a[1] is the first arg, a[0] is the return value, N is the number of args + 1 */
#define fullMultiplexS(Id, N, a) do { \
    if ((Id) == order_StatObj) { \
	if ((N) > 3) { \
	    sockStat((obj)(a)[1]_obj, (udat)(a)[2]_any, (CONST byte *)(a)[3]_vec); \
	} \
    } else { \
	sockMultiplex_S(Id, N, a); \
    } \
} while (0)


/* here a[0] is the first arg, N is the number of args */
static void sockMultiplexS(uldat id, topaque N, tsfield a) {
    sockMultiplex_S(id, N, a);
    if (id < MaxFunct)
	a[0]_type = proto_2_TWS(sockF[id].Format);
}


/* code to return array lengths V(expr) and W(expr) */
static uldat sockLengths(uldat id, uldat n, CONST tsfield a) {
    uldat L = 0;

    switch (id) {
	
#include "socket3_m4.h"
	
      case order_StatObj:
	switch (n) {
	    case 3: L = a[2]_any; break;
	}
      default:
	break;
    }
    return L;
}





#if 0 /* currently unused */

TW_INLINE udat MultiplexArgsV2S(uldat id, udat N, va_list va, tsfield a) {
    CONST byte *Format = sockF[id].Format;
    udat n;
    byte c, t, size;

    /* skip return value */
    Format += 2;
    
    for (n = 1; n < N && (c = *Format++); n++) {
	t = *Format++;
	if (t >= TWS_highest)
	    /*
	     * let (tobj) fields decade into (uldat),
	     * since in this case they are not real pointers.
	     */
	    t = TWS_uldat;
	switch (c) {
	  case '_': case 'x':
	    a[n]_any = va_arg(va, tany);
	    break;
	  case 'W': case 'Y':
	    /* FALLTHROUGH */
	  case 'V': case 'X':
	    a[n]_vec = (CONST void *)(topaque)va_arg(va, tany);
	    break;
	  default:
	    return 0;
	}
    }
    return n;
}


/*
 * same as sockMultiplexA: call the appropriate sock* function.
 * but now the args are passed with a variadic prototype, not in a tsfield array.
 * *ALL* arguments should be cast to (tany) before passing them!!!
 */
static tany sockMultiplexL(uldat id, ...) {
    struct s_tsfield a[TW_MAX_ARGS_N];
    va_list va;
    udat N;

    if (id == FIND_MAGIC)
	id = 0;
    
    if (id < MaxFunct) {
	va_start(va, id);
	N = MultiplexArgsV2S(id, TW_MAXUDAT, va, a);
	va_end(va);

	sockMultiplexS(id, N, a);
	return a[0]_any;
    }
    return 0;
}

#endif /* 0 */ /* currently unused */




TW_DECL_MAGIC(TwinMagicData);


/***********/

#include "socketstat.h"

/***********/


/*
 * translate an array of IDs to an array of obj of type c;
 * *alloced is TRUE if needed to allocate a buffer, FALSE otherwise.
 * if success, return array of obj, else return NULL.
 */
static CONST obj *AllocId2ObjVec(byte *alloced, byte c, uldat n, byte *VV) {
#if TW_SIZEOFULDAT >= TW_SIZEOFTOPAQUE && TW_CAN_UNALIGNED != 0
    CONST uldat *L = (CONST uldat *)VV;
    CONST obj *aX;
    obj *X;
    
    aX = X = (obj *)VV;	
    while (n--)
	*X++ = Id2Obj(c, *L++);
    *alloced = FALSE;
    return aX;
#else
    CONST byte *S;
    CONST obj *aX;
    obj *X;
    uldat i;
    
    if ((aX = X = (obj *)AllocMem(n * sizeof(obj)))) {
	S = (CONST byte *)VV;
	while (n--) {
	    Pop(S, uldat, i);
	    *X++ = Id2Obj(c, i);
	}
	*alloced = TRUE;
    } else
	*alloced = FALSE;
    return aX;
#endif
}

TW_INLINE ldat sockDecodeArg(uldat id, CONST byte * Format, uldat n, tsfield a, uldat mask[1], byte flag[1], ldat fail) {
    void *av;
    topaque nlen;
    byte c;
    
    switch ((c = *Format++)) {
      case '_':
	switch ((c = *Format)) {
#define CASE_(type) \
	  case CAT(TWS_,type): \
	    /* ensure type size WAS negotiated */ \
	    if (AlienSizeof(type, Slot) && Left(sizeof(type))) { \
		type an; \
		Pop(s,type,an); \
		a[n]_any = (tany)an; \
		a[n]_type = c; \
		break; \
	    } \
	    fail = -fail; \
	    break
		
	  case TWS_hwcol:
	    /*FALLTHROUGH*/
	    CASE_(byte);
	    CASE_(dat);
	    CASE_(ldat);
	    CASE_(topaque);
	    CASE_(tany);
	    CASE_(hwfont);
	    CASE_(hwattr);
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
	    Pop(s,uldat,a0);
	    c = *Format - base_magic_CHR;
	    a[n]_obj = Id2Obj(c, a0);
	    a[n]_type = obj_;
	    break;
	}
	fail = -fail;
	break;
      case 'V':
	nlen = sockLengths(id, n, a);
	c = *Format;
	/* ensure type size WAS negotiated */
	if ((c <= TWS_hwcol || AlienMagic(Slot)[c])) {
	    nlen *= AlienMagic(Slot)[c];
	    if (Left(nlen)) {
		PopAddr(s,byte,nlen,av);
		a[n]_len = nlen;
		a[n]_vec = av;
		a[n]_type = vec_|c;
		break;
	    }
	}
	fail = -fail;
	break;
      case 'W':
        /* ensure 'topaque' size WAS negotiated */
	if (AlienSizeof(topaque, Slot) && Left(sizeof(topaque))) {
	    Pop(s,topaque,nlen);
	    
	    c = *Format;
	    /* ensure type size WAS negotiated */
	    if ((c <= TWS_hwcol || AlienMagic(Slot)[c])) {
		if (!nlen || (Left(nlen) && nlen == sockLengths(id, n, a) * AlienMagic(Slot)[c])) {
		    PopAddr(s,byte,nlen,av);
		    a[n]_len = nlen;
		    a[n]_vec = av;
		    a[n]_type = vec_|vecW_|c;
		    break;
		}
	    }
	}
	fail = -fail;
	break;
      case 'X':
	nlen = sockLengths(id, n, a) * sizeof(uldat);
	if (Left(nlen)) {
	    c = *Format - base_magic_CHR;
	    PopAddr(s,byte,nlen,av);
	    if ((a[n]_vec = AllocId2ObjVec(flag, c, nlen/sizeof(uldat), av))) {
		a[n]_len = nlen;
		a[n]_type = vec_|obj_;
		*mask |= *flag << n;
		break;
	    }
	}
	fail = -fail;
	break;
      case 'Y':
        /* ensure 'topaque' size WAS negotiated */
	if (AlienSizeof(topaque, Slot) && Left(sizeof(topaque))) {
	    Pop(s,topaque,nlen);

	    nlen *= sizeof(uldat);
	    if (Left(nlen)) {
		c = *Format - base_magic_CHR;
		PopAddr(s,byte,nlen,av);
		if ((a[n]_vec = AllocId2ObjVec(flag, c, nlen/sizeof(uldat), av))) {
		    a[n]_len = nlen;
		    a[n]_type = vec_|obj_;
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
    static byte warned = FALSE;
    uldat mask = 0; /* at least 32 bits. we need TW_MAX_ARGS_N... */
    uldat nlen, n = 1;
    ldat fail = 1;
    CONST byte *Format = sockF[id].Format;
    uldat a0;
    byte c, self, flag, retT[2];

    self = *Format++;
    retT[0] = *Format++;
    retT[1] = *Format++;
    
    while (fail > 0 && *Format) {
	if (n < TW_MAX_ARGS_N) {
	    fail = sockDecodeArg(id, Format, n, a, &mask, &flag, fail);
	
	} else /* (n >= TW_MAX_ARGS_N) */ {
	    if (!warned) {
		warned = TRUE;
		printk("twin: sockMultiplexB(): got a call with %d args, only %d supported!\n",
		       n, TW_MAX_ARGS_N);
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
    
    if ((flag = (fail > 0 && s == end && !*Format && (self != '2' || a[1]_obj)))) {

	if (retT[0] == 'O' && a[n-1]_type == (TWS_vec|TWS_byte) && a[n-1]_len == 2*sizeof(byte)) {
	    /*
	     * variable return type. store it in last arg,
	     * and let function implementation overwrite it
	     * 
	     * evil trick: only a[n-1]_vec will be passed to the function,
	     * but it points to a[n-1] itself!
	     */
	    a[n-1]_type = proto_2_TWS(a[n-1]_vec);
	    if (mask & 1<<(n-1))
		FreeMem(a[n-1].TWS_field_vecVV);
	    
	    a[n-1]_vec = &a[n-1];
	    a[n-1]_len = 0;
	}
	
	fullMultiplexS(id, n, a);
    }
    
    for (nlen = 0; mask; mask >>= 1, nlen++) {
	if (mask & 1)
	    FreeMem(a[nlen].TWS_field_vecVV);
    }
    
    if (flag) {
        if (retT[0] == 'O') {
	    /* variable return type. get it from last arg */
	    /* FIXME: currently, only '_' (scalar) and 'v' (void) return types are supported */
		
	    TWS_2_proto(a[n-1]_type, retT);
	}
	
	switch (retT[0]) {
	  case '_':
	    switch (retT[1]) {
#define CASE_(type) \
	      case CAT(TWS_,type): \
    		/* ensure type size WAS negotiated */ \
		if (CAT(TWS_,type) <= TWS_hwcol || AlienSizeof(type, Slot)) { \
		    /* move to first bytes on MSB machines */ \
		    *(type *)&a[0]_any = (type)a[0]_any; \
		    c = sizeof(type); \
		    break; \
		} \
		fail = 0; \
		break
		
		case TWS_hwcol:
		/*FALLTHROUGH*/
		CASE_(byte);
		CASE_(dat);
		CASE_(ldat);
		CASE_(topaque);
		CASE_(tany);
		CASE_(hwfont);
		CASE_(hwattr);
#undef CASE_
	      default:
		c = 0;
		break;
	    }
	    if (c && fail > 0) {
		sockReply(OK_MAGIC, c, &a[0]_any);
		return;
	    }
	    break;
	    
	  case 'x':
	    a0 = a[0]_obj ? a[0]_obj->Id : NOID;
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
	    if (self != '2' || a[1]_obj)
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
# include "socketalien.h"
#endif

/***********/


/* actual libTw back-end utility functions */

static int CmpFormat(CONST byte *F1, CONST byte *F2, uldat Len) {
    for (; Len; F1++, F2++, Len--) {
	if (Len > 1 && *F1 == *F2 && (*F1 == 'x' || *F1 == 'X' || *F1 == 'Y')) {
	    /*
	     * pointer. must allow any, since libTw
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

static uldat sockFindFunction(byte Len, CONST byte *Name, byte FormatLen, CONST byte *Format) {
    sockfn *F = sockF;
    if (Name) {
	while (F->Name && (F->Len != Len || F->FormatLen - 1 != FormatLen ||
			   CmpMem(F->Name, Name, Len) || CmpFormat(F->Format+1, Format, FormatLen)))
	    F++;
	if (F->Name)
	    return (uldat)(F - sockF);
    }
    return NOID;
}

static byte sockSyncSocket(void) {
    return TRUE;
}

static byte sockServerSizeof(byte Type) {
    return TwinMagicData[Type < TWS_highest ? Type : TWS_void];
}


static void sockNeedResizeDisplay(void){
    if (LS.MsgPort && LS.MsgPort->AttachHW)
	ResizeDisplayPrefer(LS.MsgPort->AttachHW);
}

/*
 * this does direct write() on the connecting socket,
 * so it bypasses any compression.
 */
static void sockAttachHW(uldat len, CONST byte *arg, byte flags) {
    display_hw D_HW;
    byte buf[2] = "\0",
	verbose = flags & TW_ATTACH_HW_REDIRECT,
	exclusive = flags & TW_ATTACH_HW_EXCLUSIVE;
    int realFd;
    fd_set set;
    struct timeval tv = {2,0};
    
    realFd = LS.Fd >= 0 ? LS.Fd : FdList[LS.pairSlot].Fd;

    if (!LS.MsgPort) {
	if (verbose)
	    write(realFd, "twin: AttachHW(): client did not create a MsgPort\n\0", 52);
	else
	    write(realFd, buf, 2);
	return;
    }
    
    if (verbose)
	verbose = RegisterPrintk(realFd);
	
    if ((D_HW = AttachDisplayHW(len, arg, Slot, exclusive))) {
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
	    
    while (select(realFd+1, &set, NULL, NULL, &tv) == -1 && errno == EINTR)
	;
    read(realFd, buf, 1);

    if (verbose)
	UnRegisterPrintk();
}

static byte sockDetachHW(uldat len, CONST byte *arg) {
    return DetachDisplayHW(len, arg, 0); /* cannot detach exclusive displays !! */
}

static void sockSetFontTranslation(CONST byte trans[0x80]) {
    if (trans) {
	int i;
	hwfont *G = All->Gtranslations[USER_MAP];

	for (i = 0; i < 0x80; i++)
	    G[i] = i;
	if (sizeof(hwfont) == sizeof(byte))
	    CopyMem(trans, G + 0x80, 0x80);
	else
	    for (i = 0x0; i < 0x80; i++)
		G[0x80|i] = trans[i];
    }
}

static void sockSetHWFontTranslation(CONST hwfont trans[0x80]) {
#ifdef CONF__UNICODE
    if (trans) {
	int i;
	hwfont *G = All->Gtranslations[USER_MAP];

	for (i = 0; i < 0x80; i++)
	    G[i] = i;
	CopyMem(trans, G + 0x80, sizeof(hwfont) * 0x80);
    }
#else
    sockSetFontTranslation((CONST byte *)trans);
#endif
}

static msgport sockGetMsgPortObj(obj P) {
    while (P) {
	if (IS_MSGPORT(P)) {
	    return (msgport)P;
	}
	switch (P->Id >> magic_shift) {
	  case row_magic:
	  case menuitem_magic:
	  case menu_magic:
	    P = (obj) P->Parent;
	    break;
	  case mutex_magic:
	    P = (obj) ((mutex)P)->Owner;
	    break;
	  default:
	    if (IS_WIDGET(P))
		P = (obj) ((widget)P)->Owner;
	    else
		P = (obj)0;
	    break;
	}
    }
    return (msgport)P;
}

static void sockDeleteObj(void *V) {
    obj O = (obj)V;
    msgport MsgPort = RemoteGetMsgPort(Slot);
    
    if (MsgPort && MsgPort == sockGetMsgPortObj(O))
	Delete(O);
}

static widget sockCreateWidget(dat XWidth, dat YWidth, uldat Attrib, uldat Flags, dat Left, dat Up, hwattr Fill) {
    msgport Owner;
    if ((Owner = RemoteGetMsgPort(Slot)))
	return Do(Create,Widget)
	    (FnWidget, Owner, XWidth, YWidth, Attrib, Flags, Left, Up, Fill);
    return (widget)0;
}
static void sockRecursiveDeleteWidget(widget W) {
    msgport MsgPort = RemoteGetMsgPort(Slot);
    
    /* avoid too much visual activity... UnMap top-level widget immediately */
    Act(UnMap,W)(W);
    
    Act(RecursiveDelete,W)(W, MsgPort);
}
static void sockSetXYWidget(widget W, dat x, dat y) {
    if (W) {
	if (W->Parent && IS_SCREEN(W->Parent)) {
	    x += W->Parent->XLogic;
	    y += W->Parent->YLogic;
	}
	Act(SetXY,W)(W, x, y);
    }
}
static void sockDrawWidget(widget W, dat XWidth, dat YWidth, dat Left, dat Up, CONST byte *Text, CONST hwfont *Font, CONST hwattr *Attr) {
    if (W) {
	Act(Expose,W)(W, XWidth, YWidth, Left, Up, XWidth, Text, Font, Attr);
    }
}

static void sockFocusSubWidget(widget W) {
    widget P;
    if (W && !IS_SCREEN(W) && W->Parent && !IS_SCREEN(W->Parent)) {
	W->SelectW = NULL;
	while ((P = W->Parent) && !IS_SCREEN(P)) {
	    P->SelectW = W;
	    W = P;
	}
	if (ContainsCursor((widget)WindowParent(W)))
	    UpdateCursor();
    }
}

static void sockResizeWidget(widget W, dat X, dat Y) {
    if (W) {
	if (IS_WINDOW(W)) {
	    if (!(W->Flags & WINDOWFL_BORDERLESS))
		X+=2, Y+=2;
	    ResizeRelWindow((window)W, X - W->XWidth, Y - W->YWidth);
	} else if (IS_GADGET(W)) {
	    ResizeGadget((gadget)W, X, Y);
	} else {
	    ResizeWidget((widget)W, X, Y);
	}
    }
}

static void sockCirculateChildrenWidget(widget W, byte up_or_down) {
    if (W) {
	if (up_or_down == TW_CIRCULATE_RAISE_LAST) {
	    if ((W = W->LastW))
		Act(Raise,W)(W);
	} else if (up_or_down == TW_CIRCULATE_LOWER_FIRST) {
	    if ((W = W->FirstW))
		Act(Lower,W)(W);
	}
    }
}

static void sockCirculateChildrenRow(obj O, byte up_or_down) {
    row R = (row)0;
    if (O) {
	if (IS_WINDOW(O) && W_USE((window)O, USEROWS)) {
	    
    	    if (up_or_down == TW_CIRCULATE_RAISE_LAST)
		R = ((window)O)->USE.R.LastRow;
	    else if (up_or_down == TW_CIRCULATE_LOWER_FIRST)
		R = ((window)O)->USE.R.FirstRow;
	    
	} else if (IS_MENU(O)) {
	    
    	    if (up_or_down == TW_CIRCULATE_RAISE_LAST)
		R = (row)((menu)O)->LastI;
	    else if (up_or_down == TW_CIRCULATE_LOWER_FIRST)
		R = (row)((menu)O)->FirstI;
		
	}
	
	if (R) {
	    if (up_or_down == TW_CIRCULATE_RAISE_LAST)
		Act(Raise,R)(R);
	    else if (up_or_down == TW_CIRCULATE_LOWER_FIRST)
		Act(Lower,R)(R);
	}
    }
}


static gadget sockCreateGadget
    (widget Parent, dat XWidth, dat YWidth, CONST byte *TextNormal, uldat Attrib, uldat Flags, udat Code,
     hwcol ColText, hwcol ColTextSelect, hwcol ColTextDisabled, hwcol ColTextSelectDisabled, dat Left, dat Up)
{
    msgport Owner;
    if ((Owner = RemoteGetMsgPort(Slot)))
	return Do(Create,Gadget)
	    (FnGadget, Owner, Parent, XWidth, YWidth, TextNormal, Attrib, Flags, Code,
		 ColText, ColTextSelect, ColTextDisabled, ColTextSelectDisabled, Left, Up);
    return (gadget)0;
}

static window sockCreateWindow(dat TitleLen, CONST byte *Title, CONST hwcol *ColTitle, menu Menu,
			       hwcol ColText, uldat CursorType, uldat Attrib, uldat Flags,
			       dat XWidth, dat YWidth, dat ScrollBackLines) {
    msgport Owner;
    if ((Owner = RemoteGetMsgPort(Slot)))
	return Do(Create,Window)
	    (FnWindow, Owner, TitleLen, Title, ColTitle, Menu, ColText,
	     CursorType, Attrib, Flags, XWidth, YWidth, ScrollBackLines);
    return (window)0;
}

static void sockWriteAsciiWindow(window Window, ldat Len, CONST byte *Ascii) {
    if (Window) {
	if ((Window->Flags & WINDOWFL_USEANY) == WINDOWFL_USECONTENTS)
	    Act(TtyWriteAscii,Window)(Window, Len, Ascii);
	else if ((Window->Flags & WINDOWFL_USEANY) == WINDOWFL_USEROWS)
	    Act(RowWriteAscii,Window)(Window, Len, Ascii);
    }
}

static void sockWriteStringWindow(window Window, ldat Len, CONST byte *String) {
    if (Window) {
	if ((Window->Flags & WINDOWFL_USEANY) == WINDOWFL_USECONTENTS)
	    Act(TtyWriteString,Window)(Window, Len, String);
	else if ((Window->Flags & WINDOWFL_USEANY) == WINDOWFL_USEROWS)
	    Act(RowWriteString,Window)(Window, Len, String);
    }
}

static void sockWriteHWFontWindow(window Window, ldat Len, CONST hwfont *HWFont) {
    if (Window) {
	if ((Window->Flags & WINDOWFL_USEANY) == WINDOWFL_USECONTENTS)
	    Act(TtyWriteHWFont,Window)(Window, Len, HWFont);
	else if ((Window->Flags & WINDOWFL_USEANY) == WINDOWFL_USEROWS)
	    Act(RowWriteHWFont,Window)(Window, Len, HWFont);
    }
}

static void sockWriteHWAttrWindow(window Window, dat x, dat y, ldat Len, CONST hwattr *Attr) {
    if (Window) {
	if ((Window->Flags & WINDOWFL_USEANY) == WINDOWFL_USECONTENTS)
	    Act(TtyWriteHWAttr,Window)(Window, x, y, Len, Attr);
	else if ((Window->Flags & WINDOWFL_USEANY) == WINDOWFL_USEROWS)
	    Act(RowWriteHWAttr,Window)(Window, x, y, Len, Attr);
    }
}

static void sockSetTitleWindow(window Window, dat titlelen, CONST byte *title) {
    byte *_title = NULL;
    
    if (Window) {
	if (!titlelen || (_title = CloneMem(title, titlelen + 1)))
	    Act(SetTitle,Window)(Window, titlelen, _title);
    }
}

static row sockFindRowByCodeWindow(window Window, dat Code) {
    if (Window) {
	return Act(FindRowByCode,Window)(Window, Code, NULL);
    }
    return (row)0;
}


static menuitem sockCreate4MenuAny(obj Parent, window Window, udat Code, byte Flags, ldat Len, byte CONST *Name) {
    return Do(Create4Menu,MenuItem)(FnMenuItem, Parent, Window, Code, Flags, Len, Name);
}

static menu sockCreateMenu
    (hwcol ColItem, hwcol ColSelect, hwcol ColDisabled, hwcol ColSelectDisabled,
     hwcol ColShtCut, hwcol ColSelShtCut, byte FlagDefColInfo) {
    msgport Owner;
    if ((Owner = RemoteGetMsgPort(Slot)))
	return Do(Create,Menu)(FnMenu, Owner, ColItem, ColSelect, ColDisabled, ColSelectDisabled, ColShtCut, ColSelShtCut,
			       FlagDefColInfo);
    return (menu)0;
}
			       

/* last 3 args are currently useless for remote clients */
static msgport sockCreateMsgPort(byte NameLen, CONST byte *Name) {
    msgport MsgPort;
    
    if ((MsgPort = Do(Create,MsgPort)(FnMsgPort, NameLen, Name, 0, 0, 0, SocketH))) {
	RegisterMsgPort(MsgPort, Slot);
	MsgPort->ShutDownHook = sockShutDown;
    }
    return MsgPort;
}
static msgport sockFindMsgPort(msgport Prev, byte NameLen, CONST byte *Name) {
    msgport M;
    if (!(M = Prev))
	M = All->FirstMsgPort;
    while (M) {
	if (M->NameLen == NameLen && !CmpMem(M->Name, Name, NameLen))
	    break;
	M = M->Next;
    }
    return M;
}

static group sockCreateGroup(void) {
    msgport Owner;
    if ((Owner = RemoteGetMsgPort(Slot)))
	return Do(Create,Group)(FnGroup, Owner);
    return (group)0;
}

static obj sockPrevObj(obj Obj) {
    return Obj ? Obj->Prev : Obj;
}
static obj sockNextObj(obj Obj) {
    return Obj ? Obj->Next : Obj;
}
static obj sockParentObj(obj Obj) {
    return Obj ? (obj)Obj->Parent : Obj;
}

static screen sockFirstScreen(void) {
    return All->FirstScreen;
}
static widget sockFirstWidget(widget W) {
    return W ? W->FirstW : W;
}
static msgport sockFirstMsgPort(void) {
    return All->FirstMsgPort;
}
static menu sockFirstMenu(msgport MsgPort) {
    return MsgPort ? MsgPort->FirstMenu : (void *)MsgPort;
}
static widget sockFirstW(msgport MsgPort) {
    return MsgPort ? MsgPort->FirstW : (void *)MsgPort;
}
static group sockFirstGroup(msgport MsgPort) {
    return MsgPort ? MsgPort->FirstGroup : (void *)MsgPort;
}
static mutex sockFirstMutex(msgport MsgPort) {
    return MsgPort ? MsgPort->FirstMutex : (void *)MsgPort;
}
static menuitem sockFirstMenuItem(menu Menu) {
    return Menu ? Menu->FirstI : (void *)Menu;
}
static gadget sockFirstGadget(group Group) {
    return Group ? Group->FirstG : (void *)Group;
}

static all sockGetAll(void) {
    return All;
}





#ifdef CONF_EXT

#define _obj .TWS_field_obj
#define _any .TWS_field_scalar
#define _vec .TWS_field_vecV
#define _len .TWS_field_vecL

#define _type .type

#define void_ TWS_void
#define obj_  TWS_obj
#define vec_  TWS_vec
#define vecW_ TWS_vecW


static byte sockDecodeExtension(topaque *Len, CONST byte **Data, topaque *Args_n, tsfield a) {
    static byte type_warned = 0;
    topaque n = 0;
    ldat fail = 1;
    tany len, left = *Len;
    CONST byte *data = *Data;
    tany args_n = *Args_n;
    udat t;

# ifdef CONF_SOCKET_ALIEN
    if (AlienXendian(Slot) != MagicNative)
	return alienDecodeExtension(Len, Data, Args_n, a);
# endif

    while (fail > 0 && n < args_n) {
	switch ((t = a[n]_type)) {

# define CASE_(type) \
case CAT(TWS_,type): \
    /* ensure type size WAS negotiated */ \
    if ((len = AlienSizeof(type, Slot)) && left >= len) { \
	type an; \
	\
	left -= len; \
	Pop(data,type,an); \
	a[n]_any = (tany)an; \
    } else \
	fail = -fail; \
    break

	  case TWS_hwcol:
	    /*FALLTHROUGH*/
	    CASE_(byte);
	    CASE_(dat);
	    CASE_(ldat);
	    CASE_(topaque);
	    CASE_(tany);
	    CASE_(hwfont);
	    CASE_(hwattr);
#undef CASE_
	    
	  case TWS_vec|TWS_vecW|TWS_byte:
	    /* ensure (topaque) size WAS negotiated */
	    if ((len = AlienSizeof(topaque, Slot)) && left >= len) {
		topaque nlen;
		
		left -= len;
		Pop(data,topaque,nlen);
		a[n]_len = nlen;
		
		if (nlen <= left) {
		    void *addr;
		    left -= nlen;
		    PopAddr(data,byte,nlen,addr);
		    a[n]_vec = addr;
		    break;
		}
	    }
	    fail = -fail;
	    break;
	  default:
	    if (type_warned < 5) {
		type_warned = 5;
		printk("twin: sockDecodeExtension(): got a call with unknown type 0x%02X' !\n",
		       (int)t);
	    }
	    fail = -fail;
	    break;
	}
	
	if (fail <= 0)
	    break;
	
	fail++;
	n++;
    }
    
    if (fail > 0) {
	*Len -= data - *Data;
	*Data = data;
	*Args_n = n;
    }
    
    return fail > 0;
}

static extension sockOpenExtension(byte namelen, CONST byte *name) {
    /*
     * FIXME: loading an extension from a Slot without msgport
     * results in the extension stay loaded at least until someone uses it
     */
    return Do(Query,Extension)(namelen, name);
}
static void sockCloseExtension(extension e) {
    msgport Owner;
    
    if ((Owner = RemoteGetMsgPort(Slot)) && e && IS_EXTENSION(e))
	Act(UnuseExtension,Owner)(Owner, e);
}


static tany sockCallBExtension(extension e, topaque len, CONST byte *data, CONST byte *return_type) {
    msgport M;
    
    if (e && IS_EXTENSION(e)) {
	/* ensure we are registered as using the extension */
	if ((M = RemoteGetMsgPort(Slot)))
	    Act(UseExtension,M)(M, e);

	/* actually, we receive a (tsfield) instead of return_type and we pass it through */
	return e->CallB(e, len, data, (void *)return_type);
    }
    return (tany)0;
}


#else /* !CONF_EXT */

static byte sockDecodeExtension(topaque *len, CONST byte **data, topaque *args_n, tsfield args) {
    return FALSE;
}
static extension sockOpenExtension(byte namelen, CONST byte *name) {
    return (extension)0;
}
static void sockCloseExtension(extension e) {
}
static tany sockCallBExtension(extension e, topaque len, CONST byte *data, CONST byte *return_type) {
    return (tany)0;
}

#endif /* CONF_EXT */





/*
 * turn the (msg) into a (tmsg) and write it on the MsgPort file descriptor.
 * 
 * this can be called in nasty places like detaching non-exclusive displays
 * when an exclusive one is started. Must preserve Slot, Fd and other globals!
 */
static void sockSendMsg(msgport MsgPort, msg Msg) {
    uldat Len, Tot;
    byte *t, Easy;
    uldat save_Slot;
    int save_Fd;

#ifdef CONF_SOCKET_ALIEN
    if (AlienXendian(Slot) != MagicNative) {
	alienSendMsg(MsgPort, Msg);
	return;
    }
#endif
    
    Len = 0;
    save_Slot = Slot;
    save_Fd = Fd;
    
    Easy = sizeof(twindow) == sizeof(window);
    RequestN = MSG_MAGIC;
    Fd = MsgPort->RemoteData.Fd;
    Slot = MsgPort->RemoteData.FdSlot;

    switch (Msg->Type) {
#if defined(CONF__MODULES) || defined(CONF_HW_DISPLAY)
      case MSG_DISPLAY:
	Easy = FALSE;
	sockReply(Msg->Type, Len = sizeof(twindow) + 4*sizeof(udat) + Msg->Event.EventDisplay.Len, NULL);

	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    
	    Push(t, twindow, NOID); /* not used here */
	    if (sizeof(event_display) == sizeof(window) + 4*sizeof(dat) + sizeof(byte *)) {
		PushV(t, 4*sizeof(dat), &Msg->Event.EventDisplay.Code);
	    } else {
		Push(t, udat,    Msg->Event.EventDisplay.Code);
		Push(t, udat,    Msg->Event.EventDisplay.Len);
		Push(t, udat,    Msg->Event.EventDisplay.X);
		Push(t, udat,    Msg->Event.EventDisplay.Y);
	    }
	    PushV(t, Msg->Event.EventDisplay.Len, Msg->Event.EventDisplay.Data);
	}
	break;
#endif /* defined(CONF__MODULES) || defined (CONF_HW_DISPLAY) */
      case MSG_WIDGET_KEY:
	if (Easy && sizeof(event_keyboard) == sizeof(window) + 3*sizeof(dat) + 2*sizeof(byte))
	    break;
	else
	    Easy = FALSE;
	sockReply(Msg->Type, Len = sizeof(twindow) + 3*sizeof(udat) + 2*sizeof(byte) + Msg->Event.EventKeyboard.SeqLen, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    Push(t, twidget, Obj2Id(Msg->Event.EventKeyboard.W));
	    Push(t, udat,    Msg->Event.EventKeyboard.Code);
	    Push(t, udat,    Msg->Event.EventKeyboard.ShiftFlags);
	    Push(t, udat,    Msg->Event.EventKeyboard.SeqLen);
	    Push(t, byte,    Msg->Event.EventKeyboard.pad);
	    PushV(t, Msg->Event.EventKeyboard.SeqLen + 1, /* the final '\0' */
		  Msg->Event.EventKeyboard.AsciiSeq);
	}
	break;
      case MSG_WIDGET_MOUSE:
	if (Easy && sizeof(event_mouse) == sizeof(window) + 4*sizeof(dat))
	    break;
	else
	    Easy = FALSE;
	sockReply(Msg->Type, Len = sizeof(twindow) + 4*sizeof(udat), NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    Push(t, twidget, Obj2Id(Msg->Event.EventMouse.W));
	    Push(t, udat,    Msg->Event.EventMouse.Code);
	    Push(t, udat,    Msg->Event.EventMouse.ShiftFlags);
	    Push(t, dat,     Msg->Event.EventMouse.X);
	    Push(t, dat,     Msg->Event.EventMouse.Y);
	}
	break;
      case MSG_WIDGET_CHANGE:
	if (Easy && sizeof(event_widget) == sizeof(widget) + 6*sizeof(dat))
	    break;
	else
	    Easy = FALSE;
	sockReply(Msg->Type, Len = sizeof(twindow) + 6*sizeof(dat), NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    Push(t, twidget, Obj2Id(Msg->Event.EventWidget.W));
	    Push(t, udat,    Msg->Event.EventWidget.Code);
	    Push(t, udat,    Msg->Event.EventWidget.Flags);
	    Push(t, udat,    Msg->Event.EventWidget.XWidth);
	    Push(t, udat,    Msg->Event.EventWidget.YWidth);
	    Push(t, udat,    Msg->Event.EventWidget.X);
	    Push(t, udat,    Msg->Event.EventWidget.Y);
	}
	break;
      case MSG_WIDGET_GADGET:
	if (Easy && sizeof(event_gadget) == sizeof(window) + 2*sizeof(dat))
	    break;
	else
	    Easy = FALSE;
	sockReply(Msg->Type, Len = sizeof(twindow) + 2*sizeof(dat), NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    Push(t, twidget, Obj2Id(Msg->Event.EventGadget.W));
	    Push(t, udat,    Msg->Event.EventGadget.Code);
	    Push(t, udat,    Msg->Event.EventGadget.Flags);
	}
	break;
      case MSG_MENU_ROW:
	if (Easy && sizeof(event_menu) == sizeof(window) + 2*sizeof(udat) + sizeof(menu) + sizeof(row))
	    break;
	else
	    Easy = FALSE;
	sockReply(Msg->Type, Len = sizeof(twindow) + 2*sizeof(dat) + sizeof(tmenu) + sizeof(row), NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    Push(t, twindow, Obj2Id(Msg->Event.EventMenu.W));
	    Push(t, udat,    Msg->Event.EventMenu.Code);
	    Push(t, udat,    Msg->Event.EventMenu.pad);
	    Push(t, tmenu,   Obj2Id(Msg->Event.EventMenu.Menu));
	    Push(t, trow,    Obj2Id(Msg->Event.EventMenu.Row));
	}
	break;
      case MSG_SELECTION:
	if (Easy && sizeof(event_selection) == sizeof(window) + 4*sizeof(dat))
	    break;
	else
	    Easy = FALSE;
	sockReply(Msg->Type, Len = sizeof(twindow) + 4*sizeof(dat), NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    Push(t, twidget, Obj2Id(Msg->Event.EventSelection.W));
	    Push(t, udat,    Msg->Event.EventSelection.Code);
	    Push(t, udat,    Msg->Event.EventSelection.pad);
	    Push(t, dat,     Msg->Event.EventSelection.X);
	    Push(t, dat,     Msg->Event.EventSelection.Y);
	}
	break;
      case MSG_SELECTIONNOTIFY:
	if (Easy && sizeof(event_selectionnotify) == sizeof(window) + 2*sizeof(dat) +
	    3*sizeof(ldat) + MAX_MIMELEN + sizeof(uldat))
	    break;
	else
	    Easy = FALSE;
	sockReply(Msg->Type, Len = sizeof(twindow) + 2*sizeof(dat) + 3*sizeof(ldat)
	      + MAX_MIMELEN + Msg->Event.EventSelectionNotify.Len, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    Push(t, twidget, Obj2Id(Msg->Event.EventSelectionNotify.W));
	    Push(t, udat,    Msg->Event.EventSelectionNotify.Code);
	    Push(t, udat,    Msg->Event.EventSelectionNotify.pad);
	    Push(t, uldat,   Msg->Event.EventSelectionNotify.ReqPrivate);
	    Push(t, uldat,   Msg->Event.EventSelectionNotify.Magic);
	    PushV(t, MAX_MIMELEN, Msg->Event.EventSelectionNotify.MIME);
	    Push(t, uldat,   Msg->Event.EventSelectionNotify.Len);
	    PushV(t, Msg->Event.EventSelectionNotify.Len, Msg->Event.EventSelectionNotify.Data);
	}
	break;
      case MSG_SELECTIONREQUEST:
	if (Easy && sizeof(event_selectionrequest) == sizeof(window) + 2*sizeof(dat) + sizeof(obj) + sizeof(ldat))
	    break;
	else
	    Easy = FALSE;
	sockReply(Msg->Type, Len = sizeof(twindow) + 2*sizeof(dat) + sizeof(tobj) + sizeof(ldat), NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    Push(t, twidget, Obj2Id(Msg->Event.EventSelectionRequest.W));
	    Push(t, udat,    Msg->Event.EventSelectionRequest.Code);
	    Push(t, udat,    Msg->Event.EventSelectionRequest.pad);
	    Push(t, tobj,    Obj2Id(Msg->Event.EventSelectionRequest.Requestor));
	    Push(t, uldat,   Msg->Event.EventSelectionRequest.ReqPrivate);
	}
	break;
	
      case MSG_USER_CONTROL:
	if (Easy && sizeof(event_control) == sizeof(window) + 4*sizeof(dat) + sizeof(uldat))
	    break;
	else
	    Easy = FALSE;
	sockReply(Msg->Type, Len = sizeof(twindow) + 4*sizeof(dat) +
		  Msg->Event.EventControl.Len, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    Push(t, twidget, Obj2Id(Msg->Event.EventControl.W));
	    Push(t, udat,    Msg->Event.EventControl.Code);
	    Push(t, udat,    Msg->Event.EventControl.Len);
	    Push(t, dat,     Msg->Event.EventControl.X);
	    Push(t, dat,     Msg->Event.EventControl.Y);
	    PushV(t, Msg->Event.EventControl.Len, Msg->Event.EventControl.Data);
	}
	break;
	
      case MSG_USER_CLIENTMSG:
	if (Easy && sizeof(event_clientmsg) == sizeof(window) + 2*sizeof(dat) + 2*sizeof(uldat))
	    break;
	else
	    Easy = FALSE;
	sockReply(Msg->Type, Len = sizeof(twindow) + 2*sizeof(dat) +
		  Msg->Event.EventClientMsg.Len, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    Push(t, twidget, Obj2Id(Msg->Event.EventClientMsg.W));
	    Push(t, udat,    Msg->Event.EventClientMsg.Code);
	    Push(t, udat,    Msg->Event.EventClientMsg.Format);
	    Push(t, uldat,   Msg->Event.EventClientMsg.Len);
	    PushV(t, Msg->Event.EventClientMsg.Len, Msg->Event.EventClientMsg.Data.b);
	}
	break;
	
      default:
	Easy = FALSE;
    }
#if TW_SIZEOFTOPAQUE == TW_SIZEOFULDAT
    if (Easy) {
	Msg->Event.EventCommon.W = (void *)Obj2Id(Msg->Event.EventCommon.W);
	if (Msg->Type == MSG_MENU_ROW) {
	    Msg->Event.EventMenu.Menu = (void *)Obj2Id(Msg->Event.EventMenu.Menu);
	    Msg->Event.EventMenu.Row  = (void *)Obj2Id(Msg->Event.EventMenu.Row);
	} else if (Msg->Type == MSG_SELECTIONREQUEST)
	    Msg->Event.EventSelectionRequest.Requestor
	    = (void *)Obj2Id(Msg->Event.EventSelectionRequest.Requestor);
	sockReply(Msg->Type, Msg->Len, &Msg->Event);
    }
#endif
    Slot = save_Slot;
    Fd = save_Fd;
}





#define tmsgEventDelta ((udat)(size_t)&(((tmsg)0)->Event))
#define tmsgEventOffset(x) ((udat)(size_t)&(((tmsg)0)->Event.x))

/* extract the (tmsg) from Data, turn it into a (msg) and send it to MsgPort */
static byte sockSendToMsgPort(msgport MsgPort, udat Len, CONST byte *Data) {
    tmsg tMsg;
    msgport Sender;
    msg Msg;
    uldat dstSlot;
    udat _Len, minType;
    byte ok = TRUE;

    /* FIXME: must code alienSendToMsgPort() and call it if AlienMagic(Slot) != MagicNative */
    
    /* be careful with alignment! */
#if TW_CAN_UNALIGNED != 0
    tMsg = (tmsg)Data;
#else
    tMsg = (tmsg)CloneMem(Data, Len);
#endif /* TW_CAN_UNALIGNED != 0 */
    
    do if (MsgPort && Len && tMsg && Len >= tmsgEventDelta && Len == tMsg->Len &&
	   tMsg->Magic == msg_magic) {

	Sender = RemoteGetMsgPort(Slot);
	if (Sender && Sender->AttachHW && MsgPort->Handler != SocketH)
	    minType = TW_MSG_DISPLAY;
	else
	    minType = TW_MSG_USER_FIRST;
	if (tMsg->Type < minType)
	    /* not allowed! */
	    break;
	    
	_Len = 0;
	switch (tMsg->Type) {
	  case TW_MSG_WIDGET_KEY:
	    if (Len >= tmsgEventOffset(EventKeyboard.AsciiSeq)) {
		if (tMsg->Event.EventKeyboard.SeqLen + tmsgEventOffset(EventKeyboard.AsciiSeq) > Len)
		    tMsg->Event.EventKeyboard.SeqLen = Len - tmsgEventOffset(EventKeyboard.AsciiSeq);
		_Len += tMsg->Event.EventKeyboard.SeqLen;
	    } else
		/* (tmsg) too short */
		ok = FALSE;
	    break;
	  case TW_MSG_USER_CONTROL:
	    if (Len >= tmsgEventOffset(EventControl.Data)) {
		if (tMsg->Event.EventControl.Len + tmsgEventOffset(EventControl.Data) > Len)
		    tMsg->Event.EventControl.Len = Len - tmsgEventOffset(EventControl.Data);
		_Len += tMsg->Event.EventControl.Len;
	    } else
		/* (tmsg) too short */
		ok = FALSE;
	    break;
	  case TW_MSG_USER_CLIENTMSG:
	    if (Len >= tmsgEventOffset(EventClientMsg.Data)) {
		if (tMsg->Event.EventClientMsg.Len + tmsgEventOffset(EventClientMsg.Data) > Len)
		    tMsg->Event.EventClientMsg.Len = Len - tmsgEventOffset(EventClientMsg.Data);
		_Len += tMsg->Event.EventClientMsg.Len;
	    } else
		/* (tmsg) too short */
		ok = FALSE;
	    break;
	  default:
	    break;
	}

	if (!ok)
	    break;
	
	if (MsgPort->Handler == SocketH &&
	    MsgPort->RemoteData.Fd != NOFD &&
	    (dstSlot = MsgPort->RemoteData.FdSlot) != NOSLOT &&
	    !CmpMem(FdList[dstSlot].AlienMagic, LS.AlienMagic, TWS_highest-1)) {
	    /*
	     * shortcut: we have a (tmsg); instead of turning it into a (msg)
	     * then have SocketH() call sockSendMsg() to turn it back into a (tmsg),
	     * we directly copy it --- only one condition must be met:
	     * both clients must have the same AlienMagic()
	     * (i.e. endianity and data sizes).
	     */
	    tMsg->Len -= AlienSizeof(uldat,Slot);
	    tMsg->Magic = MSG_MAGIC;
	    RemoteWriteQueue(dstSlot, Len, (CONST byte *)tMsg);
	    break;
	}
	
	if ((Msg = Do(Create,Msg)(FnMsg, tMsg->Type, _Len))) {
		
	    Msg->Event.EventCommon.W = (void *)Id2Obj(widget_magic_id, tMsg->Event.EventCommon.W);

	    switch (tMsg->Type) {
#if defined(CONF__MODULES) || defined (CONF_HW_DISPLAY)
	      case TW_MSG_DISPLAY:
		if (sizeof(struct s_event_display)  == sizeof(window)  + 4*sizeof(dat) + sizeof(byte *) &&
		    sizeof(struct s_tevent_display) == sizeof(twindow) + 4*sizeof(dat) + sizeof(uldat)) {
		    
		    CopyMem((void *)&tMsg->Event.EventDisplay.Code,
			    (void *)& Msg->Event.EventDisplay.Code, 4*sizeof(dat));
		} else {
		    Msg->Event.EventDisplay.Code   = tMsg->Event.EventDisplay.Code;
		    Msg->Event.EventDisplay.Len    = tMsg->Event.EventDisplay.Len;
		    Msg->Event.EventDisplay.X      = tMsg->Event.EventDisplay.X;
		    Msg->Event.EventDisplay.Y      = tMsg->Event.EventDisplay.Y;
		}
		if (!(Msg->Event.EventDisplay.Data =
		      CloneMem(tMsg->Event.EventDisplay.Data, tMsg->Event.EventDisplay.Len))
		    && tMsg->Event.EventDisplay.Len)
		    
		    ok = FALSE;
		      
		break;
#endif /* defined(CONF__MODULES) || defined (CONF_HW_DISPLAY) */
	      case TW_MSG_WIDGET_KEY:
		if (sizeof(struct s_event_keyboard)  == sizeof(window)  + 3*sizeof(dat) + 2*sizeof(byte) &&
		    sizeof(struct s_tevent_keyboard) == sizeof(twindow) + 3*sizeof(dat) + 2*sizeof(byte)) {
		    
		    CopyMem((void *)&tMsg->Event.EventKeyboard.Code,
			    (void *)& Msg->Event.EventKeyboard.Code, 3*sizeof(dat) + sizeof(byte));
		} else {
		    Msg->Event.EventKeyboard.Code       = tMsg->Event.EventKeyboard.Code;
		    Msg->Event.EventKeyboard.ShiftFlags = tMsg->Event.EventKeyboard.ShiftFlags;
		    Msg->Event.EventKeyboard.SeqLen     = tMsg->Event.EventKeyboard.SeqLen;
		    Msg->Event.EventKeyboard.pad        = tMsg->Event.EventKeyboard.pad;
		}
		CopyMem(tMsg->Event.EventKeyboard.AsciiSeq, Msg->Event.EventKeyboard.AsciiSeq,
			tMsg->Event.EventKeyboard.SeqLen);
		
		Msg->Event.EventKeyboard.AsciiSeq[tMsg->Event.EventKeyboard.SeqLen] = '\0';
		
		break;
	      case TW_MSG_WIDGET_MOUSE:
		if (sizeof(struct s_event_mouse)  == sizeof(window)  + 4*sizeof(dat) &&
		    sizeof(struct s_tevent_mouse) == sizeof(twindow) + 4*sizeof(dat)) {
		    
		    CopyMem((void *)&tMsg->Event.EventMouse.Code,
			    (void *)& Msg->Event.EventMouse.Code, 4*sizeof(dat));
		} else {
		    Msg->Event.EventMouse.Code          = tMsg->Event.EventMouse.Code;
		    Msg->Event.EventMouse.ShiftFlags    = tMsg->Event.EventMouse.ShiftFlags;
		    Msg->Event.EventMouse.X             = tMsg->Event.EventMouse.X;
		    Msg->Event.EventMouse.Y             = tMsg->Event.EventMouse.Y;
		}
		break;
	      case TW_MSG_SELECTIONCLEAR:
		if (sizeof(struct s_event_common)  == sizeof(window)  + 2*sizeof(dat) &&
		    sizeof(struct s_tevent_common) == sizeof(twindow) + 2*sizeof(dat)) {
		    
		    CopyMem((void *)&tMsg->Event.EventCommon.Code,
			    (void *)& Msg->Event.EventCommon.Code, 2*sizeof(dat));
		} else {
		    Msg->Event.EventCommon.Code         = tMsg->Event.EventCommon.Code;
		    Msg->Event.EventCommon.pad          = tMsg->Event.EventCommon.pad;
		}
		break;

/* TODO: is this needed? */
#if 0
	      case TW_MSG_WIDGET_CHANGE:
		Push(t, udat,    Msg->Event.EventWidget.Code);
		Push(t, udat,    Msg->Event.EventWidget.XWidth);
		Push(t, udat,    Msg->Event.EventWidget.YWidth);
		break;
	      case TW_MSG_WIDGET_GADGET:
		Push(t, udat,    Msg->Event.EventGadget.Code);
		Push(t, udat,    Msg->Event.EventGadget.pad);
		break;
	      case TW_MSG_MENU_ROW:
		Push(t, udat,    Msg->Event.EventMenu.Code);
		Push(t, udat,    Msg->Event.EventMenu.pad);
		Push(t, tmenu,   Obj2Id(Msg->Event.EventMenu.Menu));
		Push(t, trow,    Obj2Id(Msg->Event.EventMenu.Row));
		break;
#endif
	      case TW_MSG_USER_CONTROL:
		if (sizeof(struct s_event_control)  == sizeof(window)  + 4*sizeof(dat) + sizeof(uldat) &&
		    sizeof(struct s_tevent_control) == sizeof(twindow) + 4*sizeof(dat) + sizeof(uldat)) {
		    
		    CopyMem((void *)&tMsg->Event.EventControl.Code,
			    (void *)& Msg->Event.EventControl.Code, 4*sizeof(dat));
		} else {
		    Msg->Event.EventControl.Code  = tMsg->Event.EventControl.Code;
		    Msg->Event.EventControl.Len   = tMsg->Event.EventControl.Len;
		    Msg->Event.EventControl.X     = tMsg->Event.EventControl.X;
		    Msg->Event.EventControl.Y     = tMsg->Event.EventControl.Y;
		}
		CopyMem(tMsg->Event.EventControl.Data, Msg->Event.EventControl.Data,
			tMsg->Event.EventControl.Len);
		
		Msg->Event.EventControl.Data[Msg->Event.EventControl.Len] = '\0';
		break;
	      case TW_MSG_USER_CLIENTMSG:
		if (sizeof(struct s_event_clientmsg)  == sizeof(window)  + 2*sizeof(dat) + 2*sizeof(uldat) &&
		    sizeof(struct s_tevent_clientmsg) == sizeof(twindow) + 2*sizeof(dat) + 2*sizeof(uldat)) {
		    
		    CopyMem((void *)&tMsg->Event.EventClientMsg.Code,
			    (void *)& Msg->Event.EventClientMsg.Code, 2*sizeof(dat) + sizeof(uldat));
		} else {
		    Msg->Event.EventClientMsg.Code   = tMsg->Event.EventClientMsg.Code;
		    Msg->Event.EventClientMsg.Format = tMsg->Event.EventClientMsg.Format;
		    Msg->Event.EventClientMsg.Len    = tMsg->Event.EventClientMsg.Len;
		}
#ifdef CONF_SOCKET_ALIEN
		/* FIXME: this must be replaced with a call to alienSendToMsgPort() above */
		if (AlienXendian(Slot) == MagicAlienXendian) {
		    tevent_clientmsg tC = &tMsg->Event.EventClientMsg;
		    event_clientmsg *C = &Msg->Event.EventClientMsg;
		    
		    alienReadVec(tC->Data.b, C->Data.b, tC->Len, tC->Format, tC->Format, TRUE);
		} else
#endif
		    CopyMem(tMsg->Event.EventClientMsg.Data.b, Msg->Event.EventClientMsg.Data.b,
			    tMsg->Event.EventClientMsg.Len);
		break;
	      default:
		ok = FALSE;
		break;
	    }
	    
	    if (ok)
		SendMsg(MsgPort, Msg);
	    else
		Delete(Msg);
	}
    } while (0);
    
#if TW_CAN_UNALIGNED == 0
    FreeMem(tMsg);
#endif
    return ok;
}

#undef tmsgEventDelta
#undef tmsgEventOffset

static void sockBlindSendToMsgPort(msgport MsgPort, udat Len, CONST byte *Data) {
    (void)sockSendToMsgPort(MsgPort, Len, Data);
}

static obj sockGetOwnerSelection(void) {
    return TwinSelectionGetOwner();
}

static void sockSetOwnerSelection(tany Time, tany Frac) {
    if (LS.MsgPort)
	TwinSelectionSetOwner((obj)LS.MsgPort, Time, Frac);
}

static void sockNotifySelection(obj Requestor, uldat ReqPrivate, uldat Magic, CONST byte MIME[MAX_MIMELEN],
			    uldat Len, CONST byte *Data) {
    TwinSelectionNotify(Requestor, ReqPrivate, Magic, MIME, Len, Data);
}

static void sockRequestSelection(obj Owner, uldat ReqPrivate) {
    if (LS.MsgPort)
	TwinSelectionRequest((obj)LS.MsgPort, ReqPrivate, Owner);
}


#ifdef CONF_SOCKET_GZ

static byte sockCanCompress(void) {
    return TRUE;
}

static voidpf sockZAlloc(voidpf opaque, uInt items, uInt size) {
    void *ret = AllocMem(items * (size_t)size);
    return ret ? (voidpf)ret : Z_NULL;
}

static void sockZFree(voidpf opaque, voidpf address) {
    if (address != Z_NULL)
	FreeMem((void *)address);
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
	    (z1 = AllocMem(sizeof(*z1))) &&
	    (z2 = AllocMem(sizeof(*z2)))) {
	    
	    z1->zalloc = z2->zalloc = sockZAlloc;
	    z1->zfree  = z2->zfree  = sockZFree;
	    z1->opaque = z2->opaque = NULL;

	    if (deflateInit(z1, Z_BEST_COMPRESSION) == Z_OK) {
		if (inflateInit(z2) == Z_OK) {
		
		    /* ok, start pairing the two slots */
		    ls.pairSlot = Slot;
		    ls.PrivateData = (void *)z1;
		    
		    LS.pairSlot = slot;
		    LS.PrivateData = (void *)z2;
		    LS.PrivateAfterFlush = FixupGzip;
		    
		    /* we have ls.Fd == specFD. it will be fixed by LS.PrivateAfterFlush() */
		    
		    return TRUE;
		}
		deflateEnd(z1);
	    }
	}
	if (z2) FreeMem(z2);
	if (z1) FreeMem(z1);
	if (slot != NOSLOT) UnRegisterRemote(slot);
	return FALSE;
    } else {
	/* inform RemoteFlush() we are shutting down the compression... */
	LS.PrivateAfterFlush = ShutdownGzip;
	
	return TRUE;
    }
}

#else

static byte sockCanCompress(void) {
    return FALSE;
}

static byte sockDoCompress(byte on_off) {
    return FALSE;
}

#endif /* CONF_SOCKET_GZ */




/* socket initialization functions */


static uldat SendTwinProtocol(void) {
    static byte buf[] = " Twin-" TW_PROTOCOL_VERSION_STR "-" TWIN_VERSION_STR "\n";
    buf[0] = strlen(buf);
    return RemoteWriteQueue(Slot, buf[0], buf);
}

static byte SendTwinMagic(byte *t, byte len) {
    return RemoteWriteQueue(Slot, len, t) == len;
}
    
static byte sockReply(uldat code, uldat len, CONST void *data) {
    uldat buf[3];
    buf[0] = 2*sizeof(uldat) + len;
    buf[1] = RequestN;
    buf[2] = code;
    if (RemoteWriteQueue(Slot, 3*sizeof(uldat), buf) == 3*sizeof(uldat) && len)
	return RemoteWriteQueue(Slot, len, data) == len;
    return FALSE;
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
	    alienPUSH(t,uldat,data);
	    return TRUE;
	}
	return FALSE;
    }
#else /* !CONF_SOCKET_ALIEN */
    
    return RemoteWriteQueue(Slot, sizeof(uldat), &data) == sizeof(uldat);
    
#endif /* CONF_SOCKET_ALIEN */
    
}


#define digestLen	16  /* hardcoded in MD5 routines */
#define AuthLen		256 /* length of ~/.TwinAuth */
#define ChallLen	256 /* length of random data */
#define TotalLen	(AuthLen+ChallLen)

static byte AuthData[TotalLen];

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
	    AuthData[len++] = res & MAXBYTE;
	    res >>= 8;
	    got--;
	}
    }
    return len;
}

static byte CreateAuth(byte *path) {
    int fd, got = -1;
    uldat len = 0;
    
    if ((fd = open(path, O_WRONLY|O_CREAT|O_TRUNC, 0600)) >= 0 &&
	chmod(path, 0600) == 0) {
	
        len = GetRandomData();
	
	if (len == AuthLen) for (len = 0; len < AuthLen; len += got) {
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
    
    return len == AuthLen ? TRUE : Error(SYSCALLERROR);
}

static byte sockInitAuth(void) {
    int fd, got;
    uldat len;
    
    if (!HOME)
	return FALSE;
    
    len = LenStr(HOME);
    len = Min2(len, TotalLen-11);
    CopyMem(HOME, AuthData, len);
    CopyMem("/.TwinAuth", AuthData+len, 11);
    
    if ((fd = open(AuthData, O_RDONLY)) < 0)
	return CreateAuth(AuthData);

    for (len = 0; len < AuthLen; len += got) {
	got = read(fd, AuthData + len, AuthLen - len);
	if (got < 0) {
	    if (errno == EINTR || errno == EWOULDBLOCK)
		got = 0;
	    else {
		close(fd);
		return Error(SYSCALLERROR);
	    }
	}
    }
    close(fd);
    if (len < AuthLen)
    	return CreateAuth(AuthData);

    return TRUE;
}

static byte SendChallenge(void) {
    struct MD5Context ctx;
    int fd, got;
    uldat len, queue;
    byte *t;
    
    if ((fd = open("/dev/urandom", O_RDONLY)) < 0)
	return FALSE;
    len = AuthLen;
    for (got = 1; len < TotalLen && got; len += got) {
	do {
	    got = read(fd, AuthData + len, TotalLen - len);
	} while (got < 0 && errno == EINTR);
	if (got <= 0)
	    break;
    }
    close(fd);
    if (got > 0 && len == TotalLen && (len -= AuthLen) &&
	RemoteReadGrowQueue(Slot, digestLen) &&
	/* make space for digest, and also check there's no pending data */
	(t = RemoteReadGetQueue(Slot, &queue)) && queue == digestLen && SendUldat(len) &&
	RemoteWriteQueue(Slot, len, AuthData+AuthLen)) {
	
	MD5Init(&ctx);
	MD5Update(&ctx, AuthData, TotalLen);
	MD5Final(t, &ctx); /* write digest into t */
	
	return TRUE;
    }
    return FALSE;
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

static handler_io_s GetHandlerIO(void) {
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
    
    got = EnsureRead(fd, Slot = slot, digestLen*2);
    
    if (got < 0)
	;
    else if (got < digestLen*2)
	/* not yet ready to check */
	return;
    else { /* (got >= digestLen*2) */
	t = RemoteReadGetQueue(Slot, NULL);
	if (!CmpMem(t, t+digestLen, digestLen)) {
	    /* OK! */
	    if ((LS.HandlerIO.S = GetHandlerIO())) {
		RemoteReadDeQueue(Slot, digestLen*2);
		SendUldat(GO_MAGIC);
		return;
	    }
	}
    }
    
    /* I/O error or Auth error */
    SendUldat(STOP_MAGIC);
    RemoteFlush(Slot);
    UnRegisterRemote(Slot);
    close(fd);
}

static byte Check4MagicTranslation(uldat slot, byte *magic, byte len) {
#ifdef CONF_SOCKET_ALIEN
    static byte warn_count = 0;
#endif
    
    byte *zero = memchr(magic, '\0', len);
    byte len1 = zero ? (byte)(zero - magic): 0;
    
    if (len1 > TWS_hwcol && len == magic[0] && len == len1+1 + sizeof(uldat) &&
	/*check negotiated size to match ours*/
	!CmpMem(magic+1, TwinMagicData+1, Min2(len1, TWS_highest)-1) &&
	/*pre-0.3.9 compatibility: if hwattr is not negotiated, assume 2 bytes*/
	(len1 > TWS_hwattr || sizeof(hwattr) == 2) &&
	/*check endianity*/
	!CmpMem(magic+len1+1, TwinMagicData+TWS_highest+1, sizeof(uldat))) {
	
	/* store client magic numbers */
	CopyMem(magic, AlienMagic(slot), Min2(len1, TWS_highest));
	if (len1 < TWS_highest)
	    /* version mismatch compatibility: zero out unnegotiated sizes */
	    WriteMem(AlienMagic(slot) + len1, '\0', TWS_highest-len1);
	
	return MagicNative;
    }
    
#ifdef CONF_SOCKET_ALIEN

    if (len1 > TWS_hwattr && len == magic[0] && 
	magic[TWS_byte] == 1 &&  /* sizeof(byte) MUST be 1, or passing byte[] vectors would fail */
	magic[TWS_udat] >= 2 &&
	magic[TWS_uldat] >= 4 &&
	len - len1 == magic[TWS_uldat] + 1 &&
	magic[TWS_hwcol] == 1 && /* sizeof(hwcol) MUST be 1, or passing hwcol[] vectors would fail */
	magic[TWS_topaque] >= 4 &&
	magic[TWS_tany] >= 4 &&
	magic[TWS_hwfont] >= 1 &&
	magic[TWS_hwattr] >= 2) {
	    
	/* store client magic numbers */
	CopyMem(magic, AlienMagic(slot), Min2(len1, TWS_highest));
	if (len1 < TWS_highest)
	    /* version mismatch compatibility: zero out unnegotiated sizes */
	    WriteMem(AlienMagic(slot) + len1, '\0', TWS_highest-len1);

	if (warn_count < 6) {
	    zero = NULL;
	    if (AlienMagic(slot)[TWS_hwattr] != sizeof(hwattr))
		zero = "hwattr";
	    else if (AlienMagic(slot)[TWS_hwfont] != sizeof(hwfont))
		zero = "hwfont";
	    
	    if (zero) {
		if (warn_count == 5)
		    printk("twin: warning: many client with different sizes, suppressing further messages.\n");
		else
		    printk("twin: warning: client has different `%."STR(SMALLBUFF)"s' size, it may "
# ifdef CONF__UNICODE
			   "not "
# endif
			   "be Unicode aware.\n", zero);
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
	if (!CmpMem(magic+len1+1, "Twin", 4))
	    /* little endian client. and us? */
	    return TW_BYTE_ORDER == TW_LITTLE_ENDIAN ? MagicAlien : MagicAlienXendian;
	if (!CmpMem(magic+len1+1+(magic[TWS_uldat]-4), "niwT", 4))
	    /* big endian client. and us? */
	    return TW_BYTE_ORDER == TW_BIG_ENDIAN ? MagicAlien : MagicAlienXendian;
    }

#endif /* CONF_SOCKET_ALIEN */

    return MagicUnknown;
}

static void Wait4Magic(int fd, uldat slot, byte isUnix) {
    byte *t;
    uldat max;
    int got;
    
    t = RemoteReadGetQueue(Slot = slot, &max);
    if (max == 0)
	max = MAXBYTE;
    else
	max = t[0];

    got = EnsureRead(fd, Slot, max);

    t = RemoteReadGetQueue(Slot, NULL);
    if (got > 0)
	max = t[0];
    
    if (got < 0)
	;
    else if (got < max)
	/* not yet ready to check */
	return;
    else { /* (got >= max) */
	/*
	 * check whether the client has our same sizes and endianity
	 * or one of the available translations is needed.
	 */
	if ((AlienXendian(Slot) = Check4MagicTranslation(Slot, t, max)) == MagicUnknown) {
	    /*
	     * no suitable translation available. use our native magic,
	     * in case library can handle it.
	     */
	    CopyMem(TwinMagicData, AlienMagic(Slot), TWS_highest);
	    AlienXendian(Slot) = MagicNative;
	    got = SendTwinMagic(TwinMagicData, TwinMagicData[0]);
	} else {
	    /*
	     * we have a translation for client's magic.
	     * answer its same magic.
	     */
	    got = SendTwinMagic(t, max);
	}
	RemoteReadDeQueue(Slot, max);
	
	if (got) {
	    if (isUnix) {
		if ((LS.HandlerIO.S = GetHandlerIO()) && SendUldat(GO_MAGIC))
		    return;
	    } else {
		LS.HandlerIO.S = Wait4AuthIO;
		if (SendUldat(WAIT_MAGIC) && SendChallenge())
		    return;
	    }
	}
    }
    
    /* problem... out of memory or I/O error. */
    UnRegisterRemote(Slot);
    close(fd);
}

static void Wait4MagicUnixIO(int fd, uldat slot) {
    Wait4Magic(fd, slot, TRUE);
}

static void Wait4MagicInetIO(int fd, uldat slot) {
    Wait4Magic(fd, slot, FALSE);
}


static void unixSocketIO(int fd, uldat slot) {
    struct sockaddr_un un_addr;
    int len = sizeof(un_addr);
    if ((fd = accept(unixFd, (struct sockaddr *)&un_addr, &len)) >= 0) {
	/* programs on the unix socket are always authorized */
	if ((Slot = RegisterRemoteFd(fd, Wait4MagicUnixIO)) != NOSLOT) {
	    fcntl(fd, F_SETFL, O_NONBLOCK);
	    fcntl(fd, F_SETFD, FD_CLOEXEC);
	    if (SendTwinProtocol())
		return;
	    UnRegisterRemote(Slot);
	}
	close(fd);
    }
}

static void inetSocketIO(int fd, uldat slot) {
    struct sockaddr_in in_addr;
    int len = sizeof(in_addr);
    if ((fd = accept(inetFd, (struct sockaddr *)&in_addr, &len)) >= 0) {
	if ((Slot = RegisterRemoteFd(fd, Wait4MagicInetIO)) != NOSLOT) {
	    fcntl(fd, F_SETFL, O_NONBLOCK);
	    fcntl(fd, F_SETFD, FD_CLOEXEC);
	    if (SendTwinProtocol())
		return;       
	    UnRegisterRemote(Slot);	    
	}
	close(fd);
    }
}

static void sockKillSlot(uldat slot) {
    msgport MsgPort;
    display_hw D_HW;
    
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
		Delete(D_HW);
	    }

	    Delete(MsgPort); /* and all its children ! */
	}
	
	if (ls.Fd >= 0)
	    close(ls.Fd);
	UnRegisterRemote(slot);
    }
}


static void SocketIO(int fd, uldat slot) {
    uldat len, Funct;
    byte *t, *tend;
    int tot = 0;
#ifdef CONF_SOCKET_GZ
    uldat gzSlot;
#endif
    
    Fd = fd;
    Slot = slot;

    if (ioctl(Fd, FIONREAD, &tot) != 0 || tot == 0)	
	tot = SMALLBUFF;
    else if (tot > BIGBUFF*BIGBUFF)
	tot = BIGBUFF*BIGBUFF;
    
    if (!(t = RemoteReadGrowQueue(Slot, tot)))
	return;
    
    if ((len = read(Fd, t, tot)) && len && len != (uldat)-1) {
	if (len < tot)
	    RemoteReadShrinkQueue(Slot, tot - len);
	
	/* ok, now process the data */

#ifdef CONF_SOCKET_GZ
	if ((gzSlot = LS.pairSlot) != NOSLOT) {
	    /* hmmm, a compressed socket. */
	    if (RemoteGunzip(Slot))
		Slot = gzSlot;
	    else {
		Ext(Remote,KillSlot)(Slot);
		return;
	    }
	}
#endif
	
	s = t = RemoteReadGetQueue(Slot, &len);
	tend = s + len;
	
	while (s + 3*sizeof(uldat) <= tend) {
	    Pop(s,uldat,len);
	    if (len < 2*sizeof(uldat)) {
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
		    Slot = slot;	/*
					 * restore, in case sockF[Funct].F() changed it;
					 * without this, tw* clients can freeze
					 * if twdisplay is in use
					 */
		}
		else if (Funct == FIND_MAGIC)
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
	Ext(Remote,KillSlot)(Slot);
    }
}




static void SocketH(msgport MsgPort) {
    msg Msg;
    byte buf[10], len;
    widget W;
    
    while ((Msg=MsgPort->FirstMsg)) {
	Remove(Msg);

	if (Msg->Type==MSG_WIDGET_MOUSE && (W = Msg->Event.EventMouse.W) && IS_WINDOW(W) &&
	    (W->Flags & WINDOWFL_USECONTENTS) && ((window)W)->USE.C.TtyData &&
	    ((window)W)->USE.C.TtyData->Flags & (TTY_REPORTMOUSE|TTY_REPORTMOUSE2)) {
	    
	    len = CreateXTermMouseEvent(&Msg->Event.EventMouse, 10, buf);
	    /*
	     * SyntheticKey() will send an appropriate keyboard message to this MsgPort
	     * and we will sockSendMsg() it later in the while() loop.
	     */
	    if (len)
		SyntheticKey(Msg->Event.EventMouse.W, TW_XTermMouse, 0, len, buf);
	} else
	    sockSendMsg(MsgPort, Msg);
	
	Delete(Msg);
    }
}


#ifdef THIS_MODULE

static void (*save_unixSocketIO)(int fd, uldat slot);

byte InitModule(module Module)
#else
byte InitSocket(void)
#endif
{
    uldat m;
    struct sockaddr_in addr;
    char opt[15];

    if (!sockInitAuth()) {
	printk("twin: failed to create ~/.TwinAuth: %."STR(SMALLBUFF)"s\n", ErrStr);
	return FALSE;
    }
    
    WriteMem(&addr, 0, sizeof(addr));
	
    addr.sin_family = AF_INET;
    addr.sin_port = htons(TW_INET_PORT + strtoul(TWDisplay+1, NULL, 16));
	
    if ((inetFd = socket(AF_INET, SOCK_STREAM, 0)) >= 0 &&
	setsockopt(inetFd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt)) >= 0 &&
	bind(inetFd, (struct sockaddr *)&addr, sizeof(addr)) >= 0 &&
	listen(inetFd, 1) >= 0 &&
	fcntl(inetFd, F_SETFD, FD_CLOEXEC) >= 0) {
	
	if ((inetSlot = RegisterRemoteFd(inetFd, inetSocketIO)) != NOSLOT)
	    ;
	else
	    close(inetFd);
    } else {
	Error(SYSCALLERROR);
	if (inetFd >= 0)
	    close(inetFd);
    }
    
    if (unixSlot != NOSLOT || inetSlot != NOSLOT) {
	for (MaxFunct = 0; sockF[MaxFunct].Name; MaxFunct++) {
	    sockF[MaxFunct].Len = strlen(sockF[MaxFunct].Name);
	    sockF[MaxFunct].FormatLen = strlen(sockF[MaxFunct].Format);
	}
	
	RegisterExt(Remote,KillSlot,sockKillSlot);
	RegisterExt(Socket,SendMsg,sockSendMsg);
	RegisterExt(Socket,InitAuth,sockInitAuth);
	RegisterExt(Socket,DecodeExtension,sockDecodeExtension);
	RegisterExt(Socket,MultiplexS,sockMultiplexS);

	m = TWIN_MAGIC;
	CopyMem(&m, TwinMagicData+TwinMagicData[0]-sizeof(uldat), sizeof(uldat));

	if (unixSlot != NOSLOT) {
#ifdef THIS_MODULE
	    save_unixSocketIO = FdList[unixSlot].HandlerIO.S;
#endif
	    FdList[unixSlot].HandlerIO.S = unixSocketIO;
	}

	return TRUE;
    }
    printk("twin: failed to create sockets: %."STR(SMALLBUFF)"s\n", ErrStr);
    return FALSE;
}

#ifdef THIS_MODULE
void QuitModule(module Module) {
    if (unixSlot != NOSLOT)
	FdList[unixSlot].HandlerIO.S = save_unixSocketIO;
    
    if (inetSlot != NOSLOT) {
	UnRegisterRemote(inetSlot);
	close(inetFd);
    }
    for (Slot = 0; Slot < FdTop; Slot++) {
	if (LS.Fd != NOFD && (LS.HandlerIO.S == Wait4MagicUnixIO ||
			      LS.HandlerIO.S == Wait4MagicInetIO ||
			      LS.HandlerIO.S == Wait4AuthIO ||
# ifdef CONF_SOCKET_ALIEN
			      LS.HandlerIO.S == AlienIO ||
# endif
			      LS.HandlerIO.S == SocketIO)) {
	    Ext(Remote,KillSlot)(Slot);
	}
    } 
    UnRegisterExt(Remote,KillSlot,sockKillSlot);
    UnRegisterExt(Socket,SendMsg,sockSendMsg);
    UnRegisterExt(Socket,InitAuth,sockInitAuth);
    UnRegisterExt(Socket,DecodeExtension,sockDecodeExtension);
    UnRegisterExt(Socket,MultiplexS,sockMultiplexS);
}
#endif
