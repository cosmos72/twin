/*
 *  util.c  --  utility functions
 *
 *  Copyright (C) 1993-2001 by Massimiliano Ghilardi
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
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <grp.h>
#include <errno.h>

#include "twin.h"
#include "data.h"
#include "methods.h"
#include "main.h"
#include "draw.h"
#include "remote.h"
#include "resize.h"
#include "printk.h"
#include "util.h"

#include "hw.h"

#include "Tw/Twkeys.h"

udat ErrNo;
byte CONST * ErrStr;

#if 0
uldat MemLeft(void) {
    /* TODO? */
    return 0;
}
#endif

byte Error(udat Code_Error) {
    switch ((ErrNo = Code_Error)) {
      case NOMEMORY:
	ErrStr = "Out of memory!";
	break;
      case NOTABLES:
	ErrStr = "Internal tables full!";
	break;
      case SYSCALLERROR:
	ErrStr = strerror(errno);
	break;
      default:
	break;
    }
    return FALSE;
}

void *CloneMem(CONST void *From, uldat Size) {
    void *temp;
    if (From && Size && (temp = AllocMem(Size)))
	return CopyMem(From, temp, Size);
    return NULL;
}

byte *CloneStr(CONST byte *s) {
    byte *q;
    uldat len;
    
    if (s) {
	len = 1 + LenStr(s);
	if ((q = AllocMem(len)))
	    CopyMem(s, q, len);
	return q;
    }
    return NULL;
}

byte *CloneStrL(CONST byte *s, uldat len) {
    byte *q;
    
    if (s) {
	if ((q = AllocMem(len+1))) {
	    if (len)
		CopyMem(s, q, len);
	    q[len] = '\0';
	}
	return q;
    }
    return NULL;
}

byte **CloneStrList(byte **s) {
    uldat n = 1;
    byte **t = s, **v;
    
    if (t) {
	while (*t) {
	    t++;
	    n++;
	}
	t = AllocMem(n * sizeof(byte *));
    }
    
    if ((v = t)) {
	for (; *s && n; v++, s++, n--) {
	    if (!(*v = CloneStr(*s)))
		break;
	}

	if (*s && n) {
	    /* failed... clean up */
	    for (; t < v; t++)
		FreeMem(*t);
	    t = NULL;
	} else
	    *v = NULL;
    }
    return t;
}

void NormalizeTime(timevalue *Time) {
    while (Time->Fraction >= 1 FullSECs) {
	Time->Seconds++;
	Time->Fraction -= 1 FullSECs;
    }
    while (Time->Fraction < 0) {
	Time->Seconds--;
	Time->Fraction += 1 FullSECs;
    }
}

timevalue *InstantNow(timevalue *Now) {
    struct timeval sysNow;
    
    gettimeofday(&sysNow, NULL);
    Now->Seconds = sysNow.tv_sec;
    Now->Fraction = sysNow.tv_usec MicroSECs;
    return Now;
}

timevalue *IncrTime(timevalue *Time, timevalue *Incr) {
    Time->Seconds+=Incr->Seconds;
    Time->Fraction+=Incr->Fraction;
    NormalizeTime(Time);
    return Time;
}

timevalue *DecrTime(timevalue *Time, timevalue *Decr) {
    Time->Seconds-=Decr->Seconds;
    Time->Fraction-=Decr->Fraction;
    NormalizeTime(Time);
    return Time;
}

timevalue *SumTime(timevalue *Result, timevalue *Time, timevalue *Incr) {
    CopyMem(Time, Result, sizeof(timevalue));
    return IncrTime(Result, Incr);
}

timevalue *SubTime(timevalue *Result, timevalue *Time, timevalue *Decr) {
    CopyMem(Time, Result, sizeof(timevalue));
    return DecrTime(Result, Decr);
}

dat CmpTime(timevalue *T1, timevalue *T2) {
    NormalizeTime(T1);
    NormalizeTime(T2);
    
    if (T1->Seconds>T2->Seconds)
	return (dat)1;
    else if (T1->Seconds==T2->Seconds) {
	if (T1->Fraction>T2->Fraction)
	    return (dat)1;
	else if (T1->Fraction==T2->Fraction)
	    return (dat)0;
    }
    
    return (dat)-1;
}

