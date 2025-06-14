/*
 *  tty.c --  linux terminal emulator
 *
 *  linux terminal control sequences decoder taken from:
 *    linux/drivers/char/console.c
 *    Copyright (C) 1991, 1992  Linus Torvalds
 *
 *  all the rest written by twin author:
 *    Copyright (C) 1993-2001  Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twin.h"
#include "algo.h"
#include "alloc.h"
#include "data.h"
#include "methods.h"
#include "palette.h"
#include "draw.h"
#include "resize.h"

#include "remote.h"
#include "hw.h"
#include "common.h"
#include "tty.h"

#include <Tw/Tw.h>
#include <Tw/Twstat.h>
#include <Tw/Twstat_defs.h>
#include <Tutf/Tutf.h>

/*
 * VT102 emulator
 */

/* static variables, common to most functions */

static Twindow Win;
static ttydata *Data;
static uldat *Flags;
/* enable keypad by default */
static udat kbdFlags = TTY_KBDAPPLIC | TTY_AUTOWRAP, defaultFlags = TTY_KBDAPPLIC | TTY_AUTOWRAP;

static dat dirty[2][4];
static ldat dirtyS[2];
static byte dirtyN;

#define ColText Win->ColText
#define DState Data->State
#define Effects Data->Effects
#define ScrollBack Data->ScrollBack
#define SizeX Data->SizeX
#define SizeY Data->SizeY
#define Top Data->Top
#define Bottom Data->Bottom
#define X Data->X
#define Y Data->Y
#define saveX Data->saveX
#define saveY Data->saveY
#define Base Win->USE.C.Contents
#define Start Data->Start
#define Split Data->Split
#define Pos Data->Pos
#define Color Data->Color
#define DefColor Data->DefColor
#define saveColor Data->saveColor
#define Underline Data->Underline
#define HalfInten Data->HalfInten
#define TabStop Data->TabStop
#define Par Data->Par
#define nPar Data->nPar
#define currG Data->currG
#define Charset Win->Charset
#define InvCharset Data->InvCharset
#define G Data->G
#define G0 Data->G0
#define G1 Data->G1
#define saveG Data->saveG
#define saveG0 Data->saveG0
#define saveG1 Data->saveG1
#define utf8 Data->utf8
#define utf8_count Data->utf8_count
#define utf8_char Data->utf8_char
#define newLen Data->newLen
#define newMax Data->newMax
#define newName Data->newName

/* A bitmap for codes <32. A bit of 1 indicates that the code
 * corresponding to that bit number invokes some special action
 * (such as cursor movement) and should not be displayed as a
 * glyph unless the TTY_DISPCTRL mode is explicitly enabled.
 */
#define CTRL_ACTION 0x0d00ff81
#define CTRL_ALWAYS 0x0800f501 /* Cannot be overridden by TTY_DISPCTRL */

#define CHANGE_BIT(bit, on_off)                                                                    \
  do {                                                                                             \
    if (on_off)                                                                                    \
      *Flags |= (bit);                                                                             \
    else                                                                                           \
      *Flags &= ~(bit);                                                                            \
  } while (0)

/*
 * for better cleannes, dirty_tty()
 * should be used *before* actually touching Win->Contents[]
 */
static void dirty_tty(dat x1, dat y1, dat x2, dat y2) {
  byte i;
  ldat S[2] = {0, 0};
  dat xy[2][4];

  if (dirtyN == TW_MAXBYTE || x1 > x2 || x1 >= SizeX || y1 > y2 || y1 >= SizeY)
    return;

  x2 = Min2(x2, SizeX - 1);
  y2 = Min2(y2, SizeY - 1);

  for (i = 0; i < dirtyN; i++) {
    xy[i][0] = Min2(dirty[i][0], x1);
    xy[i][1] = Min2(dirty[i][1], y1);
    xy[i][2] = Max2(dirty[i][2], x2);
    xy[i][3] = Max2(dirty[i][3], y2);
    S[i] += (xy[0][2] - xy[0][0] + 1) * (xy[0][3] - xy[0][1] + 1);
    S[!i] += dirtyS[i];
  }
  if (dirtyN < 2)
    S[dirtyN] += (x2 - x1 + 1) * (y2 - y1 + 1);

  i = dirtyN && S[0] > S[1];

  if (S[i] >= SizeX * SizeY * 3 / 4) {
    dirtyN = TW_MAXBYTE;
    return;
  } else if (i < dirtyN) {
    CopyMem(xy[i], dirty[i], 4 * sizeof(dat));
  } else {
    dirtyN++;
    dirty[i][0] = x1;
    dirty[i][1] = y1;
    dirty[i][2] = x2;
    dirty[i][3] = y2;
  }
  dirtyS[i] = (dirty[i][2] - dirty[i][0] + 1) * (dirty[i][3] - dirty[i][1] + 1);
}

static void flush_tty(void) {
  byte doupdate = tfalse;
  dat i;

  /* first, draw on Tscreen whatever changed in the window */
  if (dirtyN) {
    if (dirtyN == TW_MAXBYTE)
      DrawLogicWidget((Twidget)Win, 0, ScrollBack, SizeX - 1, SizeY - 1 + ScrollBack);
    else
      for (i = 0; i < dirtyN; i++)
        DrawLogicWidget((Twidget)Win, dirty[i][0], dirty[i][1] + ScrollBack, dirty[i][2],
                        dirty[i][3] + ScrollBack);
    dirtyN = 0;
  }

  /* then update cursor */
  if (Win->CurX != (ldat)X || Win->CurY != (ldat)Y + ScrollBack) {

    Win->CurX = (ldat)X;
    Win->CurY = (ldat)Y + ScrollBack;
    Pos = Base + Win->CurX + (Win->CurY + Win->USE.C.HSplit) * SizeX;
    if (Pos >= Split)
      Pos -= Split - Base;

    doupdate = ttrue;
  } else
    doupdate = tfalse;

  if ((doupdate || (*Flags & TTY_UPDATECURSOR)) && ContainsCursor((Twidget)Win))
    UpdateCursor();

  *Flags &= ~TTY_UPDATECURSOR;

  /* finally, keyboard focus configuration: */
  if (*Flags & TTY_NEEDREFOCUS) {
    *Flags &= ~TTY_NEEDREFOCUS;
    if (Win == (Twindow)All->FirstScreen->FocusW())
      Win->KbdFocus();
  }
}

/* Note: inverting the screen twice should revert to the original state */

static void invert_screen(void) {
  tcell a, *p = Start;
  ldat count;

  dirty_tty(0, 0, SizeX - 1, SizeY - 1);
  count = SizeX * SizeY;

  while (count--) {
    a = *p;
    *p++ = (a & ~TCELL(TW_MAXWCOL, 0)) | TCELL(TCOL(TCOLBG(TCOLOR(a)), TCOLFG(TCOLOR(a))), 0);
    if (p == Split)
      p = Base;
  }
}

