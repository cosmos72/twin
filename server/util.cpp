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

#include "twin.h"

#ifdef TW_HAVE_PWD_H
#include <pwd.h>
#endif
#ifdef TW_HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef TW_HAVE_SIGNAL_H
#include <signal.h>
#endif
#ifdef TW_HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef TW_HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef TW_HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#include "algo.h"
#include "alloc.h"
#include "common.h"
#include "data.h"
#include "extreg.h"
#include "methods.h"
#include "main.h"
#include "draw.h"
#include "remote.h"
#include "resize.h"
#include "printk.h"
#include "privilege.h"
#include "util.h"
#include "stl/vector.h"
#include "hw.h"

#include <Tw/Twkeys.h>
#include <Tutf/Tutf.h>

void NormalizeTime(timevalue *Time) {
  if (Time->Fraction >= FullSEC || Time->Fraction < 0) {
    tany delta = Time->Fraction / FullSEC;
    Time->Seconds += delta;
    Time->Fraction -= delta * FullSEC;
  }
}

timevalue *InstantNow(timevalue *Now) {
#if defined(TW_HAVE_GETTIMEOFDAY)
  struct timeval sysNow;

  gettimeofday(&sysNow, NULL);

  Now->Seconds = sysNow.tv_sec;
  Now->Fraction = sysNow.tv_usec MicroSECs;
#elif defined(TW_HAVE_FTIME)
  timeb sysNow;

  ftime(&sysNow);

  Now->Seconds = sysNow.time;
  Now->Fraction = sysNow.millitm MilliSECs;
#else
  Now->Seconds = time(NULL);
  Now->Fraction = 0;
#endif
  return Now;
}

timevalue *IncrTime(timevalue *Time, timevalue *Incr) {
  NormalizeTime(Time);
  NormalizeTime(Incr);

  Time->Seconds += Incr->Seconds;
  if ((Time->Fraction += Incr->Fraction) >= FullSEC) {
    Time->Seconds++;
    Time->Fraction -= FullSEC;
  }
  return Time;
}

