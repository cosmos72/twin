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

/* ttydata->Flags */
typedef enum e_tty_flag {
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
  TTY_UPDATECURSOR = 0x1000,
  TTY_REPORTMOUSE = 0x2000,
  TTY_REPORTMOUSE2 = 0x4000,
  TTY_NEEDREFOCUS = 0x8000,
} tty_flag;

/* ttydata->Effects */
typedef enum e_tty_effect {
  EFF_INTENSITY = 0x0001,
  EFF_HALFINTENS = 0x0002,
  EFF_UNDERLINE = 0x0004,
  EFF_BLINK = 0x0008,
  EFF_REVERSE = 0x0010,
} tty_effect;

/* ttydata->nPar */
typedef enum e_tty_par {
  NPAR = 16,
} tty_par;

/* ttydata->*G? */
typedef enum e_tty_charmap {
  VT100GR_MAP = 0,
  LATIN1_MAP = 1,
  IBMPC_MAP = 2,
  USER_MAP = 3,
} tty_charmap;

typedef enum e_ttystate {
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
} ttystate;

struct s_ttydata {
  ttystate State;
  udat Flags;
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

byte TtyWriteAscii(window Window, uldat Len, const char *AsciiSeq);
byte TtyWriteString(window Window, uldat Len, const char *string);
byte TtyWriteTRune(window Window, uldat Len, const trune *TRune);
byte TtyWriteTCell(window Window, dat x, dat y, uldat Len, const tcell *Attr);

widget TtyKbdFocus(widget Window);
void ForceKbdFocus(void);

#endif /* TWIN_TTY_H */