static void insert_char(ldat nr) {
  tcell *p, *q = Pos;

  p = q + SizeX - X - nr;

  dirty_tty(X, Y, SizeX - 1, Y);

  while (--p >= q)
    p[nr] = *p;

  while (nr--)
    *q++ = TCELL(ColText, ' ');

  *Flags &= ~TTY_NEEDWRAP;
}

inline void delete_char(ldat nr) {
  dat i;
  tcell *p = Pos;

  i = SizeX - X - nr;

  dirty_tty(X, Y, SizeX - 1, Y);

  while (i--) {
    *p = p[nr];
    p++;
  }
  while (nr--)
    *p++ = TCELL(ColText, ' ');

  *Flags &= ~TTY_NEEDWRAP;
}

/*
 * goto_xy() must verify all boundaries, because the arguments
 * might also be negative. If the given position is out of
 * bounds, the cursor is placed at the nearest margin.
 */
static void goto_xy(ldat new_x, ldat new_y) {
  dat min_y, max_y;

  if (new_x < 0)
    X = 0;
  else if (new_x >= (ldat)SizeX)
    X = SizeX - 1;
  else
    X = (dat)new_x;

  if (*Flags & TTY_RELORIG) {
    min_y = Top;
    max_y = Bottom;
  } else {
    min_y = 0;
    max_y = SizeY;
  }

  if (new_y < (ldat)min_y)
    Y = min_y;
  else if (new_y >= (ldat)max_y)
    Y = max_y - 1;
  else
    Y = (dat)new_y;

  *Flags &= ~TTY_NEEDWRAP;

  /* never flush here, just update Pos */
  Pos = Start + X + (ldat)Y * SizeX;
  if (Pos >= Split)
    Pos -= Split - Base;
}

/* for absolute user moves, when TTY_RELORIG (decom) is set */
inline void goto_axy(ldat new_x, ldat new_y) {
  goto_xy(new_x, *Flags & TTY_RELORIG ? (Top + new_y) : new_y);
}

/* WARNING: fwd_copy() doesn't call dirty_tty(), you must call it manually! */
static void fwd_copy(tcell *s, tcell *d, ldat len) {
  ldat l;

  while (s >= Split)
    s -= Split - Base;
  while (d >= Split)
    d -= Split - Base;

  while (len > 0) {
    l = Min2(len, Split - s);
    l = Min2(l, Split - d);
    MoveMem(s, d, l * sizeof(tcell)); // s and d can overlap!
    s += l;
    d += l;
    len -= l;
    if (s == Split)
      s = Base;
    if (d == Split)
      d = Base;
  }
}

/* WARNING: rev_copy() doesn't call dirty_tty(), you must call it manually! */
static void rev_copy(tcell *s, tcell *d, ldat len) {
  ldat l;

  s += len;
  d += len;
  while (s > Split)
    s -= Split - Base;
  while (d > Split)
    d -= Split - Base;

  while (len > 0) {
    l = Min2(len, s - Base);
    l = Min2(l, d - Base);
    s -= l;
    d -= l;
    len -= l;
    MoveMem(s, d, l * sizeof(tcell));
    if (s == Base)
      s = Split;
    if (d == Base)
      d = Split;
  }
}

/* WARNING: fill() doesn't call dirty_tty(), you must call it manually! */
static void fill(tcell *s, tcell c, ldat len) {
  ldat l;

  while (s >= Split)
    s -= Split - Base;

  while (len > 0) {
    l = Min2(len, Split - s);
    len -= l;
    while (l--)
      *s++ = c;
    if (s == Split)
      s = Base;
  }
}

static void scrollup(dat t, dat b, dat nr) {
  tcell *d, *s;
  byte accel = tfalse;

  if (t + nr >= b)
    nr = b - t - 1;
  if (b > SizeY || t >= b || nr < 1)
    return;

  /* try to accelerate this */
  if ((Twidget)Win == All->FirstScreen->FirstW) {
    accel = ttrue;
    flush_tty();
  } else
    dirty_tty(0, t, SizeX - 1, b - 1);

  if (t == 0 && b == SizeY) {
    /* full screen scrolls. use splitline+scrollback */

    Win->USE.C.HSplit += nr;
    if (Win->USE.C.HSplit >= Win->HLogic)
      Win->USE.C.HSplit -= Win->HLogic;

    Start += nr * SizeX;
    if (Start >= Split)
      Start -= Split - Base;

    Pos += nr * SizeX;
    if (Pos >= Split)
      Pos -= Split - Base;

    s = Start + SizeX * (t + nr);
    d = Start + SizeX * t;
  } else {
    /* plain memcpy */
    s = Start + SizeX * (t + nr);
    d = Start + SizeX * t;
    fwd_copy(s, d, (b - t - nr) * SizeX);
  }

  /* clear the last nr lines */
  fill(d + (b - t - nr) * SizeX, TCELL(ColText, ' '), nr * SizeX);

  if (accel)
    ScrollFirstWindowArea(0, t, SizeX - 1, b - 1, 0, -nr);
}

static void scrolldown(dat t, dat b, dat nr) {
  tcell *s;
  ldat step;
  byte accel = tfalse;

  if (t + nr >= b)
    nr = b - t - 1;
  if (b > SizeY || t >= b || nr < 1)
    return;

  /* try to accelerate this */
  if ((Twidget)Win == All->FirstScreen->FirstW) {
    accel = ttrue;
    flush_tty();
  } else
    dirty_tty(0, t, SizeX - 1, b - 1);

  s = Start + SizeX * t;
  step = SizeX * nr;

  rev_copy(s, s + step, (b - t - nr) * SizeX);
  fill(s, TCELL(ColText, ' '), step);

  if (accel)
    ScrollFirstWindowArea(0, t, SizeX - 1, b - 1, 0, nr);
}

inline void lf(void) {
  /* don't scroll if above bottom of scrolling region, or
   * if below scrolling region
   */
  if (Y + 1 == Bottom) {
    scrollup(Top, Bottom, 1);
  } else if (Y < SizeY - 1) {
    Y++;
    Pos += SizeX;
    if (Pos >= Split)
      Pos -= Split - Base;
  }
  *Flags &= ~TTY_NEEDWRAP;
}

static void ri(void) {
  /* don't scroll if below top of scrolling region,
   * or if above scrolling region
   */
  if (Y == Top) {
    scrolldown(Top, Bottom, 1);
  } else if (Y > 0) {
    Y--;
    Pos -= SizeX;
    if (Pos < Base)
      Pos += Split - Base;
  }
  *Flags &= ~TTY_NEEDWRAP;
}

inline void cr(void) {
  Pos -= X;
  X = 0;
  *Flags &= ~TTY_NEEDWRAP;
}

inline void bs(void) {
  if (X) {
    X--;
    Pos--;
    *Flags &= ~TTY_NEEDWRAP;
  }
}

inline void del(void) { /* ignored */
}

