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
#include "extensions.h"

#include "fdlist.h"
#include "remote.h"
#include "socket.h"
#include "md5.h"
#include "hw_multi.h"
#include "common.h"
#include "version.h"

#include "Tw/Tw.h"
#include "Tw/Twkeys.h"
#include "Tw/Twstat.h"

#ifdef CONF__UNICODE
# include "Tutf/Tutf.h"
#endif

typedef union {
#if TW_SIZEOFULDAT >= TW_SIZEOFTIME_T && TW_SIZEOFULDAT >= TW_SIZEOFFRAC_T
    uldat  _;
#elif TW_SIZEOFTIME_T >= TW_SIZEOFFRAC_T
    time_t _;
#else
    frac_t _;
#endif
    obj    x;
    CONST byte * V;
    byte * VV; /* avoid warnings about discarding CONST */
} any;



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
    delta = LS.RQmax - LS.RQstart - LS.RQlen; /* yet available */
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
	z->next_out = RemoteWriteFillQueue(slot, &z->avail_out);

	while (z->avail_in && zret == Z_OK) {
	    
	    if (z->avail_out < (delta = z->avail_in + 12)) {
		if (RemoteWriteQueue(slot, delta - z->avail_out, NULL)) {
		    if (!(ls.WQlen -= delta))
			FdWQueued--;
		    z->next_out = RemoteWriteFillQueue(slot, &z->avail_out);
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
	z->next_in = RemoteReadGetQueue(Slot, &z->avail_in);
	z->next_out = RemoteReadFillQueue(slot, &z->avail_out);
	
	while (z->avail_in && zret == Z_OK) {
	    
	    /* approx. guess of uncompression ratio: 1 to 5 */
	    if (z->avail_out < (delta = 5 * z->avail_in + 12)) {
		if (RemoteReadGrowQueue(slot, delta - z->avail_out)) {
		    ls.RQlen -= delta;
		    z->next_out = RemoteReadFillQueue(slot, &z->avail_out);
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

static uldat FindFunction(byte Len, CONST byte *name, byte ProtoLen, CONST byte *Proto);
static byte SyncSocket(void);
static byte ServerSizeof(byte Type);
static byte CanCompress(void);
static byte DoCompress(byte on_off);

static void NeedResizeDisplay(void);
static void AttachHW(uldat len, CONST byte *arg, byte flags);
static byte DetachHW(uldat len, CONST byte *arg);
static void SetFontTranslation(CONST byte trans[0x80]);
static void SetUniFontTranslation(CONST hwfont trans[0x80]);

static void DeleteObj(void *V);

static widget CreateWidget(dat XWidth, dat YWidth, uldat Attrib, uldat Flags, dat Left, dat Up, hwattr Fill);
static void RecursiveDeleteWidget(void *V);
static msgport GetOwnerWidget(widget W);
static void SetXYWidget(widget W, dat x, dat y);
static void ExposeWidget(widget W, dat XWidth, dat YWidth, dat Left, dat Up, CONST byte *Text, CONST hwfont *Font, CONST hwattr *Attr);

static void FocusSubWidget(widget W);

static gadget CreateGadget
    (widget Parent, dat XWidth, dat YWidth, CONST byte *TextNormal, uldat Attrib, uldat Flags, udat Code,
     hwcol ColText, hwcol ColTextSelect, hwcol ColTextDisabled, hwcol ColTextSelectDisabled,
     dat Left, dat Up);

static void ResizeWindow(window Window, dat X, dat Y);

static void Create4MenuRow(window, udat Code, byte Flags, uldat Len, byte CONST *Name);

static menuitem Create4MenuMenuItem(obj Parent, window Window, byte Flags, dat Len, byte CONST *Name);

static menu CreateMenu(hwcol ColItem, hwcol ColSelect,
		       hwcol ColDisabled, hwcol ColSelectDisabled, hwcol ColShtCut, hwcol ColSelShtCut,
		       byte FlagDefColInfo);

static msgport CreateMsgPort(byte NameLen, CONST byte *Name, time_t WakeUp, frac_t WakeUpFrac, byte Flags);
static msgport FindMsgPort(msgport Prev, byte NameLen, CONST byte *Name);

static group CreateGroup(void);

static obj PrevObj(obj Obj);
static obj NextObj(obj Obj);
static obj ParentObj(obj Obj);

static gadget G_PrevGadget(gadget Gadget);
static gadget G_NextGadget(gadget Gadget);
static group GroupGadget(gadget Gadget);

static widget O_PrevWidget(widget Widget);
static widget O_NextWidget(widget Widget);
static msgport OwnerWidget(widget Widget);

static screen FirstScreen(void);
static widget FirstWidget(widget W);
static msgport FirstMsgPort(void);
static menu FirstMenu(msgport MsgPort);
static widget FirstW(msgport MsgPort);
static group FirstGroup(msgport MsgPort);
static mutex FirstMutex(msgport MsgPort);
static menuitem FirstMenuItem(menu Menu);
static gadget FirstGadget(group Group);

static byte SendToMsgPort(msgport MsgPort, udat Len, CONST byte *Data);
static void BlindSendToMsgPort(msgport MsgPort, udat Len, CONST byte *Data);

static obj  GetOwnerSelection(void);
static void SetOwnerSelection(time_t Time, frac_t Frac);
static void NotifySelection(obj Requestor, uldat ReqPrivate,
			       uldat Magic, CONST byte MIME[MAX_MIMELEN], uldat Len, CONST byte *Data);
static void RequestSelection(obj Owner, uldat ReqPrivate);

/* Second: socket handling functions */

static uldat MaxFunct, Slot, RequestN;
static byte *s, *end;
static int inetFd = NOFD, Fd;
static uldat inetSlot = NOSLOT;

static byte sockReply(uldat code, uldat len, CONST void *data);

static void SocketIO(int fd, uldat slot);


#define Left(size)	(s + (size) <= end)

#include "unaligned.h"

enum sock_id {
    order_DoesNotExist = -1,
#define EL(name) CAT(order_,name),
#include "socklistm4.h"
#undef EL
    order_StatObj,
};

static void sockStat(any *a);

static void sockMultiplex(uldat Id, any *a) {
    switch (Id) {
#include "socket1m4.h"
      case order_StatObj:
	sockStat(a);
	break;
      default:
	break;
    }
}

typedef struct {
    byte Len, FormatLen, *Name, *Format;
} sock_fn;

static sock_fn sockF [] = {
#include "socket2m4.h"
    { 0, 0, "StatObj", "0S0x"obj_magic_STR"_"TWS_udat_STR"V"TWS_udat_STR },
    { 0, 0, NULL, NULL }
};

/* code to return array lengths V(expr) and W(expr) */
static uldat sockLengths(uldat id, uldat n, const any *a) {
    uldat L = 0;

    switch (id) {
#include "socket3m4.h"
      case order_StatObj:
	switch (n) {
	    case 3: L = a[2]._; break;
	}
    }
    return L;
}



/***********/


static byte TwinMagicData[10+sizeof(uldat)] = {
    10+sizeof(uldat),
    sizeof(byte),
    sizeof(udat),
    sizeof(uldat),
    sizeof(hwcol),
    sizeof(time_t),
    sizeof(frac_t),
    sizeof(hwfont),
    sizeof(hwattr),
    0
};



static byte sockStatObj(obj x, tsfield TSF) {
    switch(TSF[0].hash) {
      case TWS_Id:     x = (obj)x;         break;
      case TWS_Prev:   x = (obj)x->Prev;   break;
      case TWS_Next:   x = (obj)x->Next;   break;
      case TWS_Parent: x = (obj)x->Parent; break;
      default: return FALSE;
    }
    TSF[0].TWS_field_obj = x;
    TSF[0].type = TWS_obj;
    return TRUE;
}

#define TWScase(objtype,field,fieldtype) \
  case CAT(CAT(CAT(TWS_,objtype),_),field): \
    /* ensure type size WAS negotiated */ \
    if (CAT(TWS_,fieldtype) <= TWS_hwcol || CAT(TWS_,fieldtype) >= TWS_highest \
	    || AlienSizeof(fieldtype, Slot)) { \
	TSF[0].CAT(TWS_field_,fieldtype) = (fieldtype)((objtype)x)->field; \
	TSF[0].type = CAT(TWS_,fieldtype); \
	break; \
    } \
    return FALSE

#define TWScaseUSE(objtype,use,field,fieldtype) \
  case CAT(CAT(CAT(TWS_,objtype),_USE_),field): \
    /* ensure type size WAS negotiated */ \
    if (CAT(TWS_,fieldtype) <= TWS_hwcol || CAT(TWS_,fieldtype) >= TWS_highest \
	    || AlienSizeof(fieldtype, Slot)) { \
	TSF[0].CAT(TWS_field_,fieldtype) = (fieldtype)((objtype)x)->USE.use.field; \
	TSF[0].type = CAT(TWS_,fieldtype); \
	break; \
    } \
    return FALSE

#define TWScaseA(objtype,field,n,fieldtype) \
  case CAT(CAT(CAT(CAT(CAT(TWS_,objtype),_),field),V),n): \
    /* ensure type size WAS negotiated */ \
    if (CAT(TWS_,fieldtype) <= TWS_hwcol || CAT(TWS_,fieldtype) >= TWS_highest \
	    || AlienSizeof(fieldtype, Slot)) { \
	TSF[0].CAT(TWS_field_,fieldtype) = (fieldtype)((objtype)x)->field[n]; \
	TSF[0].type = CAT(TWS_,fieldtype); \
	break; \
    } \
    return FALSE

#define TWScaseAUSE(objtype,use,field,n,fieldtype) \
  case CAT(CAT(CAT(CAT(CAT(TWS_,objtype),_USE_),field),V),n): \
    /* ensure type size WAS negotiated */ \
    if (CAT(TWS_,fieldtype) <= TWS_hwcol || CAT(TWS_,fieldtype) >= TWS_highest \
	    || AlienSizeof(fieldtype, Slot)) { \
	TSF[0].CAT(TWS_field_,fieldtype) = (fieldtype)((objtype)x)->USE.use.field[n]; \
	TSF[0].type = CAT(TWS_,fieldtype); \
	break; \
    } \
    return FALSE

#define TWScasevec(objtype,field,_type,len) \
  case CAT(CAT(CAT(TWS_,objtype),_),field): \
    if ((TSF[0].TWS_field_vecV = (void *)((objtype)x)->field)) \
	TSF[0].TWS_field_vecL = (uldat)sizeof(_type)*(len); \
    else \
	TSF[0].TWS_field_vecL = (uldat)0; \
    TSF[0].type = TWS_vec | CAT(TWS_,_type); \
    break

#define TWScasevecUSE(objtype,use,field,_type,len) \
  case CAT(CAT(CAT(TWS_,objtype),_USE_),field): \
    if ((TSF[0].TWS_field_vecV = (void *)((objtype)x)->USE.use.field)) \
	TSF[0].TWS_field_vecL = (uldat)sizeof(_type)*(len); \
    else \
	TSF[0].TWS_field_vecL = (uldat)0; \
    TSF[0].type = TWS_vec | CAT(TWS_,_type); \
    break

#define TWScaseAvec(objtype,field,n,_type,len) \
  case CAT(CAT(CAT(CAT(CAT(TWS_,objtype),_),field),V),n): \
    if ((TSF[0].TWS_field_vecV = (void *)((objtype)x)->field[n])) \
	TSF[0].TWS_field_vecL = (uldat)sizeof(_type)*(len); \
    else \
	TSF[0].TWS_field_vecL = (uldat)0; \
    TSF[0].type = TWS_vec | CAT(TWS_,_type); \
    break

#define TWScaseAvecUSE(objtype,use,field,n,_type,len) \
  case CAT(CAT(CAT(CAT(CAT(TWS_,objtype),_USE_),field),V),n): \
    if ((TSF[0].TWS_field_vecV = (void *)((objtype)x)->USE.use.field[n])) \
	TSF[0].TWS_field_vecL = (uldat)sizeof(_type)*(len); \
    else \
	TSF[0].TWS_field_vecL = (uldat)0; \
    TSF[0].type = TWS_vec | CAT(TWS_,_type); \
    break

static byte sockStatWidget(obj x, tsfield TSF) {
    switch (TSF[0].hash) {
	TWScase(widget,FirstW,obj);
	TWScase(widget,LastW,obj);
	TWScase(widget,SelectW,obj);
	TWScase(widget,Left,dat);
	TWScase(widget,Up,dat);
	TWScase(widget,Attrib,uldat);
	TWScase(widget,Flags,uldat);
	TWScase(widget,XWidth,dat);
	TWScase(widget,YWidth,dat);
	TWScase(widget,XLogic,ldat);
	TWScase(widget,YLogic,ldat);
	TWScase(widget,O_Prev,obj);
	TWScase(widget,O_Next,obj);
	TWScase(widget,Owner,obj);
	TWScase(widget,USE_Fill,hwattr);
      default:
	return FALSE;
    }
    /* correct for screen scrolling */
    if (((widget)x)->Parent && IS_SCREEN(((widget)x)->Parent)) {
	if (TSF[0].hash == TWS_widget_Left)
	    TSF[0].TWS_field_scalar -= ((widget)x)->Parent->XLogic;
	else if (TSF[0].hash == TWS_widget_Up)
	    TSF[0].TWS_field_scalar -= ((widget)x)->Parent->YLogic;
    }
    return TRUE;
}

static byte sockStatGadget(obj x, tsfield TSF) {
    switch (TSF[0].hash) {
	TWScase(gadget,ColText,hwcol);
	TWScase(gadget,ColSelect,hwcol);
	TWScase(gadget,ColDisabled,hwcol);
	TWScase(gadget,ColSelectDisabled,hwcol);
	TWScase(gadget,Code,udat);
	TWScase(gadget,Flags,udat);
	TWScase(gadget,G_Prev,obj);
	TWScase(gadget,G_Next,obj);
	TWScase(gadget,Group,obj);
      default:
	if (G_USE((gadget)x, USETEXT)) {
	    switch (TSF[0].hash) {
		TWScaseAvecUSE(gadget,T,Text,0,hwfont,((gadget)x)->XWidth*((gadget)x)->YWidth);
		TWScaseAvecUSE(gadget,T,Text,1,hwfont,((gadget)x)->XWidth*((gadget)x)->YWidth);
		TWScaseAvecUSE(gadget,T,Text,2,hwfont,((gadget)x)->XWidth*((gadget)x)->YWidth);
		TWScaseAvecUSE(gadget,T,Text,3,hwfont,((gadget)x)->XWidth*((gadget)x)->YWidth);
		TWScaseAvecUSE(gadget,T,Color,0,hwcol,((gadget)x)->XWidth*((gadget)x)->YWidth);
		TWScaseAvecUSE(gadget,T,Color,1,hwcol,((gadget)x)->XWidth*((gadget)x)->YWidth);
		TWScaseAvecUSE(gadget,T,Color,2,hwcol,((gadget)x)->XWidth*((gadget)x)->YWidth);
		TWScaseAvecUSE(gadget,T,Color,3,hwcol,((gadget)x)->XWidth*((gadget)x)->YWidth);
	      default:
		return FALSE;
	    }
	} else
	    return FALSE;
	break;
    }
    return TRUE;
}

static byte sockStatWindow(obj x, tsfield TSF) {
    switch (TSF[0].hash) {
      case TWS_widget_Left:
      case TWS_widget_Up:
      case TWS_widget_XWidth:
      case TWS_widget_YWidth:
	sockStatWidget(x, TSF);
	break;

        TWScase(window,Menu,obj);
        TWScase(window,NameLen,dat);
        TWScasevec(window,Name,byte,((window)x)->NameLen);
        TWScasevec(window,ColName,hwcol,((window)x)->NameLen);
        TWScaseAvec(window,BorderPattern,0,hwfont,9);
        TWScaseAvec(window,BorderPattern,1,hwfont,9);
        TWScase(window,CurX,ldat);
        TWScase(window,CurY,ldat);
        TWScase(window,XstSel,ldat);
        TWScase(window,YstSel,ldat);
        TWScase(window,XendSel,ldat);
        TWScase(window,YendSel,ldat);
        TWScase(window,ColGadgets,hwcol);
        TWScase(window,ColArrows,hwcol);
        TWScase(window,ColBars,hwcol);
        TWScase(window,ColTabs,hwcol);
        TWScase(window,ColBorder,hwcol);
        TWScase(window,ColText,hwcol);
        TWScase(window,ColSelect,hwcol);
        TWScase(window,ColDisabled,hwcol);
        TWScase(window,ColSelectDisabled,hwcol);
        TWScase(window,State,uldat);
        TWScase(window,CursorType,uldat);
        TWScase(window,MinXWidth,dat);
        TWScase(window,MinYWidth,dat);
        TWScase(window,MaxXWidth,dat);
        TWScase(window,MaxYWidth,dat);
        TWScase(window,WLogic,ldat);
        TWScase(window,HLogic,ldat);
      default:
	if (W_USE((window)x, USECONTENTS)) {
	    switch (TSF[0].hash) {
		TWScasevecUSE(window,C,Contents,hwattr,((window)x)->WLogic * ((window)x)->HLogic);
		TWScaseUSE(window,C,HSplit,ldat);
	      default:
		return FALSE;
	    }
	} else
	    return FALSE;
	break;
    }
    /* correct for window borders */
    if (!(((window)x)->Flags & WINDOWFL_BORDERLESS)) {
	if (TSF[0].hash == TWS_widget_Left || TSF[0].hash == TWS_widget_Up)
	    TSF[0].TWS_field_scalar ++;
	else if (TSF[0].hash == TWS_widget_XWidth || TSF[0].hash == TWS_widget_YWidth)
	    TSF[0].TWS_field_scalar -= 2;
    }
    return TRUE;
}

static byte sockStatScreen(obj x, tsfield TSF) {
    switch (TSF[0].hash) {
	TWScase(screen,NameLen,dat);
	TWScasevec(screen,Name,byte,((screen)x)->NameLen);
      default:
	if (S_USE((gadget)x, USEBG)) {
	    switch (TSF[0].hash) {
		TWScaseUSE(screen,B,BgWidth,dat);
		TWScaseUSE(screen,B,BgHeight,dat);
		TWScasevecUSE(screen,B,Bg,hwattr,((screen)x)->USE.B.BgWidth*((screen)x)->USE.B.BgHeight);
	      default:
		return FALSE;
	    }
	} else
	    return FALSE;
	break;
    }
    return TRUE;
}

static byte sockStatGroup(obj x, tsfield TSF) {
    switch (TSF[0].hash) {
	TWScase(group,FirstG,obj);
	TWScase(group,LastG,obj);
	TWScase(group,SelectG,obj);
      default:
	return FALSE;
    }
    return TRUE;
}

static byte sockStatRow(obj x, tsfield TSF) {
    switch (TSF[0].hash) {
	TWScase(row,Code,udat);
	TWScase(row,Flags,byte);
	TWScase(row,Len,uldat);
	TWScasevec(row,Text,hwfont,((row)x)->Len);
	TWScasevec(row,ColText,hwcol,((row)x)->Len);
      default:
	return FALSE;
    }
    return TRUE;
}

static byte sockStatMenuItem(obj x, tsfield TSF) {
    switch (TSF[0].hash) {
	/* missing: */
#if 0
	TWS_menuitem_FlagActive;/*TWS_row_Flags*/
	TWS_menuitem_NameLen;	/*TWS_row_Len*/
	TWS_menuitem_Name;	/*TWS_row_Text*/
#endif
	TWScase(menuitem,Window,obj);
	TWScase(menuitem,Left,dat);
	TWScase(menuitem,ShortCut,dat);
      default:
	return FALSE;
    }
    return TRUE;
}

static byte sockStatMenu(obj x, tsfield TSF) {
    switch (TSF[0].hash) {
	TWScase(menu,ColItem,hwcol);
	TWScase(menu,ColSelect,hwcol);
	TWScase(menu,ColDisabled,hwcol);
	TWScase(menu,ColSelectDisabled,hwcol);
	TWScase(menu,ColShtCut,hwcol);
	TWScase(menu,ColSelShtCut,hwcol);
	TWScase(menu,CommonItems,byte);
	TWScase(menu,FlagDefColInfo,byte);
	TWScase(menu,FirstI,obj);
	TWScase(menu,LastI,obj);
	TWScase(menu,SelectI,obj);
      default:
	return FALSE;
    }
    return TRUE;
}

static byte sockStatMsgPort(obj x, tsfield TSF) {
    switch (TSF[0].hash) {
	TWScase(msgport,WakeUp,byte);
	TWScase(msgport,NameLen,byte);
	TWScasevec(msgport,Name,byte,((msgport)x)->NameLen);
	TWScase(msgport,FirstMenu,obj);
	TWScase(msgport,LastMenu,obj);
	TWScase(msgport,FirstW,obj);
	TWScase(msgport,LastW,obj);
	TWScase(msgport,FirstGroup,obj);
	TWScase(msgport,LastGroup,obj);
	TWScase(msgport,FirstMutex,obj);
	TWScase(msgport,LastMutex,obj);
      default:
	return FALSE;
    }
    return TRUE;
}

static byte sockStatMutex(obj x, tsfield TSF) {
    switch (TSF[0].hash) {
	TWScase(mutex,O_Prev,obj);
	TWScase(mutex,O_Next,obj);
	TWScase(mutex,Owner,obj);
	TWScase(mutex,Perm,byte);
	TWScase(mutex,NameLen,byte);
	TWScasevec(mutex,Name,byte,((mutex)x)->NameLen);
      default:
	return FALSE;
    }
    return TRUE;
}

static void sockStat(any *a) {
    obj x = a[1].x;
    udat i, j, n = a[2]._;
    CONST byte *in = a[3].V;
    tsfield TSF;
    uldat len, q;
    byte *data, ok;
    
    ok = x && n && (TSF = (tsfield)AllocMem(n * sizeof(*TSF)));
    
    if (ok) {
	for (i = j = 0; j < n; j++) {
	    Pop(in, udat, TSF[i].hash);
	    switch (x->Id >> magic_shift) {
	      case widget_magic_id:
		ok = sockStatWidget(x, &TSF[i]) || sockStatObj(x, &TSF[i]);
		break;
	      case gadget_magic_id:
		ok = sockStatGadget(x, &TSF[i]) || sockStatWidget(x, &TSF[i]) || sockStatObj(x, &TSF[i]);
		break;
	      case window_magic_id:
		ok = sockStatWindow(x, &TSF[i]) || sockStatWidget(x, &TSF[i]) || sockStatObj(x, &TSF[i]);
		break;
	      case screen_magic_id:
		ok = sockStatScreen(x, &TSF[i]) || sockStatWidget(x, &TSF[i]) || sockStatObj(x, &TSF[i]);
		break;
	      case group_magic_id:
		ok = sockStatGroup(x, &TSF[i]) || sockStatObj(x, &TSF[i]);
		break;
	      case menuitem_magic_id:
		ok = sockStatMenuItem(x, &TSF[i]) || sockStatRow(x, &TSF[i]) || sockStatObj(x, &TSF[i]);
		break;
	      case menu_magic_id:
		ok = sockStatMenu(x, &TSF[i]) || sockStatObj(x, &TSF[i]);
		break;
	      case msgport_magic_id:
		ok = sockStatMsgPort(x, &TSF[i]) || sockStatObj(x, &TSF[i]);
		break;
	      case mutex_magic_id:
		ok = sockStatMutex(x, &TSF[i]) || sockStatObj(x, &TSF[i]);
		break;
	      default:
		ok = FALSE;
		break;
	    }
	    if (ok)
		i++;
	}
	
	len = 2 * (j = i) * sizeof(udat) + 2 * sizeof(udat);
	
	for (i = 0; i < j; i++) {
	    if (TSF[i].type < TWS_highest)
		len += TwinMagicData[TSF[i].type];
	    else if (TSF[i].type == TWS_obj) {
		TSF[i].TWS_field_tobj = TSF[i].TWS_field_obj ? TSF[i].TWS_field_obj->Id : NOID;
		len += sizeof(tobj);
	    } else {
		if (!TSF[i].TWS_field_vecV)
		    TSF[i].TWS_field_vecL = 0;
		len += TSF[i].TWS_field_vecL + sizeof(uldat);
	    }
	}
	
	ok = sockReply(OK_MAGIC, len, NULL) &&
	    (data = RemoteWriteGetQueue(Slot, &q)) && q >= len;

	if (ok) {
	    i = 0;
	    data += q - len;

	    Push(data,udat,j);
	    Push(data,udat,i); /* pad */
	    
	    for (; i < j; i++) {
		Push(data,udat,TSF[i].hash);
		Push(data,udat,TSF[i].type);
		
		switch (TSF[i].type) {
#if TW_BYTE_ORDER == TW_LITTLE_ENDIAN
#define Pushcase(type) case CAT(TWS_,type): \
			Push(data,type,TSF[i].CAT(TWS_field_,type)); \
		    break
#else
#define Pushcase(type) case CAT(TWS_,type): \
		    { \
			/* avoid padding problems on big-endian machines */ \
			type f = (type)TSF[i].CAT(TWS_field_,type); \
			Push(data,type,f); \
		    } \
		    break
#endif
		    Pushcase(byte);
		    Pushcase(udat);
		    Pushcase(uldat);
		    Pushcase(hwcol);
		    Pushcase(time_t);
		    Pushcase(frac_t);
		    Pushcase(hwfont);
		    Pushcase(hwattr);
		    Pushcase(tobj);
#undef Pushcase
		  default:
		    Push(data,uldat,TSF[i].TWS_field_vecL);
		    if (TSF[i].TWS_field_vecL)
			PushV(data, TSF[i].TWS_field_vecL, TSF[i].TWS_field_vecV);
		    break;
		}
	    }
	}
	    
	FreeMem(TSF);
    }
    if (!ok)
	sockReply(1, 0, NULL);
}

static void sockDecode(uldat id) {
    static any a[20];
    uldat nlen, n = 1;
    ldat fail = 1;
    byte *Format = sockF[id].Format;
    uldat a0;
    byte c, self, retF, retT; 

    self = *Format++;
    retF = *Format++;
    retT = *Format++;
    
    while (fail > 0 && (c = *Format++)) {
	switch (c) {
	  case '_':
	    switch ((c = *Format)) {
#define CASE_(type) \
	      case CAT(TWS_,type) + TWS_base_CHR: \
    		/* ensure type size WAS negotiated */ \
		if ((CAT(TWS_,type) <= TWS_hwcol || AlienSizeof(type, Slot)) && Left(sizeof(type))) { \
		    type an; \
		    Pop(s,type,an); \
		    a[n]._ = (uldat)an; \
		} else \
		    fail = -fail; \
		break
		
		case TWS_hwcol + TWS_base_CHR:
		/*FALLTHROUGH*/
		CASE_(byte);
		CASE_(dat);
		CASE_(ldat);
		CASE_(time_t);
		CASE_(frac_t);
		CASE_(hwfont);
		CASE_(hwattr);
#undef CASE_
	      default:
		break;
	    }
	    break;
	  case 'x':
	    /* all kind of pointers */
	    if (Left(sizeof(uldat))) {
		Pop(s,uldat,a[n]._);
		c = *Format - base_magic_CHR;
		a[n].x = Id2Obj(c, a[n]._);
	    } else
		fail = -fail;
	    break;
	  case 'V':
	    nlen = sockLengths(id, n, a);
	    c = *Format - TWS_base_CHR;
	    /* ensure type size WAS negotiated */
	    if ((c <= TWS_hwcol || AlienMagic(Slot)[c])) {
		nlen *= AlienMagic(Slot)[c];
		if (Left(nlen)) {
		    PopAddr(s,byte,nlen,a[n].V);
		    break;
		}
	    }
	    fail = -fail;
	    break;
	  case 'W':
	    if (Left(sizeof(uldat))) {
		Pop(s,uldat,nlen);

		c = *Format - TWS_base_CHR;
		/* ensure type size WAS negotiated */
		if ((c <= TWS_hwcol || AlienMagic(Slot)[c])) {
		    if (!nlen || (Left(nlen) && nlen == sockLengths(id, n, a) * AlienMagic(Slot)[c])) {
			PopAddr(s,byte,nlen,a[n].V);
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
	if (fail > 0) {
	    Format++;
	    fail++;
	    n++;
	} else
	    break;
    }
    
    if (fail > 0 && s == end && !c && (self != '2' || a[1].x)) {

	sockMultiplex(id,a);

	switch (retF) {
	  case '_':
	    switch (retT) {
#define CASE_(type) \
	      case CAT(TWS_,type) + TWS_base_CHR: \
    		/* ensure type size WAS negotiated */ \
		if (CAT(TWS_,type) <= TWS_hwcol || AlienSizeof(type, Slot)) { \
		    type tmp = (type)a[0]._; \
		    *(type *)&a[0] = tmp; \
		    c = sizeof(type); \
		} else \
		    fail = 0; \
		break
		
		case TWS_hwcol + TWS_base_CHR:
		/*FALLTHROUGH*/
		CASE_(byte);
		CASE_(dat);
		CASE_(ldat);
		CASE_(time_t);
		CASE_(frac_t);
		CASE_(hwfont);
		CASE_(hwattr);
#undef CASE_
	      default:
		c = 0;
		break;
	    }
	    if (c && fail > 0) {
		sockReply(OK_MAGIC, c, &a[0]);
		return;
	    }
	    break;
	    
	  case 'x':
	    a0 = a[0].x ? a[0].x->Id : NOID;
	    sockReply(OK_MAGIC, sizeof(tobj), &a0);
	    return;
	    
	  case 'S':
	  case 'v':
	    return;
	    
	  default:
	    break;
	}
    }
    if (retF != 'v') {
	if (fail > 0) {
	    if (self != '2' || a[1].x)
		fail = FAIL_MAGIC;
	    else
		fail = 1;
	}
	sockReply(fail, 0, NULL);
    }
}
    


/***********/



/* actual libTw back-end utility functions */

static int CmpFormat(CONST byte *F1, CONST byte *F2, uldat Len) {
    for (; Len; F1++, F2++, Len--) {
	if (*F1 == 'x' && *F2 == 'x' && Len > 1) {
	    /* pointer. must allow any */
	    F1++, F2++, Len--;
	    continue;
	}
	if (*F1 != *F2)
	    break;
    }
    return Len ? *F1 - *F2 : 0;
}

static uldat FindFunction(byte Len, CONST byte *Name, byte FormatLen, CONST byte *Format) {
    sock_fn *F = sockF;
    if (Name) {
	while (F->Name && (F->Len != Len || F->FormatLen - 1 != FormatLen ||
			   CmpMem(F->Name, Name, Len) || CmpFormat(F->Format+1, Format, FormatLen)))
	    F++;
	if (F->Name)
	    return (uldat)(F - sockF);
    }
    return NOID;
}

static byte SyncSocket(void) {
    return TRUE;
}

static byte ServerSizeof(byte Type) {
    byte l;
    switch (Type) {
#define CASE_(t) case CAT(TWS_,t): l = sizeof(t); break;
	CASE_(byte);
	CASE_(udat);
	CASE_(uldat);
	CASE_(hwcol);
	CASE_(time_t);
	CASE_(frac_t);
	CASE_(hwfont);
	CASE_(hwattr);
#undef CASE_
	default: l = 0; break;
    }
    return l;
}

#ifdef CONF_SOCKET_GZ

static byte CanCompress(void) {
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
static byte DoCompress(byte on_off) {
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

static byte CanCompress(void) {
    return FALSE;
}

static byte DoCompress(byte on_off) {
    return FALSE;
}

#endif /* CONF_SOCKET_GZ */



static void NeedResizeDisplay(void){
    if (LS.MsgPort && LS.MsgPort->AttachHW)
	ResizeDisplayPrefer(LS.MsgPort->AttachHW);
}

/*
 * this does direct write() on the connecting socket,
 * so it bypasses any compression.
 */
static void AttachHW(uldat len, CONST byte *arg, byte flags) {
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

static byte DetachHW(uldat len, CONST byte *arg) {
    return DetachDisplayHW(len, arg, 0); /* cannot detach exclusive displays !! */
}

static void SetFontTranslation(CONST byte trans[0x80]) {
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

static void SetUniFontTranslation(CONST hwfont trans[0x80]) {
#ifdef CONF__UNICODE
    if (trans) {
	int i;
	hwfont *G = All->Gtranslations[USER_MAP];

	for (i = 0; i < 0x80; i++)
	    G[i] = i;
	CopyMem(trans, G + 0x80, sizeof(hwfont) * 0x80);
    }
#else
    SetFontTranslation(trans);
#endif
}

static msgport GetMsgPortObj(obj P) {
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

static void DeleteObj(void *V) {
    obj O = (obj)V;
    msgport MsgPort = RemoteGetMsgPort(Slot);
    
    if (MsgPort && MsgPort == GetMsgPortObj(O))
	Delete(O);
}

static widget CreateWidget(dat XWidth, dat YWidth, uldat Attrib, uldat Flags, dat Left, dat Up, hwattr Fill) {
    msgport Owner;
    if ((Owner = RemoteGetMsgPort(Slot)))
	return Do(Create,Widget)
	    (FnWidget, Owner, XWidth, YWidth, Attrib, Flags, Left, Up, Fill);
    return (widget)0;
}
static void RecursiveDeleteWidget(void *V) {
    msgport MsgPort = RemoteGetMsgPort(Slot);

    /* avoid too much visual activity... UnMap top-level widget immediately */
    Act(UnMap,(widget)V)((widget)V);

    if (MsgPort)
	Act(RecursiveDelete,(widget)V)((widget)V, MsgPort);
}
static msgport GetOwnerWidget(widget W) {
    if (W)
	return W->Owner;
    return (msgport)0;
}
static void SetXYWidget(widget W, dat x, dat y) {
    if (W) {
	if (W->Parent && IS_SCREEN(W->Parent)) {
	    x += W->Parent->XLogic;
	    y += W->Parent->YLogic;
	}
	Act(SetXY,W)(W, x, y);
    }
}
static void ExposeWidget(widget W, dat XWidth, dat YWidth, dat Left, dat Up, CONST byte *Text, CONST hwfont *Font, CONST hwattr *Attr) {
    if (W) {
	Act(Expose,W)(W, XWidth, YWidth, Left, Up, XWidth, Text, Font, Attr);
    }
}

static void FocusSubWidget(widget W) {
    widget P;
    if (W) {
	W->SelectW = NULL;
	while ((P = W->Parent) && !IS_SCREEN(P)) {
	    P->SelectW = W;
	    W = P;
	}
	if (ContainsCursor((widget)WindowParent(W)))
	    UpdateCursor();
    }
}

static gadget CreateGadget
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


static void ResizeWindow(window Window, dat X, dat Y) {
    if (Window) {
	if (!(Window->Flags & WINDOWFL_BORDERLESS))
	    X+=2, Y+=2;
	ResizeRelWindow(Window, X - Window->XWidth, Y - Window->YWidth);
    }
}

static void Create4MenuRow(window Window, udat Code, byte Flags, uldat Len, byte CONST *Name) {
    Row4Menu(Window, Code, Flags, Len, Name);
}

static menuitem Create4MenuMenuItem(obj Parent, window Window, byte Flags, dat Len, byte CONST *Name) {
    return Item4Menu(Parent, Window, Flags, Len, Name);
}

static menu CreateMenu(hwcol ColItem, hwcol ColSelect,
			hwcol ColDisabled, hwcol ColSelectDisabled, hwcol ColShtCut, hwcol ColSelShtCut,
			byte FlagDefColInfo) {
    msgport Owner;
    if ((Owner = RemoteGetMsgPort(Slot)))
	return Do(Create,Menu)(FnMenu, Owner, ColItem, ColSelect, ColDisabled, ColSelectDisabled, ColShtCut, ColSelShtCut,
			       FlagDefColInfo);
    return (menu)0;
}
			       

/* last 3 args are currently useless for remote clients */
static msgport CreateMsgPort(byte NameLen, CONST byte *Name, time_t WakeUp, frac_t WakeUpFrac, byte Flags) {
    msgport MsgPort;
    
    if ((MsgPort = Do(Create,MsgPort)(FnMsgPort, NameLen, Name, 0, 0, 0, SocketH))) {
	RegisterMsgPort(MsgPort, Slot);
	MsgPort->ShutDownHook = sockShutDown;
    }
    return MsgPort;
}
static msgport FindMsgPort(msgport Prev, byte NameLen, CONST byte *Name) {
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

static group CreateGroup(void) {
    msgport Owner;
    if ((Owner = RemoteGetMsgPort(Slot)))
	return Do(Create,Group)(FnGroup, Owner);
    return (group)0;
}

static obj PrevObj(obj Obj) {
    return Obj ? Obj->Prev : Obj;
}
static obj NextObj(obj Obj) {
    return Obj ? Obj->Next : Obj;
}
static obj ParentObj(obj Obj) {
    return Obj ? (obj)Obj->Parent : Obj;
}

static gadget G_PrevGadget(gadget Gadget) {
    return Gadget ? Gadget->G_Prev : Gadget;
}
static gadget G_NextGadget(gadget Gadget) {
    return Gadget ? Gadget->G_Next : Gadget;
}
static group GroupGadget(gadget Gadget) {
    return Gadget ? Gadget->Group : (group)Gadget;
}

static widget O_PrevWidget(widget Widget) {
    return Widget ? Widget->O_Prev : Widget;
}
static widget O_NextWidget(widget Widget) {
    return Widget ? Widget->O_Next : Widget;
}
static msgport OwnerWidget(widget Widget) {
    return Widget ? Widget->Owner : (msgport)Widget;
}

static screen FirstScreen(void) {
    return All->FirstScreen;
}
static widget FirstWidget(widget W) {
    return W ? W->FirstW : W;
}
static msgport FirstMsgPort(void) {
    return All->FirstMsgPort;
}
static menu FirstMenu(msgport MsgPort) {
    return MsgPort ? MsgPort->FirstMenu : (void *)MsgPort;
}
static widget FirstW(msgport MsgPort) {
    return MsgPort ? MsgPort->FirstW : (void *)MsgPort;
}
static group FirstGroup(msgport MsgPort) {
    return MsgPort ? MsgPort->FirstGroup : (void *)MsgPort;
}
static mutex FirstMutex(msgport MsgPort) {
    return MsgPort ? MsgPort->FirstMutex : (void *)MsgPort;
}
static menuitem FirstMenuItem(menu Menu) {
    return Menu ? Menu->FirstI : (void *)Menu;
}
static gadget FirstGadget(group Group) {
    return Group ? Group->FirstG : (void *)Group;
}





/* shortcut: add a (tmsg) in the remote MsgPort's file descriptor queue */
static byte ShortcutSendMsg(msgport MsgPort, udat Len, CONST tmsg tMsg) {
    switch (tMsg->Type) {
      /* the only types of messages allowed between clients: */
      case TW_MSG_USER_CONTROL:
      case TW_MSG_USER_CLIENTMSG:
	tMsg->Len -= AlienSizeof(uldat,Slot);
	tMsg->Magic = MSG_MAGIC;
	RemoteWriteQueue(MsgPort->RemoteData.FdSlot, Len, (CONST byte *)tMsg);
	return TRUE;
    }
    return FALSE;
}

#define tDelta ((udat)(size_t)&(((tmsg)0)->Event))
#define  Delta ((udat)(size_t)&(((msg)0)->Event))
#define tOffset(x) ((udat)(size_t)&(((tmsg)0)->Event.x))

/* extract the (tmsg) from Data, turn it into a (msg) and send it to MsgPort */
static byte SendToMsgPort(msgport MsgPort, udat Len, CONST byte *Data) {
    tmsg tMsg;
    msgport Sender;
    msg Msg;
    udat _Len, minType;
    byte ok = TRUE;
    
    /* be careful with alignment! */
#ifdef __i386__
    tMsg = (tmsg)Data;
#else
    tMsg = (tmsg)CloneMem(Data, Len);
#endif /* __i386__ */
    
    do if (MsgPort && Len && tMsg && Len >= tDelta && Len == tMsg->Len &&
	   tMsg->Magic == msg_magic) {

	Sender = RemoteGetMsgPort(Slot);
	if (Sender && Sender->AttachHW)
	    minType = TW_MSG_DISPLAY;
	else
	    minType = TW_MSG_USER_FIRST;
	if (tMsg->Type < minType)
	    /* not allowed! */
	    break;
	    


	_Len = FnMsg->Size - Delta;
	switch (tMsg->Type) {
	  case TW_MSG_WIDGET_KEY:
	    if (Len >= tOffset(EventKeyboard.AsciiSeq)) {
		if (tMsg->Event.EventKeyboard.SeqLen + tOffset(EventKeyboard.AsciiSeq) > Len)
		    tMsg->Event.EventKeyboard.SeqLen = Len - tOffset(EventKeyboard.AsciiSeq);
		_Len += tMsg->Event.EventKeyboard.SeqLen;
	    } else
		/* (tmsg) too short */
		ok = FALSE;
	    break;
	  case TW_MSG_USER_CONTROL:
	    if (Len >= tOffset(EventControl.Data)) {
		if (tMsg->Event.EventControl.Len + tOffset(EventControl.Data) > Len)
		    tMsg->Event.EventControl.Len = Len - tOffset(EventControl.Data);
		_Len += tMsg->Event.EventControl.Len;
	    } else
		/* (tmsg) too short */
		ok = FALSE;
	    break;
	  case TW_MSG_USER_CLIENTMSG:
	    if (Len >= tOffset(EventClientMsg.Data)) {
		if (tMsg->Event.EventClientMsg.Len + tOffset(EventClientMsg.Data) > Len)
		    tMsg->Event.EventClientMsg.Len = Len - tOffset(EventClientMsg.Data);
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
	
	if (MsgPort->RemoteData.Fd != NOFD && MsgPort->Handler == SocketH &&
	    !CmpMem(FdList[MsgPort->RemoteData.FdSlot].AlienMagic, LS.AlienMagic, 8) &&
	    ShortcutSendMsg(MsgPort, Len, tMsg))
	    /*
	     * shortcut: we have a (tmsg); instead of turning it into a (msg)
	     * then have SocketH() call sockSendMsg() to turn it back into a (tmsg),
	     * we directly copy it --- only one condition must be met:
	     * both clients must have the same AlienMagic()
	     */
	    break;

	if ((Msg = Do(Create,Msg)(FnMsg, tMsg->Type, _Len))) {
		
	    Msg->Event.EventCommon.W = (void *)Id2Obj(widget_magic_id, tMsg->Event.EventCommon.W);

	    switch (tMsg->Type) {
#if defined(CONF__MODULES) || defined (CONF_HW_DISPLAY)
	      case TW_MSG_DISPLAY:
		if (sizeof(struct s_tevent_display) == sizeof(twindow) + 4*sizeof(dat) + sizeof(uldat)) {
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
		if (sizeof(struct s_tevent_keyboard) == sizeof(twindow) + 3*sizeof(dat) + 2*sizeof(byte)) {
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
		if (sizeof(struct s_tevent_mouse) == sizeof(twindow) + 4*sizeof(dat)) {
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
		if (sizeof(struct s_tevent_common) == sizeof(twindow) + 2*sizeof(dat)) {
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
		break;
#endif
	      case TW_MSG_USER_CONTROL:
		if (sizeof(struct s_tevent_control) == sizeof(twindow) + 4*sizeof(dat) + sizeof(uldat)) {
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
		if (sizeof(struct s_tevent_clientmsg) == sizeof(twindow) + 2*sizeof(dat) + sizeof(uldat)) {
		    CopyMem((void *)&tMsg->Event.EventControl.Code,
			    (void *)& Msg->Event.EventControl.Code, 2*sizeof(dat));
		} else {
		    Msg->Event.EventControl.Code  = tMsg->Event.EventControl.Code;
		    Msg->Event.EventControl.Len   = tMsg->Event.EventControl.Len;
		}
		CopyMem(tMsg->Event.EventControl.Data, Msg->Event.EventControl.Data,
			tMsg->Event.EventControl.Len);
		
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
    
#ifndef __i386__
    FreeMem(tMsg);
#endif
    return ok;
}

static void BlindSendToMsgPort(msgport MsgPort, udat Len, CONST byte *Data) {
    (void)SendToMsgPort(MsgPort, Len, Data);
}

static obj GetOwnerSelection(void) {
    return TwinSelectionGetOwner();
}

static void SetOwnerSelection(time_t Time, frac_t Frac) {
    if (LS.MsgPort)
	TwinSelectionSetOwner((obj)LS.MsgPort, Time, Frac);
}

static void NotifySelection(obj Requestor, uldat ReqPrivate, uldat Magic, CONST byte MIME[MAX_MIMELEN],
			    uldat Len, CONST byte *Data) {
    TwinSelectionNotify(Requestor, ReqPrivate, Magic, MIME, Len, Data);
}

static void RequestSelection(obj Owner, uldat ReqPrivate) {
    if (LS.MsgPort)
	TwinSelectionRequest((obj)LS.MsgPort, ReqPrivate, Owner);
}

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

static byte InitAuth(void) {
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
    uldat len;
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
	(t = RemoteReadGetQueue(Slot, &got)) && got == digestLen && SendUldat(len) &&
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

static void Wait4Auth(int fd, uldat slot) {
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
	    /* zero out unnegotiated sizes */
	    WriteMem(AlienMagic(slot) + len1, '\0', TWS_highest-len1);
	
	if (len1 <= TWS_hwattr)
	    AlienMagic(slot)[TWS_hwattr] = 2;
	
	return MagicNative;
    }
    
#ifdef CONF_SOCKET_ALIEN

    if (len1 > TWS_hwcol && len == magic[0] && 
	magic[TWS_byte] == 1 &&  /* sizeof(byte) MUST be 1, or passing byte[] vectors would fail */
	magic[TWS_udat] >= 2 &&
	magic[TWS_uldat] >= 4 &&
	len - len1 == magic[TWS_uldat] + 1 &&
	magic[TWS_hwcol] == 1 && /* sizeof(hwcol) MUST be 1, or passing hwcol[] vectors would fail */
	(len1 == TWS_time_t ||
	 (magic[TWS_time_t] >= 4 &&
	  (len1 == TWS_frac_t ||
	   (magic[TWS_frac_t] >= 4 &&
	    (len1 == TWS_hwfont ||
	     (magic[TWS_hwfont] >= 1 &&
	      (len1 == TWS_hwattr ||
	       (magic[TWS_hwattr] >= 2
		))))))))) {
	    
	/* store client magic numbers */
	CopyMem(magic, AlienMagic(slot), Min2(len1, TWS_highest));
	if (len1 < TWS_highest)
	    /* zero out unnegotiated sizes */
	    WriteMem(AlienMagic(slot) + len1, '\0', TWS_highest-len1);

	/*pre-0.3.9 compatibility: if hwattr is not negotiated, assume 2 bytes*/
	if (len1 <= TWS_hwattr)
	    AlienMagic(slot)[TWS_hwattr] = 2;
	
	if (warn_count < 6) {
	    zero = NULL;
	    if (AlienMagic(slot)[TWS_hwattr] != sizeof(hwattr))
		zero = "hwattr";
	    else if (len1 > TWS_hwfont && AlienMagic(slot)[TWS_hwfont] != sizeof(hwfont))
		zero = "hwfont";
	    
	    if (zero) {
		if (warn_count == 5)
		    printk("twin: warning: many client with different sizes, suppressing further messages.\n");
		else
		    printk("twin: warning: client has different `%s' size, it may "
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
	    /* we have a translation. answer the same client's magic */
	    got = SendTwinMagic(t, max);
	}
	RemoteReadDeQueue(Slot, max);
	
	if (got) {
	    if (isUnix) {
		if ((LS.HandlerIO.S = GetHandlerIO()) && SendUldat(GO_MAGIC))
		    return;
	    } else {
		LS.HandlerIO.S = Wait4Auth;
		if (SendUldat(WAIT_MAGIC) && SendChallenge())
		    return;
	    }
	}
    }
    
    /* problem... out of memory or I/O error. */
    UnRegisterRemote(Slot);
    close(fd);
}

static void Wait4MagicUnix(int fd, uldat slot) {
    Wait4Magic(fd, slot, TRUE);
}

static void Wait4MagicInet(int fd, uldat slot) {
    Wait4Magic(fd, slot, FALSE);
}


static void unixSocketIO(int fd, uldat slot) {
    struct sockaddr_un un_addr;
    int len = sizeof(un_addr);
    if ((fd = accept(unixFd, (struct sockaddr *)&un_addr, &len)) >= 0) {
	/* programs on the unix socket are always authorized */
	if ((Slot = RegisterRemoteFd(fd, Wait4MagicUnix)) != NOSLOT) {
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
	if ((Slot = RegisterRemoteFd(fd, Wait4MagicInet)) != NOSLOT) {
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
    size_t tot;
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
		    sockDecode(Funct); /* Slot is the uncompressed socket here ! */
		    Slot = slot;	/*
					 * restore, in case sockF[Funct].F() changed it;
					 * without this, tw* clients can freeze
					 * if twdisplay is in use
					 */
		}
		else if (Funct == FIND_MAGIC)
		    sockDecode(0);
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
#if defined(CONF__MODULES) || defined (CONF_HW_DISPLAY)
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
	    PushV(t, Msg->Event.EventKeyboard.SeqLen+1, Msg->Event.EventKeyboard.AsciiSeq);
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
	if (Easy && sizeof(event_menu) == sizeof(window) + 2*sizeof(udat) + sizeof(menu))
	    break;
	else
	    Easy = FALSE;
	sockReply(Msg->Type, Len = sizeof(twindow) + 2*sizeof(dat) + sizeof(tmenu), NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    Push(t, twindow, Obj2Id(Msg->Event.EventMenu.W));
	    Push(t, udat,    Msg->Event.EventMenu.Code);
	    Push(t, udat,    Msg->Event.EventMenu.pad);
	    Push(t, tmenu,   Obj2Id(Msg->Event.EventMenu.Menu));
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
	    3*sizeof(ldat) + MAX_MIMELEN + sizeof(byte *))
	    break;
	else
	    Easy = FALSE;
	sockReply(Msg->Type, Len = sizeof(twindow) + 2*sizeof(dat) + 3*sizeof(ldat)
	      + MAX_MIMELEN + Msg->Event.EventKeyboard.SeqLen, NULL);
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
	sockReply(Msg->Type, Len = sizeof(twindow) + 2*sizeof(dat) + 2*sizeof(ldat), NULL);
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
	sockReply(Msg->Type, Len = sizeof(twindow) + 4*sizeof(dat) + sizeof(byte)
	      + Msg->Event.EventControl.Len, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    Push(t, twidget, Obj2Id(Msg->Event.EventControl.W));
	    Push(t, udat,    Msg->Event.EventControl.Code);
	    Push(t, udat,    Msg->Event.EventControl.Len);
	    Push(t, dat,     Msg->Event.EventControl.X);
	    Push(t, dat,     Msg->Event.EventControl.Y);
	    PushV(t, Msg->Event.EventControl.Len + 1 /*the '\0'*/, Msg->Event.EventControl.Data);
	}
	break;
	
      case MSG_USER_CLIENTMSG:
	if (Easy && sizeof(event_clientmsg) == sizeof(window) + 2*sizeof(dat) + sizeof(uldat))
	    break;
	else
	    Easy = FALSE;
	sockReply(Msg->Type, Len = sizeof(twindow) + 2*sizeof(dat) + Msg->Event.EventClientMsg.Len, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    Push(t, twidget, Obj2Id(Msg->Event.EventClientMsg.W));
	    Push(t, udat,    Msg->Event.EventClientMsg.Code);
	    Push(t, udat,    Msg->Event.EventClientMsg.Len);
	    PushV(t, Msg->Event.EventClientMsg.Len, Msg->Event.EventClientMsg.Data);
	}
	break;
	
      default:
	Easy = FALSE;
    }
    if (Easy) {
	Msg->Event.EventCommon.W = (void *)Obj2Id(Msg->Event.EventCommon.W);
	if (Msg->Type == MSG_MENU_ROW)
	    Msg->Event.EventMenu.Menu = (void *)Obj2Id(Msg->Event.EventMenu.Menu);
	else if (Msg->Type == MSG_SELECTIONREQUEST)
	    Msg->Event.EventSelectionRequest.Requestor
	    = (void *)Obj2Id(Msg->Event.EventSelectionRequest.Requestor);
	sockReply(Msg->Type, Msg->Len, &Msg->Event);
    }
    Slot = save_Slot;
    Fd = save_Fd;
}


#ifdef CONF_SOCKET_ALIEN
# include "socketalien.h"
#endif

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


#ifdef CONF_THIS_MODULE

# include "version.h"
MODULEVERSION;

static void (*orig_unixSocketIO)(int fd, uldat slot);

byte InitModule(module Module)
#else
byte InitSocket(void)
#endif
{
    uldat m;
    struct sockaddr_in addr;
    char opt[15];

    if (!InitAuth()) {
	printk("twin: failed to create ~/.TwinAuth: %s\n", ErrStr);
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
	
	RegisterExtension(Remote,KillSlot,sockKillSlot);
	RegisterExtension(Socket,SendMsg,sockSendMsg);
	RegisterExtension(Socket,InitAuth,InitAuth);

	m = TWIN_MAGIC;
	CopyMem(&m, TwinMagicData+TwinMagicData[0]-sizeof(uldat), sizeof(uldat));

	if (unixSlot != NOSLOT) {
#ifdef CONF_THIS_MODULE
	    orig_unixSocketIO = FdList[unixSlot].HandlerIO.S;
#endif
	    FdList[unixSlot].HandlerIO.S = unixSocketIO;
	}

	return TRUE;
    }
    printk("twin: failed to create sockets: %s\n", ErrStr);
    return FALSE;
}

#ifdef CONF_THIS_MODULE
void QuitModule(module Module) {
    if (unixSlot != NOSLOT)
	FdList[unixSlot].HandlerIO.S = orig_unixSocketIO;
    
    if (inetSlot != NOSLOT) {
	UnRegisterRemote(inetSlot);
	close(inetFd);
    }
    for (Slot = 0; Slot < FdTop; Slot++) {
	if (LS.Fd != NOFD && (LS.HandlerIO.S == Wait4MagicUnix ||
			      LS.HandlerIO.S == Wait4MagicInet ||
			      LS.HandlerIO.S == Wait4Auth ||
# ifdef CONF_SOCKET_ALIEN
			      LS.HandlerIO.S == AlienIO ||
# endif
			      LS.HandlerIO.S == SocketIO)) {
	    Ext(Remote,KillSlot)(Slot);
	}
    } 
    UnRegisterExtension(Remote,KillSlot,sockKillSlot);
    UnRegisterExtension(Socket,SendMsg,sockSendMsg);
    UnRegisterExtension(Socket,InitAuth,InitAuth);
}
#endif
