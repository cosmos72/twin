/*
 *  socket.c  --  remote libTw server implementation: supports unix and inet sockets
 *
 *  Copyright (C) 1999-2000 by Massimiliano Ghilardi
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
#include <zlib.h>
#endif

#include "twin.h"
#include "methods.h"
#include "data.h"

#include "main.h"
#include "util.h"
#include "remote.h"
#include "md5.h"
#include "resize.h"
#include "hw.h"
#include "hw_multi.h"
#include "common.h"

#include "libTw.h"

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

static byte *RemoteWriteGetQueue(uldat Slot, uldat *len) {
    if (Slot >= FdTop || LS.Fd == NOFD || !LS.WQlen) {
	if (len) *len = 0;
	return NULL;
    }
    if (len)
	*len = LS.WQlen;
    return LS.WQueue;
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
    LS.WQlen = LS.WQmax; /* alloc them */
    return LS.WQueue + LS.WQmax - delta; /* return the address of first one */
}



#ifdef CONF_SOCKET_GZ

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
		    ls.WQlen -= delta;
		    z->next_out = RemoteWriteFillQueue(slot, &z->avail_out);
		} else /* out of memory ! */
		    break;
	    }
	    
	    zret = deflate(z, Z_SYNC_FLUSH);

	    /* update the compressed queue */
	    ls.WQlen -= z->avail_out;
	}
    }
    /* update the uncompressed queue */
    if (z->avail_in)
	CopyMem(LS.WQueue + LS.WQlen - z->avail_in, LS.WQueue, z->avail_in);
    LS.WQlen = z->avail_in;
    
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


/* back-end of some libTw utility functions */


static void AttachHW(uldat len, byte *arg, byte redirect);
static byte DetachHW(uldat len, byte *arg);
static void SetFontTranslation(byte trans[0x80]);

static byte CanCompress(void);
static byte DoCompress(byte on_off);

static void NeedResizeDisplay(void);

static void sockShutDown(msgport *MsgPort) {
    if (MsgPort->RemoteData.FdSlot < FdTop)
	UnRegisterMsgPort(MsgPort);
}

static byte SyncSocket(void) {
    return TRUE;
}

static void ResizeWindow(window *Window, udat X, udat Y) {
    if (Window)
	ResizeRelWindow(Window, X - Window->XWidth, Y - Window->YWidth);
}

static screen *FirstScreen(void) {
    return All->FirstScreen;
}
static window *FirstWindow(screen *Screen) {
    return Screen ? Screen->FirstWindow : (void *)Screen;
}
static gadget *FirstGadget(window *Window) {
    return Window ? Window->FirstGadget : (void *)Window;
}
static msgport *FirstMsgPort(void) {
    return All->FirstMsgPort;
}
static menu *FirstMenu(msgport *MsgPort) {
    return MsgPort ? MsgPort->FirstMenu : (void *)MsgPort;
}
static menuitem *FirstMenuItem(menu *Menu) {
    return Menu ? Menu->FirstMenuItem : (void *)Menu;
}


static obj *PrevObj(obj *Obj) {
    return Obj ? Obj->Prev : Obj;
}
static obj *NextObj(obj *Obj) {
    return Obj ? Obj->Next : Obj;
}
static obj *ParentObj(obj *Obj) {
    return Obj ? (obj *)Obj->Parent : Obj;
}


INLINE void *CloneMem(void *From, uldat Size) {
    void *temp;
    if (From && Size && (temp = AllocMem(Size)))
	return CopyMem(From, temp, Size);
    return NULL;
}

#define tDelta ((size_t)&(((tmsg)0)->Event))
#define  Delta ((size_t)&(((msg *)0)->Event))

