#include <sys/ioctl.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

#include "twin.h"
#include "methods.h"

#include "fdlist.h"
#include "remote.h"
#include "socket.h"
#include "socketprivate.h"
#include "extensions.h"
#include "util.h"

#include "unaligned.h"

#include "Tw/Tw.h"


/* First: private variables from remote.c */
extern fdlist *FdList;
extern uldat FdSize, FdTop, FdBottom, FdWQueued;
#define LS	FdList[Slot]
#define ls      FdList[slot]
#define ls_p    FdList[ls.pairSlot]


static uldat MaxFunct, Slot, RequestN, noId = NOID;
static byte *s, *end;
static int Fd;

static void alienReply(uldat code, uldat alien_len, uldat len, CONST void *data) {
    byte AlienSizeofUldat = AlienSizeof(uldat,Slot);
    
    if (RemoteWriteQueue(Slot, 3*AlienSizeofUldat + alien_len, NULL)
	== 3*AlienSizeofUldat + alien_len) {
	
	uldat queued;
	byte *T = RemoteWriteGetQueue(Slot, &queued);
	T += queued - 3*AlienSizeofUldat - alien_len;
	
	alien_len += 2*AlienSizeofUldat;
	AlienPush(T,uldat,alien_len);
	AlienPush(T,uldat,RequestN);
	AlienPush(T,uldat,code);
	alien_len -= 2*AlienSizeofUldat;
	
	if (alien_len && len && data) {
	    T = RemoteWriteGetQueue(Slot, &queued);
	    T += queued - alien_len;
	    SocketAlienPush(data, len, &T, alien_len);
	}
    }
}


#define ACT0(funct,name)		(SocketF.funct##name)
#define NAME_PREFIX			alien
#define SIZEOF(type)			AlienSizeof(type,Slot)
#define POP(s,type,lval)		AlienPop(s,type,lval)
#define POPADDR(s,type,len,ptr)		PopAddr(s,type,len,ptr)
#define PUSH(s,type,val)		do {type tmp = (val); AlienPush(s,type,tmp); } while(0)
#define REPLY(code, type, lval)		alienReply(code, SIZEOF(type), sizeof(type), lval)

#include "socketmacros.h"
#include "sockproto.h"

static FN_DECL {
#  define EL(funct) { 0, #funct, CAT(NAME_PREFIX,funct) },
#  include "sockprotolist.h"
#  undef EL
FN_ENDDECL };

# include "socketundefs.h"

# undef ACT0
# undef NAME_PREFIX


