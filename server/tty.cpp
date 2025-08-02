/*
 *  tty->c --  linux terminal emulator
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

/* enable keypad by default */
static udat kbdFlags = TTY_KBDAPPLIC | TTY_AUTOWRAP, defaultFlags = TTY_KBDAPPLIC | TTY_AUTOWRAP;

static dat dirty[2][4];
static ldat dirtyS[2];
static byte dirtyN;

/* A bitmap for codes <32. A bit of 1 indicates that the code
 * corresponding to that bit number invokes some special action
 * (such as cursor movement) and should not be displayed as a
 * glyph unless the TTY_DISPCTRL mode is explicitly enabled.
 */
#define CTRL_ACTION 0x0d00ff81
#define CTRL_ALWAYS 0x0800f501 /* Cannot be overridden by TTY_DISPCTRL */

static inline void change_flags(tty_data *tty, uldat bits, byte on_off) {
  if (on_off) {
    tty->Flags |= bits;
  } else {
    tty->Flags &= ~bits;
  }
}

/*
 * for better cleannes, dirty_tty()
 * should be used *before* actually touching Win->Contents[]
 */
static void dirty_tty(tty_data *tty, dat x1, dat y1, dat x2, dat y2) {
  ldat S[2] = {0, 0};
  dat xy[2][4];
  byte i;

  if (dirtyN == TW_MAXBYTE || x1 > x2 || x1 >= tty->SizeX || y1 > y2 || y1 >= tty->SizeY)
    return;

  x2 = Min2(x2, tty->SizeX - 1);
  y2 = Min2(y2, tty->SizeY - 1);

  for (i = 0; i < dirtyN; i++) {
    xy[i][0] = Min2(dirty[i][0], x1);
    xy[i][1] = Min2(dirty[i][1], y1);
    xy[i][2] = Max2(dirty[i][2], x2);
    xy[i][3] = Max2(dirty[i][3], y2);
    S[i] += (xy[0][2] - xy[0][0] + 1) * (xy[0][3] - xy[0][1] + 1);
    S[!i] += dirtyS[i];
  }
  if (dirtyN < 2) {
    S[dirtyN] += (x2 - x1 + 1) * (y2 - y1 + 1);
  }
  i = dirtyN && S[0] > S[1];

  if (S[i] >= tty->SizeX * tty->SizeY * 3 / 4) {
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

static void flush_tty(tty_data *tty) {
  Twindow w = tty->Win;
  dat i;
  bool cursor_moved = false;

  /* first, draw on Tscreen whatever changed in the window */
  if (dirtyN) {
    if (dirtyN == TW_MAXBYTE) {
      DrawLogicWidget(w, 0, tty->ScrollBack, tty->SizeX - 1, tty->SizeY - 1 + tty->ScrollBack);
    } else {
      for (i = 0; i < dirtyN; i++) {
        DrawLogicWidget(w, dirty[i][0], dirty[i][1] + tty->ScrollBack, dirty[i][2],
                        dirty[i][3] + tty->ScrollBack);
      }
    }
    dirtyN = 0;
  }

  /* then update cursor */
  if (w->CurX != (ldat)tty->X || w->CurY != (ldat)tty->Y + tty->ScrollBack) {

    w->CurX = (ldat)tty->X;
    w->CurY = (ldat)tty->Y + tty->ScrollBack;
    tty->Pos = w->USE.C.Contents + w->CurX + (w->CurY + w->USE.C.HSplit) * tty->SizeX;
    if (tty->Pos >= tty->Split) {
      tty->Pos -= tty->Split - w->USE.C.Contents;
    }
    cursor_moved = true;
  } else {
    cursor_moved = false;
  }

  if ((cursor_moved || (tty->Flags & TTY_UPDATECURSOR)) && ContainsCursor(w)) {
    UpdateCursor();
  }
  tty->Flags &= ~TTY_UPDATECURSOR;

  /* finally, keyboard focus configuration: */
  if (tty->Flags & TTY_NEEDREFOCUS) {
    tty->Flags &= ~TTY_NEEDREFOCUS;
    if (w == (Twindow)All->Screens.First->FocusW()) {
      w->KbdFocus();
    }
  }
}

/* Note: inverting the screen twice should revert to the original state */

static void invert_screen(tty_data *tty) {
  tcell a, *p = tty->Start;
  ldat count;

  dirty_tty(tty, 0, 0, tty->SizeX - 1, tty->SizeY - 1);
  count = tty->SizeX * tty->SizeY;

  while (count--) {
    a = *p;
    *p++ = (a & ~TCELL(TW_MAXWCOL, 0)) | TCELL(TCOL(TCOLBG(TCOLOR(a)), TCOLFG(TCOLOR(a))), 0);
    if (p == tty->Split) {
      p = tty->Win->USE.C.Contents;
    }
  }
}

static void insert_char(tty_data *tty, ldat nr) {
  tcell *q = tty->Pos;
  tcell *p = q + tty->SizeX - tty->X - nr;

  dirty_tty(tty, tty->X, tty->Y, tty->SizeX - 1, tty->Y);

  while (--p >= q) {
    p[nr] = *p;
  }
  tcell cell = TCELL(tty->Win->ColText, ' ');
  while (nr--) {
    *q++ = cell;
  }
  tty->Flags &= ~TTY_NEEDWRAP;
}

static inline void delete_char(tty_data *tty, ldat nr) {
  tcell *p = tty->Pos;
  dat i = tty->SizeX - tty->X - nr;

  dirty_tty(tty, tty->X, tty->Y, tty->SizeX - 1, tty->Y);

  while (i--) {
    *p = p[nr];
    p++;
  }
  tcell cell = TCELL(tty->Win->ColText, ' ');
  while (nr--) {
    *p++ = cell;
  }
  tty->Flags &= ~TTY_NEEDWRAP;
}

/*
 * goto_xy() must verify all boundaries, because the arguments
 * might also be negative. If the given position is out of
 * bounds, the cursor is placed at the nearest margin.
 */
static void goto_xy(tty_data *tty, ldat new_x, ldat new_y) {
  dat min_y, max_y;

  if (new_x < 0) {
    tty->X = 0;
  } else if (new_x >= (ldat)tty->SizeX) {
    tty->X = tty->SizeX - 1;
  } else {
    tty->X = (dat)new_x;
  }
  if (tty->Flags & TTY_RELORIG) {
    min_y = tty->Top;
    max_y = tty->Bottom;
  } else {
    min_y = 0;
    max_y = tty->SizeY;
  }

  if (new_y < (ldat)min_y) {
    tty->Y = min_y;
  } else if (new_y >= (ldat)max_y) {
    tty->Y = max_y - 1;
  } else {
    tty->Y = (dat)new_y;
  }
  tty->Flags &= ~TTY_NEEDWRAP;

  /* never flush here, just update tty->Pos */
  tty->Pos = tty->Start + tty->X + (ldat)tty->Y * tty->SizeX;
  if (tty->Pos >= tty->Split) {
    tty->Pos -= tty->Split - tty->Win->USE.C.Contents;
  }
}

/* for absolute user moves, when TTY_RELORIG (decom) is set */
static inline void goto_axy(tty_data *tty, ldat new_x, ldat new_y) {
  goto_xy(tty, new_x, tty->Flags & TTY_RELORIG ? (tty->Top + new_y) : new_y);
}

/* WARNING: fwd_copy() doesn't call dirty_tty(), you must call it manually! */
static void fwd_copy(tty_data *tty, tcell *s, tcell *d, ldat len) {
  ptrdiff_t split_delta = tty->Split - tty->Win->USE.C.Contents;
  ldat l;

  while (s >= tty->Split) {
    s -= split_delta;
  }
  while (d >= tty->Split) {
    d -= split_delta;
  }
  while (len > 0) {
    l = Min2(len, tty->Split - s);
    l = Min2(l, tty->Split - d);
    MoveMem(s, d, l * sizeof(tcell)); // s and d can overlap!
    s += l;
    d += l;
    len -= l;
    if (s == tty->Split)
      s = tty->Win->USE.C.Contents;
    if (d == tty->Split)
      d = tty->Win->USE.C.Contents;
  }
}

/* WARNING: rev_copy() doesn't call dirty_tty(), you must call it manually! */
static void rev_copy(tty_data *tty, tcell *s, tcell *d, ldat len) {
  ptrdiff_t split_delta = tty->Split - tty->Win->USE.C.Contents;
  ldat l;

  s += len;
  d += len;
  while (s > tty->Split) {
    s -= split_delta;
  }
  while (d > tty->Split) {
    d -= split_delta;
  }

  while (len > 0) {
    l = Min2(len, s - tty->Win->USE.C.Contents);
    l = Min2(l, d - tty->Win->USE.C.Contents);
    s -= l;
    d -= l;
    len -= l;
    MoveMem(s, d, l * sizeof(tcell));
    if (s == tty->Win->USE.C.Contents) {
      s = tty->Split;
    }
    if (d == tty->Win->USE.C.Contents) {
      d = tty->Split;
    }
  }
}

/* WARNING: fill() doesn't call dirty_tty(), you must call it manually! */
static void fill(tty_data *tty, tcell *s, tcell c, ldat len) {
  ptrdiff_t split_delta = tty->Split - tty->Win->USE.C.Contents;
  ldat l;

  while (s >= tty->Split) {
    s -= split_delta;
  }

  while (len > 0) {
    l = Min2(len, tty->Split - s);
    len -= l;
    while (l--) {
      *s++ = c;
    }
    if (s == tty->Split) {
      s = tty->Win->USE.C.Contents;
    }
  }
}

static void scrollup(tty_data *tty, dat t, dat b, dat nr) {
  Twindow w = tty->Win;
  tcell *d, *s;
  bool accel = false;

  if (t + nr >= b) {
    nr = b - t - 1;
  }
  if (b > tty->SizeY || t >= b || nr < 1) {
    return;
  }
  /* try to accelerate this */
  if (w == All->Screens.First->Widgets.First) {
    accel = true;
    flush_tty(tty);
  } else {
    dirty_tty(tty, 0, t, tty->SizeX - 1, b - 1);
  }
  if (t == 0 && b == tty->SizeY) {
    /* full screen scrolls. use splitline+scrollback */

    w->USE.C.HSplit += nr;
    if (w->USE.C.HSplit >= w->HLogic) {
      w->USE.C.HSplit -= w->HLogic;
    }
    tty->Start += nr * tty->SizeX;
    if (tty->Start >= tty->Split) {
      tty->Start -= tty->Split - w->USE.C.Contents;
    }
    tty->Pos += nr * tty->SizeX;
    if (tty->Pos >= tty->Split) {
      tty->Pos -= tty->Split - w->USE.C.Contents;
    }
    s = tty->Start + tty->SizeX * (t + nr);
    d = tty->Start + tty->SizeX * t;
  } else {
    /* plain memcpy */
    s = tty->Start + tty->SizeX * (t + nr);
    d = tty->Start + tty->SizeX * t;
    fwd_copy(tty, s, d, (b - t - nr) * tty->SizeX);
  }

  /* clear the last nr lines */
  fill(tty, d + (b - t - nr) * tty->SizeX, TCELL(w->ColText, ' '), nr * tty->SizeX);

  if (accel) {
    ScrollFirstWindowArea(0, t, tty->SizeX - 1, b - 1, 0, -nr);
  }
}

static void scrolldown(tty_data *tty, dat t, dat b, dat nr) {
  tcell *s;
  ldat step;
  bool accel = false;

  if (t + nr >= b) {
    nr = b - t - 1;
  }
  if (b > tty->SizeY || t >= b || nr < 1) {
    return;
  }
  /* try to accelerate this */
  if (tty->Win == All->Screens.First->Widgets.First) {
    accel = true;
    flush_tty(tty);
  } else {
    dirty_tty(tty, 0, t, tty->SizeX - 1, b - 1);
  }

  s = tty->Start + tty->SizeX * t;
  step = tty->SizeX * nr;

  rev_copy(tty, s, s + step, (b - t - nr) * tty->SizeX);
  fill(tty, s, TCELL(tty->Win->ColText, ' '), step);

  if (accel) {
    ScrollFirstWindowArea(0, t, tty->SizeX - 1, b - 1, 0, nr);
  }
}

static inline void lf(tty_data *tty) {
  /* don't scroll if above bottom of scrolling region, or
   * if below scrolling region
   */
  if (tty->Y + 1 == tty->Bottom) {
    scrollup(tty, tty->Top, tty->Bottom, 1);
  } else if (tty->Y < tty->SizeY - 1) {
    tty->Y++;
    tty->Pos += tty->SizeX;
    if (tty->Pos >= tty->Split) {
      tty->Pos -= tty->Split - tty->Win->USE.C.Contents;
    }
  }
  tty->Flags &= ~TTY_NEEDWRAP;
}

static void ri(tty_data *tty) {
  /* don't scroll if below top of scrolling region,
   * or if above scrolling region
   */
  if (tty->Y == tty->Top) {
    scrolldown(tty, tty->Top, tty->Bottom, 1);
  } else if (tty->Y > 0) {
    tty->Y--;
    tty->Pos -= tty->SizeX;
    if (tty->Pos < tty->Win->USE.C.Contents) {
      tty->Pos += tty->Split - tty->Win->USE.C.Contents;
    }
  }
  tty->Flags &= ~TTY_NEEDWRAP;
}

static inline void cr(tty_data *tty) {
  tty->Pos -= tty->X;
  tty->X = 0;
  tty->Flags &= ~TTY_NEEDWRAP;
}

static inline void bs(tty_data *tty) {
  if (tty->X) {
    tty->X--;
    tty->Pos--;
    tty->Flags &= ~TTY_NEEDWRAP;
  }
}

static inline void del(tty_data * /*tty*/) {
  /* ignored */
}

static void csi_J(tty_data *tty, int vpar) {
  tcell *start;
  ldat count;

  switch (vpar) {
  case 0: /* erase from cursor to end of display */
    dirty_tty(tty, 0, tty->Y, tty->SizeX - 1, tty->SizeY - 1);
    count = (tty->SizeY - tty->Y) * (ldat)tty->SizeX + tty->SizeX - tty->X;
    start = tty->Pos;
    break;
  case 1: /* erase from start to cursor */
    dirty_tty(tty, 0, 0, tty->SizeX - 1, tty->Y);
    count = tty->Y * (ldat)tty->SizeX + tty->X;
    start = tty->Start;
    break;
  case 2: /* erase whole display */
    dirty_tty(tty, 0, 0, tty->SizeX - 1, tty->SizeY - 1);
    count = (ldat)tty->SizeX * tty->SizeY;
    start = tty->Start;
    break;
  default:
    return;
  }
  fill(tty, start, TCELL(tty->Win->ColText, ' '), count);

  tty->Flags &= ~TTY_NEEDWRAP;
}

static void csi_K(tty_data *tty, int vpar) {
  dat count;
  tcell *start;

  switch (vpar) {
  case 0: /* erase from cursor to end of line */
    dirty_tty(tty, tty->X, tty->Y, tty->SizeX - 1, tty->Y);
    count = tty->SizeX - tty->X;
    start = tty->Pos;
    break;
  case 1: /* erase from start of line to cursor */
    dirty_tty(tty, 0, tty->Y, tty->X, tty->Y);
    count = tty->X + 1;
    start = tty->Pos - tty->X;
    break;
  case 2: /* erase whole line */
    dirty_tty(tty, 0, tty->Y, tty->SizeX - 1, tty->Y);
    count = tty->SizeX;
    start = tty->Pos - tty->X;
    break;
  default:
    return;
  }
  const tcell cell = TCELL(tty->Win->ColText, ' ');
  while (count--) {
    *start++ = cell;
  }
  tty->Flags &= ~TTY_NEEDWRAP;
}

static void csi_X(tty_data *tty, int vpar) /* erase the following vpar positions */
{                                          /* not vt100? */
  tcell *start = tty->Pos;
  if (!vpar) {
    vpar++;
  }
  vpar = Min2(vpar, tty->SizeX - tty->X);

  dirty_tty(tty, tty->X, tty->Y, tty->X + vpar - 1, tty->Y);

  const tcell cell = TCELL(tty->Win->ColText, ' ');
  while (vpar--) {
    *start++ = cell;
  }
  tty->Flags &= ~TTY_NEEDWRAP;
}

static void update_eff(tty_data *tty) {
  tcolor coltext = tty->Win->ColText;
  trgb fg = TCOLFG(coltext), bg = TCOLBG(coltext);
  const udat effects = tty->Effects;

  if (effects & EFF_UNDERLINE) {
    fg = TCOLFG(tty->Underline);
  } else if (effects & EFF_HALFINTENS) {
    fg = TCOLFG(tty->HalfInten);
  }
  if (!!(effects & EFF_REVERSE) != !!(tty->Flags & TTY_INVERTSCR)) {
    const trgb inv_fg = (bg & ~thigh) | (fg & thigh);
    const trgb inv_bg = (fg & ~thigh) | (bg & thigh);
    fg = inv_fg;
    bg = inv_bg;
  }
  if (effects & EFF_INTENSITY) {
    fg |= thigh;
  }
  if (effects & EFF_BLINK) {
    bg |= thigh;
  }
  tty->Color = TCOL(fg, bg);
}

static void set_charset(tty_data *tty, byte g) {
  Twindow w = tty->Win;
  switch (g) {
  case VT100GR_MAP:
    w->Charset = Tutf_VT100GR_to_UTF_32;
    tty->InvCharset = Tutf_UTF_32_to_VT100GR;
    break;
  case LATIN1_MAP:
    w->Charset = Tutf_ISO8859_1_to_UTF_32;
    tty->InvCharset = Tutf_UTF_32_to_ISO8859_1;
    break;
  case IBMPC_MAP:
    w->Charset = Tutf_CP437_to_UTF_32;
    tty->InvCharset = Tutf_UTF_32_to_CP437;
    break;
  case USER_MAP:
    w->Charset = All->Gtranslations[USER_MAP];
    tty->InvCharset = Tutf_UTF_32_to_ISO8859_1; /* very rough :( */
    break;
  }
}

static inline trune applyG(tty_data *tty, trune c) {
  if (c < 0x100) {
    const trune *charset = tty->Win->Charset;
    if (charset != Tutf_ISO8859_1_to_UTF_32) {
      c = charset[(byte)c];
    }
  }
  return c;
}

static void csi_m(tty_data *tty) {
  tcolor coltext = tty->Win->ColText;
  trgb fg = TCOLFG(coltext), bg = TCOLBG(coltext);
  uldat i;
  udat effects = tty->Effects;

  for (i = 0; i <= tty->nPar;) {
    uldat par = tty->Par[i];
    switch (par) {
    case 0:
      /* all attributes off */
      fg = TCOLFG(tty->DefColor);
      bg = TCOLBG(tty->DefColor);
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
      set_charset(tty, tty->Gv[tty->Gi]);
      tty->Flags &= ~(TTY_DISPCTRL | TTY_SETMETA);
      break;
    case 11: /* ANSI X3.64-1979 (SCO-ish?)
              * Select first alternate font, lets
              * chars < 32 be displayed as ROM chars.
              */
      set_charset(tty, IBMPC_MAP);
      tty->Flags |= TTY_DISPCTRL;
      tty->Flags &= ~TTY_SETMETA;
      break;
    case 12: /* ANSI X3.64-1979 (SCO-ish?)
              * Select second alternate font, toggle
              * high bit before displaying as ROM char.
              */
      set_charset(tty, IBMPC_MAP);
      tty->Flags |= TTY_DISPCTRL | TTY_SETMETA;
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
      if (i < tty->nPar) {
        if (tty->Par[i + 1] == 5) {
          /* 8-bit palette foreground: ESC[38;5;<8BITPALETTE>m */
          uldat index = tty->Par[i + 2];
          if (index <= 255) {
            fg = Palette[index];
          }
          i += 3;
          continue;
        } else if (tty->Par[i + 1] == 2) {
          /* 24-bit truecolor foreground: ESC[38;2;<R>;<G>;<B>m */
          uldat r = tty->Par[i + 2];
          uldat g = tty->Par[i + 3];
          uldat b = tty->Par[i + 4];
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
      fg = TCOLFG(tty->DefColor);
      effects |= EFF_UNDERLINE;
      break;
    case 39:
      /* ANSI X3.64-1979 (SCO-ish?) Disable underline.
       * Reset colour to default? It did this before...
       */
      fg = TCOLFG(tty->DefColor);
      effects &= ~EFF_UNDERLINE;
      break;
    case 48:
      if (i < tty->nPar) {
        if (tty->Par[i + 1] == 5) {
          /* 8-bit palette background: ESC[48;5;<8BITPALETTE>m */
          uldat index = tty->Par[i + 2];
          if (index <= 255) {
            bg = Palette[index];
          }
          i += 3;
          continue;
        } else if (tty->Par[i + 1] == 2) {
          /* 24-bit truecolor background: ESC[48;2;<R>;<G>;<B>m */
          uldat r = tty->Par[i + 2];
          uldat g = tty->Par[i + 3];
          uldat b = tty->Par[i + 4];
          if (r <= 255 && g <= 255 && b <= 255) {
            bg = TRGB(r, g, b);
          }
          i += 5;
          continue;
        }
      }
      break;
    case 49: /* restore default bg */
      bg = TCOLBG(tty->DefColor);
      break;
    default:
      if (par >= 30 && par <= 37) {
        fg = Palette[par - 30];
      } else if (par >= 40 && par <= 47) {
        bg = Palette[par - 40];
      } else if (par >= 90 && par <= 97) {
        fg = Palette[par - 90 + 8];
      } else if (par >= 100 && par <= 107) {
        bg = Palette[par - 100 + 8];
      }
    }
    i++;
  }
  tty->Effects = effects;
  tty->Win->ColText = TCOL(fg, bg);

  update_eff(tty);
}

static void respond_string(tty_data *tty, const char *p, int len) {
  if (len < 0) {
    return;
  }
  Twindow w = tty->Win;

  /* the remote program may be directly attached to the window */
  if (!RemoteWindowWriteQueue(w, len, p)) {

    /* or we may need to send a message to Win->Owner */
    Tmsg msg;
    event_keyboard *event;
    if ((msg = Smsg::Create(msg_widget_key, len))) {
      /* this is the same code as in KeyboardEvent() in hw.c */
      event = &msg->Event.EventKeyboard;
      event->W = w;
      if (len == 1 && (p[0] == ENTER || p[0] == ESCAPE)) {
        event->Code = p[0];
      } else {
        event->Code = 1;
      }
      event->ShiftFlags = (udat)0;
      event->SeqLen = len;
      CopyMem(p, event->AsciiSeq, len);
      event->AsciiSeq[len] = '\0'; /* terminate string with \0 */
      SendMsg(w->Owner, msg);
    }
  }
}

static void cursor_report(tty_data *tty) {
  char buf[40];
  int n = snprintf(buf, sizeof(buf), "\033[%d;%dR",
                   tty->Y + (tty->Flags & TTY_RELORIG ? tty->Top + 1 : 1), tty->X + 1);
  respond_string(tty, buf, n);
}

static inline void status_report(tty_data *tty) {
  respond_string(tty, "\033[0n", 4); /* Terminal ok */
}

/*
 * this is what the terminal answers to a ESC-Z or csi0c query.
 */
static inline void respond_ID(tty_data *tty) {
  /* VT102ID (returned by linux console) is "\033[?6c" */
  /* twin <= 0.3.8  reports "\033[?6;3c" to indicate xterm-style mouse features */
  /* twin <= 0.3.10 reports "\033[?6;4c", can also report mouse motion with no buttons pressed */
  /* twin >= 0.3.11 reports "\033[?6;5c", also supports utf-8 mode */
  respond_string(tty, "\033[?6;5c", 7);
}

static void set_mode(tty_data *tty, byte on_off) {
  uldat i;

  for (i = 0; i <= tty->nPar; i++) {
    uldat par = tty->Par[i];

    /* DEC private modes set/reset */

    if (tty->State & ESques) {
      switch (par) {

      case 1: /* Cursor keys send ^[Ox/^[[x */
        change_flags(tty, TTY_ALTCURSKEYS, on_off);
        tty->Flags |= TTY_NEEDREFOCUS;
        break;
      case 2: /* set G0...G3 to LATIN1 charset */
        std::memset(tty->Gv, LATIN1_MAP, sizeof(tty->Gv));
        set_charset(tty, LATIN1_MAP);
        break;
      case 3: /* 80/132 mode switch unimplemented */
        break;
      case 5: /* Inverted screen on/off */
        change_flags(tty, TTY_INVERTSCR, on_off);
        update_eff(tty);
        invert_screen(tty);
        break;
      case 6: /* Origin relative/absolute */
        change_flags(tty, TTY_RELORIG, on_off);
        goto_axy(tty, 0, 0);
        break;
      case 7: /* Autowrap on/off */
        change_flags(tty, TTY_AUTOWRAP, on_off);
        break;
      case 8: /* Autorepeat on/off */
        break;
      case 9: /* X10-compatible mouse reporting */
        change_flags(tty,
                     TTY_REPORTMOUSE_STYLE | TTY_REPORTMOUSE_RELEASE | TTY_REPORTMOUSE_DRAG |
                         TTY_REPORTMOUSE_MOVE,
                     tfalse);
        change_flags(tty, TTY_REPORTMOUSE_X10, on_off);
        tty->Win->ChangeField(TWS_window_Attr, WINDOW_WANT_MOUSE, on_off ? WINDOW_WANT_MOUSE : 0);
        break;
      case 25: /* Cursor on/off */
        tty->Win->ChangeField(TWS_window_Flags, WINDOWFL_CURSOR_ON,
                              on_off ? WINDOWFL_CURSOR_ON : 0);
        tty->Flags |= TTY_UPDATECURSOR;
        break;
      case 999: /* twterm mouse reporting, includes mouse movement */
        change_flags(tty, TTY_REPORTMOUSE_STYLE, tfalse);
        change_flags(tty,
                     TTY_REPORTMOUSE_TWTERM | TTY_REPORTMOUSE_RELEASE | TTY_REPORTMOUSE_DRAG |
                         TTY_REPORTMOUSE_MOVE,
                     on_off);
        tty->Win->ChangeField(TWS_window_Attr, WINDOW_WANT_MOUSE | WINDOW_WANT_MOUSE_MOTION,
                              on_off ? WINDOW_WANT_MOUSE | WINDOW_WANT_MOUSE_MOTION : 0);
        break;
      case 1000: /* xterm mouse reporting, button press/release only */
        change_flags(tty, TTY_REPORTMOUSE_STYLE | TTY_REPORTMOUSE_DRAG | TTY_REPORTMOUSE_MOVE,
                     tfalse);
        change_flags(tty, TTY_REPORTMOUSE_XTERM | TTY_REPORTMOUSE_RELEASE, on_off);
        tty->Win->ChangeField(TWS_window_Attr, WINDOW_WANT_MOUSE, on_off ? WINDOW_WANT_MOUSE : 0);
        break;
      case 1002: /* xterm mouse reporting, includes mouse dragging */
        change_flags(tty, TTY_REPORTMOUSE_STYLE | TTY_REPORTMOUSE_MOVE, tfalse);
        change_flags(tty, TTY_REPORTMOUSE_XTERM | TTY_REPORTMOUSE_RELEASE | TTY_REPORTMOUSE_DRAG,
                     on_off);
        tty->Win->ChangeField(TWS_window_Attr, WINDOW_WANT_MOUSE | WINDOW_WANT_MOUSE_MOTION,
                              on_off ? WINDOW_WANT_MOUSE | WINDOW_WANT_MOUSE_MOTION : 0);
        break;
      case 1003: /* xterm mouse reporting, includes mouse movement */
        change_flags(tty, TTY_REPORTMOUSE_STYLE, tfalse);
        change_flags(tty,
                     TTY_REPORTMOUSE_XTERM | TTY_REPORTMOUSE_RELEASE | TTY_REPORTMOUSE_DRAG |
                         TTY_REPORTMOUSE_MOVE,
                     on_off);
        tty->Win->ChangeField(TWS_window_Attr, WINDOW_WANT_MOUSE | WINDOW_WANT_MOUSE_MOTION,
                              on_off ? WINDOW_WANT_MOUSE | WINDOW_WANT_MOUSE_MOTION : 0);
        break;
      case 1006: /* xterm mouse reporting, toggles decimal mode */
        change_flags(tty, TTY_REPORTMOUSE_DECIMAL, on_off);
        break;
      default:
        break;
      }
    } else {
      switch (par) {

      case 3: /* Monitor (display ctrls) */
        change_flags(tty, TTY_DISPCTRL, on_off);
        break;
      case 4: /* Insert Mode on/off */
        change_flags(tty, TTY_INSERT, on_off);
        break;
      case 20: /* Lf, Enter == CrLf/Lf */
        change_flags(tty, TTY_CRLF, on_off);
        break;
      }
    }
  }
}

static void setterm_command(tty_data *tty) {
  switch (tty->Par[0]) {

  case 1: /* set fg color for underline mode */
    if (tty->Par[1] < tpalette_n) {
      tty->Underline = TCOL(tty->Par[1], 0);
      if (tty->Effects & EFF_UNDERLINE) {
        update_eff(tty);
      }
    }
    break;
  case 2: /* set color for half intensity mode */
    if (tty->Par[1] < tpalette_n) {
      tty->HalfInten = TCOL(tty->Par[1], 0);
      if (tty->Effects & EFF_HALFINTENS) {
        update_eff(tty);
      }
    }
    break;
  case 8: /* store colors - but not effects - as defaults */
    tty->DefColor = tty->Win->ColText;
    break;
  case 9: /* set blanking interval */
    break;
  case 10: /* set bell frequency in Hz */
  case 11: /* set bell duration in msec */
    ConfigureHW(tty->Par[0] == 10 ? HW_BELLPITCH : HW_BELLDURATION, tty->nPar == 0, tty->Par[1]);
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

static inline void insert_line(tty_data *tty, ldat nr) {
  scrolldown(tty, tty->Y, tty->Bottom, nr);
  tty->Flags &= ~TTY_NEEDWRAP;
}

static inline void delete_line(tty_data *tty, ldat nr) {
  scrollup(tty, tty->Y, tty->Bottom, nr);
  tty->Flags &= ~TTY_NEEDWRAP;
}

static inline void csi_at(tty_data *tty, ldat nr) {
  if (nr > (ldat)tty->SizeX - tty->X) {
    nr = (ldat)tty->SizeX - tty->X;
  } else if (!nr) {
    nr = 1;
  }
  insert_char(tty, nr);
}

static inline void csi_L(tty_data *tty, ldat nr) {
  if (nr > (ldat)tty->SizeY - tty->Y) {
    nr = (ldat)tty->SizeY - tty->Y;
  } else if (!nr) {
    nr = 1;
  }
  insert_line(tty, nr);
}

static inline void csi_P(tty_data *tty, ldat nr) {
  if (nr > (ldat)tty->SizeX - tty->X) {
    nr = (ldat)tty->SizeX - tty->X;
  } else if (!nr) {
    nr = 1;
  }
  delete_char(tty, nr);
}

static inline void csi_M(tty_data *tty, ldat nr) {
  if (nr > (ldat)tty->SizeY - tty->Y) {
    nr = (ldat)tty->SizeY - tty->Y;
  } else if (!nr) {
    nr = 1;
  }
  delete_line(tty, nr);
}

static inline void save_current(tty_data *tty) {
  tty->saveX = tty->X;
  tty->saveY = tty->Y;
  tty->saveColor = tty->Win->ColText;
  tty->saveGi = tty->Gi;
  std::memcpy(tty->saveGv, tty->Gv, sizeof(tty->Gv));
}

static inline void restore_current(tty_data *tty) {
  goto_xy(tty, tty->saveX, tty->saveY);
  tty->Win->ColText = tty->saveColor;
  update_eff(tty);
  tty->Flags &= ~TTY_NEEDWRAP;
  std::memcpy(tty->Gv, tty->saveGv, sizeof(tty->Gv));
  set_charset(tty, tty->Gv[tty->Gi = tty->saveGi]);
}

static void reset_tty(tty_data *tty, bool do_clear) {

  tty->State = ESnormal;
  if (tty->Flags & TTY_INVERTSCR) {
    invert_screen(tty);
  }
  tty->Flags = TTY_AUTOWRAP;
  tty->Effects = 0;

  tty->Top = 0;
  tty->Bottom = tty->SizeY;

  Twindow w = tty->Win;
  w->ColText = tty->Color = tty->DefColor = TCOL(twhite, tblack);
  tty->Underline = TCOL(tWHITE, tblack);
  tty->HalfInten = TCOL(tBLACK, tblack);

  w->Flags |= WINDOWFL_CURSOR_ON;
  w->CursorType = LINECURSOR;

  tty->TabStop[0] = 0x01010100;
  tty->TabStop[1] = tty->TabStop[2] = tty->TabStop[3] = tty->TabStop[4] = 0x01010101;

  tty->nPar = 0;

  /* default to latin1 charset */
  set_charset(tty, LATIN1_MAP);
  std::memset(tty->Gv, LATIN1_MAP, sizeof(tty->Gv));
  tty->Gv[1] = VT100GR_MAP;
  tty->Gi = tty->saveGi = 0;

  /* default to UTF-8 mode */
  tty->utf8 = 1;
  tty->utf8_count = tty->utf8_char = 0;

  /*
  bell_pitch = DEFAULT_BELL_PITCH;
  bell_duration = DEFAULT_BELL_DURATION;

  kbd_table.ledmode = LED_SHOW_FLAGS;
  kbd_table.ledflagstate = kbd_table.default_ledflagstate;
  */

  goto_xy(tty, 0, 0);
  save_current(tty);
  if (do_clear) {
    csi_J(tty, 2);
  }
}

static bool grow_newtitle(tty_data *tty) {
  ldat maxlen;
  char *name;
  if (tty->newMax < TW_MAXDAT) {
    maxlen = ((ldat)tty->newMax + (tty->newMax >> 1) + 3) | All->SetUp->MinAllocSize;
    if (maxlen > TW_MAXDAT) {
      maxlen = TW_MAXDAT;
    }
    if ((name = (char *)ReAllocMem(tty->newName, maxlen))) {
      tty->newName = name;
      tty->newMax = maxlen;
      return true;
    }
  }
  return false;
}

static bool insert_newtitle(tty_data *tty, byte c) {
  if (tty->newLen < tty->newMax || grow_newtitle(tty)) {
    tty->newName[tty->newLen++] = c;
    return true;
  }
  return false;
}

static void set_newtitle(tty_data *tty) {
  char *name;
  dat len;

  if (!tty->newName) {
    return;
  }
  /* try to shrink... */
  if (!(name = (char *)ReAllocMem(tty->newName, len = tty->newLen))) {
    name = tty->newName;
  }
  tty->newLen = tty->newMax = 0;
  tty->newName = NULL;

  tty->Win->SetTitle(len, name);
}

static void clear_newtitle(tty_data *tty) {
  if (tty->newName) {
    FreeMem(tty->newName);
  }
  tty->newName = NULL;
  tty->newLen = tty->newMax = 0;
}

static inline void write_ctrl(tty_data *tty, byte c) {
  /*
   *  Control characters can be used in the _middle_
   *  of an escape sequence.
   */
  switch (c) {
  case 0:
    return;
  case 7:
    if (tty->State != ESxterm_ignore && tty->State != ESxterm_title) {
      BeepHW();
      return;
    }
    if (tty->State == ESxterm_title) {
      /* \x07 is an (un)official termination for ESC]0; which starts window title change */
      set_newtitle(tty);
    }
    tty->State = ESnormal;
    return;
  case 8:
    bs(tty);
    return;
  case 9: /* TAB */
    tty->Pos -= tty->X;
    while (tty->X < tty->SizeX - 1) {
      tty->X++;
      if (tty->TabStop[tty->X >> 5] & (1 << (tty->X & 31))) {
        break;
      }
    }
    tty->Pos += tty->X;
    return;
  case 10:
  case 11:
  case 12:
    lf(tty);
    if (!(tty->Flags & TTY_CRLF)) {
      return;
    }
    /* FALLTHROUGH */
  case 13:
    cr(tty);
    return;
  case 14:
    set_charset(tty, tty->Gv[tty->Gi = 1]);
    tty->Flags |= TTY_DISPCTRL;
    return;
  case 15:
    set_charset(tty, tty->Gv[tty->Gi = 0]);
    tty->Flags &= ~TTY_DISPCTRL;
    return;
  case 24:
  case 26:
    tty->State = ESnormal;
    return;
  case 27:
    tty->State = ESesc;
    return;
  case 127:
    del(tty);
    return;
  case 128 + 27:
    tty->State = ESsquare;
    return;
  }

  /* tty->State machine */

  /* mask out ESques for now */
  switch (tty->State & ESany) {

  case ESesc:
    switch (c) {
    case '#':
      tty->State = EShash;
      return;
    case '%':
      tty->State = ESpercent;
      return;
    case '(':
      tty->State = ESsetG0;
      return;
    case ')':
    case '-':
      tty->State = ESsetG1;
      return;
    case '*':
    case '.':
      tty->State = ESsetG2;
      return;
    case '+':
    case '/':
      tty->State = ESsetG3;
      return;

    case '7':
      save_current(tty);
      break;
    case '8':
      restore_current(tty);
      break;

    case '=': /* Appl. keypad */
      tty->Flags |= TTY_KBDAPPLIC | TTY_NEEDREFOCUS;
      break;
    case '>': /* Numeric keypad */
      tty->Flags &= ~TTY_KBDAPPLIC;
      tty->Flags |= TTY_NEEDREFOCUS;
      break;

    case 'E':
      cr(tty);
      lf(tty);
      break;
    case 'M':
      ri(tty);
      break;
    case 'D':
      lf(tty);
      break;
    case 'H':
      tty->TabStop[tty->X >> 5] |= (1 << (tty->X & 31));
      break;
    case 'Z':
      respond_ID(tty);
      break;

    case '[':
      tty->State = ESsquare;
      return;
    case ']':
      tty->State = ESnonstd;
      return;
    case 'c':
      reset_tty(tty, true);
      break;
    case '~':
      set_charset(tty, tty->Gv[tty->Gi = 1]);
      break;
    case 'n':
    case '}':
      set_charset(tty, tty->Gv[tty->Gi = 2]);
      break;
    case 'o':
    case '|':
      set_charset(tty, tty->Gv[tty->Gi = 3]);
      break;

    case '\\': /* ESC\ is the official termination for ESC]0; which starts window title change */
      set_newtitle(tty);
      break;
    }
    break;

  case ESnonstd:
    if (c == 'P') { /* Palette escape sequence */
      tty->nPar = 0;
      memset(tty->Par, 0, NPAR * sizeof(ldat));
      tty->State = ESrgb;
      return;
    } else if (c == 'R') /* Reset palette */
      ResetPaletteHW();
    else if (c == '1') {
      /* may be xterm set window icon title */
      tty->State = ESxterm_ignore_;
      return;
    } else if (c == '0' || c == '2' || c == '7') {
      /* may be xterm "set icon name & window title" or xterm "set window title" or OS tty->X "set
       * current directory title"
       */
      tty->State = ESxterm_title_;
      return;
    }
    break;

  case ESrgb:
    if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) {
      tty->Par[tty->nPar++] = (c > '9' ? (c & 0xDF) - 'A' + 10 : c - '0');
      if (tty->nPar == 7)
        SetPaletteHW(tty->Par[0], tty->Par[1] * 16 + tty->Par[2], tty->Par[3] * 16 + tty->Par[4],
                     tty->Par[5] * 16 + tty->Par[6]);
      else
        return; /* avoid resetting tty->State */
    }
    break;

  case ESsquare:
    tty->Par[0] = tty->nPar = 0;
    /*memset((byte *)&Par, 0, NPAR * sizeof(ldat));*/
    tty->State = ESgetpars;
    if (c == '[') { /* Function key */
      tty->State = ESfunckey;
      return;
    }
    if (c == '?') {
      tty->State = (tty_state)(tty->State | ESques);
      return;
    }
    /* FALLTHROUGH */

  case ESgetpars:
    if ((c == ';' || c == ':') && tty->nPar < NPAR - 1) {
      tty->Par[++tty->nPar] = 0;
      return;
    } else if (c >= '0' && c <= '9') {
      tty->Par[tty->nPar] *= 10;
      tty->Par[tty->nPar] += c - '0';
      return;
    } else {
      tty->State = (tty_state)(ESgotpars | (tty->State & ESques));
    }
    /* FALLTHROUGH */

  case ESgotpars:
    switch (c) {
    case 'h':
      set_mode(tty, 1);
      break;
    case 'l':
      set_mode(tty, 0);
      break;
    case 'c':
      if (tty->State & ESques) {
        if (!tty->Par[0])
          tty->Par[1] = tty->Par[2] = tty->Par[0];
        else if (tty->nPar == 1)
          tty->Par[2] = 0;
        tty->Win->CursorType = tty->Par[0] | (tty->Par[1] << 8) | (tty->Par[2] << 16);
        tty->Flags |= TTY_UPDATECURSOR;
      }
    case 'm':
      /* selection complement mask */
      break;
    case 'n':
      if (!(tty->State & ESques)) {
        if (tty->Par[0] == 5) {
          status_report(tty);
        } else if (tty->Par[0] == 6) {
          cursor_report(tty);
        }
      }
      break;
    }
    if (tty->State & ESques) {
      break;
    }
    switch (c) {
    case 'm':
      csi_m(tty);
      break;
    case 'H':
    case 'f':
      if (tty->Par[0]) {
        tty->Par[0]--;
      }
      if (!tty->nPar) {
        tty->Par[1] = 0;
      } else if (tty->Par[1]) {
        tty->Par[1]--;
      }
      goto_axy(tty, tty->Par[1], tty->Par[0]);
      break;
    case 'J':
      csi_J(tty, tty->Par[0]);
      break;
    case 'K':
      csi_K(tty, tty->Par[0]);
      break;
    case 'G':
    case '`':
      if (tty->Par[0]) {
        tty->Par[0]--;
      }
      goto_xy(tty, tty->Par[0], tty->Y);
      break;
    case 'A':
      if (!tty->Par[0]) {
        tty->Par[0]++;
      }
      goto_xy(tty, tty->X, tty->Y - tty->Par[0]);
      break;
    case 'B':
    case 'e':
      if (!tty->Par[0]) {
        tty->Par[0]++;
      }
      goto_xy(tty, tty->X, tty->Y + tty->Par[0]);
      break;
    case 'C':
    case 'a':
      if (!tty->Par[0]) {
        tty->Par[0]++;
      }
      goto_xy(tty, tty->X + tty->Par[0], tty->Y);
      break;
    case 'D':
      if (!tty->Par[0]) {
        tty->Par[0]++;
      }
      goto_xy(tty, tty->X - tty->Par[0], tty->Y);
      break;
    case 'E':
      if (!tty->Par[0]) {
        tty->Par[0]++;
      }
      goto_xy(tty, 0, tty->Y + tty->Par[0]);
      break;
    case 'F':
      if (!tty->Par[0]) {
        tty->Par[0]++;
      }
      goto_xy(tty, 0, tty->Y - tty->Par[0]);
      break;
    case 'd':
      if (tty->Par[0]) {
        tty->Par[0]--;
      }
      goto_axy(tty, tty->X, tty->Par[0]);
      break;
    case 'L':
      csi_L(tty, tty->Par[0]);
      break;
    case 'M':
      csi_M(tty, tty->Par[0]);
      break;
    case 'P':
      csi_P(tty, tty->Par[0]);
      break;
    case 'c':
      if (!tty->Par[0]) {
        respond_ID(tty);
      }
      break;
    case 'g':
      if (!tty->Par[0]) {
        tty->TabStop[tty->X >> 5] &= ~(1 << (tty->X & 31));
      } else if (tty->Par[0] == 3) {
        tty->TabStop[0] = tty->TabStop[1] = tty->TabStop[2] = tty->TabStop[3] = tty->TabStop[4] = 0;
      }
      break;
    case 'q': /* DECLL - but only 3 leds */
              /* map 0,1,2,3 to 0,1,2,4 */
#if 0
      if (tty->Par[0] < 4)
        setledstate(kbd_table, (tty->Par[0] < 3) ? tty->Par[0] : 4);
#endif
      break;
    case 'r':
      if (!tty->Par[0]) {
        tty->Par[0]++;
      }
      if (!tty->nPar || !tty->Par[1]) {
        tty->Par[1] = tty->SizeY;
      }
      /* Minimum allowed region is 2 lines */
      if (tty->Par[0] < tty->Par[1] && tty->SizeY >= 0 && tty->Par[1] <= (uldat)tty->SizeY) {
        tty->Top = tty->Par[0] - 1;
        tty->Bottom = tty->Par[1];
        goto_axy(tty, 0, 0);
      }
      break;
    case 's':
      save_current(tty);
      break;
    case 'u':
      restore_current(tty);
      break;
    case 'X':
      csi_X(tty, tty->Par[0]);
      break;
    case '@':
      csi_at(tty, tty->Par[0]);
      break;
    case ']': /* setterm functions */
      setterm_command(tty);
      break;
    }
    break;

  case ESpercent:
    switch (c) {
    case '@': /* defined in ISO 2022 */
      tty->utf8 = 0;
      break;
    case 'G': /* official escape code */
    case '8': /* retained for compatibility */
      tty->utf8 = 1;
      break;
    }
    break;
  case ESfunckey:
    break;

  case EShash:
    if (c == '8') {
      /* DEC screen alignment test */
      dirty_tty(tty, 0, 0, tty->SizeX - 1, tty->SizeY - 1);
      fill(tty, tty->Start, TCELL(tty->Win->ColText, 'E'), (ldat)tty->SizeX * tty->SizeY);
    }
    break;

  case ESsetG0:
    switch (c) {
    case '0':
      tty->Gv[0] = VT100GR_MAP;
      break;
    case 'B':
      tty->Gv[0] = LATIN1_MAP;
      break;
    case 'K':
      tty->Gv[0] = USER_MAP;
      break;
    case 'U':
      tty->Gv[0] = IBMPC_MAP;
      break;
    default:
      break;
    }
    if (tty->Gi == 0) {
      set_charset(tty, tty->Gv[0]);
    }
    break;

  case ESsetG1:
    switch (c) {
    case '0':
      tty->Gv[1] = VT100GR_MAP;
      break;
    case 'B':
      tty->Gv[1] = LATIN1_MAP;
      break;
    case 'K':
      tty->Gv[1] = USER_MAP;
      break;
    case 'U':
      tty->Gv[1] = IBMPC_MAP;
      break;
    default:
      break;
    }
    if (tty->Gi == 1) {
      set_charset(tty, tty->Gv[1]);
    }
    break;

  case ESsetG2:
    switch (c) {
    case '0':
      tty->Gv[2] = VT100GR_MAP;
      break;
    case 'B':
      tty->Gv[2] = LATIN1_MAP;
      break;
    case 'K':
      tty->Gv[2] = USER_MAP;
      break;
    case 'U':
      tty->Gv[2] = IBMPC_MAP;
      break;
    default:
      break;
    }
    if (tty->Gi == 2) {
      set_charset(tty, tty->Gv[2]);
    }
    break;

  case ESsetG3:
    switch (c) {
    case '0':
      tty->Gv[3] = VT100GR_MAP;
      break;
    case 'B':
      tty->Gv[3] = LATIN1_MAP;
      break;
    case 'K':
      tty->Gv[3] = USER_MAP;
      break;
    case 'U':
      tty->Gv[3] = IBMPC_MAP;
      break;
    default:
      break;
    }
    if (tty->Gi == 3) {
      set_charset(tty, tty->Gv[3]);
    }
    break;

  case ESxterm_ignore_:
    if (c == ';') {
      tty->State = ESxterm_ignore;
      return;
    }
    break;

  case ESxterm_ignore:
    /* ignore, cannot set icon name */
    return;

  case ESxterm_title_:
    if (c == ';') {
      tty->State = ESxterm_title;
      return;
    }
    break;

  case ESxterm_title:
    if (c >= ' ' && insert_newtitle(tty, c))
      return;
    break;

  default:
    break;
  }

  if (tty->newName && c != 27) {
    clear_newtitle(tty);
  }
  tty->State = ESnormal;
}

Twidget TtyKbdFocus(Twidget newW) {
  udat newFlags;
  Twidget oldW;
  Twidget P;
  Tscreen screen = newW && (P = newW->Parent) && IS_SCREEN(P) ? (Tscreen)P : All->Screens.First;

  if (screen) {
    oldW = screen->FocusW();
    screen->FocusW(newW);
  } else {
    oldW = newW = (Twidget)0;
  }
  if (screen == All->Screens.First) {
    if (!newW || !IS_WINDOW(newW) || !W_USE((Twindow)newW, USECONTENTS) ||
        !((Twindow)newW)->USE.C.TtyData) {
      newFlags = defaultFlags;
    } else {
      newFlags = ((Twindow)newW)->USE.C.TtyData->Flags;
    }

    if ((newFlags ^ kbdFlags) & TTY_KBDAPPLIC) {
      ConfigureHW(HW_KBDAPPLIC, tfalse, newFlags & TTY_KBDAPPLIC);
    }
    if ((newFlags ^ kbdFlags) & TTY_ALTCURSKEYS) {
      ConfigureHW(HW_ALTCURSKEYS, tfalse, newFlags & TTY_ALTCURSKEYS);
    }
    kbdFlags = newFlags;
  }
  return oldW;
}

void ForceKbdFocus(void) {
  kbdFlags = ~defaultFlags;
  (void)TtyKbdFocus(All->Screens.First->FocusW());
}

static tty_data *common(Twindow w) {
  tty_data *tty = w->USE.C.TtyData;
  tty->Win = w; /* in case it's not set yet */
  if (!tty->SizeX || !tty->SizeY) {
    return tty;
  }

  /* scroll YLogic to bottom */
  if (w->YLogic < tty->ScrollBack) {
    if (w == All->Screens.First->Widgets.First) {
      ScrollFirstWindow(0, tty->ScrollBack - w->YLogic, ttrue);
    } else {
      dirty_tty(tty, 0, 0, tty->SizeX - 1, tty->SizeY - 1);
      w->YLogic = tty->ScrollBack;
      DrawBorderWindow(w, BORDER_RIGHT);
    }
  }
  /* clear any selection */
  if (w->State & WINDOW_ANYSEL) {
    ClearHilight(w);
  }
  return tty;
}

/*
 * combine (*pc) with partial utf-8 char stored in tty->utf8_char.
 * return ttrue if the utf-8 char is complete, and can be displayed.
 */
static bool combine_utf8(tty_data *tty, trune *pc) {
  trune c = *pc;

  if (tty->utf8_count > 0 && (c & 0xc0) == 0x80) {
    tty->utf8_char = (tty->utf8_char << 6) | (c & 0x3f);
    tty->utf8_count--;
    if (tty->utf8_count == 0) {
      *pc = tty->utf8_char;
    }
    return tty->utf8_count == 0;
  }

  if ((c & 0xe0) == 0xc0) {
    tty->utf8_count = 1;
    tty->utf8_char = (c & 0x1f);
  } else if ((c & 0xf0) == 0xe0) {
    tty->utf8_count = 2;
    tty->utf8_char = (c & 0x0f);
  } else if ((c & 0xf8) == 0xf0) {
    tty->utf8_count = 3;
    tty->utf8_char = (c & 0x07);
  } else {
    tty->utf8_count = 0;
  }
  return false;
}

/* this is the main entry point */
static bool TtyWriteCharsetOrUtf8(Twindow w, uldat len, const char *chars, bool force_utf8) {
  tty_data *tty;
  trune c;
  bool utf8_in_use, printable, disp_ctrl, state_normal;

  if (!w || !W_USE(w, USECONTENTS) || !w->USE.C.TtyData) {
    return false;
  }
  if (!len || !chars) {
    return true;
  }
  tty = common(w);

  while (!(tty->Flags & TTY_STOPPED) && len) {
    c = (byte)*chars++;
    len--;

    /* If the original code was a control character we only allow a glyph
     * to be displayed if the code is not normally used (such as for cursor movement)
     * or if the TTY_DISPCTRL mode has been explicitly enabled. Certain characters
     * (as given by the CTRL_ALWAYS bitmap) are always displayed as control characters,
     * as the console would be pretty useless without them; to display an arbitrary
     * font position use the direct-to-font zone in UTF-8 mode.
     */
    if ((state_normal = (tty->State == ESnormal))) {
      disp_ctrl = tty->Flags & TTY_DISPCTRL;
      utf8_in_use = tty->utf8 && !disp_ctrl;

      if (force_utf8 || utf8_in_use) {
        if (c & 0x80) {
          if (!combine_utf8(tty, &c)) {
            continue;
          }
        } else {
          tty->utf8_count = 0;
        }
        printable = c >= 32 && c != 127 && c != 128 + 27;

        if (printable && c < 128) {
          // apply G[] charsets also in UTF-8 mode
          c = applyG(tty, (byte)c);
        }

      } else {
        if (tty->Flags & TTY_SETMETA) {
          c |= 0x80;
        }
        printable = (c >= 32 || !(((disp_ctrl ? CTRL_ALWAYS : CTRL_ACTION) >> c) & 1)) &&
                    (c != 127 || disp_ctrl) && (c != 128 + 27);

        if (printable) {
          c = applyG(tty, (byte)c);
        }
      }
    } else {
      utf8_in_use = printable = false;
    }
    if (printable && state_normal) {
      /* Now try to find out how to display it */
      if (tty->Flags & TTY_NEEDWRAP) {
        cr(tty);
        lf(tty);
      }
      if (tty->Flags & TTY_INSERT) {
        insert_char(tty, 1);
      }
      dirty_tty(tty, tty->X, tty->Y, tty->X, tty->Y);
      *tty->Pos = TCELL(tty->Color, c);

      if (tty->X == tty->SizeX - 1) {
        if (tty->Flags & TTY_AUTOWRAP) {
          tty->Flags |= TTY_NEEDWRAP;
        }
      } else {
        tty->X++;
        tty->Pos++;
      }
    } else {
      write_ctrl(tty, (byte)c);
    }
    /* don't flush here, it just decreases performance */
    /* flush_tty(); */
  }
  flush_tty(tty);
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
  tty_data *tty;
  trune c;
  bool ok;

  if (!w || !W_USE(w, USECONTENTS) || !w->USE.C.TtyData) {
    return false;
  }
  if (!len || !runes) {
    return true;
  }
  tty = common(w);

  while (!(tty->Flags & TTY_STOPPED) && len) {
    c = *runes++;
    len--;

    if (c < 0x100) {
      if (tty->Flags & TTY_SETMETA) {
        c |= 0x80;
      }
      /* If the original code < 32, interpret as control character */
      ok = (c >= 32 || !(((tty->Flags & TTY_DISPCTRL ? CTRL_ALWAYS : CTRL_ACTION) >> c) & 1)) &&
           (c != 127 || (tty->Flags & TTY_DISPCTRL)) && (c != 128 + 27);
    } else {
      ok = true;
    }
    if (ok && tty->State == ESnormal) {

      /* Now try to find out how to display it */
      if (tty->Flags & TTY_NEEDWRAP) {
        cr(tty);
        lf(tty);
      }
      if (tty->Flags & TTY_INSERT) {
        insert_char(tty, 1);
      }
      dirty_tty(tty, tty->X, tty->Y, tty->X, tty->Y);
      *tty->Pos = TCELL(tty->Color, c);

      if (tty->X == tty->SizeX - 1) {
        if (tty->Flags & TTY_AUTOWRAP) {
          tty->Flags |= TTY_NEEDWRAP;
        }
      } else {
        tty->X++;
        tty->Pos++;
      }
    } else {
      write_ctrl(tty, (byte)c);
    }
  }
  flush_tty(tty);
  return true;
}

/*
 * this currently wraps at window width so it can write multiple rows at time.
 * does not move cursor position, nor interacts with wrapglitch.
 */
bool TtyWriteTCell(Twindow w, dat x, dat y, uldat len, const tcell *text) {
  if (!w || !W_USE(w, USECONTENTS) || !w->USE.C.TtyData) {
    return false;
  }
  if (!len || !text) {
    return true;
  }
  tty_data *tty = common(w);

  x = Max2(x, 0);
  x = Min2(x, tty->SizeX - 1);
  y = Max2(y, 0);
  y = Min2(y, tty->SizeY - 1);

  uldat maxlen = (uldat)(tty->SizeY - y) * (uldat)tty->SizeX - (uldat)x;
  if (len > maxlen) {
    len = maxlen;
  }
  ldat left = len;
  tcell *dst = tty->Start + y * tty->SizeX + x;

  /* scroll YLogic to bottom */
  if (w->YLogic < tty->ScrollBack) {
    if (w == All->Screens.First->Widgets.First) {
      ScrollFirstWindow(0, tty->ScrollBack - w->YLogic, ttrue);
    } else {
      dirty_tty(tty, 0, 0, tty->SizeX - 1, tty->SizeY - 1);
      w->YLogic = tty->ScrollBack;
      DrawBorderWindow(w, BORDER_RIGHT);
    }
  }
  /* clear any selection */
  if (w->State & WINDOW_ANYSEL) {
    ClearHilight(w);
  }
  ldat chunk;
  do {
    if (dst >= tty->Split) {
      dst -= tty->Split - w->USE.C.Contents;
    }
    ldat max = tty->Split - dst;
    chunk = Min2(left, max);
    for (ldat i = chunk; i; i--) {
      *dst++ = *text++;
    }
  } while ((left -= chunk) > 0);

  if (len > (uldat)(tty->SizeX - x)) {
    dirty_tty(tty, 0, y, tty->SizeX - 1, y + (x + len - 1) / tty->SizeX);
  } else {
    dirty_tty(tty, x, y, x + len - 1, y);
  }
  flush_tty(tty);
  return true;
}