static dat CmpCallTime(msgport m1, msgport m2) {
    if ((!m1->FirstMsg) != (!m2->FirstMsg))
	/* one of the two doesn't have msgs */
	return m1->FirstMsg ? (dat)-1 : (dat)1;
    if ((!m1->WakeUp) != (!m2->WakeUp))
	/* one doesn't need to be called */
	return m1->WakeUp ? (dat)-1 : (dat)1;
    if ((!m1->WakeUp) && (!m2->WakeUp))
	return 0;
    return CmpTime(&m1->CallTime, &m2->CallTime);
}

byte Minimum(byte MaxIndex, CONST uldat *Array) {
    byte i, MinIndex;
    uldat Temp;
    
    Temp=MAXULDAT;
    MinIndex=(byte)0;
    if (!MaxIndex)
	return 0xFF;
    
    for(i=0; i<MaxIndex; i++)
	if (Array[i]<Temp) {
	    Temp = Array[i];
	    MinIndex = i;
	}
    
    return MinIndex;
}

/*
byte HexDigitToNum(byte HexDigit, byte *Error) {
     if (HexDigit>='0' && HexDigit<='9')
	 return HexDigit - (byte)'0';
     
     if (HexDigit>='A' && HexDigit<='F')
	 return (byte)10 + HexDigit - (byte)'A';
     
     if (HexDigit>='a' && HexDigit<='f')
	 return (byte)10 + HexDigit - (byte)'a';
     
     *Error = TRUE;
     return (byte)0;
}

uldat HexStrToNum(byte *StringHex) {
     byte Len, Error = FALSE;
     uldat Value=(uldat)0;
     
     Len=(byte)LenStr(StringHex);
     if (Len>(byte)8)
	 return (uldat)0;
     
     do {
	 Len--;
	 Value |= (uldat)HexDigitToNum(StringHex[Len], &Error) << (Len << 2);
     }	while (!Error && Len>(byte)0);
    
    return Error ? (uldat)0 : Value;
}
*/

/* adapted from similar code in bdflush */
void SetArgv_0(byte *CONST *argv, byte CONST * src) {
    byte *ptr;
    uldat count, len = LenStr(src);
    
    ptr = argv[0] + LenStr(argv[0]);
    for (count = 1; argv[count]; count++) {
	if (argv[count] == ptr + 1)
	    ptr += LenStr(++ptr);
    }
    
    if (len + 1 < (size_t)(ptr - argv[0])) {
	CopyMem(src, argv[0], len);
	WriteMem(argv[0] + len, '\0', (size_t)(ptr - argv[0]) - len);
    } else
	CopyMem(src, argv[0], (size_t)(ptr - argv[0]));
}
                                           


/*
 * move a msgport to the right place in an already sorted list,
 * ordering by CallTime
 */
void SortMsgPortByCallTime(msgport Port) {
    msgport other;
    if ((other = Port->Next) && CmpCallTime(Port, other) > 0) {
	Remove(Port);
	do {
	    other = other->Next;
	} while (other && CmpCallTime(Port, other) > 0);
	if (other)
	    InsertMiddle(MsgPort, Port, All, other->Prev, other);
	else
	    InsertLast(MsgPort, Port, All);
    } else if ((other = Port->Prev) && CmpCallTime(Port, other) < 0) {
	Remove(Port);
	do {
	    other = other->Prev;
	} while (other && CmpCallTime(Port, other) < 0);
	if (other)
	    InsertMiddle(MsgPort, Port, All, other, other->Next);
	else
	    InsertFirst(MsgPort, Port, All);
    }
}

/*
 * sort the msgport list by growing CallTime
 * 
 * we use a bubble sort... no need to optimize to death this
 */
