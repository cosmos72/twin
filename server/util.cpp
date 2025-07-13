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

#include <stdio.h> // rename()

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
#include "draw.h"
#include "extreg.h"
#include "log.h"
#include "methods.h"
#include "main.h"
#include "hw.h"
#include "privilege.h"
#include "remote.h"
#include "resize.h"
#include "stl/string.h"
#include "stl/vector.h"
#include "util.h"

#include <Tw/Twkeys.h>
#include <Tutf/Tutf.h>

String TmpDir;
String SocketDir;

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

static dat CmpCallTime(Tmsgport m1, Tmsgport m2) {
  if ((!m1->Msgs.First) != (!m2->Msgs.First)) {
    /* one of the two doesn't have msgs */
    return m1->Msgs.First ? (dat)-1 : (dat)1;
  }
  if ((!m1->WakeUp) != (!m2->WakeUp)) {
    /* one doesn't need to be called */
    return m1->WakeUp ? (dat)-1 : (dat)1;
  }
  if ((!m1->WakeUp) && (!m2->WakeUp)) {
    return 0;
  }
  return CmpTime(&m1->CallTime, &m2->CallTime);
}

byte Minimum(byte max_index, const ldat *array) {
  byte i, min_i;
  ldat temp;

  if (!max_index)
    return 0xFF;

  temp = array[0];
  min_i = 0;

  for (i = 1; i < max_index; i++) {
    if (array[i] < temp) {
      temp = array[i];
      min_i = i;
    }
  }
  return min_i;
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
 * move a Tmsgport to the right place in an already sorted list,
 * ordering by CallTime
 */
void SortMsgPortByCallTime(Tmsgport Port) {
  Tmsgport other;
  if ((other = Port->Next) && CmpCallTime(Port, other) > 0) {
    Port->Remove();
    do {
      other = other->Next;
    } while (other && CmpCallTime(Port, other) > 0);
    if (other) {
      InsertMiddle(MsgPorts, Port, All, other->Prev, other);
    } else {
      InsertLast(MsgPorts, Port, All);
    }
  } else if ((other = Port->Prev) && CmpCallTime(Port, other) < 0) {
    Port->Remove();
    do {
      other = other->Prev;
    } while (other && CmpCallTime(Port, other) < 0);
    if (other) {
      InsertMiddle(MsgPorts, Port, All, other, other->Next);
    } else {
      InsertFirst(MsgPorts, Port, All);
    }
  }
}

/*
 * sort the Tmsgport list by growing CallTime
 *
 * we use a bubble sort... no need to optimize to death this
 */
void SortAllMsgPortsByCallTime(void) {
  Tmsgport Max, This, Port = All->MsgPorts.First;
  Tmsgport Start, End;

  Start = End = (Tmsgport)0;

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
  All->MsgPorts.First = Start;
  All->MsgPorts.Last = End;
}

byte SendControlMsg(Tmsgport MsgPort, udat Code, udat Len, const char *Data) {
  Tmsg msg;
  event_control *event;

  if (MsgPort && (msg = Smsg::Create(msg_control, Len))) {
    event = &msg->Event.EventControl;
    event->Code = Code;
    event->Len = Len;
    CopyMem(Data, event->Data, Len);
    SendMsg(MsgPort, msg);

    return ttrue;
  }
  return tfalse;
}

bool SelectionStore(e_id magic, Chars mime, Chars data) {
  Tselection Sel = All->Selection;
  uldat len = Min2(mime.size(), TW_MAX_MIMELEN);
  if (mime) {
    CopyMem(mime.data(), Sel->MIME, len);
  }
  if (len < TW_MAX_MIMELEN) {
    memset(Sel->MIME + len, '\0', TW_MAX_MIMELEN - len);
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

bool SetSelectionFromWindow(Twindow w) {
  ldat y, slen, i;
  bool ok = true, w_useC = W_USE(w, USECONTENTS);

  if (!(w->State & WINDOW_DO_SEL)) {
    return ok;
  }

  if (!(w->State & WINDOW_ANYSEL) || w->YstSel > w->YendSel ||
      (w->YstSel == w->YendSel && w->XstSel > w->XendSel)) {

    ok = SelectionStore(SEL_UTF8MAGIC, Chars(), Chars());
    if (ok) {
      NeedHW |= NeedSelectionExport;
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

    slen = Max2(w->WLogic, 0);
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
      ok = SelectionStore(SEL_UTF8MAGIC, Chars(), Chars());
      for (i = slen; ok && i-- > 0; cells++) {
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
        for (i = slen; ok && i-- > 0; cells++) {
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
        for (i = slen = w->XendSel + 1; ok && i-- > 0; cells++) {
          ok = appender.rune(TRUNE(*cells));
        }
      }
      if (ok) {
        ok = appender.flush();
        if (ok) {
          NeedHW |= NeedSelectionExport;
        }
      }
    } while (false);

    return ok;

  } else if (!W_USE(w, USEROWS)) {
    return false;
  }

  ok = SelectionStore(SEL_UTF8MAGIC, Chars(), Chars());
  Trow r;

  /* Gap not supported! */
  y = w->YstSel;
  r = w->FindRow(y);
  if (r && r->Text) {
    uldat xstSel = Min2u(r->Len, (uldat)w->XstSel);
    if (y < w->YendSel) {
      slen = r->Len - xstSel;
    } else {
      slen = Min2u(r->Len, (uldat)w->XendSel + 1) - xstSel;
    }

    if (slen > 0) {
      const trune *runes = r->Text + Min2(r->Len, xstSel);
      ok = ok && SelectionAppendRunes(TRunes(runes, slen));
    }
  }

  if (y < w->YendSel || !r || !r->Text || (ldat)r->Len <= w->XendSel) {
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
      slen = Min2((ldat)r->Len, w->XendSel + 1);
      if (slen > 0) {
        ok = ok && SelectionAppendRunes(TRunes(r->Text, slen));
      }
    }
    if (!r || !r->Text || (ldat)r->Len <= w->XendSel) {
      ok = ok && _SelAppendNL();
    }
  }
  if (ok) {
    NeedHW |= NeedSelectionExport;
  }
  return ok;
}

/* twterm-style mouse reporting */
static byte CreateXTermMouseEventTwterm(event_mouse *event, byte buflen, char *buf,
                                        const uldat flags) {
  udat code = event->Code;

  /* if TTY_REPORTMOUSE_MOVE is set, also report motion */
  if (buflen < 9 || (code == MOVE_MOUSE && !(flags & TTY_REPORTMOUSE_MOVE))) {
    /* buffer too small, or nothing to report */
    return 0;
  } else if (isPRESS(code)) {
    /* report also button just pressed as down */
    code |= HOLD_CODE(PRESS_N(code));
  }
  CopyMem("\033[5M", buf, 4);

  const dat x = event->X, y = event->Y;

  buf[4] = ' ' + ((code & HOLD_ANY) >> HOLD_BITSHIFT);
  buf[5] = '!' + (x & 0x7f);
  buf[6] = '!' + ((x >> 7) & 0x7f);
  buf[7] = '!' + (y & 0x7f);
  buf[8] = '!' + ((y >> 7) & 0x7f);
  return 9;
}

/* xterm-style mouse reporting */
static byte CreateXTermMouseEventXterm(event_mouse *event, byte buflen, char *buf,
                                       const uldat flags) {

  if (buflen < 6) {
    /* buffer too small! */
    return 0;
  }
  CopyMem("\033[M", buf, 4);
  const udat code = event->Code;
  byte ch = 0;

  if (isPRESS(code)) {
    const udat press = code & PRESS_ANY;
    if (press == PRESS_LEFT) {
      ch = ' ';
    } else if (press == PRESS_MIDDLE) {
      ch = '!';
    } else if (press == PRESS_RIGHT) {
      ch = '"';
    } else {
      /* WHEEL_REV and WHEEL_FWD are reported only at release */
      return 0;
    }
  } else if (isRELEASE(code)) {
    const udat release = code & RELEASE_ANY;
    if (release == RELEASE_WHEEL_REV) {
      ch = '`';
    } else if (release == RELEASE_WHEEL_FWD) {
      ch = 'a';
    } else {
      ch = '#';
    }
  } else if (code == MOVE_MOUSE) {
    if (!(flags & TTY_REPORTMOUSE_MOVE)) {
      return 0;
    }
    /* also report mouse move */
    ch = '@';
  } else {
    if (!(flags & TTY_REPORTMOUSE_DRAG)) {
      return 0;
    }
    /* also report mouse drag */
    const udat hold = code & HOLD_ANY;
    if (hold == HOLD_LEFT) {
      ch = '@';
    } else if (hold == HOLD_MIDDLE) {
      ch = 'A';
    } else if (hold == HOLD_RIGHT) {
      ch = 'B';
    } else {
      ch = '@';
    }
  }
  buf[3] = ch;
  const dat x = event->X, y = event->Y;
  buf[4] = '!' + Clamp(0, x, 222);
  buf[5] = '!' + Clamp(0, y, 222);
  return 6;
}

static char *PrintUdat100k(char *dst, udat n) {
  if (n < 10) {
    dst[0] = n + '0';
    return dst + 1;
  }
  if (n < 100) {
    dst[0] = (n / 10) + '0';
    dst[1] = (n % 10) + '0';
    return dst + 2;
  }
  byte len;
  if (n < 1000) {
    len = 3;
  } else if (n < 10000) {
    len = 4;
  } else {
    len = 5;
  }
  char *ret = dst + len;
  while (len--) {
    dst[len] = (n % 10) + '0';
    n /= 10;
  }
  return ret;
}

/* xterm-style mouse reporting, decimal coordinates */
static byte CreateXTermMouseEventXtermDecimal(event_mouse *event, byte buflen, char *buf,
                                              const uldat flags) {
  if (buflen < 18) {
    /* buffer too small! */
    return 0;
  }
  CopyMem("\033[<", buf, 4);

  char *p = buf + 3;
  const udat code = event->Code;

  if (isPRESS(code)) {
    const udat press = code & PRESS_ANY;
    if (press == PRESS_LEFT) {
      *p++ = '0';
    } else if (press == PRESS_MIDDLE) {
      *p++ = '1';
    } else if (press == PRESS_RIGHT) {
      *p++ = '2';
    } else if (press == PRESS_WHEEL_REV) {
      *p++ = '6';
      *p++ = '4';
    } else if (press == PRESS_WHEEL_FWD) {
      *p++ = '6';
      *p++ = '5';
    } else {
      return 0;
    }
  } else if (isRELEASE(code)) {
    const udat release = code & RELEASE_ANY;
    if (release == RELEASE_LEFT) {
      *p++ = '0';
    } else if (release == RELEASE_MIDDLE) {
      *p++ = '1';
    } else if (release == RELEASE_RIGHT) {
      *p++ = '2';
    } else {
      // RELEASE_WHEEL_REV and RELEASE_WHEEL_FWD are not reported
      return 0;
    }
  } else if (code == MOVE_MOUSE) {
    if (!(flags & TTY_REPORTMOUSE_MOVE)) {
      return 0;
    }
    /* also report mouse move */
    *p++ = '3';
    *p++ = '5';
  } else {
    if (!(flags & TTY_REPORTMOUSE_DRAG)) {
      return 0;
    }
    *p++ = '3';
    /* also report mouse dragging */
    const udat hold = code & HOLD_ANY;
    if (hold == HOLD_LEFT) {
      *p++ = '2';
    } else if (hold == HOLD_MIDDLE) {
      *p++ = '3';
    } else if (hold == HOLD_RIGHT) {
      *p++ = '4';
    } else {
      *p++ = '2';
    }
  }
  *p++ = ';';
  const udat x = 1 + Clamp(0, event->X, 99998);
  p = PrintUdat100k(p, x);
  *p++ = ';';
  const udat y = 1 + Clamp(0, event->Y, 99998);
  p = PrintUdat100k(p, y);
  *p++ = isRELEASE(code) ? 'm' : 'M';
  return p - buf;
}

/* X10-style mouse reporting */
static byte CreateXTermMouseEventX10(event_mouse *event, byte buflen, char *buf,
                                     const uldat flags) {

  const udat code = event->Code;

  if (buflen < 6 || !isPRESS(code)) {
    return 0;
  }
  CopyMem("\033[M", buf, 4);
  char ch;
  switch (code & PRESS_ANY) {
  case PRESS_LEFT:
    ch = ' ';
    break;
  case PRESS_MIDDLE:
    ch = '!';
    break;
  case PRESS_RIGHT:
    ch = '!';
    break;
  default:
    /* WHEEL_REV and WHEEL_FWD are not reported */
    return 0;
  }
  buf[3] = ch;
  const dat x = event->X, y = event->Y;
  buf[4] = '!' + Clamp(0, x, 222);
  buf[5] = '!' + Clamp(0, y, 222);
  return 6;
}

byte CreateXTermMouseEvent(event_mouse *event, byte buflen, char *buf) {
  Twindow w = (Twindow)event->W;

  if (!w || !IS_WINDOW(w) || !W_USE(w, USECONTENTS) || !w->USE.C.TtyData) {
    return 0;
  }
  const uldat flags = w->USE.C.TtyData->Flags;

  switch (flags & TTY_REPORTMOUSE_STYLE) {
  case TTY_REPORTMOUSE_TWTERM:
    return CreateXTermMouseEventTwterm(event, buflen, buf, flags);
  case TTY_REPORTMOUSE_XTERM:
    if (flags & TTY_REPORTMOUSE_DECIMAL) {
      return CreateXTermMouseEventXtermDecimal(event, buflen, buf, flags);
    } else {
      return CreateXTermMouseEventXterm(event, buflen, buf, flags);
    }
  case TTY_REPORTMOUSE_X10:
    return CreateXTermMouseEventX10(event, buflen, buf, flags);
  default:
    return 0;
  }
}

void closeAllFds(int tty_fd_to_dup) {
  // do not assume all other file descriptors are set to fcntl(fd, F_SETFD, FD_CLOEXEC)
  int i;
  if (tty_fd_to_dup >= 0) {
    for (i = 0; i <= 2; i++) {
      if (i != tty_fd_to_dup) {
        close(i);
        dup2(tty_fd_to_dup, i);
      }
    }
    if (tty_fd_to_dup > 2) {
      close(tty_fd_to_dup);
    }
  } else {
    i = 0;
  }
  int last_successful_close = i - 1;
  for (; i < INT_MAX && i - 10 < last_successful_close; i++) {
    if (close(i) >= 0) {
      last_successful_close = i;
    }
  }
}

void ResetBorderPattern(void) {
  Tmsgport MsgP;
  Twidget w;

  for (MsgP = All->MsgPorts.First; MsgP; MsgP = MsgP->Next) {
    for (w = MsgP->Widgets.First; w; w = w->O_Next) {
      if (IS_WINDOW(w))
        ((Twindow)w)->BorderPattern[0] = ((Twindow)w)->BorderPattern[1] = NULL;
    }
  }
}

static Tgadget _PrevGadget(Tgadget G) {
  while (G->Prev) {
    G = (Tgadget)G->Prev;
    if (IS_GADGET(G))
      return (Tgadget)G;
  }
  return (Tgadget)G->Prev;
}

static Tgadget _NextGadget(Tgadget G) {
  while (G->Next) {
    G = (Tgadget)G->Next;
    if (IS_GADGET(G))
      return (Tgadget)G;
  }
  return (Tgadget)G->Next;
}

/* handle common keyboard actions like cursor moving and button navigation */
void FallBackKeyAction(Twindow w, event_keyboard *EventK) {
  ldat NumRow, OldNumRow;
  Tgadget G, H;

  if ((G = (Tgadget)w->SelectW) && IS_GADGET(G))
    switch (EventK->Code) {
    case TW_Escape:
      UnPressGadget(G, tfalse);
      w->SelectW = (Twidget)0;
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
        w->SelectW = (Twidget)H;
        PressGadget(H);
      }
      break;
    case TW_Down:
    case TW_Right:
    case TW_Tab:
      if ((H = _NextGadget(G))) {
        if (!(G->Flags & GADGETFL_TOGGLE))
          UnPressGadget(G, tfalse);
        w->SelectW = (Twidget)H;
        PressGadget(H);
      }
      break;
    default:
      break;
    }
  else if ((G = (Tgadget)w->Widgets.First) && IS_GADGET(G)) {
    PressGadget(G);
    w->SelectW = (Twidget)G;
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
          DrawLogicWidget((Twidget)w, (ldat)0, OldNumRow, (ldat)TW_MAXDAT - (ldat)2, OldNumRow);
      } else
        w->CurY = NumRow = w->HLogic - (ldat)1;
      if (w->Flags & WINDOWFL_ROWS_SELCURRENT)
        DrawLogicWidget((Twidget)w, (ldat)0, NumRow, (ldat)TW_MAXDAT - (ldat)2, NumRow);
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
          DrawLogicWidget((Twidget)w, (ldat)0, OldNumRow, (ldat)TW_MAXDAT - (ldat)2, OldNumRow);
      } else
        w->CurY = NumRow = (ldat)0;
      if (w->Flags & WINDOWFL_ROWS_SELCURRENT)
        DrawLogicWidget((Twidget)w, (ldat)0, NumRow, (ldat)TW_MAXDAT - (ldat)2, NumRow);
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
  struct sockaddr_un un_addr = {};
  socklen_t len = sizeof(un_addr);

  if ((fd = accept(fd, (struct sockaddr *)&un_addr, &len)) >= 0) {
    close(fd);
  }
}

static char envTWD[] = "TWDISPLAY=\0\0\0\0\0";

static bool moveOldTwinrcFile(void);

static bool initHOME(void) {
  const char *home = getenv("HOME");
  if (!home) {
    log(ERROR) << "twin: required environment variable $HOME is not set. Aborting.\n";
    return false;
  }
  if (!HOME.format(Chars::from_c(home))) { // also append final '\0' but do not count it
    log(ERROR) << "twin: out of memory! Aborting.\n";
    return false;
  }
  if (!moveOldTwinrcFile()) {
    return false;
  }
  return true;
}

// create directory and all its parent directories.
// temporarily modifies path, restores it before returning
static void makeDirectories(char *path) {
  if (!path || !path[0]) {
    return;
  }
  char *slash = path;
  char *end = path + strlen(path);
  while ((slash = (char *)memchr(slash + 1, '/', end - (slash + 1))) != NULL) {
    slash[0] = '\0';
    (void)mkdir(path, 0700);
    slash[0] = '/';
  }
  (void)mkdir(path, 0700);
}

bool moveOldTwinrcFile(void) {
  String oldPath;
  if (!oldPath.format(HOME, "/.twinrc")) {
    log(ERROR) << "twin: out of memory! Aborting.\n";
    return false;
  }
  struct stat buf;
  if (stat(oldPath.data(), &buf) != 0) {
    return true;
  }
  String newPath;
  if (!newPath.format(HOME, "/.config/twin/twinrc")) {
    log(ERROR) << "twin: out of memory! Aborting.\n";
    return false;
  }
  newPath[newPath.size() - 7] = '\0';
  makeDirectories(newPath.data());
  newPath[newPath.size() - 7] = '/';
  if (rename(oldPath.data(), newPath.data()) != 0) {
    log(ERROR)
        << "twin: failed to move configuration file from old (and no longer supported) location "
        << oldPath << "\n      to the new location " << newPath << ": "
        << Chars::from_c(strerror(errno));
    return false;
  }
  log(WARNING) << "twin: moved configuration file from old (and no longer supported) location "
               << oldPath << "\n      to the new location " << newPath;
  return true;
}

/**
 * initialize TmpDir to the absolutep path of temporary directory:
 * if getenv("TMPDIR") is not null, use it;
 * otherwise use "/tmp"
 * on out-of-memory, return false
 */
static bool initTmpDir(void) {
  const char *dir = getenv("TMPDIR");
  if (dir == NULL) {
    dir = "/tmp";
  }
  if (TmpDir.assign(dir, 1 + strlen(dir))) { // also append final '\0'
    TmpDir.pop_back();                       // but do not count it
    return true;
  }
  log(ERROR) << "twin: out of memory! Aborting.\n";
  return false;
}

/**
 * initialize SOCKET_DIR to the directory where to create unix domain socket to listen on:
 * if getenv("XDG_STATE_HOME") is not null, use it + "/twin"
 * otherwise use HOME + "/.local/state/twin"
 * on out-of-memory, return false
 */
/** return getenv("XDG_STATE_HOME") + "/twin", getenv("HOME") + "/.local/state/twin"  */
static bool initSocketDir(void) {
  const char *env = getenv("XDG_STATE_HOME");
  if (env != NULL && SocketDir.format(Chars::from_c(env), Chars("/twin"))) {
    return true;
  } else if (SocketDir.format(HOME, Chars("/.local/state/twin"))) {
    return true;
  } else {
    SocketDir.clear();
    log(ERROR) << "twin: out of memory! Aborting.\n";
    return false;
  }
}

bool InitGlobalVariables(void) {
  return initHOME() && initTmpDir() && initSocketDir();
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

/*
 * create unix domain socket $HOME/.local/state/twin/socket:HHH
 * create symbolic link /tmp/.Twin:HHH pointing to the former
 * set TWDISPLAY to :HHH
 */
bool InitTWDisplay(void) {
  char *arg0;
  int fd = NOFD;
  unsigned short i;
  udat len;
  byte ok;

  memset(&addr_unix, 0, sizeof(addr_unix));

  if ((unixFd = socket(AF_UNIX, SOCK_STREAM, 0)) >= 0) {

    addr_unix.sun_family = AF_UNIX;

    for (i = 0; i < 0x1000; i++) {
      snprintf(twd, sizeof(twd), ":%hx", i);

      len = CopyToSockaddrUn(TmpDir.data(), &addr_unix, 0);
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
             * also, we must close `fd' since SOCK_STREAM sockets
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
             * with older ones, but having two different server versions
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
            size_t arg0_len = strlen(TWDisplay) + 6;
            if ((arg0 = (char *)AllocMem(arg0_len))) {
              snprintf(arg0, arg0_len, "twin %s", TWDisplay);
              SetArgv0(main_argv, main_argv_usable_len, arg0);
              FreeMem(arg0);
            }
            return true;
          }
        } else
          Error(SYSERROR);
        close(unixFd);
      }
    }
  }
  if (fd != NOFD)
    close(fd);

  CopyToSockaddrUn(TmpDir.data(), &addr_unix, 0);
  arg0 = addr_unix.sun_path;

  log(ERROR) << "twin: failed to create any " << Chars::from_c(addr_unix.sun_path)
             << "/.Twin* socket: " << Errstr << "\n      possible reasons: either "
             << Chars::from_c(arg0)
             << " is not writable, or all TWDISPLAY are already in use,\n"
                "      or too many stale "
             << Chars::from_c(arg0) << "/.Twin* sockets. Aborting.\n";
  return false;
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
  tty_grgid = getTtyGrgid();

  if (GainRootPrivileges() >= 0)
    Privilege = suidroot;
  else if (tty_grgid != (gid_t)-1 && GainGroupPrivileges(tty_grgid) >= 0)
    Privilege = sgidtty;
  else
    Privilege = none;

  dropPrivileges();

  return Privilege;
}

void gainPrivileges(void) {
  if (Privilege == suidroot)
    GainRootPrivileges();
  else if (Privilege == sgidtty)
    GainGroupPrivileges(getTtyGrgid());
}

static bool SetEnvs(struct passwd *p) {
  char buf[TW_BIGBUFF];

  chdir(p->pw_dir);
  if (!HOME.assign(p->pw_dir, 1 + strlen(p->pw_dir))) { // also append final '\0'
    return false;
  }
  // keep final '\0' but do not count it
  HOME.pop_back();

#if defined(TW_HAVE_SETENV)
  setenv("HOME", HOME.data(), 1);
  setenv("SHELL", p->pw_shell, 1);
  setenv("LOGNAME", p->pw_name, 1);
  snprintf(buf, TW_BIGBUFF, "/var/mail/%.*s", (int)(TW_BIGBUFF - 11), p->pw_name);
  setenv("MAIL", buf, 1);
#elif defined(TW_HAVE_PUTENV)
  snprintf(buf, TW_BIGBUFF, "HOME=%.*s", (int)(TW_BIGBUFF - 6), HOME.data());
  putenv(buf);
  snprintf(buf, TW_BIGBUFF, "SHELL=%.*s", (int)(TW_BIGBUFF - 7), p->pw_shell);
  putenv(buf);
  snprintf(buf, TW_BIGBUFF, "LOGNAME=%.*s", (int)(TW_BIGBUFF - 9), p->pw_name);
  putenv(buf);
  snprintf(buf, TW_BIGBUFF, "MAIL=/var/mail/%.*s", (int)(TW_BIGBUFF - 16), p->pw_name);
  putenv(buf);
#endif
  return true;
}

byte SetServerUid(uldat uid, byte privileges) {
  Tmsgport WM_MsgPort;
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
          ok = SetEnvs(p);
          if (ok && (ok = Ext(Socket, InitAuth)())) {
            /*
             * it's time to execute ~/.config/twin/twenvrc.sh and read its output to set
             * environment variables (mostly useful for twdm)
             */
            RunTwEnvRC();

            /* tell the WM to restart itself (so that it reads user's ~/.config/twin/twinrc) */
            SendControlMsg(WM_MsgPort, MSG_CONTROL_OPEN, 0, NULL);
            return ttrue;
          }
        } else {
          ok = tfalse;
        }

        if (!ok) {
          flag_secure = 1;
          if (setuid(0) < 0 || setgid(0) < 0 || chown(fullTWD, 0, 0) < 0 || !SetEnvs(getpwuid(0))) {
            /* tried to recover, but screwed up uids too badly. */
            log(ERROR) << "twin: failed switching to uid " << uid << ": "
                       << Chars::from_c(strerror(errno))
                       << "\ntwin: also failed to recover. Quitting NOW!\n";
            Quit(0);
          }
        }
      }
      log(ERROR) << "twin: failed switching to uid " << uid << ": "
                 << Chars::from_c(strerror(errno)) << "\n";
    }
  } else
    log(ERROR) << "twin: SetServerUid() can be called only if started by root with \"-secure\".\n";
  return tfalse;
}

char *FindConfigFile(const char *name, uldat *file_size) {
  const Chars prefix[4] = {
      Chars::from_c(getenv("XDG_CONFIG_HOME")),
      HOME,
      confdir,
      Chars("."),
  };
  const Chars infix[4] = {
      Chars("/twin/"),
      Chars("/.config/twin/"),
      Chars("/"),
      Chars("/"),
  };
  unsigned min_i, max_i;
  if (flag_secure)
    min_i = max_i = 2; /* only confdir */
  else
    min_i = 0, max_i = 3;

  String path;
  Chars cname = Chars::from_c(name);
  struct stat buf;

  for (unsigned i = min_i; i <= max_i; i++) {
    if (prefix[i] && path.format(prefix[i], infix[i], cname)) {
      if (stat(path.data(), &buf) == 0) {
        if (file_size) {
          *file_size = buf.st_size;
        }
        return path.release();
      }
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

    std::memmove(buff, p, left);
    p = buff + left;
    left = TW_BIGBUFF - left;
  }
}

/*
 * execute ~/.config/twin/twenvrc.sh and read its output to set
 * environment variables (mostly useful for twdm)
 */
void RunTwEnvRC(void) {
  char *path = NULL;
  int fds[2];

  if (flag_envrc != 1)
    return;

  if (flag_secure == 0) {
    flag_envrc = 0;

    if ((path = FindConfigFile("twenvrc.sh", NULL))) {
      if ((pipe(fds) >= 0)) {
        switch (fork()) {
        case -1: /* error */
          close(fds[0]);
          close(fds[1]);
          log(ERROR) << "twin: RunTwEnvRC(): fork() failed: " << Chars::from_c(strerror(errno))
                     << "\n";
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
          execlp("bash", "bash", "-i", "-l", path, NULL);
          // if bash is not found, try plain sh
          execlp("sh", "sh", path, NULL);
          exit(1);
          break;
        default: /* parent */
          close(fds[1]);
          ReadTwEnvRC(fds[0]);
          close(fds[0]);
          break;
        }
      } else
        log(ERROR) << "twin: RunTwEnvRC(): pipe() failed: " << Chars::from_c(strerror(errno))
                   << "\n";
    } else
      log(ERROR) << "twin: RunTwEnvRC(): twenvrc.sh: File not found\n";
  } else
    log(ERROR) << "twin: RunTwEnvRC(): delaying twenvrc.sh execution until secure mode ends.\n";
  FreeMem(path);
}

/* remove const from a pointer and suppress compiler warnings */
void *RemoveConst(const void *x) {
  union {
    const void *cv;
    void *v;
  } u = {x};
  return u.v;
}
