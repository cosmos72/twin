


#ifdef CONF_HW_TTY_TERMCAP

/* this is the usual mess of ad-hoc includes and defines... */

# if defined (__sun__) || defined(__SVR4)

   /*  curses interfaces (emulated) to the termcap library (Solaris7, bwitt) */
#  undef TRUE
#  undef FALSE
#  include <curses.h>
#  include <termio.h>         /* Solaris7: needed "typedef struct sgttyb SGTTY" */
#  include <term.h>

#  ifdef __sun__
   /*  Some one forget a few prototypes!  ugh  (Solaris7, bwitt) */
   extern char * tgoto(), * tgetstr();
#  endif

# else /* rest of the world */

#  ifdef HAVE_TERMCAP_H
#   include <termcap.h>
#  else
#   ifdef HAVE_NCURSES_H
#    include <ncurses.h>
#   else
#    ifdef HAVE_NCURSES_NCURSES_H
#     include <ncurses/ncurses.h>
#    endif
#   endif
#  endif

# endif /* rest of the world */

static void termcap_QuitVideo(void);
static void termcap_ShowMouse(void);
static void termcap_HideMouse(void);
static void termcap_FlushVideo(void);
static void termcap_UpdateMouseAndCursor(void);


static void termcap_Beep(void);
static void termcap_Configure(udat resource, byte todefault, udat value);

static byte termcap_CanDragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp);
static void termcap_DragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp);

INLINE void termcap_SetCursorType(uldat type) {
    fprintf(stdOUT, "%s", (type & 0xFFFFFFl) == NOCURSOR ? tc_cursor_off : tc_cursor_on);
}
INLINE void termcap_MoveToXY(udat x, udat y) {
    fputs(tgoto(tc_cursor_goto, x, y), stdOUT);
}

static byte *termcap_extract(byte *cap, byte **dest) {
    char buf[20], *d = buf, *s = tgetstr(cap, &d);

    if (!s || !*s) {
	return *dest = CloneStr("");
    }
    /*
     * Remove the padding information. We assume that no terminals
     * need padding nowadays. At least it makes things much easier.
     */
    s += strspn(s, "0123456789*");
    
    for (d = buf; *s; *d++ = *s++) {
	if (*s == '$' && *(s + 1) == '<')
	    if (!(s = strchr(s, '>')) || !*++s) break;
    }
    *d = '\0';
    return *dest = CloneStr(buf);
}



static void termcap_cleanup(void) {
    byte ** np, *tc_quit[13];
    
    tc_quit[0] = tc_cursor_goto;
    tc_quit[1] = tc_cursor_on;
    tc_quit[2] = tc_cursor_off;
    tc_quit[3] = tc_bold_on;
    tc_quit[4] = tc_blink_on;
    tc_quit[5] = tc_attr_off;
    tc_quit[6] = tc_kpad_on;
    tc_quit[7] = tc_kpad_off;
    tc_quit[8] = tc_scr_clear;
    tc_quit[9] = tc_audio_bell;
    tc_quit[10] = tc_charset_start;
    tc_quit[11] = tc_charset_end;
    tc_quit[12] = NULL;
    
    for (np = tc_quit; *np; np++)
	if (*np) FreeMem(*np);
}

static void fixup_colorbug(void) {
    uldat len = LenStr(tc_attr_off);
    byte *s = AllocMem( len + 9);
    
    if (s) {
	CopyMem(tc_attr_off, s, len);
	CopyMem("\033[37;40m", s + len, 9);
	FreeMem(tc_attr_off);
	tc_attr_off = s;
    }
}

struct tc_init_node {
    byte *cap, **buf;
};