static byte SendToMsgPort(msgport *MsgPort, udat Len, byte *Data) {
    msg *Msg;
    tmsg tMsg = (tmsg)Data;
    uldat _Len;
    byte ok = TRUE;
    
    if (MsgPort && Len && tMsg && Len >= tDelta && Len == tMsg->Len &&
	tMsg->Magic == msg_magic) {
	
	if (tMsg->Type == TW_MSG_WINDOW_KEY)
	    _Len = Delta + tMsg->Event.EventKeyboard.SeqLen;
	else
	    _Len = FnMsg->Size;
	
	if ((Msg = Do(Create,Msg)(FnMsg, tMsg->Type, _Len))) {

	    Msg->Event.EventCommon.Window =
		(void *)Id2Obj(window_magic >> magic_shift,
			       tMsg->Event.EventCommon.Window);

	    switch (tMsg->Type) {
#if defined(CONF_MODULES) || defined (CONF_HW_DISPLAY)
	      case TW_MSG_DISPLAY:

		if (sizeof(struct tevent_display) == sizeof(twindow) + 4*sizeof(dat) + sizeof(byte *)) {
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
#endif /* defined(CONF_MODULES) || defined (CONF_HW_DISPLAY) */
	      case TW_MSG_WINDOW_KEY:
		if (sizeof(struct tevent_keyboard) == sizeof(twindow) + 3*sizeof(dat) + 2*sizeof(byte)) {
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
		if (sizeof(struct tevent_mouse) == sizeof(twindow) + 4*sizeof(dat)) {
		    CopyMem((void *)&tMsg->Event.EventMouse.Code,
			    (void *)& Msg->Event.EventMouse.Code, 4*sizeof(dat));
		} else {
		    Msg->Event.EventMouse.Code          = tMsg->Event.EventMouse.Code;
		    Msg->Event.EventMouse.ShiftFlags    = tMsg->Event.EventMouse.ShiftFlags;
		    Msg->Event.EventMouse.X             = tMsg->Event.EventMouse.X;
		    Msg->Event.EventMouse.Y             = tMsg->Event.EventMouse.Y;
		}
		break;
/* TODO: finish this */
#if 0
	      case MSG_WINDOW_CHANGE:
		Push(t, udat,    Msg->Event.EventWindow.Code);
		Push(t, udat,    Msg->Event.EventWindow.XWidth);
		Push(t, udat,    Msg->Event.EventWindow.YWidth);
		break;
	      case MSG_WINDOW_GADGET:
		Push(t, udat,    Msg->Event.EventGadget.Code);
		Push(t, udat,    Msg->Event.EventGadget.pad);
		break;
	      case MSG_MENU_ROW:
		Push(t, udat,    Msg->Event.EventMenu.Code);
		Push(t, udat,    Msg->Event.EventMenu.pad);
		Push(t, tmenu,   Obj2Id(Msg->Event.EventMenu.Menu));
		break;
	      case MSG_CLIPBOARD:
		Push(t, udat,    Msg->Event.EventClipBoard.Code);
		Push(t, udat,    Msg->Event.EventClipBoard.ShiftFlags);
		Push(t, dat,     Msg->Event.EventClipBoard.X);
		Push(t, dat,     Msg->Event.EventClipBoard.Y);
		Push(t, uldat,   All->ClipMagic);
		Push(t, uldat,   All->ClipLen);
		PushV(t, All->ClipLen, All->ClipData);
		break;
#endif
	      default:
		ok = FALSE;
		break;
	    }
	    
	    if (ok) {
		SendMsg(MsgPort, Msg);
		return TRUE;
	    }
	}
    }
    return FALSE;
}


/* Second: socket handling functions */

#include "unaligned.h"

static uldat MaxFunct, Slot, RequestN, noId = NOID, dummy;
static byte *s, *end;
static int unixFd = NOFD, inetFd = NOFD, Fd;
static uldat unixSlot = NOSLOT, inetSlot = NOSLOT;

static void Reply(uldat code, uldat len, void *data);
static void SocketH(msgport *MsgPort);
static void SocketIO(int fd, uldat slot);

static uldat FindFunction(byte Len, byte *name);

#define ORDER(arg)	(arg##_magic >> magic_shift)

#define ID(s,o)		(s=o, s ? &(s->Id) : &noId)

#define A(n)		a##n
#define AL(n)		len##n

#define FN0(name)
#define FN1(name)	Fn##name,
#define FN2(name)

#define Act1(funct,name) Do(funct,name)
#define Act2(funct,name) Act(funct,A(1))

#define DECLv(n,arg)
#define DECL_(n,arg)	arg A(n);
#define DECLx(n,arg)	uldat AL(n); arg *A(n);
#define DECLV(n,arg)	uldat AL(n); arg *A(n);
#define D(n,arg,f)	DECL##f(n,arg)

#define RETv(ret,call)	call;
#define RET_(ret,call)	Reply(OK_MAGIC, sizeof(ret), (A(0) = call, &A(0)));
#define RETx(ret,call)	Reply(OK_MAGIC, AL(0)=sizeof(uldat), ID(A(0), call));

#define FAILv(n)
#define FAIL_(n)	Reply(n, sizeof(uldat), (dummy = n, &dummy))
#define FAILx(n)	FAIL_(n)

#define RET0(ret,f,call) if (s == end) { RET##f(ret,call) } else FAIL##f(-1);
#define RET1(ret,f,call) if (s == end) { RET##f(ret,call) } else FAIL##f(-1);
#define RET2(ret,f,call) if (s == end && A(1)) { RET##f(ret,call) } else \
				if (A(1)) FAIL##f(1); \
				else FAIL##f(-1);


#define NAME(funct,name)	static void sock##funct##name(void)

#define V(len)		V
#define iV(len)		len

#define Left(size)	(s + (size) <= end)

#define PARSE_(r,n,arg,len,pass) if (Left(sizeof(arg))) { \
				    Pop(s,arg,A(n)); \
				    pass \
				} else \
				    FAIL##r(n);
#define PARSEx(r,n,arg,len,pass) if (Left(sizeof(uldat))) { \
				    Pop(s,uldat,AL(n)); \
				    A(n) = (arg *)Id2Obj(ORDER(arg), AL(n)); \
				    pass \
				} else \
				    FAIL##r(n);
#define PARSEV(r,n,arg,len,pass) if (Left(AL(n) = (len) * sizeof(arg))) { \
				    PopAddr(s, arg, AL(n), A(n)); \
				    pass \
				} else \
				    FAIL##r(n);
#define P(r,n,arg,f,len,pass)	PARSE##f(r,n,arg,len,pass)


#define PROTO0(ret0,f0, funct,name,fn) \
NAME(funct, name) \
{ D(0,ret0,f0) \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name))) \
}

#define PROTO0Abs(ret0,f0, funct,name,fn) \
NAME(funct, name) \
{ D(0,ret0,f0) \
  RET##fn(ret0,f0, (funct##name) ()) \
}