void SortAllMsgPortsByCallTime(void) {
    msgport Max, This, Port = All->FirstMsgPort;
    msgport Start, End;
    
    Start = End = (msgport)0;
    
    while (Port) {
	Max = This = Port;
	while ((This = This->Next)) {
	    if (CmpCallTime(This, Max) > 0)
		Max = This;
	}
	if (Max == Port)
	    /* careful, we are mucking the list under our feet */
	    Port = Port->Next;
	
	Remove(Max);
	/*
	 * HACK : we create a parentless list 
	 * backward, from End adding ->Prev until Start
	 */
	Max->Next = Start;
	Max->All = All;
	if (Start) Start->Prev = Max;
	Start = Max;
	if (!End) End = Max;
    }
    All->FirstMsgPort = Start;
    All->LastMsgPort  = End;
}


byte SendControlMsg(msgport MsgPort, udat Code, udat Len, CONST byte *Data) {
    msg Msg;
    event_control *Event;
    
    if (MsgPort && (Msg = Do(Create,Msg)(FnMsg, MSG_CONTROL, sizeof(event_control) + Len))) {
	Event = &Msg->Event.EventControl;
	Event->Code = Code;
	Event->Len  = Len;
	CopyMem(Data, Event->Data, Len);
	SendMsg(MsgPort, Msg);
	
	return TRUE;
    }
    return FALSE;
}

byte SelectionStore(uldat Magic, CONST byte MIME[MAX_MIMELEN], uldat Len, CONST byte *Data) {
    uldat newLen;
    byte *newData;
    selection *Sel = All->Selection;
    
    if (Magic == SEL_APPEND) 
	newLen = Sel->Len + Len;
    else
	newLen = Len;
    
    if (Sel->Max < newLen) {
	if (!(newData = ReAllocMem(Sel->Data, newLen)))
	    return FALSE;
	Sel->Data = newData;
	Sel->Max = newLen;
    }
    if (Magic != SEL_APPEND) {
	Sel->Owner = NULL;
	Sel->Len = 0;
	Sel->Magic = Magic;
	if (MIME)
	    CopyMem(MIME, Sel->MIME, MAX_MIMELEN);
	else
	    WriteMem(Sel->MIME, '\0', MAX_MIMELEN);
    }
    if (Data)
	CopyMem(Data, Sel->Data + Sel->Len, Len);
    else
	WriteMem(Sel->Data + Sel->Len, ' ', Len);
    Sel->Len += Len;
    return TRUE;
}