static byte termcap_InitVideo(void) {
    byte *term = tty_TERM;
    struct tc_init_node *np, tc_init[] = {
	{ "cm",  },
	{ "ve",  },
	{ "vi",  },
	{ "md",  },
	{ "mb",  },
	{ "me",  },
	{ "ks",  },
	{ "ke",  },
	{ "cl",  },
	{ "bl",  },
	{ "as",  },
	{ "as",  },
	{ NULL,  }
    };
    char tcbuf[2048];		/* by convention, this is enough */

    if (!term) {
	printk("      termcap_InitVideo() failed: unknown terminal type.\n");
	return FALSE;
    }
    
    tc_init[0].buf = &tc_cursor_goto;
    tc_init[1].buf = &tc_cursor_on;
    tc_init[2].buf = &tc_cursor_off;
    tc_init[3].buf = &tc_bold_on;
    tc_init[4].buf = &tc_blink_on;
    tc_init[5].buf = &tc_attr_off;
    tc_init[6].buf = &tc_kpad_on;
    tc_init[7].buf = &tc_kpad_off;
    tc_init[8].buf = &tc_scr_clear;
    tc_init[9].buf = &tc_audio_bell;
    if (tty_charset_to_UTF_16 != Tutf_CP437_to_UTF_16) {
	tc_init[10].cap = NULL;
	tc_charset_start = tc_charset_end = NULL;
    } else {
	tc_init[10].buf = &tc_charset_start;
	tc_init[11].buf = &tc_charset_end;
    }
    
    switch (tgetent(tcbuf, term)) {
      case 1:
	break;
      case 0:
	printk("      termcap_InitVideo() failed: no entry for `%."STR(SMALLBUFF)"s' in the terminal database.\n"
	       "      Please set your $TERM environment variable correctly.\n", term);
	return FALSE;
      default:
	printk("      termcap_InitVideo() failed: system call error in tgetent(): %."STR(SMALLBUFF)"s\n",
		strerror(errno));
	return FALSE;
    }
    
    for (np = tc_init; np->cap; np++)
	*np->buf = NULL;

    for (np = tc_init; np->cap; np++) {
	if (!termcap_extract(np->cap, np->buf)) {
	    printk("      termcap_InitVideo() failed: Out of memory!\n");
	    termcap_cleanup();
	    return FALSE;
	}
    }
    
    if (!*tc_cursor_goto) {
	printk("      termcap_InitVideo() failed: terminal misses `cursor goto' capability\n");
	termcap_cleanup();
	return FALSE;
    }


# ifdef CONF__UNICODE
    if (tty_can_utf8 == TRUE+TRUE)
	/* cannot autodetect an utf8-capable terminal, assume it cannot do utf8 */
	tty_can_utf8 = FALSE;
# endif

    wrapglitch = tgetflag("xn");
    if (colorbug)
	fixup_colorbug();

    fprintf(stdOUT, "%s%s%s", tc_attr_off, tc_scr_clear, tc_charset_start ? tc_charset_start : (byte *)"");
    
    HW->FlushVideo = termcap_FlushVideo;
    HW->FlushHW = stdout_FlushHW;

    HW->ShowMouse = termcap_ShowMouse;
    HW->HideMouse = termcap_HideMouse;
    HW->UpdateMouseAndCursor = termcap_UpdateMouseAndCursor;
    
    HW->DetectSize  = stdin_DetectSize;
    HW->CheckResize = stdin_CheckResize;
    HW->Resize      = stdin_Resize;
    
    HW->HWSelectionImport  = AlwaysFalse;
    HW->HWSelectionExport  = NoOp;
    HW->HWSelectionRequest = (void *)NoOp;
    HW->HWSelectionNotify  = (void *)NoOp;
    HW->HWSelectionPrivate = 0;

    HW->CanDragArea = termcap_CanDragArea;
    HW->DragArea = termcap_DragArea;
   
    HW->XY[0] = HW->XY[1] = -1;
    HW->TT = -1; /* force updating the cursor */
	
    HW->Beep = termcap_Beep;
    HW->Configure = termcap_Configure;
    HW->SetPalette = (void *)NoOp;
    HW->ResetPalette = (void *)NoOp;

    HW->QuitVideo = termcap_QuitVideo;
    
    HW->FlagsHW |= FlHWNeedOldVideo;
    HW->FlagsHW &= ~FlHWExpensiveFlushVideo;
    HW->NeedHW = 0;
    HW->merge_Threshold = 0;
    
    return TRUE;
}

