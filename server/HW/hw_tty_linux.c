
/*
 * Linux console, linux terminal and twin terminal drivers:
 * 
 * plain Unix-style tty keyboard input;
 * 
 * mouse input uses libgpm to connect to `gpm' mouse daemon
 * and read mouse state, but draws mouse pointer manually;
 * (as fallback, xterm-style mouse input)
 * 
 * output through /dev/vcsaXX;
 * (as fallback, output through stdout)
 *
 * CONF_HW_TTY_LINUX is for linux console support (gpm, /dev/vcsa*), while
 * CONF_HW_TTY_TWTERM is for twin terminal (stdout, xterm style mouse)
 */
#ifdef CONF_HW_TTY_LINUX




#if TW_SIZEOFHWATTR == 2 && TW_BYTE_ORDER == TW_LITTLE_ENDIAN

# ifdef TW_HAVE_PWRITE /* always 0... */

#  define vcsa_write(fd, buf, count, pos) pwrite(fd, buf, (count)*2, (pos)*2+4)

# else /* !TW_HAVE_PWRITE */
/*
 * we do not need the complete seek + write + seek back
 * but only seek + write
 */
#  define vcsa_write(fd, buf, count, pos) do { \
    lseek(fd, (pos)*2+4, SEEK_SET); \
    write(fd, buf, (count)*2); \
} while (0)

# endif /* TW_HAVE_PWRITE */

#else /* TW_SIZEOFHWATTR != 2 || TW_BYTE_ORDER != TW_LITTLE_ENDIAN */

static byte vcsa_buff[BIGBUFF*2];

INLINE void vcsa_write(int fd, hwattr *buf, uldat count, uldat pos) {
    byte *buf8;
    uldat chunk;
    
    lseek(fd, pos*2+4, SEEK_SET);
    while (count) {
	buf8 = vcsa_buff;
	pos = chunk = Min2(count, BIGBUFF);
	while (pos--) {
# ifdef CONF__UNICODE
	    *buf8++ = tty_UTF_16_to_charset(HWFONT(*buf));
# else
	    *buf8++ = HWFONT(*buf);
# endif
	    *buf8++ = HWCOL(*buf);
	    buf++;
	}
	write(fd, vcsa_buff, chunk*2);
	count -= chunk;
    }
}

#endif /* TW_SIZEOFHWATTR == 2 && TW_BYTE_ORDER == TW_LITTLE_ENDIAN */



/*
 * libgpm is stacked, not multi-headed (so no multiplex too)
 */
static byte GPM_InUse;


/*
 * Linux gpm mouse input:
 * mouse input uses libgpm to connect to `gpm' mouse daemon
 * and read mouse state, but draws mouse pointer manually
 */

static void GPM_QuitMouse(void);
static void GPM_MouseEvent(int fd, display_hw hw);

static int wrap_Gpm_Open(void) {
    /*
     * HACK! this works around a quirk in libgpm:
     * if Gpm_Open fails, it sets gpm_tried to non-zero
     * and following calls will just fail, not even trying anymore
     */
    extern int gpm_tried;
    
    if (!tty_name) {
	printk("      GPM_InitMouse() failed: unable to detect tty device\n");
	return NOFD;
    }
    if (tty_number < 1 || tty_number > 63) {
	printk("      GPM_InitMouse() failed: terminal `%."STR(SMALLBUFF)"s'\n"
		        "      is not a local linux console.\n", tty_name);
	return NOFD;
    }

    gpm_tried = 0;
    
    gpm_zerobased = 1;
    gpm_visiblepointer = 0;
    GPM_Conn.eventMask = ~0;		/* Get everything */
    GPM_Conn.defaultMask = ~GPM_HARD;	/* Pass everything unused */
    GPM_Conn.minMod = 0;		/* Run always... */
    GPM_Conn.maxMod = ~0;		/* ...with any modifier */

    GPM_fd = Gpm_Open(&GPM_Conn, tty_number);
    
    if (GPM_fd >= 0) {
	/* gpm_consolefd is opened by GPM_Open() */
	fcntl(gpm_consolefd, F_SETFD, FD_CLOEXEC);
    } else {
	printk("      GPM_InitMouse() failed: unable to connect to `gpm'.\n"
	       "      make sure you started `twin' from the console\n"
	       "      and/or check that `gpm' is running.\n");
    }
    return GPM_fd;
}