static void csi_J(int vpar) {
  ldat count;
  tcell *start;

  switch (vpar) {
  case 0: /* erase from cursor to end of display */
    dirty_tty(0, Y, SizeX - 1, SizeY - 1);
    count = (SizeY - Y) * (ldat)SizeX + SizeX - X;
    start = Pos;
    break;
  case 1: /* erase from start to cursor */
    dirty_tty(0, 0, SizeX - 1, Y);
    count = Y * (ldat)SizeX + X;
    start = Start;
    break;
  case 2: /* erase whole display */
    dirty_tty(0, 0, SizeX - 1, SizeY - 1);
    count = (ldat)SizeX * SizeY;
    start = Start;
    break;
  default:
    return;
  }
  fill(start, TCELL(ColText, ' '), count);

  *Flags &= ~TTY_NEEDWRAP;
}

static void csi_K(int vpar) {
  dat count;
  tcell *start;

  switch (vpar) {
  case 0: /* erase from cursor to end of line */
    dirty_tty(X, Y, SizeX - 1, Y);
    count = SizeX - X;
    start = Pos;
    break;
  case 1: /* erase from start of line to cursor */
    dirty_tty(0, Y, X, Y);
    count = X + 1;
    start = Pos - X;
    break;
  case 2: /* erase whole line */
    dirty_tty(0, Y, SizeX - 1, Y);
    count = SizeX;
    start = Pos - X;
    break;
  default:
    return;
  }
  while (count--)
    *start++ = TCELL(ColText, ' ');

  *Flags &= ~TTY_NEEDWRAP;
}

static void csi_X(int vpar) /* erase the following vpar positions */
{                           /* not vt100? */
  tcell *start = Pos;

  if (!vpar)
    vpar++;
  vpar = Min2(vpar, SizeX - X);

  dirty_tty(X, Y, X + vpar - 1, Y);

  while (vpar--)
    *start++ = TCELL(ColText, ' ');

  *Flags &= ~TTY_NEEDWRAP;
}

static void update_eff(void) {
  udat effects = Effects;
  tcolor fg = TCOLFG(ColText), bg = TCOLBG(ColText);

  if (effects & EFF_UNDERLINE) {
    fg = TCOLFG(Underline);
  } else if (effects & EFF_HALFINTENS) {
    fg = TCOLFG(HalfInten);
  }
  if (!!(effects & EFF_REVERSE) != !!(*Flags & TTY_INVERTSCR)) {
    tcolor tmp = TCOL(bg & ~thigh, fg & ~thigh) | TCOL(fg & thigh, bg & thigh);
    fg = TCOLFG(tmp);
    bg = TCOLBG(tmp);
  }
  if (effects & EFF_INTENSITY) {
    fg ^= thigh;
  }
  if (effects & EFF_BLINK) {
    bg ^= thigh;
  }
  Color = TCOL(fg, bg);
}

#define setCharset(g)                                                                              \
  do {                                                                                             \
    switch ((currG = (g))) {                                                                       \
    case VT100GR_MAP:                                                                              \
      Charset = Tutf_VT100GR_to_UTF_32;                                                            \
      InvCharset = Tutf_UTF_32_to_VT100GR;                                                         \
      break;                                                                                       \
    case LATIN1_MAP:                                                                               \
      Charset = Tutf_ISO8859_1_to_UTF_32;                                                          \
      InvCharset = Tutf_UTF_32_to_ISO8859_1;                                                       \
      break;                                                                                       \
    case IBMPC_MAP:                                                                                \
      Charset = Tutf_CP437_to_UTF_32;                                                              \
      InvCharset = Tutf_UTF_32_to_CP437;                                                           \
      break;                                                                                       \
    case USER_MAP:                                                                                 \
      Charset = All->Gtranslations[USER_MAP];                                                      \
      InvCharset = Tutf_UTF_32_to_ISO8859_1; /* very rough :( */                                   \
      break;                                                                                       \
    }                                                                                              \
  } while (0)

inline trune applyG(trune c) {
  if (c < 0x100)
    c = Charset[c];
  return c;
}

static void csi_m(void) {
  uldat i;
  udat effects = Effects;
  tcolor fg = TCOLFG(ColText), bg = TCOLBG(ColText);

  for (i = 0; i <= nPar;) {
    uldat par = Par[i];
    switch (par) {
    case 0:
      /* all attributes off */
      fg = TCOLFG(DefColor);
      bg = TCOLBG(DefColor);
      effects = 0;
      break;
    case 1:
      effects &= ~EFF_HALFINTENS;
      effects |= EFF_INTENSITY;
      break;
    case 2:
      effects &= ~EFF_INTENSITY;
      effects |= EFF_HALFINTENS;
      break;
    case 4:
      effects |= EFF_UNDERLINE;
      break;
    case 5:
      effects |= EFF_BLINK;
      break;
    case 7:
      effects |= EFF_REVERSE;
      break;
    case 10: /* ANSI X3.64-1979 (SCO-ish?)
              * Select primary font, don't display
              * control chars if defined, don't set
              * bit 8 on output.
              */
      setCharset(G ? G1 : G0);
      *Flags &= ~(TTY_DISPCTRL | TTY_SETMETA);
      break;
    case 11: /* ANSI X3.64-1979 (SCO-ish?)
              * Select first alternate font, lets
              * chars < 32 be displayed as ROM chars.
              */
      setCharset(IBMPC_MAP);
      *Flags |= TTY_DISPCTRL;
      *Flags &= ~TTY_SETMETA;
      break;
    case 12: /* ANSI X3.64-1979 (SCO-ish?)
              * Select second alternate font, toggle
              * high bit before displaying as ROM char.
              */
      setCharset(IBMPC_MAP);
      *Flags |= TTY_DISPCTRL | TTY_SETMETA;
      break;
    case 21:
    case 22:
      effects &= ~(EFF_HALFINTENS | EFF_INTENSITY);
      break;
    case 24:
      effects &= ~EFF_UNDERLINE;
      break;
    case 25:
      effects &= ~EFF_BLINK;
      break;
    case 27:
      effects &= ~EFF_REVERSE;
      break;
    case 38:
      if (i < nPar) {
        if (Par[i + 1] == 5) {
          /* 8-bit palette foreground: ESC[38;5;<8BITPALETTE>m */
          uldat index = Par[i + 2];
          if (index <= 255) {
            fg = Palette[index];
          }
          i += 3;
          continue;
        } else if (Par[i + 1] == 2) {
          /* 24-bit truecolor foreground: ESC[38;2;<R>;<G>;<B>m */
          uldat r = Par[i + 2];
          uldat g = Par[i + 3];
          uldat b = Par[i + 4];
          if (r <= 255 && g <= 255 && b <= 255) {
            fg = TRGB(r, g, b);
          }
          i += 5;
          continue;
        }
      }
      /* ANSI X3.64-1979 (SCO-ish?)
       * Enables underscore, white foreground with white underscore
       * (Linux - use default foreground).
       */
      fg = TCOLFG(DefColor);
      effects |= EFF_UNDERLINE;
      break;
    case 39:
      /* ANSI X3.64-1979 (SCO-ish?) Disable underline.
       * Reset colour to default? It did this before...
       */
      fg = TCOLFG(DefColor);
      effects &= ~EFF_UNDERLINE;
      break;
    case 48:
      if (i < nPar) {
        if (Par[i + 1] == 5) {
          /* 8-bit palette background: ESC[48;5;<8BITPALETTE>m */
          uldat index = Par[i + 2];
          if (index <= 255) {
            bg = Palette[index];
          }
          i += 3;
          continue;
        } else if (Par[i + 1] == 2) {
          /* 24-bit truecolor background: ESC[48;2;<R>;<G>;<B>m */
          uldat r = Par[i + 2];
          uldat g = Par[i + 3];
          uldat b = Par[i + 4];
          if (r <= 255 && g <= 255 && b <= 255) {
            bg = TRGB(r, g, b);
          }
          i += 5;
          continue;
        }
      }
      break;
    case 49: /* restore default bg */
      bg = TCOLBG(DefColor);
      break;
    default:
      if (par >= 30 && par <= 37) {
        fg = Palette[par - 30 + (effects & EFF_INTENSITY ? 8 : 0)];
      } else if (par >= 40 && par <= 47) {
        bg = Palette[par - 40 + (effects & EFF_INTENSITY ? 8 : 0)];
      } else if (par >= 90 && par <= 97) {
        fg = Palette[par - 90 + 8];
      } else if (par >= 100 && par <= 107) {
        bg = Palette[par - 100 + 8];
      }
    }
    i++;
  }
  Effects = effects;
  ColText = TCOL(fg, bg);

  update_eff();
}

