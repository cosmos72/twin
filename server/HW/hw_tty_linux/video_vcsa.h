
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
    HW->ConfigureKeyboard = linux_ConfigureKeyboard;
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