#define PROTO1(ret0,f0, funct,name,fn, arg1,f1) \
NAME(funct, name) \
{ D(0,ret0,f0) \
  D(1,arg1,f1) \
  P(f0,1,arg1,f1,i##f1, \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name) A(1)))) \
}

#define PROTO1Abs(ret0,f0, funct,name,fn, arg1,f1) \
NAME(funct, name) \
{ D(0,ret0,f0) \
  D(1,arg1,f1) \
  P(f0,1,arg1,f1,i##f1, \
  RET##fn(ret0,f0, (funct##name) (A(1)))) \
}

#define PROTO2(ret0,f0, funct,name,fn, arg1,f1, arg2,f2) \
NAME(funct, name) \
{ D(0,ret0,f0) \
  D(1,arg1,f1) D(2,arg2,f2) \
  P(f0,1,arg1,f1,i##f1, P(f0,2,arg2,f2,i##f2, \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name) A(1), A(2))))) \
}

#define PROTO2Abs(ret0,f0, funct,name,fn, arg1,f1, arg2,f2) \
NAME(funct, name) \
{ D(0,ret0,f0) \
  D(1,arg1,f1) D(2,arg2,f2) \
  P(f0,1,arg1,f1,i##f1, P(f0,2,arg2,f2,i##f2, \
  RET##fn(ret0,f0, (funct##name) (A(1), A(2))))) \
}

#define PROTO2FindFunction PROTO2Abs

#define PROTO3(ret0,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3) \
NAME(funct, name) \
{ D(0,ret0,f0) \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) \
  P(f0,1,arg1,f1,i##f1, P(f0,2,arg2,f2,i##f2, P(f0,3,arg3,f3,i##f3, \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name) A(1), A(2), A(3)))))) \
}

#define PROTO3Abs(ret0,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3) \
NAME(funct, name) \
{ D(0,ret0,f0) \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) \
  P(f0,1,arg1,f1,i##f1, P(f0,2,arg2,f2,i##f2, P(f0,3,arg3,f3,i##f3, \
  RET##fn(ret0,f0, funct##name (A(1), A(2), A(3)))))) \
}

#define PROTO4(ret0,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3, arg4,f4) \
NAME(funct, name) \
{ D(0,ret0,f0) \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4,f4) \
  P(f0,1,arg1,f1,i##f1, P(f0,2,arg2,f2,i##f2, P(f0,3,arg3,f3,i##f3, P(f0,4,arg4,f4,i##f4, \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name) A(1), A(2), A(3), A(4))))))) \
}

#define PROTO5(ret0,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5) \
NAME(funct, name) \
{ D(0,ret0,f0) \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4,f4) \
  D(5,arg5,f5) \
  P(f0,1,arg1,f1,i##f1, P(f0,2,arg2,f2,i##f2, P(f0,3,arg3,f3,i##f3, P(f0,4,arg4,f4,i##f4, \
  P(f0,5,arg5,f5,i##f5, \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name) A(1), A(2), A(3), A(4), A(5)))))))) \
}

#define PROTO5CreateMsgPort(ret0,f0, funct,name,fn, arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5) \
NAME(funct, name) \
{ D(0,ret0,f0) \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4,f4) \
  D(5,arg5,f5) \
  if (!RemoteGetMsgPort(Slot)) { \
  P(f0,1,arg1,f1,i##f1, P(f0,2,arg2,f2,i##f2, P(f0,3,arg3,f3,i##f3, P(f0,4,arg4,f4,i##f4, \
  P(f0,5,arg5,f5,i##f5, \
  if (s == end) { \
  RET##f0(ret0, Act##fn(funct,name) (FN##fn(name) A(1), A(2), A(3), A(4), A(5), SocketH)); \
  if (A(0)) { RegisterMsgPort(A(0), Slot); A(0)->ShutDownHook = sockShutDown; } } ))))) \
  } else { Reply(OK_MAGIC, sizeof(uldat), &noId); } \
}

#define PROTO6(ret0,f0, funct,name,fn,arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5, \
	arg6,f6) \
NAME(funct, name) \
{ D(0,ret0,f0) \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4,f4) \
  D(5,arg5,f5) D(6,arg6,f6) \
  P(f0,1,arg1,f1,i##f1, P(f0,2,arg2,f2,i##f2, P(f0,3,arg3,f3,i##f3, P(f0,4,arg4,f4,i##f4, \
  P(f0,5,arg5,f5,i##f5, P(f0,6,arg6,f6,i##f6, \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name) A(1), A(2), A(3), A(4), A(5), A(6))))))))) \
}

#define PROTO7(ret0,f0, funct,name,fn,arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5, \
	arg6,f6, arg7,f7) \
NAME(funct, name) \
{ D(0,ret0,f0) \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4,f4) \
  D(5,arg5,f5) D(6,arg6,f6) D(7,arg7,f7) \
  P(f0,1,arg1,f1,i##f1, P(f0,2,arg2,f2,i##f2, P(f0,3,arg3,f3,i##f3, P(f0,4,arg4,f4,i##f4, \
  P(f0,5,arg5,f5,i##f5, P(f0,6,arg6,f6,i##f6, P(f0,7,arg7,f7,i##f7, \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name) \
      A(1), A(2), A(3), A(4), A(5), A(6), A(7)))))))))) \
}

#define PROTO8(ret0,f0, funct,name,fn,arg1,f1, arg2,f2, arg3,f3, arg4,f4, arg5,f5, \
	arg6,f6, arg7,f7, arg8,f8) \
NAME(funct, name) \
{ D(0,ret0,f0) \
  D(1,arg1,f1) D(2,arg2,f2) D(3,arg3,f3) D(4,arg4,f4) \
  D(5,arg5,f5) D(6,arg6,f6) D(7,arg7,f7) D(8,arg8,f8) \
  P(f0,1,arg1,f1,i##f1, P(f0,2,arg2,f2,i##f2, P(f0,3,arg3,f3,i##f3, P(f0,4,arg4,f4,i##f4, \
  P(f0,5,arg5,f5,i##f5, P(f0,6,arg6,f6,i##f6, P(f0,7,arg7,f7,i##f7, P(f0,8,arg8,f8,i##f8, \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name) \
      A(1), A(2), A(3), A(4), A(5), A(6), A(7), A(8))))))))))) \
}

#define PROTO11(ret0,f0, funct,name,fn,arg1 ,f1 , arg2 ,f2 , arg3 ,f3 , arg4 ,f4 , arg5 ,f5 , \
	   arg6 ,f6 , arg7 ,f7 , arg8 ,f8 , arg9 ,f9 , arg10,f10, arg11,f11) \
NAME(funct, name) \
{ D(0 ,ret0 ,f0 ) \
  D(1 ,arg1 ,f1 ) D(2 ,arg2 ,f2 ) D(3 ,arg3 ,f3 ) D(4 ,arg4, f4 ) \
  D(5 ,arg5 ,f5 ) D(6 ,arg6 ,f6 ) D(7 ,arg7 ,f7 ) D(8 ,arg8 ,f8 ) \
  D(9 ,arg9, f9 ) D(10,arg10,f10) D(11,arg11,f11) \
  P(f0,1 ,arg1 ,f1 ,i##f1 , P(f0,2 ,arg2 ,f2 ,i##f2 , P(f0,3 ,arg3 ,f3 ,i##f3 , P(f0,4 ,arg4 ,f4 ,i##f4 , \
  P(f0,5 ,arg5 ,f5 ,i##f5 , P(f0,6 ,arg6 ,f6 ,i##f6 , P(f0,7 ,arg7 ,f7 ,i##f7 , P(f0,8 ,arg8 ,f8 ,i##f8 , \
  P(f0,9 ,arg9 ,f9 ,i##f9 , P(f0,10,arg10,f10,i##f10, P(f0,11,arg11,f11,i##f11, \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name) \
      A(1 ), A(2 ), A(3 ), A(4 ), A(5 ), A(6 ), A(7 ), A(8 ), A(9 ), A(10), \
      A(11)))))))))))))) \
}