byte SetSelectionFromWindow(window Window) {
    uldat y, slen, len;
    byte *sData, *Data;
    byte ok = TRUE;
    
    if (!(Window->Attrib & WINDOW_DO_SEL))
	return ok;
    
    if (!(Window->Attrib & WINDOW_ANYSEL) || Window->YstSel > Window->YendSel ||
	(Window->YstSel == Window->YendSel && Window->XstSel > Window->XendSel)) {
	
	ok &= SelectionStore(SEL_TEXTMAGIC, NULL, 0, NULL);
	if (ok) NeedHW |= NEEDSelectionExport;
	return ok;
    }

    /* normalize negative coords */
    if (Window->XstSel < 0)
	Window->XstSel = 0;
    else if ((Window->Flags & WINFL_USECONTENTS) &&
	     Window->XstSel >= Window->WLogic) {
	Window->XstSel = 0;
	Window->YstSel++;
    }
    if (Window->YstSel < Window->YLogic) {
	Window->YstSel = Window->YLogic;
	Window->XstSel = 0;
    } else if (Window->YstSel >= Window->HLogic) {
	Window->YstSel = Window->HLogic - 1;
	Window->XstSel = (Window->Flags & WINFL_USECONTENTS) ?
	    Window->WLogic - 1 : MAXLDAT;
    }

    if (Window->Flags & WINFL_USECONTENTS) {
	hwattr *hw;
	
	/* normalize negative coords */
        if (Window->XendSel < 0) {
	    Window->XendSel = Window->WLogic - 1;
	    Window->YendSel--;
	} else if (Window->XendSel >= Window->WLogic)
	    Window->XendSel = Window->WLogic - 1;
	    
	if (Window->YendSel < Window->YLogic) {
	    Window->YendSel = Window->YLogic;
	    Window->XendSel = 0;
	} else if (Window->YendSel >= Window->HLogic) {
	    Window->YendSel = Window->HLogic - 1;
	    Window->XendSel = Window->WLogic - 1;
	}
	
	if (!(sData = AllocMem(slen = Window->WLogic)))
	    return FALSE;
	
	
	hw = Window->USE.C.Contents + (Window->YstSel + Window->USE.C.HSplit) * slen;
	while (hw >= Window->USE.C.TtyData->Split)
	    hw -= Window->USE.C.TtyData->Split - Window->USE.C.Contents;
	
	{
	    y = Window->YstSel;
	    if (y < Window->YendSel)
		slen -= Window->XstSel;
	    else
		slen = Window->XendSel - Window->XstSel + 1;
	    Data = sData;
	    len = slen;
	    hw += Window->XstSel;
	    while (len--)
		*Data++ = HWFONT(*hw++);
	    ok &= SelectionStore(SEL_TEXTMAGIC, NULL, slen, sData);
	}
	
	if (hw >= Window->USE.C.TtyData->Split)
	    hw -= Window->USE.C.TtyData->Split - Window->USE.C.Contents;

	slen = Window->WLogic;
	for (y = Window->YstSel + 1; ok && y < Window->YendSel; y++) {
	    if (hw >= Window->USE.C.TtyData->Split)
		hw -= Window->USE.C.TtyData->Split - Window->USE.C.Contents;
	    Data = sData;
	    len = slen;
	    while (len--)
		*Data++ = HWFONT(*hw++);
	    ok &= SelectionAppend(slen, sData);
	}
	
	if (ok && Window->YendSel > Window->YstSel) {
	    if (hw >= Window->USE.C.TtyData->Split)
		hw -= Window->USE.C.TtyData->Split - Window->USE.C.Contents;
	    Data = sData;
	    len = slen = Window->XendSel + 1;
	    while (len--)
		*Data++ = HWFONT(*hw++);
	    ok &= SelectionAppend(slen, sData);
	}
	if (ok) NeedHW |= NEEDSelectionExport;
	return ok;
    }
    if (!(Window->Flags & WINFL_USEANY)) {
	row Row;
	
	/* Gap not supported! */
	y = Window->YstSel;
	Row = Act(FindRow,Window)(Window, y);
	
	if (Row && Row->Text) {
	    if (y < Window->YendSel)
		slen = Row->Len - Window->XstSel;
	    else
		slen = Min2(Row->Len, Window->XendSel+1) - Min2(Row->Len, Window->XstSel);
	    ok &= SelectionStore(SEL_TEXTMAGIC, NULL, slen,
				 Row->Text + Min2(Row->Len, Window->XstSel));
	} else
	    ok &= SelectionStore(SEL_TEXTMAGIC, NULL, 0, "");

	if (y < Window->YendSel || !Row || !Row->Text || Row->Len <= Window->XendSel)
	    ok &= SelectionAppend(1, "\n");

	for (y = Window->YstSel + 1; ok && y < Window->YendSel; y++) {
	    if ((Row = Act(FindRow,Window)(Window, y)) && Row->Text)
		ok &= SelectionAppend(Row->Len, Row->Text);
	    ok &= SelectionAppend(1, "\n");
	}
	if (Window->YendSel > Window->YstSel) {
	    if (Window->XendSel >= 0 && (Row = Act(FindRow,Window)(Window, Window->YendSel)) && Row->Text)
		ok &= SelectionAppend(Min2(Row->Len, Window->XendSel+1), Row->Text);
	    if (!Row || !Row->Text || Row->Len <= Window->XendSel)
		ok &= SelectionAppend(1, "\n");
	}
	if (ok) NeedHW |= NEEDSelectionExport;
	return ok;
    }
    return FALSE;
}