/* return FALSE if failed */
static byte GPM_InitMouse(void) {
    
    if (GPM_InUse) {
	printk("      GPM_InitMouse() failed: already connected to `gpm'.\n");
	return FALSE;
    }
    
    if (wrap_Gpm_Open() < 0)
	return FALSE;

    fcntl(GPM_fd, F_SETFD, FD_CLOEXEC);
    fcntl(GPM_fd, F_SETFL, O_NONBLOCK);
    
    HW->mouse_slot = RegisterRemote(GPM_fd, (obj)HW, GPM_MouseEvent);
    if (HW->mouse_slot == NOSLOT) {
	Gpm_Close();
	return FALSE;
    }
    
    HW->FlagsHW |= FlHWSoftMouse; /* _we_ Hide/Show it */
    
    HW->MouseEvent = GPM_MouseEvent;
    HW->QuitMouse = GPM_QuitMouse;

    GPM_InUse = TRUE;
    
    return TRUE;
}

static void GPM_QuitMouse(void) {
    /* we cannot be sure that some InitVideo() initialized HW->HideMouse */
#if 0
    HW->HideMouse();
#endif
    Gpm_Close();

    UnRegisterRemote(HW->mouse_slot);
    HW->mouse_slot = NOSLOT;

    GPM_InUse = FALSE;
    
    HW->QuitMouse = NoOp;
}

static void GPM_EnableMouseMotionEvents(byte enable) {
    /* nothing to do */
}


static void GPM_MouseEvent(int fd, display_hw hw) {
    int left = 0;
    udat IdButtons, Buttons = 0;
    Gpm_Event GPM_EV;
    
    SaveHW;
    
    /*
     * All other parts of twin read and parse data from fds in big chunks,
     * while Gpm_GetEvent() reads and parses only a single event at time.
     * To compensate this and avoid mouse to lag behind, we do a small loop.
     */
    byte loopN = 30;
    
    SetHW(hw);
    
    do {
	if (Gpm_GetEvent(&GPM_EV) <= 0) {
	    if (loopN == 30)
		HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
	    break;
	}
	
#if 0
	All->FullShiftFlags =
	    (GPM_EV.modifiers & 1 ? FULL_LEFT_SHIFT_PRESSED : 0)
	    | (GPM_EV.modifiers & 2 ? FULL_RIGHT_ALT_PRESSED  : 0)
	    | (GPM_EV.modifiers & 4 ? FULL_LEFT_CTRL_PRESSED  : 0)
	    | (GPM_EV.modifiers & 8 ? FULL_LEFT_ALT_PRESSED   : 0);
#endif
	
	/*
	 * Gpm differs from us about what buttons to report on release events:
	 * it reports which buttons get _released_, not which are still _pressed_
	 * Fixed here. SIGH.
	 */
	IdButtons = GPM_EV.buttons;
	
	if (GPM_EV.type & GPM_UP)
	    IdButtons = GPM_keys & ~IdButtons;
	GPM_keys = IdButtons;

	Buttons |=
	    (IdButtons & GPM_B_LEFT   ? HOLD_LEFT   : 0) |
	    (IdButtons & GPM_B_MIDDLE ? HOLD_MIDDLE : 0) |
	    (IdButtons & GPM_B_RIGHT  ? HOLD_RIGHT  : 0) |
#if defined(GPM_B_WHEEL_REV) && defined(HOLD_WHEEL_REV)
	    (IdButtons & GPM_B_WHEEL_REV ? HOLD_WHEEL_REV : 0) |
#endif
#if defined(GPM_B_WHEEL_FWD) && defined(HOLD_WHEEL_FWD)
	    (IdButtons & GPM_B_WHEEL_FWD ? HOLD_WHEEL_FWD : 0) |
#endif	
	    0;
	
	MouseEventCommon(GPM_EV.x, GPM_EV.y, GPM_EV.dx, GPM_EV.dy, Buttons);
	
    } while (loopN-- && ioctl(GPM_fd, FIONREAD, &left) >= 0 && left > 0);
    
    RestoreHW;
}


#endif /* CONF_HW_TTY_LINUX */





#if defined (CONF_HW_TTY_LINUX) || defined(CONF_HW_TTY_TWTERM)

