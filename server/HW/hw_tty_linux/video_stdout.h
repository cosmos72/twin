/*
 * As alternate method, we also provide
 * output through stdout.
 * 
 * it is slower (?), but in these days it shouldn't be a problem.
 * 
 * this is used both to run inside a twin terminal,
 * and as fallback (if vcsa_InitVideo() fails) to run on a Linux console.
 */

static void linux_QuitVideo(void);
static void linux_ShowMouse(void);
static void linux_HideMouse(void);
static void linux_FlushVideo(void);

static void linux_Beep(void);
static void linux_Configure(udat resource, byte todefault, udat value);
static void linux_ConfigureKeyboard(udat resource, byte todefault, udat value);
static void linux_SetPalette(udat N, udat R, udat G, udat B);
static void linux_ResetPalette(void);
static void linux_UpdateMouseAndCursor(void);

static byte linux_CanDragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp);
static void linux_DragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp);

INLINE void linux_SetCursorType(uldat type) {
    fprintf(stdOUT, "\033[?%d;%d;%dc",
		(int)(type & 0xFF),
		(int)((type >> 8) & 0xFF),
		(int)((type >> 16) & 0xFF));
}
INLINE void linux_MoveToXY(udat x, udat y) {
    fprintf(stdOUT, "\033[%d;%dH", y+1, x+1);
}


/* return FALSE if failed */
static byte linux_InitVideo(void) {
    byte *term = tty_TERM;
    
    if (!term) {
	printk("      linux_InitVideo() failed: unknown terminal type.\n");
	return FALSE;
    }
    
    if (strcmp(term, "linux")) {
	printk("      linux_InitVideo() failed: terminal `%."STR(SMALLBUFF)"s' is not `linux'.\n", term);
	return FALSE;
    }

#ifdef CONF__UNICODE
    if (tty_can_utf8 == TRUE+TRUE && ttypar[0]==6 && ttypar[1]!=3 && ttypar[1]!=4)
	/* plain linux console supports utf-8, and also twin >= 0.3.11 does. */
	tty_can_utf8 = TRUE;
    else
	tty_can_utf8 = FALSE;
#endif

    tc_scr_clear = "\033[2J";
    fprintf(stdOUT, "\033[0;11m\033%%@%s\033[3h", tc_scr_clear);
    /* clear colors, temporary escape to IBMPC consolemap, disable UTF-8 mode, clear screen, set TTY_DISPCTRL */
    
    HW->FlushVideo = linux_FlushVideo;
    HW->FlushHW = stdout_FlushHW;

    HW->ShowMouse = linux_ShowMouse;
    HW->HideMouse = linux_HideMouse;
    HW->UpdateMouseAndCursor = linux_UpdateMouseAndCursor;
    
    HW->DetectSize  = stdin_DetectSize;
    HW->CheckResize = stdin_CheckResize;
    HW->Resize      = stdin_Resize;
    
    HW->HWSelectionImport  = AlwaysFalse;
    HW->HWSelectionExport  = NoOp;
    HW->HWSelectionRequest = (void *)NoOp;
    HW->HWSelectionNotify  = (void *)NoOp;
    HW->HWSelectionPrivate = 0;

    HW->CanDragArea = linux_CanDragArea;
    HW->DragArea = linux_DragArea;
   
    HW->XY[0] = HW->XY[1] = -1;
    HW->TT = (uldat)-1; /* force updating the cursor */
	
    HW->Beep = linux_Beep;
    HW->Configure = linux_Configure;
    HW->ConfigureKeyboard = linux_ConfigureKeyboard;
    HW->SetPalette = linux_SetPalette;
    HW->ResetPalette = linux_ResetPalette;

    HW->QuitVideo = linux_QuitVideo;
    
    HW->FlagsHW |= FlHWNeedOldVideo;
    HW->FlagsHW &= ~FlHWExpensiveFlushVideo;
    HW->NeedHW = 0;
    HW->merge_Threshold = 0;

    return TRUE;
}

static void linux_QuitVideo(void) {
    linux_MoveToXY(0, DisplayHeight-1);
    linux_SetCursorType(LINECURSOR);
    /* restore original colors, consolemap, TTY_DISPCTRL, alt cursor keys, keypad settings */
    fputs("\033[0;10m\033[?1l\033>\n", stdOUT);
   
    HW->QuitVideo = NoOp;
}


#define CTRL_ALWAYS 0x0800f501	/* Cannot be overridden by TTY_DISPCTRL */

#define linux_MogrifyInit() fputs("\033[m", stdOUT); _col = COL(WHITE,BLACK);

#ifdef CONF__UNICODE
# define linux_MogrifyFinish() do { if (utf8used) utf8used = FALSE, fputs("\033%@", stdOUT); } while (0)
#else
# define linux_MogrifyFinish() do { } while (0)
#endif