byte CreateXTermMouseEvent(event_mouse *Event, byte buflen, byte *buf) {
    window W;
    udat Flags;
    udat Code = Event->Code;
    dat x = Event->X, y = Event->Y;
    byte len = 0;
	
    if (!(W = Event->Window) || !(W->Flags & WINFL_USECONTENTS) || !W->USE.C.TtyData)
	return len;
    
    Flags = W->USE.C.TtyData->Flags;
    
    if (Flags & TTY_REPORTMOUSE2) {
	/* classic xterm-style reporting */
	
	if (buflen < 6)
	    /* buffer too small! */
	    return len;
	
	CopyMem("\033[M", buf, 3);
    
	if (isSINGLE_PRESS(Code)) switch (Code & PRESS_ANY) {
	  case PRESS_LEFT: buf[3] = ' '; break;
	  case PRESS_MIDDLE: buf[3] = '!'; break;
	  case PRESS_RIGHT: buf[3] = '\"'; break;
	}
	else if (isRELEASE(Code))
	    buf[3] = '#';
	else
	    return len;

	buf[4] = '!' + x;
	buf[5] = '!' + y;
	len = 6;
	
    } else if (Flags & TTY_REPORTMOUSE) {
	/* new-style reporting */
	
	if (buflen < 9)
	    /* buffer too small! */
	    return len;
	
	CopyMem("\033[5M", buf, 4);
	buf[4] = ' ' + (Code & HOLD_ANY);
	buf[5] = '!' + (x & 0x7f);
	buf[6] = '!' + ((x >> 7) & 0x7f);
	buf[7] = '!' + (y & 0x7f);
	buf[8] = '!' + ((y >> 7) & 0x7f);
	len = 9;
    }
    return len;
}

void ResetBorderPattern(void) {
    msgport MsgP;
    widget W;
    
    for (MsgP = All->FirstMsgPort; MsgP; MsgP=MsgP->Next) {
	for (W = MsgP->FirstW; W; W = W->ONext) {
	    if (IS_WINDOW(W))
		((window)W)->BorderPattern[0] = ((window)W)->BorderPattern[1] = NULL;
	}
    }
}

/* handle common keyboard actions like cursor moving */
void FallBackKeyAction(window W, event_keyboard *EventK) {
    ldat NumRow, OldNumRow;
    
    if (W->Attrib & WINDOW_AUTO_KEYS) switch (EventK->Code) {
      case TW_Up:
	if (!W->HLogic)
	    break;
	OldNumRow=W->CurY;
	if (OldNumRow<MAXLDAT) {
	    if (!OldNumRow)
		NumRow=W->HLogic-(ldat)1;
	    else
		NumRow=OldNumRow-(ldat)1;
	    W->CurY=NumRow;
	    if (W->Flags & WINFL_SEL_ROWCURR)
		DrawLogicWindow2(W, (ldat)0, OldNumRow, (ldat)MAXDAT-(ldat)2, OldNumRow);
	}
	else
	    W->CurY=NumRow=W->HLogic-(ldat)1;
	if (W->Flags & WINFL_SEL_ROWCURR)
	    DrawLogicWindow2(W, (ldat)0, NumRow, (ldat)MAXDAT-(ldat)2, NumRow);
	UpdateCursor();
	break;
      case TW_Down:
	if (!W->HLogic)
	    break;
	OldNumRow=W->CurY;
	if (OldNumRow<MAXLDAT) {
	    if (OldNumRow>=W->HLogic-(ldat)1)
		NumRow=(ldat)0;
	    else
		NumRow=OldNumRow+(ldat)1;
	    W->CurY=NumRow;
	    if (W->Flags & WINFL_SEL_ROWCURR)
		DrawLogicWindow2(W, (ldat)0, OldNumRow, (ldat)MAXDAT-(ldat)2, OldNumRow);
	}
	else
	    W->CurY=NumRow=(ldat)0;
	if (W->Flags & WINFL_SEL_ROWCURR)
	    DrawLogicWindow2(W, (ldat)0, NumRow, (ldat)MAXDAT-(ldat)2, NumRow);
	UpdateCursor();
	break;
      case TW_Left:
	if (W->CurX > 0) {
	    W->CurX--;
	    UpdateCursor();
	}
	break;
      case TW_Right:
	if (((W->Flags & WINFL_USECONTENTS) && W->CurX < W->XWidth - 3) ||
	    (!(W->Flags & WINFL_USEANY) && W->CurX < MAXLDAT - 1)) {
	    W->CurX++;
	    UpdateCursor();
	}
	break;
      default:
	break;
    }
}

/*
 * create a (malloced) array of non-space args
 * from arbitrary text command line
 * 
 * FIXME: need proper handling of double quotes:
 * "a b" is the string `a b' NOT the two strings `"a' `b"'
 * (same for single quotes, backslashes, ...)
 */