#if defined(CONF_THIS_MODULE) && !defined(CONF_SOCKET_ALIEN)
static
#endif
void SocketAlienIO(int fd, uldat slot) {
    uldat len, Funct;
    byte *t, *tend;
    size_t tot;
#ifdef CONF_SOCKET_GZ
    uldat gzSlot;
#endif
    byte AlienSizeofUldat;
    
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

	AlienSizeofUldat = AlienSizeof(uldat,Slot);
	
	while (s + 3*AlienSizeofUldat <= tend) {
	    POP(s,uldat,len);
	    if (len < 2*AlienSizeofUldat) {
		s += len;
		continue;
	    }
	    if (s + len > s && s + len <= tend) {
		end = s + len;
		POP(s, uldat, RequestN);
		POP(s, uldat, Funct);
		if (Funct < MaxFunct) {
		    slot = Slot;
		    alienF[Funct].F();   /* Slot is the uncompressed socket here ! */
		    Slot = slot;	/*
					 * restore, in case alienF[Funct].F() changed it;
					 * without this, tw* clients can freeze
					 * if twdisplay is in use
					 */
		}
		else if (Funct == FIND_MAGIC)
		    alienFindFunction();
		s = end;
	    } else if (s + len < s) {
		s = tend;
		break;
	    } else { /* if (s + len > tend) */
		/* must wait for rest of packet... unpop len */
		s -= AlienSizeofUldat;
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
 * Turn the (msg) into a (tmsg) and write it on the MsgPort file descriptor.
 * Used when MsgPort slot is using non-native data sizes and endianity.
 * 
 * this can be called in nasty places like detaching non-exclusive displays
 * when an exclusive one is started. Must preserve Slot, Fd and other globals!
 */
static void alienSendMsg(msgport MsgPort, msg Msg) {
    uldat Len = 0, Tot;
    byte *t;

    uldat save_Slot = Slot;
    int save_Fd = Fd;
    
    RequestN = MSG_MAGIC;
    Fd = MsgPort->RemoteData.Fd;
    Slot = MsgPort->RemoteData.FdSlot;

    switch (Msg->Type) {
#if defined(CONF__MODULES) || defined (CONF_HW_DISPLAY)
      case MSG_DISPLAY:
	alienReply(Msg->Type, Len = SIZEOF(uldat) + 4*sizeof(udat) + Msg->Event.EventDisplay.Len, 0, NULL);

	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    
	    PUSH(t, uldat, NOID); /* not used here */
	    AlienPush(t, udat,  Msg->Event.EventDisplay.Code);
	    AlienPush(t, udat,  Msg->Event.EventDisplay.Len);
	    AlienPush(t, udat,  Msg->Event.EventDisplay.X);
	    AlienPush(t, udat,  Msg->Event.EventDisplay.Y);
	    PushV(t, Msg->Event.EventDisplay.Len, Msg->Event.EventDisplay.Data);
	}
	break;
#endif /* defined(CONF__MODULES) || defined (CONF_HW_DISPLAY) */
      case MSG_WINDOW_KEY:
	alienReply(Msg->Type, Len = SIZEOF(uldat) + 3*SIZEOF(udat) + 2*SIZEOF(byte) + Msg->Event.EventKeyboard.SeqLen, 0, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    PUSH(t, uldat, Obj2Id(Msg->Event.EventKeyboard.Window));
	    AlienPush(t, udat,  Msg->Event.EventKeyboard.Code);
	    AlienPush(t, udat,  Msg->Event.EventKeyboard.ShiftFlags);
	    AlienPush(t, udat,  Msg->Event.EventKeyboard.SeqLen);
	    AlienPush(t, byte,  Msg->Event.EventKeyboard.pad);
	    PushV(t, Msg->Event.EventKeyboard.SeqLen+1, Msg->Event.EventKeyboard.AsciiSeq);
	}
	break;
      case MSG_WINDOW_MOUSE:
	alienReply(Msg->Type, Len = SIZEOF(uldat) + 4*SIZEOF(udat), 0, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    PUSH(t, uldat, Obj2Id(Msg->Event.EventMouse.Window));
	    AlienPush(t, udat,  Msg->Event.EventMouse.Code);
	    AlienPush(t, udat,  Msg->Event.EventMouse.ShiftFlags);
	    AlienPush(t, dat,   Msg->Event.EventMouse.X);
	    AlienPush(t, dat,   Msg->Event.EventMouse.Y);
	}
	break;
      case MSG_WINDOW_CHANGE:
	alienReply(Msg->Type, Len = SIZEOF(uldat) + 4*SIZEOF(dat), 0, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    PUSH(t, uldat, Obj2Id(Msg->Event.EventWindow.Window));
	    AlienPush(t, udat,  Msg->Event.EventWindow.Code);
	    AlienPush(t, udat,  Msg->Event.EventWindow.Flags);
	    AlienPush(t, udat,  Msg->Event.EventWindow.XWidth);
	    AlienPush(t, udat,  Msg->Event.EventWindow.YWidth);
	    AlienPush(t, udat,  Msg->Event.EventWindow.XWidth);
	    AlienPush(t, udat,  Msg->Event.EventWindow.Y);
	}
	break;
      case MSG_WINDOW_GADGET:
	alienReply(Msg->Type, Len = SIZEOF(uldat) + 2*SIZEOF(dat), 0, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    PUSH(t, uldat, Obj2Id(Msg->Event.EventGadget.Window));
	    AlienPush(t, udat,  Msg->Event.EventGadget.Code);
	    AlienPush(t, udat,  Msg->Event.EventGadget.Flags);
	}
	break;
      case MSG_MENU_ROW:
	alienReply(Msg->Type, Len = SIZEOF(uldat) + 2*SIZEOF(dat) + SIZEOF(uldat), 0, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    PUSH(t, uldat, Obj2Id(Msg->Event.EventMenu.Window));
	    AlienPush(t, udat,  Msg->Event.EventMenu.Code);
	    AlienPush(t, udat,  Msg->Event.EventMenu.pad);
	    PUSH(t, uldat, Obj2Id(Msg->Event.EventMenu.Menu));
	}
	break;
      case MSG_SELECTION:
	alienReply(Msg->Type, Len = SIZEOF(uldat) + 4*SIZEOF(dat), 0, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    PUSH(t, uldat, Obj2Id(Msg->Event.EventSelection.Window));
	    AlienPush(t, udat,  Msg->Event.EventSelection.Code);
	    AlienPush(t, udat,  Msg->Event.EventSelection.pad);
	    AlienPush(t, dat,   Msg->Event.EventSelection.X);
	    AlienPush(t, dat,   Msg->Event.EventSelection.Y);
	}
	break;
      case MSG_SELECTIONNOTIFY:
	alienReply(Msg->Type, Len = 4*SIZEOF(uldat) + 2*SIZEOF(dat) + MAX_MIMELEN
		   + Msg->Event.EventKeyboard.SeqLen, 0, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    PUSH(t, uldat, Obj2Id(Msg->Event.EventSelectionNotify.Window));
	    AlienPush(t, udat,  Msg->Event.EventSelectionNotify.Code);
	    AlienPush(t, udat,  Msg->Event.EventSelectionNotify.pad);
	    AlienPush(t, uldat, Msg->Event.EventSelectionNotify.ReqPrivate);
	    AlienPush(t, uldat, Msg->Event.EventSelectionNotify.Magic);
	    PushV(t, MAX_MIMELEN, Msg->Event.EventSelectionNotify.MIME);
	    AlienPush(t, uldat, Msg->Event.EventSelectionNotify.Len);
	    PushV(t, Msg->Event.EventSelectionNotify.Len, Msg->Event.EventSelectionNotify.Data);
	}
	break;
      case MSG_SELECTIONREQUEST:
	alienReply(Msg->Type, Len = SIZEOF(uldat) + 2*SIZEOF(dat) + 2*SIZEOF(ldat), 0, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    PUSH(t, uldat, Obj2Id(Msg->Event.EventSelectionRequest.Window));
	    AlienPush(t, udat,  Msg->Event.EventSelectionRequest.Code);
	    AlienPush(t, udat,  Msg->Event.EventSelectionRequest.pad);
	    PUSH(t, uldat, Obj2Id(Msg->Event.EventSelectionRequest.Requestor));
	    AlienPush(t, uldat, Msg->Event.EventSelectionRequest.ReqPrivate);
	}
	break;
	
      case MSG_USER_CONTROL:
	alienReply(Msg->Type, Len = SIZEOF(uldat) + 4*SIZEOF(dat) + SIZEOF(byte)
	      + Msg->Event.EventControl.Len, 0, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    PUSH(t, uldat, Obj2Id(Msg->Event.EventControl.Window));
	    AlienPush(t, udat,  Msg->Event.EventControl.Code);
	    AlienPush(t, udat,  Msg->Event.EventControl.Len);
	    AlienPush(t, dat,   Msg->Event.EventControl.X);
	    AlienPush(t, dat,   Msg->Event.EventControl.Y);
	    PushV(t, Msg->Event.EventControl.Len + 1 /*the '\0'*/, Msg->Event.EventControl.Data);
	}
	break;
	
      case MSG_USER_CLIENTMSG:
	alienReply(Msg->Type, Len = SIZEOF(uldat) + 2*SIZEOF(dat) + Msg->Event.EventClientMsg.Len, 0, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    PUSH(t, uldat, Obj2Id(Msg->Event.EventClientMsg.Window));
	    AlienPush(t, udat,  Msg->Event.EventClientMsg.Code);
	    AlienPush(t, udat,  Msg->Event.EventClientMsg.Len);
	    PushV(t, Msg->Event.EventClientMsg.Len, Msg->Event.EventClientMsg.Data);
	}
	break;
	
      default:
	break;
    }
    Slot = save_Slot;
    Fd = save_Fd;
}


#ifdef CONF_SOCKET_ALIEN
byte InitSocketAlien(void)
#else /* !CONF_SOCKET_ALIEN */
# include "version.h"
  MODULEVERSION;

byte InitModule(module Module)
#endif /* CONF_SOCKET_ALIEN */
{
    for (; alienF[MaxFunct].name; MaxFunct++)
	alienF[MaxFunct].Len = LenStr(alienF[MaxFunct].name);
    RegisterExtension(SocketAlien,SendMsg,alienSendMsg);
#ifndef CONF_SOCKET_ALIEN
    Module->Private = SocketAlienIO;
#endif
    return TRUE;
}

#ifdef CONF_SOCKET_ALIEN
void QuitSocketAlien(void)
#else
void QuitModule(module Module)
#endif
{
    for (Slot = 0; Slot < FdTop; Slot++) {
	if (LS.Fd != NOFD && LS.HandlerIO.S == SocketAlienIO) {
	    Ext(Remote,KillSlot)(Slot);
	}
    } 
    UnRegisterExtension(SocketAlien,SendMsg,alienSendMsg);
}