#define PROTO18(ret0,f0, funct,name,fn,   arg1 ,f1 , arg2 ,f2 , arg3 ,f3 , arg4 ,f4 , arg5 ,f5 , \
	   arg6 ,f6 , arg7 ,f7 , arg8 ,f8 , arg9 ,f9 , arg10,f10, arg11,f11, arg12,f12, arg13,f13, \
	   arg14,f14, arg15,f15, arg16,f16, arg17,f17, arg18,f18) \
NAME(funct, name) \
{ D(0 ,ret0 ,f0 ) \
  D(1 ,arg1 ,f1 ) D(2 ,arg2 ,f2 ) D(3 ,arg3 ,f3 ) D(4 ,arg4, f4 ) \
  D(5 ,arg5 ,f5 ) D(6 ,arg6 ,f6 ) D(7 ,arg7 ,f7 ) D(8 ,arg8 ,f8 ) \
  D(9 ,arg9, f9 ) D(10,arg10,f10) D(11,arg11,f11) D(12,arg12,f12) \
  D(13,arg13,f13) D(14,arg14,f14) D(15,arg15,f15) D(16,arg16,f16) \
  D(17,arg17,f17) D(18,arg18,f18) \
  P(f0,1 ,arg1 ,f1 ,i##f1 , P(f0,2 ,arg2 ,f2 ,i##f2 , P(f0,3 ,arg3 ,f3 ,i##f3 , P(f0,4 ,arg4 ,f4 ,i##f4 , \
  P(f0,5 ,arg5 ,f5 ,i##f5 , P(f0,6 ,arg6 ,f6 ,i##f6 , P(f0,7 ,arg7 ,f7 ,i##f7 , P(f0,8 ,arg8 ,f8 ,i##f8 , \
  P(f0,9 ,arg9 ,f9 ,i##f9 , P(f0,10,arg10,f10,i##f10, P(f0,11,arg11,f11,i##f11, P(f0,12,arg12,f12,i##f12, \
  P(f0,13,arg13,f13,i##f13, P(f0,14,arg14,f14,i##f14, P(f0,15,arg15,f15,i##f15, P(f0,16,arg16,f16,i##f16, \
  P(f0,17,arg17,f17,i##f17, P(f0,18,arg18,f18,i##f18, \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name) \
      A(1 ), A(2 ), A(3 ), A(4 ), A(5 ), A(6 ), A(7 ), A(8 ), A(9 ), A(10), \
      A(11), A(12), A(13), A(14), A(15), A(16), A(17), A(18))))))))))))))))))))) \
}

#define PROTO19(ret0,f0, funct,name,fn,   arg1 ,f1 , arg2 ,f2 , arg3 ,f3 , arg4 ,f4 , arg5 ,f5 , \
	   arg6 ,f6 , arg7 ,f7 , arg8 ,f8 , arg9 ,f9 , arg10,f10, arg11,f11, arg12,f12, arg13,f13, \
	   arg14,f14, arg15,f15, arg16,f16, arg17,f17, arg18,f18, arg19,f19) \
NAME(funct, name) \
{ D(0 ,ret0 ,f0 ) \
  D(1 ,arg1 ,f1 ) D(2 ,arg2 ,f2 ) D(3 ,arg3 ,f3 ) D(4 ,arg4, f4 ) \
  D(5 ,arg5 ,f5 ) D(6 ,arg6 ,f6 ) D(7 ,arg7 ,f7 ) D(8 ,arg8 ,f8 ) \
  D(9 ,arg9, f9 ) D(10,arg10,f10) D(11,arg11,f11) D(12,arg12,f12) \
  D(13,arg13,f13) D(14,arg14,f14) D(15,arg15,f15) D(16,arg16,f16) \
  D(17,arg17,f17) D(18,arg18,f18) D(19,arg19,f19) \
  P(f0,1 ,arg1 ,f1 ,i##f1 , P(f0,2 ,arg2 ,f2 ,i##f2 , P(f0,3 ,arg3 ,f3 ,i##f3 , P(f0,4 ,arg4 ,f4 ,i##f4 , \
  P(f0,5 ,arg5 ,f5 ,i##f5 , P(f0,6 ,arg6 ,f6 ,i##f6 , P(f0,7 ,arg7 ,f7 ,i##f7 , P(f0,8 ,arg8 ,f8 ,i##f8 , \
  P(f0,9 ,arg9 ,f9 ,i##f9 , P(f0,10,arg10,f10,i##f10, P(f0,11,arg11,f11,i##f11, P(f0,12,arg12,f12,i##f12, \
  P(f0,13,arg13,f13,i##f13, P(f0,14,arg14,f14,i##f14, P(f0,15,arg15,f15,i##f15, P(f0,16,arg16,f16,i##f16, \
  P(f0,17,arg17,f17,i##f17, P(f0,18,arg18,f18,i##f18, P(f0,19,arg19,f19,i##f19, \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name) \
      A(1 ), A(2 ), A(3 ), A(4 ), A(5 ), A(6 ), A(7 ), A(8 ), A(9 ), A(10), \
      A(11), A(12), A(13), A(14), A(15), A(16), A(17), A(18), A(19) \
      ))))))))))))))))))))) \
}

#define PROTO20(ret0,f0, funct,name,fn,   arg1 ,f1 , arg2 ,f2 , arg3 ,f3 , arg4 ,f4 , arg5 ,f5 , \
	   arg6 ,f6 , arg7 ,f7 , arg8 ,f8 , arg9 ,f9 , arg10,f10, arg11,f11, arg12,f12, arg13,f13, \
	   arg14,f14, arg15,f15, arg16,f16, arg17,f17, arg18,f18, arg19,f19, arg20,f20) \