byte **TokenizeStringVec(uldat len, byte *s) {
    byte **cmd = NULL, *buf, c;
    uldat save_len, save_n, n = 0;
    
    /* skip initial spaces */
    while (len && ((c = *s) == '\0' || c == ' ')) {
	len--, s++;
    }
    save_len = len;
    
    if (len && (buf = AllocMem(len + 1))) {
	CopyMem(s, buf, len);
	buf[len] = '\0';
	
	/* how many args? */
	while (len) {
	    len--, c = *s++;
	    if (c && c != ' ') {
		n++;
		while (len && (c = *s) && c != ' ') {
		    len--, s++;
		}
	    }
	}
	if ((cmd = AllocMem((n + 1) * sizeof(byte *)))) {
	    save_n = n;
	    n = 0;
	    len = save_len;
	    s = buf;
	    
	    /* put args in cmd[] */
	    while (len) {
		len--, c = *s++;
		if (c && c != ' ') {
		    cmd[n++] = s - 1;
		    while (len && (c = *s) && c != ' ') {
			len--, s++;
		    }
		    *s = '\0'; /* safe, we did a malloc(len+1) */
		}
	    }
	    cmd[n] = NULL; /* safe, we did a malloc(n+1) */
	}
    }
    return cmd;
}

void FreeStringVec(byte **cmd) {
    if (cmd) {
	FreeMem(cmd[0]);
	FreeMem(cmd);
    }
}




int unixFd;
uldat unixSlot;

static void TWDisplayIO(int fd, uldat slot) {
    struct sockaddr_un un_addr;
    int len = sizeof(un_addr);
    
    if ((fd = accept(fd, (struct sockaddr *)&un_addr, &len)) >= 0) {
	close(fd);
    }
}

static byte fullTWD[]="/tmp/.Twin:\0\0\0";
static byte envTWD[]="TWDISPLAY=\0\0\0\0";

/* set TWDISPLAY and create /tmp/.Twin:<x> */
byte InitTWDisplay(void) {
    struct sockaddr_un addr;
    int i, fd = NOFD;
    byte *arg0, ok;
    
    HOME = getenv("HOME");

    
    if ((unixFd = socket(AF_UNIX, SOCK_STREAM, 0)) >= 0) {

    WriteMem(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;

	for (i=0; i<0x1000; i++) {
	    sprintf(fullTWD+11, "%x", i);
	    CopyStr(fullTWD, addr.sun_path);

	    ok = bind(unixFd, (struct sockaddr *)&addr, sizeof(addr)) >= 0;
	    if (!ok) {
		/* maybe /tmp/.Twin:<x> is already in use... */
		if (fd >= 0 || (fd = socket(AF_UNIX, SOCK_STREAM, 0)) >= 0) {
		    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) >= 0) {
			/*
			 * server is alive, try to grab another TWDISPLAY.
			 * also, we must junk `fd' since SOCK_STREAM sockets
			 * can connect() only once
			 */
			close(fd);
			fd = NOFD;
			continue;
		    }
		    /* either server is dead or is owned by another user */
		    if (unlink(fullTWD) >= 0) {
			/*
			 * if we have permission to delete it, we also had
			 * the permission to connect to it (hope).
			 * So it must have been be a stale socket.
			 * 
			 * Trying to delete a /tmp/.Twin:<x> entry we cannot
			 * connect to wreaks havoc if you mix this twin server
			 * with older ones, but having two different servers
			 * installed on one system should be rare enough.
			 */
			ok = bind(unixFd, (struct sockaddr *)&addr, sizeof(addr)) >= 0;
		    }
		}
	    }
	    if (ok) {
		if (chmod(fullTWD, 0700) >= 0 &&
		    listen(unixFd, 3) >= 0 &&
		    fcntl(unixFd, F_SETFD, FD_CLOEXEC) >= 0) {
		    
		    if ((unixSlot = RegisterRemoteFd(unixFd, TWDisplayIO)) != NOSLOT) {
			
			if (fd != NOFD)
			    close(fd);
			TWDisplay = fullTWD+10;
			lenTWDisplay = LenStr(TWDisplay);
			CopyMem(TWDisplay, envTWD+10, lenTWDisplay);
			putenv(envTWD);
			putenv("TERM=linux");
			if ((arg0 = AllocMem(LenStr(TWDisplay) + 6))) {
			    sprintf(arg0, "twin %s", TWDisplay);
			    SetArgv_0(main_argv, arg0);
			    FreeMem(arg0);
			}
			return TRUE;
		    }
		} else
		    Error(SYSCALLERROR);
		close(unixFd);
	    }
	}
    }
    if (fd != NOFD)
	close(fd);
    printk("twin: all TWDISPLAY already in use!\n"
	    "      Please cleanup stale /tmp/.Twin* sockets and try again\n");
    return FALSE;
}