static void respond_string(const char *p) {
  ldat len = strlen(p);

  /* the remote program may be directly attached to the window */
  if (!RemoteWindowWriteQueue(Win, len, p)) {

    /* or we may need to send a message to Win->Owner */
    Tmsg msg;
    event_keyboard *Event;
    if ((msg = New(msg)(msg_widget_key, len))) {
      /* this is the same code as in KeyboardEvent() in hw.c */
      Event = &msg->Event.EventKeyboard;
      Event->W = (Twidget)Win;
      if (len == 1 && (p[0] == ENTER || p[0] == ESCAPE))
        Event->Code = p[0];
      else
        Event->Code = 1;
      Event->ShiftFlags = (udat)0;
      Event->SeqLen = len;
      CopyMem(p, Event->AsciiSeq, len);
      Event->AsciiSeq[len] = '\0'; /* terminate string with \0 */
      SendMsg(Win->Owner, msg);
    }
  }
}

static void cursor_report(void) {
  char buf[40];
  sprintf(buf, "\033[%d;%dR", Y + (*Flags & TTY_RELORIG ? Top + 1 : 1), X + 1);
  respond_string(buf);
}

inline void status_report(void) {
  respond_string("\033[0n"); /* Terminal ok */
}

/*
 * this is what the terminal answers to a ESC-Z or csi0c query.
 */
inline void respond_ID(void) {
  /* VT102ID (returned by linux console) is "\033[?6c" */
  /* twin <= 0.3.8  reports "\033[?6;3c" to indicate xterm-style mouse features */
  /* twin <= 0.3.10 reports "\033[?6;4c", can also report mouse motion with no buttons pressed */
  /* twin >= 0.3.11 reports "\033[?6;5c", also supports utf-8 mode */
  respond_string("\033[?6;5c");
}

static void set_mode(byte on_off) {
  uldat i;

  for (i = 0; i <= nPar; i++) {
    uldat par = Par[i];

    /* DEC private modes set/reset */

    if (DState & ESques) {
      switch (par) {

      case 1: /* Cursor keys send ^[Ox/^[[x */
        CHANGE_BIT(TTY_ALTCURSKEYS, on_off);
        *Flags |= TTY_NEEDREFOCUS;
        break;
      case 3: /* 80/132 mode switch unimplemented */
        break;
      case 5: /* Inverted screen on/off */
        CHANGE_BIT(TTY_INVERTSCR, on_off);
        update_eff();
        invert_screen();
        break;
      case 6: /* Origin relative/absolute */
        CHANGE_BIT(TTY_RELORIG, on_off);
        goto_axy(0, 0);
        break;
      case 7: /* Autowrap on/off */
        CHANGE_BIT(TTY_AUTOWRAP, on_off);
        break;
      case 8: /* Autorepeat on/off */
        break;
      case 9: /* new style */
        CHANGE_BIT(TTY_REPORTMOUSE_TWTERM, on_off);
        CHANGE_BIT(TTY_REPORTMOUSE_ALSO_MOVE, tfalse);
        Win->ChangeField(TWS_window_Attr, WINDOW_WANT_MOUSE_MOTION | WINDOW_WANT_MOUSE,
                         on_off ? WINDOW_WANT_MOUSE : 0);
        break;
      case 25: /* Cursor on/off */
        Win->ChangeField(TWS_window_Flags, WINDOWFL_CURSOR_ON, on_off ? WINDOWFL_CURSOR_ON : 0);
        *Flags |= TTY_UPDATECURSOR;
        break;
      case 999: /* new style, also report mouse move */
        CHANGE_BIT(TTY_REPORTMOUSE_TWTERM, on_off);
        CHANGE_BIT(TTY_REPORTMOUSE_ALSO_MOVE, on_off);
        Win->ChangeField(TWS_window_Attr, WINDOW_WANT_MOUSE | WINDOW_WANT_MOUSE_MOTION,
                         on_off ? WINDOW_WANT_MOUSE | WINDOW_WANT_MOUSE_MOTION : 0);
        break;
      case 1000: /* classic xterm style */
        CHANGE_BIT(TTY_REPORTMOUSE_XTERM, on_off);
        CHANGE_BIT(TTY_REPORTMOUSE_ALSO_MOVE, tfalse);
        Win->ChangeField(TWS_window_Attr, WINDOW_WANT_MOUSE | WINDOW_WANT_MOUSE_MOTION,
                         on_off ? WINDOW_WANT_MOUSE : 0);
        break;

      case 1002: /* classic xterm style, also report mouse dragging */
        CHANGE_BIT(TTY_REPORTMOUSE_XTERM, on_off);
        CHANGE_BIT(TTY_REPORTMOUSE_ALSO_MOVE, on_off);
        Win->ChangeField(TWS_window_Attr, WINDOW_WANT_MOUSE | WINDOW_WANT_MOUSE_MOTION,
                         on_off ? WINDOW_WANT_MOUSE | WINDOW_WANT_MOUSE_MOTION : 0);
        break;

        /* ANSI modes set/reset */
      }
    } else {
      switch (par) {

      case 3: /* Monitor (display ctrls) */
        CHANGE_BIT(TTY_DISPCTRL, on_off);
        break;
      case 4: /* Insert Mode on/off */
        CHANGE_BIT(TTY_INSERT, on_off);
        break;
      case 20: /* Lf, Enter == CrLf/Lf */
        CHANGE_BIT(TTY_CRLF, on_off);
        break;
      }
    }
  }
}