NAME(funct, name) \
{ D(0 ,ret0 ,f0 ) \
  D(1 ,arg1 ,f1 ) D(2 ,arg2 ,f2 ) D(3 ,arg3 ,f3 ) D(4 ,arg4, f4 ) \
  D(5 ,arg5 ,f5 ) D(6 ,arg6 ,f6 ) D(7 ,arg7 ,f7 ) D(8 ,arg8 ,f8 ) \
  D(9 ,arg9, f9 ) D(10,arg10,f10) D(11,arg11,f11) D(12,arg12,f12) \
  D(13,arg13,f13) D(14,arg14,f14) D(15,arg15,f15) D(16,arg16,f16) \
  D(17,arg17,f17) D(18,arg18,f18) D(19,arg19,f19) D(20,arg20,f20) \
  P(f0,1 ,arg1 ,f1 ,i##f1 , P(f0,2 ,arg2 ,f2 ,i##f2 , P(f0,3 ,arg3 ,f3 ,i##f3 , P(f0,4 ,arg4 ,f4 ,i##f4 , \
  P(f0,5 ,arg5 ,f5 ,i##f5 , P(f0,6 ,arg6 ,f6 ,i##f6 , P(f0,7 ,arg7 ,f7 ,i##f7 , P(f0,8 ,arg8 ,f8 ,i##f8 , \
  P(f0,9 ,arg9 ,f9 ,i##f9 , P(f0,10,arg10,f10,i##f10, P(f0,11,arg11,f11,i##f11, P(f0,12,arg12,f12,i##f12, \
  P(f0,13,arg13,f13,i##f13, P(f0,14,arg14,f14,i##f14, P(f0,15,arg15,f15,i##f15, P(f0,16,arg16,f16,i##f16, \
  P(f0,17,arg17,f17,i##f17, P(f0,18,arg18,f18,i##f18, P(f0,19,arg19,f19,i##f19, P(f0,20,arg20,f20,i##f20, \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name) \
      A(1 ), A(2 ), A(3 ), A(4 ), A(5 ), A(6 ), A(7 ), A(8 ), A(9 ), A(10), \
      A(11), A(12), A(13), A(14), A(15), A(16), A(17), A(18), A(19), A(20) \
      )))))))))))))))))))))) \
}

#define PROTO25(ret0,f0, funct,name,fn, arg1 ,f1 , arg2 ,f2 , arg3 ,f3 , arg4 ,f4 , arg5 ,f5 , \
	   arg6 ,f6 , arg7 ,f7 , arg8 ,f8 , arg9 ,f9 , arg10,f10, arg11,f11, arg12,f12, arg13,f13, \
	   arg14,f14, arg15,f15, arg16,f16, arg17,f17, arg18,f18, arg19,f19, arg20,f20, arg21,f21, \
	   arg22,f22, arg23,f23, arg24,f24, arg25,f25) \
NAME(funct, name) \
{ D(0 ,ret0 ,f0 ) \
  D(1 ,arg1 ,f1 ) D(2 ,arg2 ,f2 ) D(3 ,arg3 ,f3 ) D(4 ,arg4, f4 ) \
  D(5 ,arg5 ,f5 ) D(6 ,arg6 ,f6 ) D(7 ,arg7 ,f7 ) D(8 ,arg8 ,f8 ) \
  D(9 ,arg9, f9 ) D(10,arg10,f10) D(11,arg11,f11) D(12,arg12,f12) \
  D(13,arg13,f13) D(14,arg14,f14) D(15,arg15,f15) D(16,arg16,f16) \
  D(17,arg17,f17) D(18,arg18,f18) D(19,arg19,f19) D(20,arg20,f20) \
  D(21,arg21,f21) D(22,arg22,f22) D(23,arg23,f23) D(24,arg24,f24) \
  D(25,arg25,f25) \
  P(f0,1 ,arg1 ,f1 ,i##f1 , P(f0,2 ,arg2 ,f2 ,i##f2 , P(f0,3 ,arg3 ,f3 ,i##f3 , P(f0,4 ,arg4 ,f4 ,i##f4 , \
  P(f0,5 ,arg5 ,f5 ,i##f5 , P(f0,6 ,arg6 ,f6 ,i##f6 , P(f0,7 ,arg7 ,f7 ,i##f7 , P(f0,8 ,arg8 ,f8 ,i##f8 , \
  P(f0,9 ,arg9 ,f9 ,i##f9 , P(f0,10,arg10,f10,i##f10, P(f0,11,arg11,f11,i##f11, P(f0,12,arg12,f12,i##f12, \
  P(f0,13,arg13,f13,i##f13, P(f0,14,arg14,f14,i##f14, P(f0,15,arg15,f15,i##f15, P(f0,16,arg16,f16,i##f16, \
  P(f0,17,arg17,f17,i##f17, P(f0,18,arg18,f18,i##f18, P(f0,19,arg19,f19,i##f19, P(f0,20,arg20,f20,i##f20, \
  P(f0,21,arg21,f21,i##f21, P(f0,22,arg22,f22,i##f22, P(f0,23,arg23,f23,i##f23, P(f0,24,arg24,f24,i##f24, \
  P(f0,25,arg25,f25,i##f25, \
  RET##fn(ret0,f0, Act##fn(funct,name) (FN##fn(name) \
      A(1 ), A(2 ), A(3 ), A(4 ), A(5 ), A(6 ), A(7 ), A(8 ), A(9 ), A(10), \
      A(11), A(12), A(13), A(14), A(15), A(16), A(17), A(18), A(19), A(20), \
      A(21), A(22), A(23), A(24), A(25) \
      ))))))))))))))))))))))))))) \
}


#include "sockproto.h"


typedef struct {
    byte Len, *name;
    void (*F)(void);
} sock_fn;
static sock_fn sockF [] = {
    
#   define EL(funct) { 0, #funct, sock##funct },
#   include "sockprotolist.h"
#   undef EL

    { 0, NULL, (void (*)(void))0 }
};

/***********/


static uldat SendTwinMagic(void) {
    byte data[8+sizeof(uldat)] = { sizeof(byte), sizeof(udat), sizeof(uldat), sizeof(hwcol), sizeof(time_t), 0, sizeof(frac_t), 0};
    *(uldat *)(data+8) = TWIN_MAGIC;
    return RemoteWriteQueue(Slot, 8+sizeof(uldat), data);
}