/* unlink /tmp/.Twin<TWDISPLAY> */
void QuitTWDisplay(void) {
    unlink(fullTWD);
}




/* suid/sgid privileges related functions */

static enum { none, sgidtty, suidroot } Privilege = none;
static gid_t tty_grgid;

gid_t get_tty_grgid(void) {
    struct group *gr;
    
    if (!tty_grgid) {
	if ((gr = getgrnam("tty")))
	    tty_grgid = gr->gr_gid;
	else
	    tty_grgid = (gid_t)-1;
    }
    return tty_grgid;
}

void CheckPrivileges(void) {
    if (GetRootPrivileges() >= 0)
	Privilege = suidroot;
    else {
	(void)get_tty_grgid();
	if (tty_grgid != (gid_t)-1 && GetGroupPrivileges(tty_grgid) >= 0)
	    Privilege = sgidtty;
    }
    
    DropPrivileges();
    
#if 0
# ifdef CONF_TERM
#  ifdef CONF_TERM_DEVPTS
    if (Privilege < sgidtty) {
	byte c;
	printk("twin: not running setgid tty.\n"
		"      might be unable to start the terminal emulator.\n"
		"      hit RETURN to continue, or CTRL-C to quit.\n");
	read(0, &c, 1);
    }
#  else /* !CONF_TERM_DEVPTS */
    if (Privilege < suidroot) {
	byte c;
	printk("twin: not running setuid root.\n"
		"      might be unable to start the terminal emulator.\n"
		"      hit RETURN to continue, or CTRL-C to quit.\n");
	read(0, &c, 1);
    }
#  endif /* CONF_TERM_DEVPTS */
    
# else /* !CONF_TERM */

#  ifdef CONF__MODULES
    if (Privilege == none) {
	byte c;
	printk("twin: not running setgid tty or setuid root.\n"
		"      might be unable to start the terminal emulator module.\n"
		"      hit RETURN to continue, or CTRL-C to quit.\n");
	read(0, &c, 1);
    }
#  endif /* CONF__MODULES */
# endif /* CONF_TERM */
#endif /* 0 */
}

void GetPrivileges(void) {
    if (Privilege == suidroot)
	GetRootPrivileges();
    else if (Privilege == sgidtty)
	GetGroupPrivileges(get_tty_grgid());
}

/* finally, functions to manage Ids */

static obj *IdList[magic_n];
static uldat IdSize[magic_n], IdTop[magic_n], IdBottom[magic_n];

INLINE uldat IdListGrow(byte i) {
    uldat oldsize, size;
    obj *newIdList;
    
    oldsize = IdSize[i];
    if (oldsize >= MAXID)
	return NOSLOT;
    
    size = oldsize < SMALLBUFF/3 ? SMALLBUFF/2 : oldsize + (oldsize>>1);
    if (size > MAXID)
	size = MAXID;
    
    if (!(newIdList = (obj *)ReAllocMem(IdList[i], size*sizeof(obj))))
	return NOSLOT;
    
    WriteMem(newIdList+oldsize, 0, (size-oldsize)*sizeof(obj));
    
    IdList[i] = newIdList;
    IdSize[i] = size;
    
    return oldsize;
}

INLINE void IdListShrink(byte i) {
    obj *newIdList;
    uldat size = Max2(BIGBUFF, IdTop[i] << 1);
    
    if (size < IdSize[i] && (newIdList = (obj *)ReAllocMem(IdList[i], size*sizeof(obj)))) {
	IdList[i] = newIdList;
	IdSize[i] = size;
    }
}