static void setterm_command(void) {
  switch (Par[0]) {

  case 1: /* set fg color for underline mode */
    if (Par[1] < tpalette_n) {
      Underline = TCOL(Par[1], 0);
      if (Effects & EFF_UNDERLINE)
        update_eff();
    }
    break;
  case 2: /* set color for half intensity mode */
    if (Par[1] < tpalette_n) {
      HalfInten = TCOL(Par[1], 0);
      if (Effects & EFF_HALFINTENS)
        update_eff();
    }
    break;
  case 8: /* store colors - but not effects - as defaults */
    DefColor = ColText;
    break;
  case 9: /* set blanking interval */
    break;
  case 10: /* set bell frequency in Hz */
  case 11: /* set bell duration in msec */
    ConfigureHW(Par[0] == 10 ? HW_BELLPITCH : HW_BELLDURATION, nPar == 0, Par[1]);
    break;
  case 12: /* bring specified console to the front */
    break;
  case 13: /* unblank the screen */
    break;
  case 14: /* set vesa powerdown interval */
    break;
  default:
    break;
  }
}

inline void insert_line(ldat nr) {
  scrolldown(Y, Bottom, nr);
  *Flags &= ~TTY_NEEDWRAP;
}

inline void delete_line(ldat nr) {
  scrollup(Y, Bottom, nr);
  *Flags &= ~TTY_NEEDWRAP;
}

inline void csi_at(ldat nr) {
  if (nr > (ldat)SizeX - X)
    nr = (ldat)SizeX - X;
  else if (!nr)
    nr = 1;
  insert_char(nr);
}

inline void csi_L(ldat nr) {
  if (nr > (ldat)SizeY - Y)
    nr = (ldat)SizeY - Y;
  else if (!nr)
    nr = 1;
  insert_line(nr);
}

inline void csi_P(ldat nr) {
  if (nr > (ldat)SizeX - X)
    nr = (ldat)SizeX - X;
  else if (!nr)
    nr = 1;
  delete_char(nr);
}

inline void csi_M(ldat nr) {
  if (nr > (ldat)SizeY - Y)
    nr = (ldat)SizeY - Y;
  else if (!nr)
    nr = 1;
  delete_line(nr);
}

inline void save_current(void) {
  saveX = X;
  saveY = Y;
  saveColor = ColText;
  saveG = G;
  saveG0 = G0;
  saveG1 = G1;
}

inline void restore_current(void) {
  goto_xy(saveX, saveY);
  ColText = saveColor;
  update_eff();
  *Flags &= ~TTY_NEEDWRAP;
  G = saveG;
  G0 = saveG0;
  G1 = saveG1;
  setCharset(G ? G1 : G0);
}

static void reset_tty(byte do_clear) {

  DState = ESnormal;
  if (*Flags & TTY_INVERTSCR)
    invert_screen();
  *Flags = TTY_AUTOWRAP;
  Effects = 0;

  Top = 0;
  Bottom = SizeY;

  ColText = Color = DefColor = TCOL(twhite, tblack);
  Underline = TCOL(tWHITE, tblack);
  HalfInten = TCOL(tBLACK, tblack);

  Win->Flags |= WINDOWFL_CURSOR_ON;
  Win->CursorType = LINECURSOR;

  TabStop[0] = 0x01010100;
  TabStop[1] = TabStop[2] = TabStop[3] = TabStop[4] = 0x01010101;

  nPar = 0;

  G = saveG = 0;
  /* default to latin1 charset */
  setCharset(G0 = saveG0 = LATIN1_MAP);
  G1 = saveG1 = VT100GR_MAP;

  utf8 = utf8_count = utf8_char = 0;

  /*
  bell_pitch = DEFAULT_BELL_PITCH;
  bell_duration = DEFAULT_BELL_DURATION;

  kbd_table.ledmode = LED_SHOW_FLAGS;
  kbd_table.ledflagstate = kbd_table.default_ledflagstate;
  */

  goto_xy(0, 0);
  save_current();
  if (do_clear)
    csi_J(2);
}

static byte grow_newtitle(void) {
  ldat maxlen;
  char *name;
  if (newMax < TW_MAXDAT) {
    maxlen = ((ldat)newMax + (newMax >> 1) + 3) | All->SetUp->MinAllocSize;
    if (maxlen > TW_MAXDAT)
      maxlen = TW_MAXDAT;
    if ((name = (char *)ReAllocMem(newName, maxlen))) {
      newName = name;
      newMax = maxlen;
      return ttrue;
    }
  }
  return tfalse;
}

static byte insert_newtitle(byte c) {
  if (newLen < newMax || grow_newtitle()) {
    newName[newLen++] = c;
    return ttrue;
  }
  return tfalse;
}

static void set_newtitle(void) {
  char *name;
  dat len;

  if (!newName) {
    return;
  }
  /* try to shrink... */
  if (!(name = (char *)ReAllocMem(newName, len = newLen))) {
    name = newName;
  }
  newLen = newMax = 0;
  newName = NULL;

  Win->SetTitle(len, name);
}

static void clear_newtitle(void) {
  if (newName)
    FreeMem(newName);
  newName = NULL;
  newLen = newMax = 0;
}