static void linux_Beep(void);
static void linux_Configure(udat resource, byte todefault, udat value);
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

#ifdef CONF_HW_TTY_LINUX

/* output through /dev/vcsaXX */

static void vcsa_QuitVideo(void);
static void vcsa_FlushVideo(void);
static void vcsa_ShowMouse(void);
static void vcsa_HideMouse(void);

/* return FALSE if failed */
static byte vcsa_InitVideo(void) {
    static byte vcsa_name[] = "/dev/vcsaXX";

    if (!tty_name) {
	printk("      vcsa_InitVideo() failed: unable to detect tty device\n");
	return FALSE;
    }
    
    if (tty_number < 1 || tty_number > 63) {
	printk("      vcsa_InitVideo() failed: terminal `%."STR(SMALLBUFF)"s'\n"
			 "      is not a local linux console.\n", tty_name);
	return FALSE;
    }

    vcsa_name[9] = tty_name[8];
    vcsa_name[10] = tty_name[9];
    
    GainPrivileges();
    VcsaFd = open(vcsa_name, O_WRONLY|O_NOCTTY);
    DropPrivileges();
    
    if (VcsaFd < 0) {
	printk("      vcsa_InitVideo() failed: unable to open `%."STR(SMALLBUFF)"s': %."STR(SMALLBUFF)"s\n",
		vcsa_name, strerror(errno));
	return FALSE;
    }
    fcntl(VcsaFd, F_SETFD, FD_CLOEXEC);
    
    tc_scr_clear = "\033[2J";
    fputs(tc_scr_clear, stdOUT); /* clear screen */
    fflush(stdOUT);
    
    HW->FlushVideo = vcsa_FlushVideo;
    HW->FlushHW = stdout_FlushHW;

    HW->ShowMouse = vcsa_ShowMouse;
    HW->HideMouse = vcsa_HideMouse;
    HW->UpdateMouseAndCursor = linux_UpdateMouseAndCursor;

    HW->DetectSize  = stdin_DetectSize; stdin_DetectSize(&HW->usedX, &HW->usedY);
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
    HW->TT = (uldat)-1; /* force updating cursor */
    
    HW->Beep = linux_Beep;
    HW->Configure = linux_Configure;
    HW->SetPalette = linux_SetPalette;
    HW->ResetPalette = linux_ResetPalette;

    HW->QuitVideo = vcsa_QuitVideo;
    
    HW->FlagsHW |= FlHWNeedOldVideo;
    HW->FlagsHW &= ~FlHWExpensiveFlushVideo;
    HW->NeedHW = 0;
    HW->CanResize = FALSE;
    HW->merge_Threshold = 40;

    return TRUE;
}

static void vcsa_QuitVideo(void) {
    linux_MoveToXY(0, DisplayHeight-1);
    linux_SetCursorType(LINECURSOR);
    /* restore original colors, alt cursor keys, keypad settings */
    fputs("\033[0m\033[?1l\033>\n", stdOUT);
    
    close(VcsaFd);
    
    HW->QuitVideo = NoOp;
}