static void termcap_QuitVideo(void) {

    termcap_MoveToXY(0, DisplayHeight-1);
    termcap_SetCursorType(LINECURSOR);
    /* reset colors and charset */
    fprintf(stdOUT, "%s%s", tc_attr_off, tc_charset_end ? tc_charset_end : (byte *)"");
    
    /* restore original alt cursor keys, keypad settings */
    HW->Configure(HW_KBDAPPLIC, TRUE, 0);
    HW->Configure(HW_ALTCURSKEYS, TRUE, 0);

    termcap_cleanup();
    
    HW->QuitVideo = NoOp;
}


#define termcap_MogrifyInit() fputs(tc_attr_off, stdOUT); _col = COL(WHITE,BLACK)
#define termcap_MogrifyFinish() do { if (utf8used) utf8used = FALSE, fputs("\033%@", stdOUT); } while (0)

INLINE byte *termcap_CopyAttr(byte *attr, byte *dest) {
    while ((*dest++ = *attr++))
	;
    return --dest;
}
	
INLINE void termcap_SetColor(hwcol col) {
    static byte colbuf[80];
    byte c, *colp = colbuf;
    
    if ((col & COL(HIGH,HIGH)) != (_col & COL(HIGH,HIGH))) {
	
	if (((_col & COL(0,HIGH)) && !(col & COL(0,HIGH))) ||
	    ((_col & COL(HIGH,0)) && !(col & COL(HIGH,0)))) {
	    
	    /* cannot turn off blinking or standout, reset everything */
	    colp = termcap_CopyAttr(tc_attr_off, colp);
	    _col = COL(WHITE,BLACK);
	}
	if ((col & COL(HIGH,0)) && !(_col & COL(HIGH,0)))
	    colp = termcap_CopyAttr(tc_bold_on, colp);
	if ((col & COL(0,HIGH)) && !(_col & COL(0,HIGH)))
	    colp = termcap_CopyAttr(tc_blink_on, colp);
    }
        

    if ((col & COL(WHITE,WHITE)) != (_col & COL(WHITE,WHITE))) {
	*colp++ = '\033'; *colp++ = '[';

	if ((col & COL(WHITE,0)) != (_col & COL(WHITE,0))) {
	    *colp++ = '3';
	    c = COLFG(col) & ~HIGH;
	    *colp++ = VGA2ANSI(c) + '0';
	    *colp++ = ';';
	}

	if ((col & COL(0,WHITE)) != (_col & COL(0,WHITE))) {
	    *colp++ = '4';
	    c = COLBG(col) & ~HIGH;
	    *colp++ = VGA2ANSI(c) + '0';
	    *colp++ = 'm';
	} else if (colp[-1] == ';')
	    colp[-1] = 'm';
    }
	
    *colp = '\0';
    _col = col;
    
    fputs(colbuf, stdOUT);
}


INLINE void termcap_Mogrify(dat x, dat y, uldat len) {
    uldat delta = x + (uldat)y * DisplayWidth;
    hwattr *V, *oV;
    hwcol col;
    hwfont c, _c;
    byte sending = FALSE;
    
    if (!wrapglitch && delta + len >= (uldat)DisplayWidth * DisplayHeight)
	len = (uldat)DisplayWidth * DisplayHeight - delta - 1;
    
    V = Video + delta;
    oV = OldVideo + delta;
	
    for (; len; V++, oV++, x++, len--) {
	if (!ValidOldVideo || *V != *oV) {
	    if (!sending)
		sending = TRUE, termcap_MoveToXY(x,y);

	    col = HWCOL(*V);
	    
	    if (col != _col)
		termcap_SetColor(col);
	
	    c = _c = HWFONT(*V);
#ifdef CONF__UNICODE
	    c = tty_UTF_16_to_charset(_c);
	    if (tty_can_utf8 && (tty_charset_to_UTF_16[c] != _c || (utf8used && c > 0x80))) {
		/*
		 * translation to charset did not find an exact match,
		 * use utf-8 to output this char.
		 * also use utf-8 if we already output ESC % G and we must putc(c > 0x80),
		 * which would be interpreted as part of an utf-8 sequence.
		 */
		tty_MogrifyUTF8(_c);
		continue;
	    }
#endif
	    if (c < 32 || c == 127 || c == 128+27) {
		/* can't display it */
#ifdef CONF__UNICODE
		c = T_CAT(Tutf_CP437_to_,T_MAP(US_ASCII))[c];
#else
		c = ' ';
#endif
	    }
	    putc((char)c, stdOUT);
	} else
	    sending = FALSE;
    }
}