static inline void write_ctrl(byte c) {
  /*
   *  Control characters can be used in the _middle_
   *  of an escape sequence.
   */
  switch (c) {
  case 0:
    return;
  case 7:
    if (DState != ESxterm_ignore && DState != ESxterm_title) {
      BeepHW();
      return;
    }
    if (DState == ESxterm_title) {
      /* \x07 is an (un)official termination for ESC]0; which starts window title change */
      set_newtitle();
    }
    DState = ESnormal;
    return;
  case 8:
    bs();
    return;
  case 9: /* TAB */
    Pos -= X;
    while (X < SizeX - 1) {
      X++;
      if (TabStop[X >> 5] & (1 << (X & 31)))
        break;
    }
    Pos += X;
    return;
  case 10:
  case 11:
  case 12:
    lf();
    if (!(*Flags & TTY_CRLF))
      return;
    /* FALLTHROUGH */
  case 13:
    cr();
    return;
  case 14:
    G = 1;
    setCharset(G1);
    *Flags |= TTY_DISPCTRL;
    return;
  case 15:
    G = 0;
    setCharset(G0);
    *Flags &= ~TTY_DISPCTRL;
    return;
  case 24:
  case 26:
    DState = ESnormal;
    return;
  case 27:
    DState = ESesc;
    return;
  case 127:
    del();
    return;
  case 128 + 27:
    DState = ESsquare;
    return;
  }

  /* DState machine */

  /* mask out ESques for now */
  switch (DState & ESany) {

  case ESesc:
    switch (c) {
    case '[':
      DState = ESsquare;
      return;
    case ']':
      DState = ESnonstd;
      return;
    case '%':
      DState = ESpercent;
      return;
    case 'E':
      cr();
      lf();
      break;
    case 'M':
      ri();
      break;
    case 'D':
      lf();
      break;
    case 'H':
      TabStop[X >> 5] |= (1 << (X & 31));
      break;
    case 'Z':
      respond_ID();
      break;
    case '7':
      save_current();
      break;
    case '8':
      restore_current();
      break;
    case '(':
      DState = ESsetG0;
      return;
    case ')':
      DState = ESsetG1;
      return;
    case '#':
      DState = EShash;
      return;
    case 'c':
      reset_tty(ttrue);
      break;
    case '>': /* Numeric keypad */
      *Flags &= ~TTY_KBDAPPLIC;
      *Flags |= TTY_NEEDREFOCUS;
      break;
    case '=': /* Appl. keypad */
      *Flags |= TTY_KBDAPPLIC | TTY_NEEDREFOCUS;
      break;
    case '\\': /* ESC\ is the official termination for ESC]0; which starts window title change */
      set_newtitle();
      break;
    }
    break;

  case ESnonstd:
    if (c == 'P') { /* Palette escape sequence */
      nPar = 0;
      memset((byte *)&Par, 0, NPAR * sizeof(ldat));
      DState = ESrgb;
      return;
    } else if (c == 'R') /* Reset palette */
      ResetPaletteHW();
    else if (c == '1') {
      /* may be xterm set window icon title */
      DState = ESxterm_ignore_;
      return;
    } else if (c == '0' || c == '2' || c == '7') {
      /* may be xterm "set icon name & window title" or xterm "set window title" or OS X "set
       * current directory title"
       */
      DState = ESxterm_title_;
      return;
    }
    break;

  case ESrgb:
    if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) {
      Par[nPar++] = (c > '9' ? (c & 0xDF) - 'A' + 10 : c - '0');
      if (nPar == 7)
        SetPaletteHW(Par[0], Par[1] * 16 + Par[2], Par[3] * 16 + Par[4], Par[5] * 16 + Par[6]);
      else
        return; /* avoid resetting DState */
    }
    break;

  case ESsquare:
    Par[0] = nPar = 0;
    /*memset((byte *)&Par, 0, NPAR * sizeof(ldat));*/
    DState = ESgetpars;
    if (c == '[') { /* Function key */
      DState = ESfunckey;
      return;
    }
    if (c == '?') {
      DState = (ttystate)(DState | ESques);
      return;
    }
    /* FALLTHROUGH */

  case ESgetpars:
    if ((c == ';' || c == ':') && nPar < NPAR - 1) {
      Par[++nPar] = 0;
      return;
    } else if (c >= '0' && c <= '9') {
      Par[nPar] *= 10;
      Par[nPar] += c - '0';
      return;
    } else
      DState = (ttystate)(ESgotpars | (DState & ESques));
    /* FALLTHROUGH */

  case ESgotpars:
    switch (c) {
    case 'h':
      set_mode(1);
      break;
    case 'l':
      set_mode(0);
      break;
    case 'c':
      if (DState & ESques) {
        if (!Par[0])
          Par[1] = Par[2] = Par[0];
        else if (nPar == 1)
          Par[2] = 0;
        Win->CursorType = Par[0] | (Par[1] << 8) | (Par[2] << 16);
        *Flags |= TTY_UPDATECURSOR;
      }
    case 'm':
      /* selection complement mask */
      break;
    case 'n':
      if (!(DState & ESques)) {
        if (Par[0] == 5) {
          status_report();
        } else if (Par[0] == 6) {
          cursor_report();
        }
      }
      break;
    }
    if (DState & ESques) {
      break;
    }
    switch (c) {
    case 'm':
      csi_m();
      break;
    case 'H':
    case 'f':
      if (Par[0])
        Par[0]--;
      if (!nPar)
        Par[1] = 0;
      else if (Par[1])
        Par[1]--;
      goto_axy(Par[1], Par[0]);
      break;
    case 'J':
      csi_J(Par[0]);
      break;
    case 'K':
      csi_K(Par[0]);
      break;
    case 'G':
    case '`':
      if (Par[0])
        Par[0]--;
      goto_xy(Par[0], Y);
      break;
    case 'A':
      if (!Par[0])
        Par[0]++;
      goto_xy(X, Y - Par[0]);
      break;
    case 'B':
    case 'e':
      if (!Par[0])
        Par[0]++;
      goto_xy(X, Y + Par[0]);
      break;
    case 'C':
    case 'a':
      if (!Par[0])
        Par[0]++;
      goto_xy(X + Par[0], Y);
      break;
    case 'D':
      if (!Par[0])
        Par[0]++;
      goto_xy(X - Par[0], Y);
      break;
    case 'E':
      if (!Par[0])
        Par[0]++;
      goto_xy(0, Y + Par[0]);
      break;
    case 'F':
      if (!Par[0])
        Par[0]++;
      goto_xy(0, Y - Par[0]);
      break;
    case 'd':
      if (Par[0])
        Par[0]--;
      goto_axy(X, Par[0]);
      break;
    case 'L':
      csi_L(Par[0]);
      break;
    case 'M':
      csi_M(Par[0]);
      break;
    case 'P':
      csi_P(Par[0]);
      break;
    case 'c':
      if (!Par[0])
        respond_ID();
      break;
    case 'g':
      if (!Par[0])
        TabStop[X >> 5] &= ~(1 << (X & 31));
      else if (Par[0] == 3)
        TabStop[0] = TabStop[1] = TabStop[2] = TabStop[3] = TabStop[4] = 0;
      break;
    case 'q': /* DECLL - but only 3 leds */
              /* map 0,1,2,3 to 0,1,2,4 */
#if 0
            if (Par[0] < 4)
                setledstate(kbd_table,
                            (Par[0] < 3) ? Par[0] : 4);
#endif
      break;
    case 'r':
      if (!Par[0])
        Par[0]++;
      if (!nPar || !Par[1])
        Par[1] = SizeY;
      /* Minimum allowed region is 2 lines */
      if (Par[0] < Par[1] && SizeY >= 0 && Par[1] <= (uldat)SizeY) {
        Top = Par[0] - 1;
        Bottom = Par[1];
        goto_axy(0, 0);
      }
      break;
    case 's':
      save_current();
      break;
    case 'u':
      restore_current();
      break;
    case 'X':
      csi_X(Par[0]);
      break;
    case '@':
      csi_at(Par[0]);
      break;
    case ']': /* setterm functions */
      setterm_command();
      break;
    }
    break;

  case ESpercent:
    switch (c) {
    case '@': /* defined in ISO 2022 */
      utf8 = 0;
      break;
    case 'G': /* prelim official escape code */
    case '8': /* retained for compatibility */
      utf8 = 1;
      break;
    }
    break;
  case ESfunckey:
    break;

  case EShash:
    if (c == '8') {
      /* DEC screen alignment test */
      dirty_tty(0, 0, SizeX - 1, SizeY - 1);
      fill(Start, TCELL(ColText, 'E'), (ldat)SizeX * SizeY);
    }
    break;

  case ESsetG0:
    switch (c) {
    case '0':
      G0 = VT100GR_MAP;
      break;
    case 'B':
      G0 = LATIN1_MAP;
      break;
    case 'U':
      G0 = IBMPC_MAP;
      break;
    case 'K':
      G0 = USER_MAP;
      break;
    default:
      break;
    }
    if (G == 0)
      setCharset(G0);
    break;

  case ESsetG1:
    switch (c) {
    case '0':
      G1 = VT100GR_MAP;
      break;
    case 'B':
      G1 = LATIN1_MAP;
      break;
    case 'U':
      G1 = IBMPC_MAP;
      break;
    case 'K':
      G1 = USER_MAP;
      break;
    default:
      break;
    }
    if (G == 1)
      setCharset(G1);
    break;

  case ESxterm_ignore_:
    if (c == ';') {
      DState = ESxterm_ignore;
      return;
    }
    break;

  case ESxterm_ignore:
    /* ignore, cannot set icon name */
    return;

  case ESxterm_title_:
    if (c == ';') {
      DState = ESxterm_title;
      return;
    }
    break;

  case ESxterm_title:
    if (c >= ' ' && insert_newtitle(c))
      return;
    break;

  default:
    break;
  }

  if (newName && c != 27)
    clear_newtitle();

  DState = ESnormal;
}