timevalue *DecrTime(timevalue *Time, timevalue *Decr) {
  NormalizeTime(Time);
  NormalizeTime(Decr);

  Time->Seconds -= Decr->Seconds;
  if (Time->Fraction >= Decr->Fraction)
    Time->Fraction -= Decr->Fraction;
  else {
    Time->Seconds--;
    Time->Fraction += (FullSEC - Decr->Fraction);
  }
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

  if (T1->Seconds > T2->Seconds)
    return (dat)1;
  if (T1->Seconds < T2->Seconds)
    return (dat)-1;
  if (T1->Fraction > T2->Fraction)
    return (dat)1;
  if (T1->Fraction < T2->Fraction)
    return (dat)-1;
  return (dat)0;
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

byte Minimum(byte MaxIndex, const ldat *Array) {
  byte i, MinIndex;
  ldat Temp;

  if (!MaxIndex)
    return 0xFF;

  Temp = Array[0];
  MinIndex = 0;

  for (i = 1; i < MaxIndex; i++) {
    if (Array[i] < Temp) {
      Temp = Array[i];
      MinIndex = i;
    }
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

     *Error = ttrue;
     return (byte)0;
}

uldat HexStrToNum(byte *StringHex) {
     byte Len, Error = tfalse;
     uldat Value=(uldat)0;

     Len=(byte)strlen(StringHex);
     if (Len>(byte)8)
         return (uldat)0;

     do {
         Len--;
         Value |= (uldat)HexDigitToNum(StringHex[Len], &Error) << (Len << 2);
     }        while (!Error && Len>(byte)0);

    return Error ? (uldat)0 : Value;
}
*/

/* adapted from similar code in bdflush */
uldat ComputeUsableLenArgv(char *const *argv) {
  char *ptr;
  uldat count;

  ptr = argv[0] + strlen(argv[0]);
  for (count = 1; argv[count]; count++) {
    if (argv[count] == ptr + 1)
      ptr += strlen(ptr + 1) + 1;
  }
  return ptr - argv[0];
}

void SetArgv0(char *const *argv, uldat argv_usable_len, const char *src) {
  uldat len = strlen(src);

  if (len + 1 < argv_usable_len) {
    CopyMem(src, argv[0], len);
    memset(argv[0] + len, '\0', argv_usable_len - len);
  } else
    CopyMem(src, argv[0], argv_usable_len);
}

/*
 * move a msgport to the right place in an already sorted list,
 * ordering by CallTime
 */
void SortMsgPortByCallTime(msgport Port) {
  msgport other;
  if ((other = Port->Next) && CmpCallTime(Port, other) > 0) {
    Port->Remove();
    do {
      other = other->Next;
    } while (other && CmpCallTime(Port, other) > 0);
    if (other)
      InsertMiddle(MsgPort, Port, All, other->Prev, other);
    else
      InsertLast(MsgPort, Port, All);
  } else if ((other = Port->Prev) && CmpCallTime(Port, other) < 0) {
    Port->Remove();
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

    Max->Remove();
    /*
     * HACK : we create a parentless list
     * backward, from End adding ->Prev until Start
     */
    Max->Next = Start;
    Max->All = All;
    if (Start)
      Start->Prev = Max;
    Start = Max;
    if (!End)
      End = Max;
  }
  All->FirstMsgPort = Start;
  All->LastMsgPort = End;
}

byte SendControlMsg(msgport MsgPort, udat Code, udat Len, const char *Data) {
  msg Msg;
  event_control *Event;

  if (MsgPort && (Msg = New(msg)(msg_control, Len))) {
    Event = &Msg->Event.EventControl;
    Event->Code = Code;
    Event->Len = Len;
    CopyMem(Data, Event->Data, Len);
    SendMsg(MsgPort, Msg);

    return ttrue;
  }
  return tfalse;
}

byte SelectionStore(uldat magic, const char mime[MAX_MIMELEN], Chars data) {
  selection Sel = All->Selection;
  if (mime) {
    CopyMem(mime, Sel->MIME, MAX_MIMELEN);
  } else {
    memset(Sel->MIME, '\0', MAX_MIMELEN);
  }
  Sel->Owner = NULL;
  Sel->Magic = magic;
  Sel->Data.clear();
  return Sel->Data.append(data);
}

byte SelectionAppend(Chars data) {
  return All->Selection->Data.append(data);
}

byte SelectionAppendRune(trune rune) {
  return All->Selection->Data.append(Utf8(rune));
}

byte SelectionAppendRunes(TRunes runes) {
  return All->Selection->Data.append(runes);
}

#define _SEL_MAGIC SEL_UTF8MAGIC
#define _SelAppendNL() SelectionAppend(Chars("\n", 1));

class Appender {
public:
  Appender() : spaces(0) {
  }

  bool rune(trune rune) {
    if (rune == ' ') {
      spaces++;
      return true;
    }
    return flush() && SelectionAppendRune(rune);
  }

  bool nl() {
    const uldat n = spaces;
    spaces = 0;
    switch (n) {
    case 0:
      return true;
    case 1:
      return SelectionAppend(Chars(" ", 1));
    default:
      return _SelAppendNL();
    }
  }

  bool flush() {
    const Chars buf(" ", 1);
    for (; spaces; spaces--) {
      if (!SelectionAppend(buf)) {
        return false;
      }
    }
    return true;
  }

private:
  uldat spaces;
};

bool SetSelectionFromWindow(window w) {
  ldat y;
  uldat slen, i, spaces = 0;
  bool ok = true, w_useC = W_USE(w, USECONTENTS);

  if (!(w->State & WINDOW_DO_SEL)) {
    return ok;
  }

  if (!(w->State & WINDOW_ANYSEL) || w->YstSel > w->YendSel ||
      (w->YstSel == w->YendSel && w->XstSel > w->XendSel)) {

    ok = SelectionStore(_SEL_MAGIC, NULL, Chars());
    if (ok) {
      NeedHW |= NEEDSelectionExport;
    }
    return ok;
  }

  /* normalize negative coords */
  if (w->XstSel < 0) {
    w->XstSel = 0;
  } else if (w_useC && w->XstSel >= w->WLogic) {
    w->XstSel = 0;
    w->YstSel++;
  }
  if (w->YstSel < w->YLogic) {
    w->YstSel = w->YLogic;
    w->XstSel = 0;
  } else if (w->YstSel >= w->HLogic) {
    w->YstSel = w->HLogic - 1;
    w->XstSel = w_useC ? w->WLogic - 1 : TW_MAXLDAT;
  }

  if (w_useC) {
    Appender appender;
    const tcell *cells;

    /* normalize negative coords */
    if (w->XendSel < 0) {
      w->XendSel = w->WLogic - 1;
      w->YendSel--;
    } else if (w->XendSel >= w->WLogic)
      w->XendSel = w->WLogic - 1;

    if (w->YendSel < w->YLogic) {
      w->YendSel = w->YLogic;
      w->XendSel = 0;
    } else if (w->YendSel >= w->HLogic) {
      w->YendSel = w->HLogic - 1;
      w->XendSel = w->WLogic - 1;
    }

    slen = w->WLogic;
    cells = w->USE.C.Contents + (w->YstSel + w->USE.C.HSplit) * slen;
    while (cells >= w->USE.C.TtyData->Split) {
      cells -= w->USE.C.TtyData->Split - w->USE.C.Contents;
    }

    do {
      y = w->YstSel;
      if (y < w->YendSel) {
        slen -= w->XstSel;
      } else {
        slen = w->XendSel - w->XstSel + 1;
      }
      cells += w->XstSel;
      ok = SelectionStore(_SEL_MAGIC, NULL, Chars());
      for (i = slen; ok && i--; cells++) {
        ok = appender.rune(TRUNE(*cells));
      }
      if (!ok) {
        break;
      }

      if (cells >= w->USE.C.TtyData->Split) {
        cells -= w->USE.C.TtyData->Split - w->USE.C.Contents;
      }
      slen = w->WLogic;
      for (y = w->YstSel + 1; ok && y < w->YendSel; y++) {
        appender.nl();
        if (cells >= w->USE.C.TtyData->Split) {
          cells -= w->USE.C.TtyData->Split - w->USE.C.Contents;
        }
        for (i = slen; ok && i--; cells++) {
          ok = appender.rune(TRUNE(*cells));
        }
      }
      if (!ok) {
        break;
      }

      if (w->YendSel > w->YstSel) {
        if (cells >= w->USE.C.TtyData->Split) {
          cells -= w->USE.C.TtyData->Split - w->USE.C.Contents;
        }
        appender.nl();
        for (i = slen = w->XendSel + 1; ok && i--; cells++) {
          ok = appender.rune(TRUNE(*cells));
        }
      }
      if (ok) {
        ok = appender.flush();
        if (ok) {
          NeedHW |= NEEDSelectionExport;
        }
      }
    } while (false);

    return ok;

  } else if (!W_USE(w, USEROWS)) {
    return false;
  }

  ok = SelectionStore(_SEL_MAGIC, NULL, Chars());
  row r;

  /* Gap not supported! */
  y = w->YstSel;
  r = w->FindRow(y);
  if (r && r->Text) {
    if (y < w->YendSel) {
      slen = r->Len - w->XstSel;
    } else {
      slen = Min2(r->Len, (uldat)w->XendSel + 1) - Min2(r->Len, (uldat)w->XstSel);
    }

    const trune *runes = r->Text + Min2(r->Len, (uldat)w->XstSel);
    ok = ok && SelectionAppendRunes(TRunes(runes, slen));
  }

  if (y < w->YendSel || !r || !r->Text || r->Len <= (uldat)w->XendSel) {
    ok = ok && _SelAppendNL();
  }

  for (y = w->YstSel + 1; ok && y < w->YendSel; y++) {
    if ((r = w->FindRow(y)) && r->Text) {
      ok = ok && SelectionAppendRunes(TRunes(r->Text, r->Len));
    }
    ok = ok && _SelAppendNL();
  }
  if (w->YendSel > w->YstSel) {
    if (w->XendSel >= 0 && (r = w->FindRow(w->YendSel)) && r->Text) {
      slen = Min2(r->Len, (uldat)w->XendSel + 1);
      ok = ok && SelectionAppendRunes(TRunes(r->Text, slen));
    }
    if (!r || !r->Text || r->Len <= (uldat)w->XendSel) {
      ok = ok && _SelAppendNL();
    }
  }
  if (ok) {
    NeedHW |= NEEDSelectionExport;
  }
  return ok;
}

byte CreateXTermMouseEvent(event_mouse *Event, byte buflen, char *buf) {
  window w;
  udat Flags;
  udat Code = Event->Code;
  dat x = Event->X, y = Event->Y;
  byte len = 0;

  if (!(w = (window)Event->W) || !IS_WINDOW(w) || !W_USE(w, USECONTENTS) || !w->USE.C.TtyData)
    return len;

  Flags = w->USE.C.TtyData->Flags;

  if (Flags & TTY_REPORTMOUSE_TWTERM) {
    /* new-style reporting */

    /* if TTY_REPORTMOUSE_ALSO_MOVE is set, also report motion */
    if (buflen < 9 || (Code == MOVE_MOUSE && !(Flags & TTY_REPORTMOUSE_ALSO_MOVE))) {
      /* buffer too small, or nothing to report */
      return len;
    } else if (isPRESS(Code)) {
      /* report also button just pressed as down */
      Code |= HOLD_CODE(PRESS_N(Code));
    }

    CopyMem("\033[5M", buf, 4);
    buf[4] = ' ' + ((Code & HOLD_ANY) >> HOLD_BITSHIFT);
    buf[5] = '!' + (x & 0x7f);
    buf[6] = '!' + ((x >> 7) & 0x7f);
    buf[7] = '!' + (y & 0x7f);
    buf[8] = '!' + ((y >> 7) & 0x7f);
    len = 9;
  } else if (Flags & TTY_REPORTMOUSE_XTERM) {
    /* classic xterm-style reporting */

    if (buflen < 6)
      /* buffer too small! */
      return len;

    CopyMem("\033[M", buf, 3);

    if (isSINGLE_PRESS(Code)) {
      switch (Code & PRESS_ANY) {
      case PRESS_LEFT:
        buf[3] = ' ';
        break;
      case PRESS_MIDDLE:
        buf[3] = '!';
        break;
      case PRESS_RIGHT:
        buf[3] = '\"';
        break;
        /* WHEEL_REV and WHEEL_FWD supported only at release */
      }
    } else if (isRELEASE(Code)) {
      switch (Code & RELEASE_ANY) {
#ifdef HOLD_WHEEL_REV
      case RELEASE_WHEEL_REV:
        buf[3] = '`';
        break;
#endif
#ifdef HOLD_WHEEL_FWD
      case RELEASE_WHEEL_FWD:
        buf[3] = 'a';
        break;
#endif
      default:
        buf[3] = '#';
        break;
      }
    } else if (Code != MOVE_MOUSE && (Flags & TTY_REPORTMOUSE_ALSO_MOVE)) {
      /* also report mouse dragging */
      buf[3] = '@';
    } else {
      /* pure mouse move (without pressed buttons) is not reported */
      return len;
    }

    buf[4] = '!' + x;
    buf[5] = '!' + y;
    len = 6;
  }
  return len;
}

void ResetBorderPattern(void) {
  msgport MsgP;
  widget w;

  for (MsgP = All->FirstMsgPort; MsgP; MsgP = MsgP->Next) {
    for (w = MsgP->FirstW; w; w = w->O_Next) {
      if (IS_WINDOW(w))
        ((window)w)->BorderPattern[0] = ((window)w)->BorderPattern[1] = NULL;
    }
  }
}

static gadget _PrevGadget(gadget G) {
  while (G->Prev) {
    G = (gadget)G->Prev;
    if (IS_GADGET(G))
      return (gadget)G;
  }
  return (gadget)G->Prev;
}

static gadget _NextGadget(gadget G) {
  while (G->Next) {
    G = (gadget)G->Next;
    if (IS_GADGET(G))
      return (gadget)G;
  }
  return (gadget)G->Next;
}

/* handle common keyboard actions like cursor moving and button navigation */
void FallBackKeyAction(window w, event_keyboard *EventK) {
  ldat NumRow, OldNumRow;
  gadget G, H;

  if ((G = (gadget)w->SelectW) && IS_GADGET(G))
    switch (EventK->Code) {
    case TW_Escape:
      UnPressGadget(G, tfalse);
      w->SelectW = (widget)0;
      break;
    case TW_Return:
      UnPressGadget(G, ttrue);
      PressGadget(G);
      break;
    case TW_Up:
    case TW_Left:
      if ((H = _PrevGadget(G))) {
        if (!(G->Flags & GADGETFL_TOGGLE))
          UnPressGadget(G, tfalse);
        w->SelectW = (widget)H;
        PressGadget(H);
      }
      break;
    case TW_Down:
    case TW_Right:
    case TW_Tab:
      if ((H = _NextGadget(G))) {
        if (!(G->Flags & GADGETFL_TOGGLE))
          UnPressGadget(G, tfalse);
        w->SelectW = (widget)H;
        PressGadget(H);
      }
      break;
    default:
      break;
    }
  else if ((G = (gadget)w->FirstW) && IS_GADGET(G)) {
    PressGadget(G);
    w->SelectW = (widget)G;
  } else
    switch (EventK->Code) {
    case TW_Up:
      if (!w->HLogic)
        break;
      OldNumRow = w->CurY;
      if (OldNumRow < TW_MAXLDAT) {
        if (!OldNumRow)
          NumRow = w->HLogic - (ldat)1;
        else
          NumRow = OldNumRow - (ldat)1;
        w->CurY = NumRow;
        if (w->Flags & WINDOWFL_ROWS_SELCURRENT)
          DrawLogicWidget((widget)w, (ldat)0, OldNumRow, (ldat)TW_MAXDAT - (ldat)2, OldNumRow);
      } else
        w->CurY = NumRow = w->HLogic - (ldat)1;
      if (w->Flags & WINDOWFL_ROWS_SELCURRENT)
        DrawLogicWidget((widget)w, (ldat)0, NumRow, (ldat)TW_MAXDAT - (ldat)2, NumRow);
      UpdateCursor();
      break;
    case TW_Down:
      if (!w->HLogic)
        break;
      OldNumRow = w->CurY;
      if (OldNumRow < TW_MAXLDAT) {
        if (OldNumRow >= w->HLogic - (ldat)1)
          NumRow = (ldat)0;
        else
          NumRow = OldNumRow + (ldat)1;
        w->CurY = NumRow;
        if (w->Flags & WINDOWFL_ROWS_SELCURRENT)
          DrawLogicWidget((widget)w, (ldat)0, OldNumRow, (ldat)TW_MAXDAT - (ldat)2, OldNumRow);
      } else
        w->CurY = NumRow = (ldat)0;
      if (w->Flags & WINDOWFL_ROWS_SELCURRENT)
        DrawLogicWidget((widget)w, (ldat)0, NumRow, (ldat)TW_MAXDAT - (ldat)2, NumRow);
      UpdateCursor();
      break;
    case TW_Left:
      if (w->CurX > 0) {
        w->CurX--;
        UpdateCursor();
      }
      break;
    case TW_Right:
      if ((W_USE(w, USECONTENTS) && w->CurX < w->XWidth - 3) ||
          (W_USE(w, USEROWS) && w->CurX < TW_MAXLDAT - 1)) {
        w->CurX++;
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
char **TokenizeStringVec(uldat len, char *s) {
  char **cmd = NULL, *buf, c;
  uldat save_len, n = 0;

  /* skip initial spaces */
  while (len && ((c = *s) == '\0' || c == ' ')) {
    len--, s++;
  }
  save_len = len;

  if (len && (buf = (char *)AllocMem(len + 1))) {
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
    if ((cmd = (char **)AllocMem((n + 1) * sizeof(char *)))) {
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

void FreeStringVec(char **cmd) {
  if (cmd) {
    FreeMem(cmd[0]);
    FreeMem(cmd);
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
char **TokenizeTRuneVec(uldat len, trune *s) {
  char **cmd = NULL, *buf, *v;
  trune c;
  uldat save_len, n = 0, i;

  /* skip initial spaces */
  while (len && ((c = *s) == '\0' || c == ' ')) {
    len--, s++;
  }
  save_len = len;

  if (len && (buf = (char *)AllocMem(len + 1))) {
    for (i = 0; i < len; i++)
      buf[i] = s[i];
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
    if ((cmd = (char **)AllocMem((n + 1) * sizeof(char *)))) {
      n = 0;
      len = save_len;
      v = buf;

      /* put args in cmd[] */
      while (len) {
        len--, c = *v++;
        if (c && c != ' ') {
          cmd[n++] = v - 1;
          while (len && (c = *v) && c != ' ') {
            len--, v++;
          }
          *v = '\0'; /* safe, we did a malloc(len+1) */
        }
      }
      cmd[n] = NULL; /* safe, we did a malloc(n+1) */
    }
  }
  return cmd;
}

int unixFd;
uldat unixSlot;

static void TWDisplayIO(int fd, uldat slot) {
  struct sockaddr_un un_addr;
  socklen_t len = sizeof(un_addr);

  if ((fd = accept(fd, (struct sockaddr *)&un_addr, &len)) >= 0) {
    close(fd);
  }
}

static char envTWD[] = "TWDISPLAY=\0\0\0\0\0";

const char *TmpDir(void) {
  const char *tmp = getenv("TMPDIR");
  if (tmp == NULL)
    tmp = "/tmp";
  return tmp;
}

udat CopyToSockaddrUn(const char *src, struct sockaddr_un *addr, udat pos) {
  size_t len = strlen(src), max = sizeof(addr->sun_path) - 1; /* for final '\0' */
  if (pos < max) {
    if (len >= max - pos)
      len = max - pos - 1;
    CopyMem(src, addr->sun_path + pos, len);
    addr->sun_path[pos += len] = '\0';
  }
  return pos;
}

static struct sockaddr_un addr_unix;
static const char *fullTWD = addr_unix.sun_path;
static char twd[12];

/* set TWDISPLAY and create /tmp/.Twin:<x> */
byte InitTWDisplay(void) {
  char *arg0;
  int fd = NOFD;
  unsigned short i;
  udat len;
  byte ok;

  HOME = getenv("HOME");
  memset(&addr_unix, 0, sizeof(addr_unix));

  if ((unixFd = socket(AF_UNIX, SOCK_STREAM, 0)) >= 0) {

    addr_unix.sun_family = AF_UNIX;

    for (i = 0; i < 0x1000; i++) {
      sprintf(twd, ":%hx", i);

      len = CopyToSockaddrUn(TmpDir(), &addr_unix, 0);
      len = CopyToSockaddrUn("/.Twin", &addr_unix, len);
      len = CopyToSockaddrUn(twd, &addr_unix, len);

      ok = bind(unixFd, (struct sockaddr *)&addr_unix, sizeof(addr_unix)) >= 0;
      if (!ok) {
        Error(SYSERROR);
        /* maybe /tmp/.Twin:<x> is already in use... */
        if (fd >= 0 || (fd = socket(AF_UNIX, SOCK_STREAM, 0)) >= 0) {
          if (connect(fd, (struct sockaddr *)&addr_unix, sizeof(addr_unix)) >= 0) {
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
            ok = bind(unixFd, (struct sockaddr *)&addr_unix, sizeof(addr_unix)) >= 0;
          }
        }
      }
      if (ok) {
        if (chmod(fullTWD, 0700) >= 0 && listen(unixFd, 3) >= 0 &&
            fcntl(unixFd, F_SETFD, FD_CLOEXEC) >= 0) {

          if ((unixSlot = RegisterRemoteFd(unixFd, TWDisplayIO)) != NOSLOT) {

            if (fd != NOFD)
              close(fd);
            TWDisplay = twd;
            lenTWDisplay = strlen(TWDisplay);
            CopyMem(TWDisplay, envTWD + 10, lenTWDisplay);
#if defined(TW_HAVE_SETENV)
            setenv("TWDISPLAY", TWDisplay, 1);
            setenv("TERM", "linux", 1);
#elif defined(TW_HAVE_PUTENV)
            putenv(envTWD);
            putenv("TERM=linux");
#endif
            if ((arg0 = (char *)AllocMem(strlen(TWDisplay) + 6))) {
              sprintf(arg0, "twin %s", TWDisplay);
              SetArgv0(main_argv, main_argv_usable_len, arg0);
              FreeMem(arg0);
            }
            return ttrue;
          }
        } else
          Error(SYSERROR);
        close(unixFd);
      }
    }
  }
  if (fd != NOFD)
    close(fd);

  CopyToSockaddrUn(TmpDir(), &addr_unix, 0);
  arg0 = addr_unix.sun_path;

  printk("twin: failed to create any " SS "/.Twin* socket: " SS "\n", addr_unix.sun_path, Errstr);
  printk("      possible reasons: either " SS " not writable, or all TWDISPLAY already in use,\n"
         "      or too many stale " SS "/.Twin* sockets. Aborting.\n",
         arg0, arg0);
  return tfalse;
}

/* unlink /tmp/.Twin<TWDISPLAY> */
void QuitTWDisplay(void) {
  unlink(fullTWD);
}

static e_privilege Privilege;
static uid_t Uid, EUid;
static gid_t tty_grgid;

byte CheckPrivileges(void) {
  Uid = getuid();
  EUid = geteuid();
  tty_grgid = get_tty_grgid();

  if (GainRootPrivileges() >= 0)
    Privilege = suidroot;
  else if (tty_grgid != (gid_t)-1 && GainGroupPrivileges(tty_grgid) >= 0)
    Privilege = sgidtty;
  else
    Privilege = none;

  DropPrivileges();

  return Privilege;
}

void GainPrivileges(void) {
  if (Privilege == suidroot)
    GainRootPrivileges();
  else if (Privilege == sgidtty)
    GainGroupPrivileges(get_tty_grgid());
}

static void SetEnvs(struct passwd *p) {
  char buf[TW_BIGBUFF];

  chdir(HOME = p->pw_dir);
#if defined(TW_HAVE_SETENV)
  setenv("HOME", HOME, 1);
  setenv("SHELL", p->pw_shell, 1);
  setenv("LOGNAME", p->pw_name, 1);
  sprintf(buf, "/var/mail/%.*s", (int)(TW_BIGBUFF - 11), p->pw_name);
  setenv("MAIL", buf, 1);
#elif defined(TW_HAVE_PUTENV)
  sprintf(buf, "HOME=%.*s", (int)(TW_BIGBUFF - 6), HOME);
  putenv(buf);
  sprintf(buf, "SHELL=%.*s", (int)(TW_BIGBUFF - 7), p->pw_shell);
  putenv(buf);
  sprintf(buf, "LOGNAME=%.*s", (int)(TW_BIGBUFF - 9), p->pw_name);
  putenv(buf);
  sprintf(buf, "MAIL=/var/mail/%.*s", (int)(TW_BIGBUFF - 16) p->pw_name);
  putenv(buf);
#endif
}

byte SetServerUid(uldat uid, byte privileges) {
  msgport WM_MsgPort;
  struct passwd *p;
  byte ok = tfalse;

  if (flag_secure && uid == (uldat)(uid_t)uid && Uid == 0 && EUid == 0) {
    if ((WM_MsgPort = Ext(WM, MsgPort))) {
      if ((p = getpwuid(uid)) && p->pw_uid == uid && chown(fullTWD, p->pw_uid, p->pw_gid) >= 0
#ifdef TW_HAVE_INITGROUPS
          && init_groups(p->pw_name, p->pw_gid) >= 0
#endif
      ) {

        switch (privileges) {
        case none:
          ok = setgid(p->pw_gid) >= 0 && setuid(uid) >= 0;
          break;
        case sgidtty:
          ok = setregid(p->pw_gid, tty_grgid) >= 0 && setuid(uid) >= 0;
          break;
        case suidroot:
          ok = setgid(p->pw_gid) >= 0 && setreuid(uid, 0) >= 0;
          break;
        default:
          break;
        }
        if (ok && (uid == 0 || CheckPrivileges() == privileges)) {
          flag_secure = 0;
          SetEnvs(p);
          if ((ok = Ext(Socket, InitAuth)())) {
            /*
             * it's time to execute .twenvrc.sh and read its output to set
             * environment variables (mostly useful for twdm)
             */
            RunTwEnvRC();

            /* tell the WM to restart itself (so that it reads user's .twinrc) */
            SendControlMsg(WM_MsgPort, MSG_CONTROL_OPEN, 0, NULL);
            return ttrue;
          }
        } else
          ok = tfalse;

        if (!ok) {
          flag_secure = 1;
          if (setuid(0) < 0 || setgid(0) < 0 || chown(fullTWD, 0, 0) < 0) {
            /* tried to recover, but screwed up uids too badly. */
            printk("twin: failed switching to uid %u: " SS "\n", uid, strerror(errno));
            printk("twin: also failed to recover. Quitting NOW!\n");
            Quit(0);
          }
          SetEnvs(getpwuid(0));
        }
      }
      printk("twin: failed switching to uid %u: " SS "\n", uid, strerror(errno));
    }
  } else
    printk("twin: SetServerUid() can be called only if started by root with \"-secure\".\n");
  return tfalse;
}

/*
 * search for a file relative to HOME, to PKG_LIBDIR or as path
 *
 * this for example will search "foo"
 * as "${HOME}/foo", "${PKG_LIBDIR}/system.foo" or plain "foo"
 */
char *FindFile(const char *name, uldat *fsize) {
  const char *prefix[3], *infix[3];
  char *path;
  const char *dir;
  int i, min_i, max_i, len, nlen = strlen(name);
  struct stat buf;

  prefix[0] = HOME;
  infix[0] = (HOME && *HOME) ? "/" : "";
  prefix[1] = pkg_libdir;
  infix[1] = "/system";
  prefix[2] = "";
  infix[2] = "";

  if (flag_secure)
    min_i = max_i = 1; /* only pkg_libdir */
  else
    min_i = 0, max_i = 2;

  for (i = min_i; i <= max_i; i++) {
    if (!(dir = prefix[i]))
      continue;
    len = strlen(dir) + strlen(infix[i]);
    if ((path = (char *)AllocMem(len + nlen + 2))) {
      sprintf(path, "%s%s%s", dir, infix[i], name);
      if (stat(path, &buf) == 0) {
        if (fsize)
          *fsize = buf.st_size;
        return path;
      }
      FreeMem(path);
    }
  }
  return NULL;
}

/*
 * read data from infd and set environment variables accordingly
 */
static void ReadTwEnvRC(int infd) {
  char buff[TW_BIGBUFF], *p = buff, *end, *q, *eq;
  int got, left = TW_BIGBUFF;
  for (;;) {
    do {
      got = read(infd, p, left);
    } while (got == -1 && errno == EINTR);

    if (got <= 0)
      break;

    end = p + got;
    p = buff;

    while ((eq = (char *)memchr(p, '=', end - p)) && (q = (char *)memchr(eq, '\n', end - eq))) {

      *q++ = '\0';
#if defined(TW_HAVE_SETENV)
      *eq++ = '\0';
      setenv(p, eq, 1);
#elif defined(TW_HAVE_PUTENV)
      putenv(p);
#endif
      p = q;
    }
    left = end - p;
    if (left == TW_BIGBUFF)
      /* line too long! */
      left = 0;

    memmove(buff, p, left);
    p = buff + left;
    left = TW_BIGBUFF - left;
  }
}

/*
 * execute .twenvrc.sh <dummy> and read its output to set
 * environment variables (mostly useful for twdm)
 */
void RunTwEnvRC(void) {
  char *path;
  uldat len;
  int fds[2];

  if (flag_envrc != 1)
    return;

  if (flag_secure == 0) {
    flag_envrc = 0;

    if ((path = FindFile(".twenvrc.sh", &len))) {
      if ((pipe(fds) >= 0)) {
        switch (fork()) {
        case -1: /* error */
          close(fds[0]);
          close(fds[1]);
          printk("twin: RunTwEnvRC(): fork() failed: " SS "\n", strerror(errno));
          break;
        case 0: /* child */
          close(fds[0]);
          if (fds[1] != 2) {
            close(2);
            dup2(fds[1], 2);
            close(fds[1]);
          }
          close(1);
          dup2(0, 1);
          execl(path, path, "dummy", NULL);
          exit(0);
          break;
        default: /* parent */
          close(fds[1]);
          ReadTwEnvRC(fds[0]);
          close(fds[0]);
          break;
        }
      } else
        printk("twin: RunTwEnvRC(): pipe() failed: " SS "\n", strerror(errno));
    } else
      printk("twin: RunTwEnvRC(): .twenvrc.sh: File not found\n", strerror(errno));
  } else
    printk("twin: RunTwEnvRC(): delaying .twenvrc.sh execution until secure mode ends.\n");
}

/* remove const from a pointer and suppress compiler warnings */
void *RemoveConst(const void *x) {
  union {
    const void *cv;
    void *v;
  } u = {x};
  return u.v;
}
