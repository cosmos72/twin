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

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/utsname.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef CONF_SOCKET_GZ
# include <zlib.h>
#endif

#include "twin.h"
#include "methods.h"
#include "data.h"
#include "main.h"
#include "printk.h"
#include "util.h"
#include "resize.h"
#include "extensions.h"

#include "fdlist.h"
#include "remote.h"
#include "socket.h"
#include "socketprivate.h"
#include "md5.h"
#include "hw_multi.h"
#include "common.h"
#include "version.h"

#include "Tw/Tw.h"
#include "Tw/Twkeys.h"

#if defined(CONF_SOCKET_ALIEN) || defined(CONF__MODULES)
# ifdef CONF_SOCKET_ALIEN
   void SocketAlienIO(int fd, uldat slot);
   byte InitSocketAlien(void);
   void QuitSocketAlien(void);
   byte SocketAlienOK;
# else
#  include "dl.h"
# endif
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

byte *RemoteReadGrowQueue(uldat Slot, uldat len) {
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

uldat RemoteReadShrinkQueue(uldat Slot, uldat len) {
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

uldat RemoteReadDeQueue(uldat Slot, uldat len) {    
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

byte *RemoteReadGetQueue(uldat Slot, uldat *len) {
    if (Slot >= FdTop || LS.Fd == NOFD || !LS.RQlen) {
	if (len) *len = 0;
	return NULL;
    }
    if (len)
	*len = LS.RQlen;
    return LS.RQueue + LS.RQstart;
}

byte *RemoteWriteGetQueue(uldat Slot, uldat *len) {
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
byte RemoteGzip(uldat Slot) {
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

byte RemoteGunzip(uldat Slot) {
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

static uldat FindFunction(byte Len, CONST byte *name);
static byte SyncSocket(void);
static byte CanCompress(void);
static byte DoCompress(byte on_off);

static void NeedResizeDisplay(void);
static void AttachHW(uldat len, CONST byte *arg, byte flags);
static byte DetachHW(uldat len, CONST byte *arg);
static void SetFontTranslation(CONST byte trans[0x80]);

static void DeleteObj(void *V);

static widget CreateWidget(dat XWidth, dat YWidth, hwattr Fill, dat Left, dat Up);
static void RecursiveDeleteWidget(void *V);
static msgport GetOwnerWidget(widget W);

static gadget CreateGadget
    (widget Parent, dat XWidth, dat YWidth, CONST byte *TextNormal, udat Code, udat Flags,
     hwcol ColText, hwcol ColTextSelect, hwcol ColTextDisabled, hwcol ColTextSelectDisabled,
     dat Left, dat Up,       CONST byte *TextSelect, CONST byte *TextDisabled, CONST byte *TextSelectDisabled,
     CONST hwcol *ColNormal, CONST hwcol *ColSelect, CONST hwcol *ColDisabled, CONST hwcol *ColSelectDisabled);
static void SetPressedGadget(gadget G, byte on_off);
static byte IsPressedGadget(gadget G);
static void SetToggleGadget(gadget G, byte on_off);
static byte IsToggleGadget(gadget G);

static void ResizeWindow(window Window, dat X, dat Y);

static msgport CreateMsgPort(byte LenTitle, CONST byte *Title, time_t WakeUp, frac_t WakeUpFrac, byte Flags);
static msgport FindMsgPort(msgport Prev, byte LenTitle, CONST byte *Title);

static screen FirstScreen(void);
static widget FirstWidget(widget W);
static msgport FirstMsgPort(void);
static menu FirstMenu(msgport MsgPort);
static menuitem FirstMenuItem(menu Menu);

static obj PrevObj(obj Obj);
static obj NextObj(obj Obj);
static obj ParentObj(obj Obj);

static byte SendToMsgPort(msgport MsgPort, udat Len, CONST byte *Data);
static void BlindSendToMsgPort(msgport MsgPort, udat Len, CONST byte *Data);

static obj  GetOwnerSelection(void);
static void SetOwnerSelection(time_t Time, frac_t Frac);
static void NotifySelection(obj Requestor, uldat ReqPrivate,
			       uldat Magic, CONST byte MIME[MAX_MIMELEN], uldat Len, CONST byte *Data);
static void RequestSelection(obj Owner, uldat ReqPrivate);

#if defined(CONF_SOCKET_ALIEN) || defined(CONF__MODULES)
socket_f SocketF = {
	FindFunction,
	SyncSocket,
	CanCompress,
	DoCompress,

	NeedResizeDisplay,
	AttachHW,
	DetachHW,
	SetFontTranslation,
    
	DeleteObj,
    
	CreateWidget,
	RecursiveDeleteWidget,
	GetOwnerWidget,
    
	CreateGadget,
	SetPressedGadget,
	IsPressedGadget,
	SetToggleGadget,
	IsToggleGadget,
    
	ResizeWindow,
    
	CreateMsgPort,
	FindMsgPort,
    
	FirstScreen,
	FirstWidget,
	FirstMsgPort,
	FirstMenu,
	FirstMenuItem,

	PrevObj,
	NextObj,
	ParentObj,
    
	GetDisplayWidth,	/* from h_multi.c */
	GetDisplayHeight,	/* from h_multi.c */

	SendToMsgPort,
	BlindSendToMsgPort,
    
	GetOwnerSelection,
	SetOwnerSelection,
	NotifySelection,
	RequestSelection
};
#endif /* defined(CONF_SOCKET_ALIEN) || defined(CONF__MODULES) */


/* Second: socket handling functions */

static uldat MaxFunct, Slot, RequestN, noId = NOID;
static byte *s, *end;
static int inetFd = NOFD, Fd;
static uldat inetSlot = NOSLOT;

static void sockReply(uldat code, uldat len, CONST void *data);

static void SocketIO(int fd, uldat slot);

#include "unaligned.h"

#define ACT0(funct,name)	funct##name
#define NAME_PREFIX		sock
#define SIZEOF(arg)		sizeof(arg)
#define POP(s,type,lval)	Pop(s,type,lval)
#define POPADDR(s,type,len,ptr)	PopAddr(s,type,len,ptr)
#define REPLY(code, type, lval) sockReply(code, sizeof(type), lval)

#include "socketmacros.h"
#include "sockproto.h"

static FN_DECL {
#   define EL(funct) { 0, #funct, CAT(NAME_PREFIX,funct) },
#   include "sockprotolist.h"
#   undef EL
FN_ENDDECL };

#include "socketundefs.h"

#undef ACT0
#undef NAME_PREFIX
#undef SIZEOF
#undef POP
#undef POPADDR


/***********/


/* actual libTw back-end utility functions */

static uldat FindFunction(byte Len, CONST byte *name) {
    sock_fn *F = sockF;
    if (name) {
	while (F->name && (F->Len != Len || CmpMem(F->name, name, Len)))
	    F++;
	if (F->name)
	    return (uldat)(F - sockF);
    }
    return NOID;
}

static byte SyncSocket(void) {
    return TRUE;
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
    if (trans)
	CopyMem(trans, All->Gtranslations[USER_MAP], 0x80);
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

static widget CreateWidget(dat XWidth, dat YWidth, hwattr Fill, dat Left, dat Up) {
    msgport Owner;
    if ((Owner = RemoteGetMsgPort(Slot)))
	return Do(Create,Widget)
	    (FnWidget, Owner, XWidth, YWidth, Fill, Left, Up);
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

static gadget CreateGadget
    (widget Parent, dat XWidth, dat YWidth, CONST byte *TextNormal, udat Code, udat Flags,
     hwcol ColText, hwcol ColTextSelect, hwcol ColTextDisabled, hwcol ColTextSelectDisabled,
     dat Left, dat Up,       CONST byte *TextSelect, CONST byte *TextDisabled, CONST byte *TextSelectDisabled,
     CONST hwcol *ColNormal, CONST hwcol *ColSelect, CONST hwcol *ColDisabled, CONST hwcol *ColSelectDisabled) {
    
    msgport Owner;
    if ((Owner = RemoteGetMsgPort(Slot)))
	return Do(Create,Gadget)
	    (FnGadget, Owner, Parent, XWidth, YWidth, TextNormal, Code, Flags,
		 ColText, ColTextSelect, ColTextDisabled, ColTextSelectDisabled,
		 Left, Up, TextSelect, TextDisabled, TextSelectDisabled,
		 ColNormal, ColSelect, ColDisabled, ColSelectDisabled);
    return (gadget)0;
}
static void SetPressedGadget(gadget G, byte on_off) {
    if (G) {
	if (on_off)
	    PressGadget(G);
	else
	    UnPressGadget(G, TRUE);
    }
}
static byte IsPressedGadget(gadget G) {
    return G ? !!(G->Flags & GADGET_PRESSED) : FALSE;
}
static void SetToggleGadget(gadget G, byte on_off) {
    if (G) {
	if (on_off)
	    G->Flags |= GADGET_TOGGLE;
	else
	    G->Flags &= ~GADGET_TOGGLE;
    }
}
static byte IsToggleGadget(gadget G) {
    return G ? !!(G->Flags & GADGET_TOGGLE) : FALSE;
}


static void ResizeWindow(window Window, dat X, dat Y) {
    if (Window) {
	if (!(Window->Flags & WINFL_BORDERLESS))
	    X+=2, Y+=2;
	ResizeRelWindow(Window, X - Window->XWidth, Y - Window->YWidth);
    }
}

/* last 3 args are currently useless for remote clients */
static msgport CreateMsgPort(byte LenTitle, CONST byte *Title, time_t WakeUp, frac_t WakeUpFrac, byte Flags) {
    msgport MsgPort;
    
    if ((MsgPort = Do(Create,MsgPort)(FnMsgPort, LenTitle, Title, 0, 0, 0, SocketH))) {
	RegisterMsgPort(MsgPort, Slot);
	MsgPort->ShutDownHook = sockShutDown;
    }
    return MsgPort;
}
static msgport FindMsgPort(msgport Prev, byte LenTitle, CONST byte *Title) {
    msgport M;
    if (!(M = Prev))
	M = All->FirstMsgPort;
    while (M) {
	if (M->NameLen == LenTitle && !CmpMem(M->ProgramName, Title, LenTitle))
	    break;
	M = M->Next;
    }
    return M;
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
static menuitem FirstMenuItem(menu Menu) {
    return Menu ? Menu->FirstMenuItem : (void *)Menu;
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
	  case TW_MSG_WINDOW_KEY:
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
		
	    Msg->Event.EventCommon.Window =
		(void *)Id2Obj(window_magic >> magic_shift,
			       tMsg->Event.EventCommon.Window);

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
	      case TW_MSG_WINDOW_KEY:
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
	      case TW_MSG_WINDOW_MOUSE:
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
	      case TW_MSG_WINDOW_CHANGE:
		Push(t, udat,    Msg->Event.EventWindow.Code);
		Push(t, udat,    Msg->Event.EventWindow.XWidth);
		Push(t, udat,    Msg->Event.EventWindow.YWidth);
		break;
	      case TW_MSG_WINDOW_GADGET:
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

#if defined(CONF_SOCKET_ALIEN) || defined(CONF__MODULES)

#if TW_BYTE_ORDER == TW_LITTLE_ENDIAN
/* we can use hton?() functions to speed up translation */
#include <netinet/in.h>
static void FlipCopyMem(CONST byte *src, byte *dst, uldat len) {
    switch (len) {
      case 2:
	if (sizeof(udat) == 2) {
	    udat x = *(udat *)src;
	    *(udat *)dst = htons(x);
	}
	else if (sizeof(unsigned short) == 2) {
	    unsigned short x = *(unsigned short *)src;
	    *(unsigned short *)dst = htons(x);
	} else {
	    dst[0] = src[1];
	    dst[1] = src[0];
	}
	break;
      case 4:
	if (sizeof(uldat) == 4) {
	    uldat x = *(uldat *)src;
	    *(uldat *)dst = htonl(x);
	    break;
	}
	else if (sizeof(unsigned short) == 4) {
	    unsigned short x = *(unsigned short *)src;
	    *(unsigned short *)dst = htonl(x);
	}
	else if (sizeof(unsigned int) == 4) {
	    unsigned int x = *(unsigned int *)src;
	    *(unsigned int *)dst = htonl(x);
	    break;
	}
	/* else FALLTHROUGH */
      default:
	src += len - 1;
	while (len--)
	    *dst++ = *src--;
	break;
    }
}

#else
static void FlipCopyMem(CONST byte *src, byte *dst, uldat len) {
    src += len - 1;
    while (len--)
	*dst++ = *src--;
}
#endif /* TW_BYTE_ORDER == TW_LITTLE_ENDIAN */


static void SocketAlienTranslate(CONST byte *src, uldat srclen, byte *dst, uldat dstlen, byte flip) {
    
#if TW_BYTE_ORDER == TW_LITTLE_ENDIAN
    
    /* copy the least significant bits */
    if (flip)
	FlipCopyMem(src, dst, Min2(dstlen, srclen));
    else
	CopyMem(src, dst, Min2(dstlen, srclen));
    /* and set the remaining to zero */
    if (dstlen > srclen)
	WriteMem(dst + srclen, '\0', dstlen - srclen);
    
#else /* TW_BYTE_ORDER == TW_BIG_ENDIAN */
    
    if (dstlen > srclen) {
	/* set the high bits to zero */
	WriteMem(dst, '\0', dstlen - srclen);
	/* copy the least significant bits */
	if (flip)
	    FlipCopyMem(src, dst + dstlen - srclen, Min2(dstlen, srclen));
	else
	    CopyMem(src, dst + dstlen - srclen, Min2(dstlen, srclen));
    } else {
	/* copy the least significant bits */
	if (flip)
	    FlipCopyMem(src, dst, dstlen);
	else
	    CopyMem(src, dst, dstlen);
    }
#endif /* TW_BYTE_ORDER == TW_LITTLE_ENDIAN */
}

/* convert alien type at (*src) to native and put it at (dst) */
void SocketAlienPop(CONST byte ** src, uldat alien_len, byte *dst, uldat len) {
    SocketAlienTranslate(*src, alien_len, dst, len, AlienXendian(Slot) == MagicAlienXendian);
    *src += alien_len;
}

/* convert native type at (src) to alien and put it at (*dst) */
void SocketAlienPush(CONST byte *src, uldat len, byte **dst, uldat alien_len) {
    SocketAlienTranslate(src, len, *dst, alien_len, AlienXendian(Slot) == MagicAlienXendian);
    *dst += alien_len;
}


#endif /* defined(CONF_SOCKET_ALIEN) || defined(CONF__MODULES) */


static byte TwinMagicData[8+sizeof(uldat)] = {
    8+sizeof(uldat),
    sizeof(byte),
    sizeof(udat),
    sizeof(uldat),
    sizeof(hwcol),
    sizeof(time_t),
    sizeof(frac_t),
    0
};

static uldat SendTwinProtocol(void) {
    static byte buf[] = " Twin-" TW_PROTOCOL_VERSION_STR "-" TWIN_VERSION_STR "\n";
    buf[0] = strlen(buf);
    return RemoteWriteQueue(Slot, buf[0], buf);
}

static byte SendTwinMagic(byte *t, byte len) {
    if (AlienXendian(Slot) == MagicNative)
	return RemoteWriteQueue(Slot, 8+sizeof(uldat), TwinMagicData) == 8+sizeof(uldat);
    
    return RemoteWriteQueue(Slot, len, t) == len;
}
    
static void sockReply(uldat code, uldat len, CONST void *data) {
    uldat buf[3];
    buf[0] = 2*sizeof(uldat) + len;
    buf[1] = RequestN;
    buf[2] = code;
    if (RemoteWriteQueue(Slot, 3*sizeof(uldat), buf) == 3*sizeof(uldat) && len)
	(void)RemoteWriteQueue(Slot, len, data);
}

static byte SendUldat(uldat data) {

#if defined(CONF_SOCKET_ALIEN) || defined(CONF__MODULES)

    if (AlienXendian(Slot) == MagicNative)
	return RemoteWriteQueue(Slot, sizeof(uldat), &data) == sizeof(uldat);

    {
	uldat len;
	byte *t, AlienSizeofUldat = AlienSizeof(uldat, Slot);
	
	if (RemoteWriteQueue(Slot, AlienSizeofUldat, NULL) == AlienSizeofUldat) {
	    t = RemoteWriteGetQueue(Slot, &len);
	    t += len - AlienSizeofUldat;
	    AlienPush(t,uldat,data);
	    return TRUE;
	}
	return FALSE;
    }
#else /* !defined(CONF_SOCKET_ALIEN) && !defined(CONF__MODULES) */
    
    return RemoteWriteQueue(Slot, sizeof(uldat), &data) == sizeof(uldat);
    
#endif /* defined(CONF_SOCKET_ALIEN) || defined(CONF__MODULES) */
    
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
#if defined(CONF_SOCKET_ALIEN) || defined(CONF__MODULES)
    if (AlienXendian(Slot) != MagicNative) {
		
# ifdef CONF_SOCKET_ALIEN
	return SocketAlienIO;
# else /* !CONF_SOCKET_ALIEN */
	module M = DlLoad(SocketAlienSo);
	if (M)
	    return (handler_io_s)M->Private;
	return NULL;
# endif /* CONF_SOCKET_ALIEN */
    }
#endif /* defined(CONF_SOCKET_ALIEN) || defined(CONF__MODULES) *//
	
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

    
    if (len == 8+sizeof(uldat) && !CmpMem(magic, TwinMagicData, 8+sizeof(uldat)))
	return MagicNative;
    
#if defined(CONF_SOCKET_ALIEN) || defined(CONF__MODULES)

    if (
# ifdef CONF_SOCKET_ALIEN
	SocketAlienOK &&
# else
	DlLoad(SocketAlienSo) &&
# endif
	len >= 8+sizeof(uldat) &&
	magic[Alien_byte] == 1 &&  /* sizeof(byte) MUST be 1, or passing byte[] vectors would fail */
	magic[Alien_udat] >= 2 &&
	magic[Alien_uldat] >= 4 &&
	magic[Alien_hwcol] == 1 && /* sizeof(hwcol) MUST be 1, or passing hwcol[] vectors would fail */
	magic[Alien_time_t] >= 4 &&
	magic[Alien_frac_t] >= 4 &&
	len == magic[0] && len == 8+magic[Alien_uldat]) {
	    
	CopyMem(magic + 1, AlienMagic(slot) + 1, 6);
	AlienSizeof(hwattr,Slot) = AlienSizeof(byte,Slot) + AlienSizeof(hwcol,Slot);

	/*
	 * now check endianity.
	 * 
	 * non-trivial trick: TWIN_MAGIC significant bits are 0-31,
	 * so on little endian machines (magic+8) is always "Twin"...
	 * no matter what value has magic[Alien_uldat].
	 * On big endian machines, (magic+8) is ..."niwT", with
	 * (magic[Alien_uldat] - 4) zeroed bytes at start.
	 */
	if (!CmpMem(magic+8, "Twin", 4))
	    /* little endian client. and us? */
	    return TW_BYTE_ORDER == TW_LITTLE_ENDIAN ? MagicAlien : MagicAlienXendian;
	if (!CmpMem(magic+8+(magic[Alien_uldat]-4), "niwT", 4))
	    /* big endian client. and us? */
	    return TW_BYTE_ORDER == TW_BIG_ENDIAN ? MagicAlien : MagicAlienXendian;
    }

#endif /* defined(CONF_SOCKET_ALIEN) || defined(CONF__MODULES) */

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
	if ((AlienXendian(Slot) = Check4MagicTranslation(Slot, t, max)) == MagicUnknown)
	    /*
	     * no suitable translation available. use our native magic,
	     * in case library can handle it.
	     */
	    AlienXendian(Slot) = MagicNative;
	
	got = SendTwinMagic(t, max);
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
		    sockF[Funct].F();   /* Slot is the uncompressed socket here ! */
		    Slot = slot;	/*
					 * restore, in case sockF[Funct].F() changed it;
					 * without this, tw* clients can freeze
					 * if twdisplay is in use
					 */
		}
		else if (Funct == FIND_MAGIC)
		    sockFindFunction();
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
    uldat Len = 0, Tot;
    byte *t, Easy = sizeof(twindow) == sizeof(window);

    uldat save_Slot = Slot;
    int save_Fd = Fd;
    
    RequestN = MSG_MAGIC;
    Fd = MsgPort->RemoteData.Fd;
    Slot = MsgPort->RemoteData.FdSlot;

    if (AlienXendian(Slot) != MagicNative) {
	if (Ext(SocketAlien,SendMsg))
	    Ext(SocketAlien,SendMsg)(MsgPort,Msg);
	Slot = save_Slot;
	Fd = save_Fd;
	return;
    }
	
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
      case MSG_WINDOW_KEY:
	if (Easy && sizeof(event_keyboard) == sizeof(window) + 3*sizeof(dat) + 2*sizeof(byte))
	    break;
	else
	    Easy = FALSE;
	sockReply(Msg->Type, Len = sizeof(twindow) + 3*sizeof(udat) + 2*sizeof(byte) + Msg->Event.EventKeyboard.SeqLen, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    Push(t, twindow, Obj2Id(Msg->Event.EventKeyboard.Window));
	    Push(t, udat,    Msg->Event.EventKeyboard.Code);
	    Push(t, udat,    Msg->Event.EventKeyboard.ShiftFlags);
	    Push(t, udat,    Msg->Event.EventKeyboard.SeqLen);
	    Push(t, byte,    Msg->Event.EventKeyboard.pad);
	    PushV(t, Msg->Event.EventKeyboard.SeqLen+1, Msg->Event.EventKeyboard.AsciiSeq);
	}
	break;
      case MSG_WINDOW_MOUSE:
	if (Easy && sizeof(event_mouse) == sizeof(window) + 4*sizeof(dat))
	    break;
	else
	    Easy = FALSE;
	sockReply(Msg->Type, Len = sizeof(twindow) + 4*sizeof(udat), NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    Push(t, twindow, Obj2Id(Msg->Event.EventMouse.Window));
	    Push(t, udat,    Msg->Event.EventMouse.Code);
	    Push(t, udat,    Msg->Event.EventMouse.ShiftFlags);
	    Push(t, dat,     Msg->Event.EventMouse.X);
	    Push(t, dat,     Msg->Event.EventMouse.Y);
	}
	break;
      case MSG_WINDOW_CHANGE:
	if (Easy && sizeof(event_window) == sizeof(window) + 6*sizeof(dat))
	    break;
	else
	    Easy = FALSE;
	sockReply(Msg->Type, Len = sizeof(twindow) + 6*sizeof(dat), NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    Push(t, twindow, Obj2Id(Msg->Event.EventWindow.Window));
	    Push(t, udat,    Msg->Event.EventWindow.Code);
	    Push(t, udat,    Msg->Event.EventWindow.Flags);
	    Push(t, udat,    Msg->Event.EventWindow.XWidth);
	    Push(t, udat,    Msg->Event.EventWindow.YWidth);
	    Push(t, udat,    Msg->Event.EventWindow.X);
	    Push(t, udat,    Msg->Event.EventWindow.Y);
	}
	break;
      case MSG_WINDOW_GADGET:
	if (Easy && sizeof(event_gadget) == sizeof(window) + 2*sizeof(dat))
	    break;
	else
	    Easy = FALSE;
	sockReply(Msg->Type, Len = sizeof(twindow) + 2*sizeof(dat), NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    Push(t, twindow, Obj2Id(Msg->Event.EventGadget.Window));
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
	    Push(t, twindow, Obj2Id(Msg->Event.EventMenu.Window));
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
	    Push(t, twindow, Obj2Id(Msg->Event.EventSelection.Window));
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
	    Push(t, twindow, Obj2Id(Msg->Event.EventSelectionNotify.Window));
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
	    Push(t, twindow, Obj2Id(Msg->Event.EventSelectionRequest.Window));
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
	    Push(t, twindow, Obj2Id(Msg->Event.EventControl.Window));
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
	    Push(t, twindow, Obj2Id(Msg->Event.EventClientMsg.Window));
	    Push(t, udat,    Msg->Event.EventClientMsg.Code);
	    Push(t, udat,    Msg->Event.EventClientMsg.Len);
	    PushV(t, Msg->Event.EventClientMsg.Len, Msg->Event.EventClientMsg.Data);
	}
	break;
	
      default:
	Easy = FALSE;
    }
    if (Easy) {
	Msg->Event.EventCommon.Window = (void *)Obj2Id(Msg->Event.EventCommon.Window);
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

	
static void SocketH(msgport MsgPort) {
    msg Msg;
    byte buf[10], len;
    window W;
    
    while ((Msg=MsgPort->FirstMsg)) {
	Remove(Msg);

	if (Msg->Type==MSG_WINDOW_MOUSE && (W = Msg->Event.EventMouse.Window) &&
	    (W->Flags & WINFL_USECONTENTS) && W->USE.C.TtyData &&
	    W->USE.C.TtyData->Flags & (TTY_REPORTMOUSE|TTY_REPORTMOUSE2)) {
	    
	    len = CreateXTermMouseEvent(&Msg->Event.EventMouse, 10, buf);
	    /*
	     * SyntheticKey() will send an appropriate keyboard message to this MsgPort
	     * and we will sockSendMsg() it later in the while() loop.
	     */
	    if (len)
		SyntheticKey(Msg->Event.EventMouse.Window, TW_XTermMouse, 0, len, buf);
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
	for (MaxFunct = 0; sockF[MaxFunct].name; MaxFunct++)
	    sockF[MaxFunct].Len = strlen(sockF[MaxFunct].name);
    
	RegisterExtension(Remote,KillSlot,sockKillSlot);
	RegisterExtension(Socket,SendMsg,sockSendMsg);

	*(uldat *)(TwinMagicData+8) = TWIN_MAGIC;

	if (unixSlot != NOSLOT) {
#ifdef CONF_THIS_MODULE
	    orig_unixSocketIO = FdList[unixSlot].HandlerIO.S;
#endif
	    FdList[unixSlot].HandlerIO.S = unixSocketIO;
	}

#ifdef CONF_SOCKET_ALIEN
	if (!(SocketAlienOK = InitSocketAlien()))
	    printk("twin: InitSocket: InitSocketAlien() failed.\n");
#endif
	return TRUE;
    }
    printk("twin: failed to create sockets: %s\n", ErrStr);
    return FALSE;
}

#ifdef CONF_THIS_MODULE
void QuitModule(module Module) {
# if !defined(CONF_SOCKET_ALIEN) && defined(CONF__MODULES)
    module M = DlIsLoaded(SocketAlienSo);
    handler_io_s SocketAlienIO = (handler_io_s) (M ? M->Private : NULL);
# endif
    
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
# if defined(CONF_SOCKET_ALIEN) || defined(CONF__MODULES)
			      LS.HandlerIO.S == SocketAlienIO ||
# endif
			      LS.HandlerIO.S == SocketIO)) {
	    Ext(Remote,KillSlot)(Slot);
	}
    } 
# if defined(CONF_SOCKET_ALIEN)
    QuitSocketAlien();
# elif defined(CONF__MODULES)
    if (M)
	DlUnLoad(SocketAlienSo);
# endif    
    UnRegisterExtension(Remote,KillSlot,sockKillSlot);
    UnRegisterExtension(Socket,SendMsg,sockSendMsg);
}
#endif