Twidget TtyKbdFocus(Twidget newW) {
  udat newFlags;
  Twidget oldW;
  Twidget P;
  Tscreen screen = newW && (P = newW->Parent) && IS_SCREEN(P) ? (Tscreen)P : All->FirstScreen;

  if (screen) {
    oldW = screen->FocusW();
    screen->FocusW(newW);
  } else
    oldW = newW = (Twidget)0;

  if (screen == All->FirstScreen) {
    if (!newW || !IS_WINDOW(newW) || !W_USE((Twindow)newW, USECONTENTS) ||
        !((Twindow)newW)->USE.C.TtyData)

      newFlags = defaultFlags;
    else
      newFlags = ((Twindow)newW)->USE.C.TtyData->Flags;

    if ((newFlags ^ kbdFlags) & TTY_KBDAPPLIC)
      ConfigureHW(HW_KBDAPPLIC, tfalse, newFlags & TTY_KBDAPPLIC);

    if ((newFlags ^ kbdFlags) & TTY_ALTCURSKEYS)
      ConfigureHW(HW_ALTCURSKEYS, tfalse, newFlags & TTY_ALTCURSKEYS);

    kbdFlags = newFlags;
  }
  return oldW;
}

void ForceKbdFocus(void) {
  kbdFlags = ~defaultFlags;
  (void)TtyKbdFocus(All->FirstScreen->FocusW());
}

/* initialize global static data */
static void common(Twindow w) {
  Win = w;
  Data = Win->USE.C.TtyData;
  Flags = &Data->Flags;

  if (!SizeX || !SizeY)
    return;

  /* scroll YLogic to bottom */
  if (Win->YLogic < ScrollBack) {
    if ((Twidget)Win == All->FirstScreen->FirstW)
      ScrollFirstWindow(0, ScrollBack - Win->YLogic, ttrue);
    else {
      dirty_tty(0, 0, SizeX - 1, SizeY - 1);
      Win->YLogic = ScrollBack;
      DrawBorderWindow(w, BORDER_RIGHT);
    }
  }
  /* clear any selection */
  if (Win->State & WINDOW_ANYSEL)
    ClearHilight(Win);
}

/*
 * combine (*pc) with partial utf-8 char stored in utf8_char.
 * return ttrue if the utf-8 char is complete, and can be displayed.
 */
static tbool combine_utf8(trune *pc) {
  trune c = *pc;

  if (utf8_count > 0 && (c & 0xc0) == 0x80) {
    utf8_char = (utf8_char << 6) | (c & 0x3f);
    utf8_count--;
    if (utf8_count == 0)
      *pc = utf8_char;
    return (tbool)(utf8_count == 0);
  }

  if ((c & 0xe0) == 0xc0) {
    utf8_count = 1;
    utf8_char = (c & 0x1f);
  } else if ((c & 0xf0) == 0xe0) {
    utf8_count = 2;
    utf8_char = (c & 0x0f);
  } else if ((c & 0xf8) == 0xf0) {
    utf8_count = 3;
    utf8_char = (c & 0x07);
  } else
    utf8_count = 0;
  return tfalse;
}

/* this is the main entry point */
byte TtyWriteCharsetOrUtf8(Twindow w, uldat len, const char *chars, bool force_utf8) {
  trune c;
  byte printable, utf8_in_use, disp_ctrl, state_normal;

  if (!w || !W_USE(w, USECONTENTS) || !w->USE.C.TtyData)
    return tfalse;
  if (!len || !chars)
    return ttrue;

  common(w);

  while (!(*Flags & TTY_STOPPED) && len) {
    c = (byte)*chars++;
    len--;

    /* If the original code was a control character we only allow a glyph
     * to be displayed if the code is not normally used (such as for cursor movement)
     * or if the TTY_DISPCTRL mode has been explicitly enabled. Certain characters
     * (as given by the CTRL_ALWAYS bitmap) are always displayed as control characters,
     * as the console would be pretty useless without them; to display an arbitrary
     * font position use the direct-to-font zone in UTF-8 mode.
     */
    if ((state_normal = (DState == ESnormal))) {
      disp_ctrl = *Flags & TTY_DISPCTRL;
      utf8_in_use = utf8 && !disp_ctrl;

      if (force_utf8 || utf8_in_use) {
        if (c & 0x80) {
          if (!combine_utf8(&c))
            continue;
        } else
          utf8_count = 0;

        printable = c >= 32 && c != 127 && c != 128 + 27;

      } else {
        if (*Flags & TTY_SETMETA)
          c |= 0x80;

        printable = (c >= 32 || !(((disp_ctrl ? CTRL_ALWAYS : CTRL_ACTION) >> c) & 1)) &&
                    (c != 127 || disp_ctrl) && (c != 128 + 27);

        if (printable)
          c = applyG((byte)c);
      }
    } else
      utf8_in_use = printable = tfalse;

    if (printable && state_normal) {
      /* Now try to find out how to display it */
      if (*Flags & TTY_NEEDWRAP) {
        cr();
        lf();
      }
      if (*Flags & TTY_INSERT)
        insert_char(1);

      dirty_tty(X, Y, X, Y);
      *Pos = TCELL(Color, c);

      if (X == SizeX - 1) {
        if (*Flags & TTY_AUTOWRAP)
          *Flags |= TTY_NEEDWRAP;
      } else {
        X++;
        Pos++;
      }
    } else
      write_ctrl((byte)c);
    /* don't flush here, it just decreases performance */
    /* flush_tty(); */
  }
  flush_tty();
  return true;
}