static void vcsa_FlushVideo(void) {
    dat i, j;
    uldat prevS = (uldat)-1, prevE = (uldat)-1, _prevS, _prevE, _start, _end, start, end;
    byte FlippedVideo = FALSE, FlippedOldVideo = FALSE;
    hwattr savedOldVideo;
    
    if (!ChangedVideoFlag) {
	HW->UpdateMouseAndCursor();
	return;
    }
    
    /* this hides the mouse if needed ... */
    
    /* first, check the old mouse position */
    if (HW->FlagsHW & FlHWSoftMouse) {
	if (HW->FlagsHW & FlHWChangedMouseFlag) {
	    /* dirty the old mouse position, so that it will be overwritten */
	    
	    /*
	     * with multi-display this is a hack, but since OldVideo gets restored
	     * by VideoFlip() below *BEFORE* returning from vcsa_FlushVideo(),
	     * that's ok.
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
	 * this avoids glitches if the mouse is between two dirty areas
	 * that get merged.
	 */
	if ((HW->FlagsHW & FlHWChangedMouseFlag) || (FlippedVideo = Threshold_isDirtyVideo(i, j))) {
	    VideoFlip(i, j);
	    if (!FlippedVideo)
		DirtyVideo(i, j, i, j);
	    HW->FlagsHW &= ~FlHWChangedMouseFlag;
	    FlippedVideo = TRUE;
	} else
	    FlippedVideo = FALSE;
    }
    
    for (i=0; i<DisplayHeight*2; i++) {
	_start = start = (uldat)ChangedVideo[i>>1][i&1][0];
	_end   = end   = (uldat)ChangedVideo[i>>1][i&1][1];
	    
	if (start != (uldat)-1) {
		
	    /* actual tty size could be different from DisplayWidth*DisplayHeight... */
	    start += (i>>1) * DisplayWidth;
	    end   += (i>>1) * DisplayWidth;

	    _start += (i>>1) * HW->X;
	    _end   += (i>>1) * HW->X;
		
		
	    if (prevS != (uldat)-1) {
		if (start - prevE < HW->merge_Threshold) {
		    /* the two chunks are (almost) contiguous, merge them */
		    /* if HW->X != DisplayWidth we can merge only if they do not wrap */
		    if (HW->X == DisplayWidth || prevS / DisplayWidth == end / DisplayWidth) {
			_prevE = prevE = end;
			continue;
		    }
		}
		vcsa_write(VcsaFd, (void *)&Video[prevS], prevE+1-prevS, _prevS);
	    }
	    prevS = start;
	    prevE = end;
	    _prevS = _start;
	    _prevE = _end;
	}
    }
    if (prevS != (uldat)-1) {
	vcsa_write(VcsaFd, (void *)&Video[prevS], prevE+1-prevS, _prevS);
    }
    
    /* ... and this redraws the mouse */
    if (HW->FlagsHW & FlHWSoftMouse) {
	if (FlippedOldVideo)
	    OldVideo[HW->Last_x + HW->Last_y * DisplayWidth] = savedOldVideo;
	if (FlippedVideo)
	    VideoFlip(HW->Last_x = HW->MouseState.x, HW->Last_y = HW->MouseState.y);
	else if (HW->FlagsHW & FlHWChangedMouseFlag)
	    HW->ShowMouse();
    }
    
    /* now the cursor */
    
    if (!ValidOldVideo || (CursorType != NOCURSOR && (CursorX != HW->XY[0] || CursorY != HW->XY[1]))) {
	linux_MoveToXY(HW->XY[0] = CursorX, HW->XY[1] = CursorY);
	setFlush();
    }
    if (!ValidOldVideo || CursorType != HW->TT) {
	linux_SetCursorType(HW->TT = CursorType);
	setFlush();
    }

    HW->FlagsHW &= ~FlHWChangedMouseFlag;
}


/* HideMouse and ShowMouse depend on Video setup, not on Mouse.
 * so we have vcsa_ and stdout_ versions, not GPM_ ones... */
static void vcsa_ShowMouse(void) {
    uldat pos = (HW->Last_x = HW->MouseState.x) + (HW->Last_y = HW->MouseState.y) * DisplayWidth;
    uldat _pos = HW->Last_x + HW->Last_y * HW->X;
    
    hwattr h  = Video[pos], c = HWATTR_COLMASK(~h) ^ HWATTR(COL(HIGH,HIGH),0);

    h = c | HWATTR_FONTMASK(h);

    vcsa_write(VcsaFd, (void *)&h, 1, _pos);
}

static void vcsa_HideMouse(void) {
    uldat pos = HW->Last_x + HW->Last_y * DisplayWidth;
    uldat _pos = HW->Last_x + HW->Last_y * HW->X;

    vcsa_write(VcsaFd, (void *)&Video[pos], 1, _pos);
}

#endif /* CONF_HW_TTY_LINUX */

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
#define linux_MogrifyFinish() do { if (utf8used) utf8used = FALSE, fputs("\033%@", stdOUT); } while (0)

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

static void linux_Configure(udat resource, byte todefault, udat value) {
    switch (resource) {
      case HW_KBDAPPLIC:
	fputs(todefault || !value ? "\033>" : "\033=", stdOUT);
	setFlush();
	break;
      case HW_ALTCURSKEYS:
	fputs(todefault || !value ? "\033[?1l" : "\033[?1h", stdOUT);
	setFlush();
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

#endif /* defined(CONF_HW_TTY_LINUX) || defined(CONF_HW_TTY_TWTERM) */