INLINE uldat Send(uldat data) {
    return RemoteWriteQueue(Slot, sizeof(uldat), &data);
}
    
static uldat FindFunction(byte Len, byte *name) {
    sock_fn *F = sockF;
    if (name) {
	while (F->name && (F->Len != Len || CmpMem(F->name, name, Len)))
	    F++;
	if (F->name)
	    return (uldat)(F - sockF);
    }
    return NOID;
}

static void Reply(uldat code, uldat len, void *data) {
    uldat buf[3];
    buf[0] = 2*sizeof(uldat) + len;
    buf[1] = RequestN;
    buf[2] = code;
    (void)RemoteWriteQueue(Slot, 3*sizeof(uldat), buf);
    if (len)
	(void)RemoteWriteQueue(Slot, len, data);
}


#define digestLen	16  /* hardcoded in MD5 routines */
#define lAuthLen 	64  /* min length of ~/.TwinAuth */
#define hAuthLen	256 /* max length of ~/.TwinAuth */
#define challengeLen	512 /* length of ~/.TwinAuth + random data */

static byte AuthData[challengeLen];
static uldat AuthLen;

static byte CreateAuth(byte *path) {
    int fd = NOFD, ufd = NOFD, len = 0, got = -1;
    
    if ((fd = open(path, O_WRONLY|O_CREAT|O_TRUNC, 0600)) >= 0 &&
	fchmod(fd, 0600) == 0 &&
	(ufd = open("/dev/urandom", O_RDONLY)) >= 0) {
	for (len = 0; len < hAuthLen; len += got) {
	    got = read(ufd, AuthData + len, hAuthLen - len);
	    if (got < 0) {
		if (errno == EINTR || errno == EAGAIN)
		    got = 0;
		else
		    break;
	    }
	}
	if (got > 0) for (len = 0; len < hAuthLen; len += got) {
	    got = write(fd, AuthData + len, hAuthLen - len);
	    if (got < 0) {
		if (errno == EINTR || errno == EAGAIN)
		    got = 0;
		else
		    break;
	    }
	}
    }
    if (fd != NOFD)
	close(fd);
    if (ufd != NOFD)
	close(ufd);
    AuthLen = len;
    return got > 0;
}
    
static byte InitAuth(void) {
    byte *home;
    int fd, len, got;
    
    if (!(home = getenv("HOME")))
	return FALSE;
    len = LenStr(home);
    len = Min2(len, challengeLen-11);
    CopyMem(home, AuthData, len);
    CopyMem("/.TwinAuth", AuthData+len, 11);

    if ((fd = open(AuthData, O_RDONLY)) < 0)
	return CreateAuth(AuthData);

    for (len = 0; len < hAuthLen; len += got) {
	got = read(fd, AuthData + len, hAuthLen - len);
	if (got < 0) {
	    if (errno == EINTR || errno == EAGAIN)
		got = 0;
	    else {
		close(fd);
		return FALSE;
	    }
	}
    }
    close(fd);
    if (len < lAuthLen)
    	return CreateAuth(AuthData);

    AuthLen = len;
    return TRUE;
}

static byte SendChallenge(void) {
    struct MD5Context ctx;
    int fd, len, got;
    byte *t;
    
    if ((fd = open("/dev/urandom", O_RDONLY)) < 0)
	return FALSE;
    len = AuthLen;
    for (got = 1; len < challengeLen && got; len += got) {
	do {
	    got = read(fd, AuthData + len, challengeLen - len);
	} while (got < 0 && errno == EINTR);
	if (got <= 0)
	    break;
    }
    close(fd);
    if (got > 0 && len == challengeLen && (len -= AuthLen) &&
	RemoteReadGrowQueue(Slot, digestLen) &&
	(t = RemoteReadGetQueue(Slot, &got)) && got == digestLen &&
	RemoteWriteQueue(Slot, sizeof(uldat), &len) &&
	RemoteWriteQueue(Slot, len, AuthData+AuthLen)) {
	
	MD5Init(&ctx);
	MD5Update(&ctx, AuthData, challengeLen);
	MD5Final(t, &ctx);
	
	return TRUE;
    }
    return FALSE;
}

static void Wait4Auth(int fd, uldat slot) {
    byte *t;
    uldat left, got = 0;
    
    (void)RemoteReadGetQueue(slot, &left);
    
    if (left < digestLen*2) {
	left = digestLen*2 - left;
	if ((t = RemoteReadGrowQueue(slot, left)))
	    got = read(fd, t, left);
	else
	    return;
    } else
	left = 0;
    
    if ((got < 0 && (got = 0, errno == EINTR)) || (got > 0 && got < left)) {
	/* not yet ready to check */
	RemoteReadShrinkQueue(slot, left - got);
	return;
    }
    
    if (got == left && (t = RemoteReadGetQueue(slot, &left)) && left >= digestLen*2) {
	if (!CmpMem(t, t+digestLen, digestLen)) {
	    /* OK! */
	    ls.HandlerIO = SocketIO;
	    RemoteReadDeQueue(slot, digestLen*2);
	    Send(GO_MAGIC);
	    return;
	}
    }
    
    Send(STOP_MAGIC);
    RemoteFlush(slot);
    UnRegisterRemote(slot);
    close(fd);
}


static void unixSocketIO(int fd, uldat slot) {
    struct sockaddr_un un_addr;
    int len = sizeof(un_addr);
    if ((fd = accept(unixFd, (struct sockaddr *)&un_addr, &len)) >= 0) {
	/* programs on the unix socket are always authorized */
	if ((Slot = RegisterRemoteFd(fd, SocketIO)) != NOSLOT) {
	    fcntl(fd, F_SETFL, O_NONBLOCK);
	    fcntl(fd, F_SETFD, FD_CLOEXEC);
	    if (SendTwinMagic() && Send(GO_MAGIC))
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
	if ((Slot = RegisterRemoteFd(fd, Wait4Auth)) != NOSLOT) {
	    fcntl(fd, F_SETFL, O_NONBLOCK);
	    fcntl(fd, F_SETFD, FD_CLOEXEC);
	    if (SendTwinMagic() && Send(WAIT_MAGIC) && SendChallenge())
		return;
	    UnRegisterRemote(Slot);	    
	}
	close(fd);
    }
}

void sockKillSlot(uldat slot) {
    msgport *MsgPort;
    display_hw *D_HW;
    
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
	     * DeleteMsgPort doesn't kill the slot, only tries to unregister from it
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
    uldat gzSlot, len, Funct;
    byte *t, *tend;
    size_t tot;
    
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
		KillSlot(Slot);
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
		if (Funct < MaxFunct)
		    sockF[Funct].F();   /* Slot is the uncompressed socket here ! */
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
	RemoteReadDeQueue(Slot, s - t);
	
#ifdef CONF_SOCKET_GZ
	if (gzSlot != NOSLOT)
	    /* compressed socket, restore Slot */
	    Slot = gzSlot;
#endif
	
    } else if (!len || (len == (uldat)-1 && errno != EINTR && errno != EAGAIN)) {
	/* let's close this sucker */
	KillSlot(Slot);
    }
}