bool TtyWriteCharset(Twindow w, uldat len, const char *charset_bytes) {
  return TtyWriteCharsetOrUtf8(w, len, charset_bytes, false);
}

bool TtyWriteUtf8(Twindow w, uldat len, const char *utf8_bytes) {
  return TtyWriteCharsetOrUtf8(w, len, utf8_bytes, true);
}

/* similar to TtyWriteUtf8(), but writes UTF-32 */
bool TtyWriteTRune(Twindow w, uldat len, const trune *runes) {
  trune c;
  byte ok;

  if (!w || !W_USE(w, USECONTENTS) || !w->USE.C.TtyData)
    return false;
  if (!len || !runes)
    return true;

  common(w);

  while (!(*Flags & TTY_STOPPED) && len) {
    c = *runes++;
    len--;

    if (c < 0x100) {
      if (*Flags & TTY_SETMETA)
        c |= 0x80;

      /* If the original code < 32, interpret as control character */
      ok = (c >= 32 || !(((*Flags & TTY_DISPCTRL ? CTRL_ALWAYS : CTRL_ACTION) >> c) & 1)) &&
           (c != 127 || (*Flags & TTY_DISPCTRL)) && (c != 128 + 27);
    } else
      ok = ttrue;

    if (DState == ESnormal && ok) {

      /* Now try to find out how to display it */
      if (*Flags & TTY_NEEDWRAP) {
        cr();
        lf();
      }
      if (*Flags & TTY_INSERT)
        insert_char(1);

      dirty_tty(X, Y, X, Y);
      *Pos = TCELL(Color, c);

      if (X == SizeX - 1) {
        if (*Flags & TTY_AUTOWRAP)
          *Flags |= TTY_NEEDWRAP;
      } else {
        X++;
        Pos++;
      }
    } else
      write_ctrl((byte)c);
  }
  flush_tty();
  return true;
}

/*
 * this currently wraps at window width so it can write multiple rows at time.
 * does not move cursor position, nor interacts with wrapglitch.
 */
bool TtyWriteTCell(Twindow w, dat x, dat y, uldat len, const tcell *text) {
  ldat left, max, chunk;
  ldat i;
  tcell *dst;

  if (!w || !W_USE(w, USECONTENTS) || !w->USE.C.TtyData)
    return tfalse;
  if (!len || !text)
    return ttrue;

  common(w);

  x = Max2(x, 0);
  x = Min2(x, SizeX - 1);
  y = Max2(y, 0);
  y = Min2(y, SizeY - 1);

  if (len > (uldat)(SizeY - y) * (uldat)SizeX - (uldat)x)
    len = (uldat)(SizeY - y) * (uldat)SizeX - (uldat)x;

  left = len;
  dst = Start + y * SizeX + x;

  /* scroll YLogic to bottom */
  if (Win->YLogic < ScrollBack) {
    if ((Twidget)Win == All->FirstScreen->FirstW)
      ScrollFirstWindow(0, ScrollBack - Win->YLogic, ttrue);
    else {
      dirty_tty(0, 0, SizeX - 1, SizeY - 1);
      Win->YLogic = ScrollBack;
      DrawBorderWindow(w, BORDER_RIGHT);
    }
  }
  /* clear any selection */
  if (Win->State & WINDOW_ANYSEL)
    ClearHilight(Win);

  do {
    if (dst >= Split)
      dst -= Split - Base;
    max = Split - dst;
    chunk = Min2(left, max);
    for (i = chunk; i; i--) {
      *dst++ = *text++;
    }
  } while ((left -= chunk) > 0);

  if (len > (uldat)(SizeX - x))
    dirty_tty(0, y, SizeX - 1, y + (x + len - 1) / SizeX);
  else
    dirty_tty(x, y, x + len - 1, y);

  flush_tty();
  return true;
}

#if 0
/*
 * Turn the Scroll-Lock LED on when the tty is stopped
 */
static void con_stop(struct tty_struct *tty) {
    int console_num;
    if (!tty)
        return;
    console_num = MINOR(tty->device) - (tty->driver.minor_start);
    if (!vc_cons_allocated(console_num))
        return;
    set_vc_kbd_led(kbd_table + console_num, VC_SCROLLOCK);
    set_leds();
}

/*
 * Turn the Scroll-Lock LED off when the console is started
 */
static void con_start(struct tty_struct *tty) {
    int console_num;
    if (!tty)
        return;
    console_num = MINOR(tty->device) - (tty->driver.minor_start);
    if (!vc_cons_allocated(console_num))
        return;
    clr_vc_kbd_led(kbd_table + console_num, VC_SCROLLOCK);
    set_leds();
}

static void clear_buffer_attributes(Twindow w) {
    unsigned short *p = (unsigned short *) origin;
    int count = screenbuf_size/2;
    int mask = hi_font_mask | 0xff;

    for (; count > 0; count--, p++) {
        scr_writew((scr_readw(p)&mask) | (video_erase_char&~mask), p);
    }
}

/*
 *        Palettes
 */
void set_palette(Twindow w) {
    if (vcmode != KD_GRAPHICS)
          sw->con_set_palette(vc_cons.d, color_table);
}

static int set_get_cmap(unsigned char *arg, int set) {
    int i, j, k;

    for (i = 0; i < 16; i++)
        if (set) {
            get_user(default_red[i], arg++);
            get_user(default_grn[i], arg++);
            get_user(default_blu[i], arg++);
        } else {
            put_user(default_red[i], arg++);
            put_user(default_grn[i], arg++);
            put_user(default_blu[i], arg++);
        }
    if (set) {
        for (i = 0; i < MAX_NR_CONSOLES; i++)
            if (vc_cons_allocated(i)) {
                for (j = k = 0; j < 16; j++) {
                    vc_cons[i].d->vc_palette[k++] = default_red[j];
                    vc_cons[i].d->vc_palette[k++] = default_grn[j];
                    vc_cons[i].d->vc_palette[k++] = default_blu[j];
                }
                set_palette(i);
            }
    }
    return 0;
}

/*
 * Load palette into the DAC registers. arg points to a colour
 * map, 3 bytes per colour, 16 colours, range from 0 to 255.
 */

int con_set_cmap(unsigned char *arg) {
    return set_get_cmap (arg,1);
}

int con_get_cmap(unsigned char *arg) {
    return set_get_cmap (arg,0);
}

void reset_palette(Twindow w) {
    int j, k;
    for (j=k=0; j<16; j++) {
        palette[k++] = default_red[j];
        palette[k++] = default_grn[j];
        palette[k++] = default_blu[j];
    }
    set_palette(currcons);
}
#endif /* 0 */