INLINE void termcap_SingleMogrify(dat x, dat y, hwattr V) {
    hwfont c, _c;
    
    if (!wrapglitch && x == DisplayWidth-1 && y == DisplayHeight-1)
	return;
    
    termcap_MoveToXY(x,y);

    if (HWCOL(V) != _col)
	termcap_SetColor(HWCOL(V));
	
    c = _c = HWFONT(V);
#ifdef CONF__UNICODE
    c = tty_UTF_16_to_charset(c);
    if (tty_can_utf8 && (tty_charset_to_UTF_16[c] != _c || (utf8used && c > 0x80))) {
	/*
	 * translation to charset did not find an exact match,
	 * use utf-8 to output this char
	 * also use utf-8 if we already output ESC % G and we must putc(c > 0x80),
	 * which would be interpreted as part of an utf-8 sequence.
	 */
	tty_MogrifyUTF8(_c);
	return;
    }
#endif
    if (c < 32 || c == 127 || c == 128+27) {
	/* can't display it */
#ifdef CONF__UNICODE
	c = T_CAT(Tutf_CP437_to_,T_MAP(US_ASCII))[c];
#else
	c = ' ';
#endif
    }
    putc((char)c, stdOUT);
}

/* HideMouse and ShowMouse depend on Video setup, not on Mouse.
 * so we have vcsa_ and termcap_ versions, not GPM_ ones... */
static void termcap_ShowMouse(void) {
    uldat pos = (HW->Last_x = HW->MouseState.x) + (HW->Last_y = HW->MouseState.y) * DisplayWidth;
    hwattr h  = Video[pos], 
	c = HWATTR_COLMASK(~h) ^ HWATTR(COL(HIGH,HIGH),0);

    termcap_SingleMogrify(HW->MouseState.x, HW->MouseState.y, c | HWATTR_FONTMASK(h));

    /* force updating the cursor */
    HW->XY[0] = HW->XY[1] = -1;
    setFlush();
}

static void termcap_HideMouse(void) {
    uldat pos = HW->Last_x + HW->Last_y * DisplayWidth;

    termcap_SingleMogrify(HW->Last_x, HW->Last_y, Video[pos]);

    /* force updating the cursor */
    HW->XY[0] = HW->XY[1] = -1;
    setFlush();
}

static void termcap_Beep(void) {
    fputs(tc_audio_bell, stdOUT);
    setFlush();
}

static void termcap_UpdateCursor(void) {
    if (!ValidOldVideo || (CursorType != NOCURSOR && (CursorX != HW->XY[0] || CursorY != HW->XY[1]))) {
	termcap_MoveToXY(HW->XY[0] = CursorX, HW->XY[1] = CursorY);
	setFlush();
    }
    if (!ValidOldVideo || CursorType != HW->TT) {
	termcap_SetCursorType(HW->TT = CursorType);
	setFlush();
    }
}

static void termcap_UpdateMouseAndCursor(void) {
    if ((HW->FlagsHW & FlHWSoftMouse) && (HW->FlagsHW & FlHWChangedMouseFlag)) {
	HW->HideMouse();
	HW->ShowMouse();
	HW->FlagsHW &= ~FlHWChangedMouseFlag;
    }

    termcap_UpdateCursor();
}

static void termcap_Configure(udat resource, byte todefault, udat value) {
    switch (resource) {
      case HW_KBDAPPLIC:
	fputs(todefault || !value ? tc_kpad_on : tc_kpad_off, stdOUT);
	setFlush();
	break;
      case HW_ALTCURSKEYS:
	/*
	 fputs(todefault || !value ? "\033[?1l" : "\033[?1h", stdOUT);
	 setFlush();
	 */
	break;
      case HW_BELLPITCH:
      case HW_BELLDURATION:
	break;
      case HW_MOUSEMOTIONEVENTS:
	if (todefault)
	    value = 0;
	if (HW->MouseEvent == xterm_MouseEvent)
	    xterm_EnableMouseMotionEvents(value);
#ifdef CONF_HW_TTY_LINUX
	else if (HW->MouseEvent == GPM_MouseEvent)
	    GPM_EnableMouseMotionEvents(value);
#endif
	break;
      default:
	break;
    }
}