INLINE uldat IdListGet(byte i) {
    if (IdBottom[i] == IdSize[i])
	return IdListGrow(i);
    
    return IdBottom[i];
}

INLINE byte _AssignId(byte i, obj Obj) {
    uldat Id, j;
    if ((Id = IdListGet(i)) != NOSLOT) {
	Obj->Id = Id | ((uldat)i << magic_shift);
	IdList[i][Id] = Obj;
	if (IdTop[i] <= Id)
	    IdTop[i] = Id + 1;
	for (j = IdBottom[i] + 1; j < IdTop[i]; j++)
	    if (!IdList[i][j])
		break;
	IdBottom[i] = j;
	return TRUE;
    }
    Error(NOTABLES);
    return FALSE;
}

INLINE void _DropId(byte i, obj Obj) {
    uldat Id = Obj->Id & MAXID, j;

    if (Id < IdTop[i] && IdList[i][Id] == Obj /* paranoia */) {
	Obj->Id = NOID;
	IdList[i][Id] = (obj)0;
	if (IdBottom[i] > Id)
	    IdBottom[i] = Id;
	for (j = IdTop[i] - 1; j > IdBottom[i]; j--)
	    if (IdList[i][j])
		break;
	IdTop[i] = (j == IdBottom[i]) ? j : j + 1;
	
	if (IdSize[i] > (IdTop[i] << 4) && IdSize[i] > BIGBUFF)
	    IdListShrink(i);
    }
}
    
byte AssignId(CONST fn_obj Fn_Obj, obj Obj) {
    byte i;
    if (Obj) switch (Fn_Obj->Magic) {
      case obj_magic:
	/* 'obj' is just a template type, you can't create one */
	break;
      case row_magic:
      case module_magic:
      case display_hw_magic:
      case msg_magic:
	/*
	 * we don't use Ids for rows and msgs as we expect to create *lots* of them
	 * it's unsafe to allow modules and display_hw access remotely,
	 * so no Ids for them too
	 */
        Obj->Id = Fn_Obj->Magic;
	return TRUE;
      case widget_magic:
      case gadget_magic:	
      case window_magic:
      case screen_magic:
      case group_magic:
      case menuitem_magic:
      case menu_magic:
      case msgport_magic:
      case mutex_magic:
	i = Fn_Obj->Magic >> magic_shift;
	return _AssignId(i, Obj);
      default:
	break;
    }
    return FALSE;
}

void DropId(obj Obj) {
    byte i = 0;
    if (Obj && Obj->Fn) switch (Obj->Fn->Magic) {
      case obj_magic:
	/* 'obj' is just a template type, you can't create one */
	break;
      case row_magic:
      case module_magic:
      case display_hw_magic:
      case msg_magic:
	/* we don't use Ids for rows and msgs as we expect to create *lots* of them */
	/* it's unsafe to allow modules access remotely, so no Ids for them too */
	Obj->Id = NOID;
	break;
      case widget_magic:
      case gadget_magic:	
      case window_magic:
      case screen_magic:
      case group_magic:
      case menuitem_magic:
      case menu_magic:
      case msgport_magic:
      case mutex_magic:
	i = Obj->Fn->Magic >> magic_shift;
	if (i == (Obj->Id >> magic_shift))
	    _DropId(i, Obj);
	break;
      default:
	break;
    }
}

obj Id2Obj(byte i, uldat Id) {
    byte I = Id >> magic_shift;
    
    if (i < magic_n && I < magic_n) {
	/* everything is a valid (obj) */
	/* gadgets, windows, screens are valid (widget) */
	if (i == I || i == (obj_magic >> magic_shift) ||
	    (i == (widget_magic >> magic_shift) &&
	     (I == (gadget_magic >> magic_shift) ||
	      I == (window_magic >> magic_shift) ||
	      I == (screen_magic >> magic_shift)))) {
	    
	    Id &= MAXID;
	    if (Id < IdTop[I])
		return IdList[I][Id];
	}
    }
    return (obj)0;
}