INLINE void linux_SetColor(hwcol col) {
    static byte colbuf[] = "\033[2x;2x;4x;3xm";
    byte c, *colp = colbuf+2;
    
    if ((col & COL(HIGH,0)) != (_col & COL(HIGH,0))) {
	if (_col & COL(HIGH,0)) *colp++ = '2';
	*colp++ = '1'; *colp++ = ';';
    }
    if ((col & COL(0,HIGH)) != (_col & COL(0,HIGH))) {
	if (_col & COL(0,HIGH)) *colp++ = '2';
	*colp++ = '5'; *colp++ = ';';
    }
    if ((col & COL(0,WHITE)) != (_col & COL(0,WHITE))) {
	c = COLBG(col) & ~HIGH;
	*colp++ = '4'; *colp++ = VGA2ANSI(c) + '0'; *colp++ = ';';
    }
    if ((col & COL(WHITE,0)) != (_col & COL(WHITE,0))) {
	c = COLFG(col) & ~HIGH;
	*colp++ = '3'; *colp++ = VGA2ANSI(c) + '0'; *colp++ = ';';
    }
    _col = col;
    
    if (colp[-1] == ';') --colp;
    *colp++ = 'm'; *colp   = '\0';
    
    fputs(colbuf, stdOUT);
}


INLINE void linux_Mogrify(dat x, dat y, uldat len) {
    hwattr *V, *oV;
    hwcol col;
    hwfont c, _c;
    byte sending = FALSE;
    
    V = Video + x + y * DisplayWidth;
    oV = OldVideo + x + y * DisplayWidth;
	
    for (; len; V++, oV++, x++, len--) {
	if (!ValidOldVideo || *V != *oV) {
	    if (!sending)
		sending = TRUE, linux_MoveToXY(x,y);

	    col = HWCOL(*V);
	    
	    if (col != _col)
		linux_SetColor(col);
	
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
	    
	    if ((c < 32 && ((CTRL_ALWAYS >> c) & 1)) || c == 128+27) {
		/* can't display it */
#ifdef CONF__UNICODE
		c = T_CAT(Tutf_CP437_to_,T_MAP(US_ASCII)) [ Tutf_UTF_16_to_CP437(_c) ];
#else
		c = ' ';
#endif
	    }
	    putc((char)c, stdOUT);
	} else
	    sending = FALSE;
    }
}

INLINE void linux_SingleMogrify(dat x, dat y, hwattr V) {
    hwfont c, _c;
    
    linux_MoveToXY(x,y);

    if (HWCOL(V) != _col)
	linux_SetColor(HWCOL(V));
	
    c = _c = HWFONT(V);
#ifdef CONF__UNICODE
    c = tty_UTF_16_to_charset(_c);
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

    if ((c < 32 && ((CTRL_ALWAYS >> c) & 1)) || c == 128+27) {
	/* can't display it */
#ifdef CONF__UNICODE
	c = T_CAT(Tutf_CP437_to_,T_MAP(US_ASCII)) [ Tutf_UTF_16_to_CP437(_c) ];
#else
	c = ' ';
#endif
    }
    putc((char)c, stdOUT);
}

/* HideMouse and ShowMouse depend on Video setup, not on Mouse.
 * so we have vcsa_ and linux_ versions, not GPM_ ones... */
static void linux_ShowMouse(void) {
    uldat pos = (HW->Last_x = HW->MouseState.x) + (HW->Last_y = HW->MouseState.y) * DisplayWidth;
    hwattr h  = Video[pos];
    hwcol c = ~HWCOL(h) ^ COL(HIGH,HIGH);
    
    linux_SingleMogrify(HW->MouseState.x, HW->MouseState.y, HWATTR( c, HWFONT(h)));

    /* store current cursor state for correct updating */
    HW->XY[1] = HW->MouseState.y;
    HW->XY[0] = HW->MouseState.x + 1;
    /* linux terminals have VT100 wrapglitch */
    if (HW->XY[0] == HW->X) HW->XY[0]--;
    setFlush();
}

static void linux_HideMouse(void) {
    uldat pos = HW->Last_x + HW->Last_y * DisplayWidth;

    linux_SingleMogrify(HW->Last_x, HW->Last_y, Video[pos]);

    /* store current cursor state for correct updating */
    HW->XY[1] = HW->Last_y;
    HW->XY[0] = HW->Last_x + 1;
    /* linux terminals have VT100 wrapglitch */
    if (HW->XY[0] == HW->X) HW->XY[0]--;
    setFlush();
}

static void linux_UpdateCursor(void) {
    if (!ValidOldVideo || (CursorType != NOCURSOR && (CursorX != HW->XY[0] || CursorY != HW->XY[1]))) {
	linux_MoveToXY(HW->XY[0] = CursorX, HW->XY[1] = CursorY);
	setFlush();
    }
    if (!ValidOldVideo || CursorType != HW->TT) {
	linux_SetCursorType(HW->TT = CursorType);
	setFlush();
    }
}

