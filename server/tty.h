/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_TTY_H
#define TWIN_TTY_H

#include <Tw/datatypes.h>

/* tty_data->Flags */
enum tty_flag /*: uldat*/ {
  TTY_STOPPED = 0x0001,
  TTY_AUTOWRAP = 0x0002,
  TTY_NEEDWRAP = 0x0004,
  TTY_DISPCTRL = 0x0008,
  TTY_CRLF = 0x0010,
  TTY_KBDAPPLIC = 0x0020,
  TTY_INSERT = 0x0040,
  TTY_REVERSECOL = 0x0080,
  TTY_INVERTSCR = 0x0100,
  TTY_ALTCURSKEYS = 0x0200,
  TTY_RELORIG = 0x0400,
  TTY_SETMETA = 0x0800,

  TTY_REPORTMOUSE_STYLE = 0x3000,
  TTY_REPORTMOUSE_X10 = 0x1000,
  TTY_REPORTMOUSE_XTERM = 0x2000,
  TTY_REPORTMOUSE_TWTERM = 0x3000,

  TTY_REPORTMOUSE_RELEASE = 0x4000,
  TTY_REPORTMOUSE_DRAG = 0x8000,
  TTY_REPORTMOUSE_MOVE = 0x10000,
  TTY_REPORTMOUSE_DECIMAL = 0x20000,

  TTY_UPDATECURSOR = 0x40000,
  TTY_NEEDREFOCUS = 0x80000,
};

/* tty_data->Effects */
enum tty_effect /*: udat*/ {
  EFF_INTENSITY = 0x0001,
  EFF_HALFINTENS = 0x0002,
  EFF_UNDERLINE = 0x0004,
  EFF_BLINK = 0x0008,
  EFF_REVERSE = 0x0010,
};

/* tty_data->nPar */
enum tty_par /*: byte*/ {
  NPAR = 16,
};

/* tty_data->*G? */
enum tty_charmap /*: byte*/ {
  VT100GR_MAP = 0,
  LATIN1_MAP = 1,
  IBMPC_MAP = 2,
  USER_MAP = 3,
};

enum tty_state /*: udat*/ {
  ESnormal = 0,
  ESesc,
  ESsquare,
  ESgetpars,
  ESgotpars,
  ESfunckey,
  EShash,
  ESsetG0,
  ESsetG1,
  ESpercent,
  ESignore,
  ESnonstd,
  ESrgb,
  ESxterm_ignore_,
  ESxterm_ignore,
  ESxterm_title_,
  ESxterm_title,
  ESany = 0xFF,
  ESques = 0x100
};

class tty_data {
public:
  Twindow Win;
  tty_state State;
  uldat Flags;
  udat Effects;
  dat ScrollBack;   /* Number of scrollback lines */
  dat SizeX, SizeY; /* Terminal size */
  dat Top, Bottom;  /* Y scrolling region. default 0...SizeY-1 */
  dat X, Y;         /* Cursor position in visible buffer */
  dat saveX, saveY;
  tcell *Start, *Split; /* Start and Split of visible buffer */
                        /* AfterSplit is just Window->Contents */
  tcell *Pos;           /* Pointer to cursor position in buffer */
  tcolor Color, DefColor, saveColor, Underline, HalfInten;
  uldat TabStop[5];
  uldat nPar, Par[NPAR];

  byte currG, G, G0, G1, saveG, saveG0, saveG1;
  byte utf8, utf8_count;
  trune utf8_char;
  trune (*InvCharset)(trune); /* pointer to trune -> byte translation function */

  dat newLen, newMax;
  char *newName; /* buffer for xterm set window title escape seq */
};

bool TtyWriteCharset(Twindow Window, uldat Len, const char *charset_bytes);
bool TtyWriteUtf8(Twindow Window, uldat Len, const char *utf8_bytes);
bool TtyWriteTRune(Twindow Window, uldat Len, const trune *runes);
bool TtyWriteTCell(Twindow Window, dat x, dat y, uldat Len, const tcell *cells);

Twidget TtyKbdFocus(Twidget Window);
void ForceKbdFocus(void);

#endif /* TWIN_TTY_H */