static byte termcap_CanDragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    return Left == 0 && Rgt == HW->X-1 && Dwn == HW->Y-1 && DstUp == 0;
}

static void termcap_DragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    udat delta = Up - DstUp;
	
    HW->HideMouse();
    HW->FlagsHW |= FlHWChangedMouseFlag;
	
    fprintf(stdOUT, "%s\033[0m%s",			/* hide cursor, reset color */
	    tc_cursor_off, tgoto(tc_cursor_goto, 0, HW->Y-1));/* go to last line */
	    
    while (delta--)
	putc('\n', stdOUT);
    
    setFlush();
	
    /* force updating the cursor */
    HW->XY[0] = HW->XY[1] = -1;
    
    /*
     * now the last trick: tty scroll erased the part
     * below DstUp + (Dwn - Up) so we must redraw it.
     */
    NeedRedrawVideo(0, DstUp + (Dwn - Up) + 1, HW->X - 1, HW->Y - 1);
}

static void termcap_FlushVideo(void) {
    dat i, j;
    dat start, end;
    byte FlippedVideo = FALSE, FlippedOldVideo = FALSE;
    hwattr savedOldVideo;
    
    if (!ChangedVideoFlag) {
	HW->UpdateMouseAndCursor();
	return;
    }

    /* hide the mouse if needed */
    
    /* first, check the old mouse position */
    if (HW->FlagsHW & FlHWSoftMouse) {
	if (HW->FlagsHW & FlHWChangedMouseFlag) {
	    /* dirty the old mouse position, so that it will be overwritten */
	    
	    /*
	     * with multi-display this is a hack, but since OldVideo gets restored
	     * below *BEFORE* returning from termcap_FlushVideo(), that's ok.
	     */
	    DirtyVideo(HW->Last_x, HW->Last_y, HW->Last_x, HW->Last_y);
	    if (ValidOldVideo) {
		FlippedOldVideo = TRUE;
		savedOldVideo = OldVideo[HW->Last_x + HW->Last_y * DisplayWidth];
		OldVideo[HW->Last_x + HW->Last_y * DisplayWidth] = ~Video[HW->Last_x + HW->Last_y * DisplayWidth];
	    }
	}
	
        i = HW->MouseState.x;
	j = HW->MouseState.y;
	/*
	 * instead of calling ShowMouse(),
	 * we flip the new mouse position in Video[] and dirty it if necessary.
	 */
	if ((HW->FlagsHW & FlHWChangedMouseFlag) || (FlippedVideo = Plain_isDirtyVideo(i, j))) {
	    VideoFlip(i, j);
	    if (!FlippedVideo)
		DirtyVideo(i, j, i, j);
	    HW->FlagsHW &= ~FlHWChangedMouseFlag;
	    FlippedVideo = TRUE;
	} else
	    FlippedVideo = FALSE;
    }

    termcap_MogrifyInit();
    if (HW->TT != NOCURSOR)
	termcap_SetCursorType(HW->TT = NOCURSOR);
    for (i=0; i<DisplayHeight*2; i++) {
	start = ChangedVideo[i>>1][i&1][0];
	end   = ChangedVideo[i>>1][i&1][1];
	
	if (start != -1)
	    termcap_Mogrify(start, i>>1, end-start+1);
    }
    
    /* force updating the cursor */
    HW->XY[0] = HW->XY[1] = -1;
    
    setFlush();
    
    /* ... and this redraws the mouse */
    if (HW->FlagsHW & FlHWSoftMouse) {
	if (FlippedOldVideo)
	    OldVideo[HW->Last_x + HW->Last_y * DisplayWidth] = savedOldVideo;
	if (FlippedVideo)
	    VideoFlip(HW->Last_x = HW->MouseState.x, HW->Last_y = HW->MouseState.y);
	else if (HW->FlagsHW & FlHWChangedMouseFlag)
	    HW->ShowMouse();
    }
    
    termcap_UpdateCursor();
    
    HW->FlagsHW &= ~FlHWChangedMouseFlag;
}

#endif /* CONF_HW_TTY_TERMCAP */