static void linux_UpdateMouseAndCursor(void) {
    if ((HW->FlagsHW & FlHWSoftMouse) && (HW->FlagsHW & FlHWChangedMouseFlag)) {
	HW->HideMouse();
	HW->ShowMouse();
	HW->FlagsHW &= ~FlHWChangedMouseFlag;
    }

    linux_UpdateCursor();
}

static void linux_FlushVideo(void) {
    dat i, j, start, end, XY[2];
    byte FlippedVideo = FALSE, FlippedOldVideo = FALSE;
    hwattr savedOldVideo;
    
    if (!ChangedVideoFlag) {
	HW->UpdateMouseAndCursor();
	linux_MogrifyFinish();
	return;
    }

    /* hide the mouse if needed */
    
    /* first, check the old mouse position */
    if (HW->FlagsHW & FlHWSoftMouse) {
	if (HW->FlagsHW & FlHWChangedMouseFlag) {
	    /* dirty the old mouse position, so that it will be overwritten */
	    
	    /*
	     * with multi-display this is a hack, but since OldVideo gets restored
	     * below *BEFORE* returning from linux_FlushVideo(), that's ok.
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

    linux_MogrifyInit();
    for (i=0; i<DisplayHeight*2; i++) {
	start = ChangedVideo[i>>1][i&1][0];
	end   = ChangedVideo[i>>1][i&1][1];
	
	if (start != -1)
	    /* also keep track of cursor position */
	    linux_Mogrify(start, XY[1] = i>>1, (XY[0] = end)-start+1);
    }

    /* store current cursor state for correct updating */
    HW->XY[1] = XY[1];
    if ((HW->XY[0] = XY[0] + 1) == HW->X)
	/* linux terminals have VT100 wrapglitch */
	HW->XY[0]--;
	

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
    linux_UpdateCursor();
    
    linux_MogrifyFinish();
    
    HW->FlagsHW &= ~FlHWChangedMouseFlag;
}

static void linux_Beep(void) {
    fputs("\033[3l\007\033[3h", stdOUT);
    setFlush();
}

static void linux_ConfigureKeyboard(udat resource, byte todefault, udat value) {
    switch (resource) {
      case HW_KBDAPPLIC:
	fputs(todefault || !value ? "\033>" : "\033=", stdOUT);
	setFlush();
	break;
      case HW_ALTCURSKEYS:
	fputs(todefault || !value ? "\033[?1l" : "\033[?1h", stdOUT);
	setFlush();
	break;
    }
}

static void linux_Configure(udat resource, byte todefault, udat value) {
    switch (resource) {
      case HW_KBDAPPLIC:
      case HW_ALTCURSKEYS:
	HW->ConfigureKeyboard(resource, todefault, value);
	break;
      case HW_BELLPITCH:
	if (todefault)
	    fputs("\033[10]", stdOUT);
	else
	    fprintf(stdOUT, "\033[10;%hd]", value);
	setFlush();
	break;
      case HW_BELLDURATION:
	if (todefault)
	    fputs("\033[11]", stdOUT);
	else
	    fprintf(stdOUT, "\033[11;%hd]", value);
	setFlush();
	break;
      case HW_MOUSEMOTIONEVENTS:
	HW->ConfigureMouse(resource, todefault, value);
	break;
      default:
	break;
    }
}

static void linux_SetPalette(udat N, udat R, udat G, udat B) {
    fprintf(stdOUT, "\033]P%1hx%02hx%02hx%02hx", N, R, G, B);
    setFlush();
}

static void linux_ResetPalette(void) {
    fputs("\033]R", stdOUT);
    setFlush();
}

static byte linux_CanDragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    /*
     * tty scrolling capabilities are very limited...
     * do not even consider using tty `ESC [ <n> M' (delete_line)
     * or `ESC [ <n> L' (insert_line) as they are *SLOW*,
     * so the only usable one is '\n' (newline).
     */
    return Left == 0 && Rgt == HW->X-1 && Dwn == HW->Y-1 && DstUp == 0;
}

static void linux_DragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp) {
    udat delta = Up - DstUp;

    HW->HideMouse();
    HW->FlagsHW |= FlHWChangedMouseFlag;
	
    fprintf(stdOUT,
	    "%s\033[m\033[%d;1H", /* hide cursor, reset color, go to last line */
	    HW->TT == NOCURSOR ? "" : "\033[?1c", HW->Y);
	    
    while (delta--)
	putc('\n', stdOUT);
    
    if (HW->FlushVideo == linux_FlushVideo)
	setFlush();
    else
	fflush(stdOUT);
	
    /* store actual cursor state for correct updating */
    HW->XY[0] = 0; HW->XY[1] = HW->Y - 1;
    HW->TT = NOCURSOR;
    
    /*
     * Now the last trick: tty scroll erased the part
     * below DstUp + (Dwn - Up) so we must redraw it.
     * This should not be necessary, as after a DragArea()
     * you are supposed to redraw the original position with whatever
     * is under the obj ect you dragged away, but better safe than sorry.
     */
    NeedRedrawVideo(0, DstUp + (Dwn - Up) + 1, HW->X - 1, HW->Y - 1);
}