static void sockSendMsg(msgport *MsgPort, msg *Msg) {
    tevent_any Event = (tevent_any)0;
    uldat Len = 0, Tot;
    byte *t, Easy = sizeof(twindow) == sizeof(window *);

    RequestN = MSG_MAGIC;
    Fd = MsgPort->RemoteData.Fd;
    Slot = MsgPort->RemoteData.FdSlot;
	
    switch (Msg->Type) {
#if defined(CONF_MODULES) || defined (CONF_HW_DISPLAY)
      case MSG_DISPLAY:
	Easy = FALSE;
	Reply(Msg->Type, Len = sizeof(twindow) + 4*sizeof(udat) + Msg->Event.EventDisplay.Len, NULL);

	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    
	    Push(t, twindow, NOID); /* not used here */
	    if (sizeof(event_display) == sizeof(window *) + 4*sizeof(dat) + sizeof(byte *)) {
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
#endif /* defined(CONF_MODULES) || defined (CONF_HW_DISPLAY) */
      case MSG_WINDOW_KEY:
	if (Easy && sizeof(event_keyboard) == sizeof(window *) + 3*sizeof(dat) + 2*sizeof(byte))
	    break;
	else
	    Easy = FALSE;
	Reply(Msg->Type, Len = sizeof(twindow) + 3*sizeof(udat) + 2*sizeof(byte) + Msg->Event.EventKeyboard.SeqLen, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    Push(t, twindow, Obj2Id(Msg->Event.EventKeyboard.Window));
	    Push(t, udat,    Msg->Event.EventKeyboard.Code);
	    Push(t, udat,    Msg->Event.EventKeyboard.ShiftFlags);
	    Push(t, udat,    Msg->Event.EventKeyboard.SeqLen);
	    Push(t, byte,    Msg->Event.EventKeyboard.pad);
	    PushV(t, Msg->Event.EventKeyboard.SeqLen+1, Event->EventKeyboard.AsciiSeq);
	}
	break;
      case MSG_WINDOW_MOUSE:
	if (Easy && sizeof(event_mouse) == sizeof(window *) + 4*sizeof(dat))
	    break;
	else
	    Easy = FALSE;
	Reply(Msg->Type, Len = sizeof(twindow) + 4*sizeof(udat), NULL);
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
	if (Easy && sizeof(event_window) == sizeof(window *) + 4*sizeof(dat))
	    break;
	else
	    Easy = FALSE;
	Reply(Msg->Type, Len = sizeof(twindow) + 4*sizeof(dat), NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    Push(t, twindow, Obj2Id(Msg->Event.EventWindow.Window));
	    Push(t, udat,    Msg->Event.EventWindow.Code);
	    Push(t, udat,    Msg->Event.EventWindow.XWidth);
	    Push(t, udat,    Msg->Event.EventWindow.YWidth);
	}
	break;
      case MSG_WINDOW_GADGET:
	if (Easy && sizeof(event_gadget) == sizeof(window *) + 2*sizeof(dat))
	    break;
	else
	    Easy = FALSE;
	Reply(Msg->Type, Len = sizeof(twindow) + 2*sizeof(dat), NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    Push(t, twindow, Obj2Id(Msg->Event.EventGadget.Window));
	    Push(t, udat,    Msg->Event.EventGadget.Code);
	    Push(t, udat,    Msg->Event.EventGadget.pad);
	}
	break;
      case MSG_MENU_ROW:
	if (Easy && sizeof(event_menu) == sizeof(window *) + 2*sizeof(udat) + sizeof(menu *))
	    break;
	else
	    Easy = FALSE;
	Reply(Msg->Type, Len = sizeof(twindow) + 2*sizeof(dat) + sizeof(tmenu), NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    Push(t, twindow, Obj2Id(Msg->Event.EventMenu.Window));
	    Push(t, udat,    Msg->Event.EventMenu.Code);
	    Push(t, udat,    Msg->Event.EventMenu.pad);
	    Push(t, tmenu,   Obj2Id(Msg->Event.EventMenu.Menu));
	}
	break;
      case MSG_CLIPBOARD:
	if (sizeof(event_clipboard) == sizeof(window *) + 4*sizeof(dat))
	    break;
	else
	    Easy = FALSE;
	if (!All->ClipLen)
	    return;
	Reply(Msg->Type, Len = sizeof(twindow) + 4*sizeof(dat) + 2*sizeof(ldat) + All->ClipLen, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    Push(t, twindow, Obj2Id(Msg->Event.EventClipBoard.Window));
	    Push(t, udat,    Msg->Event.EventClipBoard.Code);
	    Push(t, udat,    Msg->Event.EventClipBoard.ShiftFlags);
	    Push(t, dat,     Msg->Event.EventClipBoard.X);
	    Push(t, dat,     Msg->Event.EventClipBoard.Y);
	    Push(t, uldat,   All->ClipMagic);
	    Push(t, uldat,   All->ClipLen);
	    PushV(t, All->ClipLen, All->ClipData);
	}
	break;
      default:
	break;
    }
    if (Easy) {
	Msg->Event.EventCommon.Window = (void *)Obj2Id(Msg->Event.EventCommon.Window);
	if (Msg->Type == MSG_MENU_ROW)
	    Msg->Event.EventMenu.Menu = (void *)Obj2Id(Msg->Event.EventMenu.Menu);
	if (Msg->Type != MSG_CLIPBOARD)
	    Reply(Msg->Type, Msg->Len, &Msg->Event);
	else if (All->ClipLen) {
	    uldat buf[3];
	    buf[0] = (2 + 2) * sizeof(uldat) + Msg->Len + All->ClipLen;
	    buf[1] = RequestN;
	    buf[2] = Msg->Type;
	    (void)RemoteWriteQueue(Slot, 3*sizeof(uldat), buf);
	    (void)RemoteWriteQueue(Slot, Msg->Len, &Msg->Event);
	    buf[0] = All->ClipMagic;
	    buf[1] = All->ClipLen;
	    (void)RemoteWriteQueue(Slot, 2*sizeof(uldat), buf);
	    (void)RemoteWriteQueue(Slot, All->ClipLen, All->ClipData);
	}
    }
}

static void SocketH(msgport *MsgPort) {
    msg *Msg;
    
    while ((Msg=MsgPort->FirstMsg)) {
	Remove(Msg);
	
	sockSendMsg(MsgPort, Msg);
	
	Delete(Msg);
    }
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
	if ((slot = RegisterRemoteFd(specFD, SocketIO)) != NOSLOT &&
	    (z1 = AllocMem(sizeof(*z1))) &&
	    (z2 = AllocMem(sizeof(*z2)))) {
	    
	    z1->zalloc = z2->zalloc = sockZAlloc;
	    z1->zfree  = z2->zfree  = sockZFree;
	    z1->opaque = z2->opaque = NULL;

	    if (deflateInit(z1, Z_DEFAULT_COMPRESSION) == Z_OK) {
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



static void AttachHW(uldat len, byte *arg, byte redirect) {
    display_hw *D_HW;
    byte buf[2] = "\0";
    int errfd;
    
    if (!LS.MsgPort) {
	if (redirect)
	    write(LS.Fd, "twin: AttachHW(): client did not create a MsgPort\n\0", 52);
	else
	    write(LS.Fd, buf, 2);
	return;
    }
    
    if (redirect) {
	if ((errfd = dup(2)) >= 0) {
	    close(2);
	    dup2(LS.Fd, 2);
	} else
	    redirect = 0;
    }
	
    if ((D_HW = AttachDisplayHW(len, arg, Slot))) {
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
    write(LS.Fd, buf, 2);

    if (redirect) {
	/* wait for twattach to display messages... */
	fd_set set;
	struct timeval tv = {2,0};

	FD_ZERO(&set);
	FD_SET(LS.Fd, &set);
	    
	while (select(LS.Fd+1, &set, NULL, NULL, &tv) == -1 && errno == EINTR)
	    ;
	read(LS.Fd, buf, 1);
	
	close(2);
	dup2(errfd, 2);
	close(errfd);
    }
}

	    
static byte DetachHW(uldat len, byte *arg) {
    return DetachDisplayHW(len, arg);
}

static void SetFontTranslation(byte trans[0x80]) {
    if (trans)
	CopyMem(trans, All->Gtranslations[USER_MAP], 0x80);
}




#ifdef MODULE
byte InitModule(module *Module)
#else
byte InitSocket(void)
#endif
{
    char opt[15];

    if (!InitAuth())
	return FALSE;
    
    {
	struct sockaddr_un addr;
	WriteMem(&addr, 0, sizeof(addr));
	CopyStr("/tmp/.twin", addr.sun_path);
	CopyStr(TWDisplay, addr.sun_path+10);
	
	addr.sun_family = AF_UNIX;

	if ((unixFd = socket(AF_UNIX, SOCK_STREAM, 0)) >= 0 &&
	    /*setsockopt(unixFd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt)) >= 0 &&*/
	    bind(unixFd, (struct sockaddr *)&addr, sizeof(addr)) >= 0 &&
	    listen(unixFd, 1) >= 0 &&
	    (unixSlot = RegisterRemoteFd(unixFd, unixSocketIO)) != NOSLOT) {
	    
	    fcntl(unixFd, F_SETFD, FD_CLOEXEC);
	    CopyStr(addr.sun_path, opt);
	    opt[6] = 'T';
	    rename(addr.sun_path, opt);
	    chmod(opt, 0700);
	} else
	    close(unixFd);
    }
    
    {
	struct sockaddr_in addr;
	WriteMem(&addr, 0, sizeof(addr));
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(TW_INET_PORT + strtoul(TWDisplay+1, NULL, 16));
	
	if ((inetFd = socket(AF_INET, SOCK_STREAM, 0)) >= 0 &&
	    setsockopt(inetFd, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt)) >= 0 &&
	    bind(inetFd, (struct sockaddr *)&addr, sizeof(addr)) >= 0 &&
	    listen(inetFd, 1) >= 0 &&
	    (inetSlot = RegisterRemoteFd(inetFd, inetSocketIO)) != NOSLOT) {
	    
	    fcntl(inetFd, F_SETFD, FD_CLOEXEC);
	} else
	    close(inetFd);
    }
    
    if (unixSlot != NOSLOT || inetSlot != NOSLOT) {
	for (MaxFunct = 0; sockF[MaxFunct].name; MaxFunct++)
	    sockF[MaxFunct].Len = strlen(sockF[MaxFunct].name);
    
	KillSlot = sockKillSlot;
	SocketSendMsg = sockSendMsg;
	
	return TRUE;
    }
    return FALSE;
}

#ifdef MODULE
void QuitModule(module *Module) {
    void remoteKillSlot(uldat); /* from remote.c */

    if (unixSlot != NOSLOT) {
	UnRegisterRemote(unixSlot);
	close(unixFd);
    }
    if (inetSlot != NOSLOT) {
	UnRegisterRemote(inetSlot);
	close(inetFd);
    }
    for (Slot = 0; Slot < FdTop; Slot++) {
	if (LS.Fd != NOFD && (LS.HandlerIO == SocketIO || LS.HandlerIO == Wait4Auth)) {
	    KillSlot(Slot);
	}
    } 
    KillSlot = remoteKillSlot;
    SocketSendMsg = NULL;
}
#endif
