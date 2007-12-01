
/*
 * xterm-style mouse input
 * 
 * used to let twin run inside twin terminal, xterm, etc.
 * 
 * it is always compiled in.
 */

static void xterm_QuitMouse(void);
static void xterm_MouseEvent(int fd, display_hw hw);
static void xterm_ConfigureMouse(udat resource, byte todefault, udat value);


/* return FALSE if failed */
static byte xterm_InitMouse(byte force) {
    CONST byte *term = tty_TERM;
    
    if (force == TRUE) {
	printk("      xterm_InitMouse(): xterm-style mouse FORCED.\n"
	       "      Assuming terminal has xterm compatible mouse reporting.\n");
	term = "xterm";
    } else if (force == TRUE+TRUE) {
	printk("      xterm_InitMouse(): twterm-style mouse FORCED.\n"
	       "      Assuming terminal has twterm compatible mouse reporting.\n");
	term = "twterm";
    }

    if (!term) {
	printk("      xterm_InitMouse() failed: unknown terminal type.\n");
	return FALSE;
    }

    /* stdin_TestTty() will also fill ttypar[] */
    if (!stdin_TestTty()) {
	printk("      xterm_InitMouse() failed: unable to read from the terminal!\n");
        return FALSE;
    }
    
    mouse_start_seq = "\033[?9h";
    mouse_end_seq = "\033[?9l";
    mouse_motion_seq = "\033[?999h";
    
    if (!strcmp(term, "twterm")) {
	;
    } else if (!strcmp(term, "linux")) {
	/*
	 * additional check... out-of-the box linux
	 * doesn't have xterm-style mouse reporting
	 */
	if (ttypar[0]<6 || (ttypar[0]==6 && ttypar[1]<3)) {
	    printk("      xterm_InitMouse() failed: this `linux' terminal\n"
		   "      has no support for xterm-style mouse reporting.\n");
	    return FALSE;
	}
	if (ttypar[0]==6 && ttypar[1]<4) {
	    printk("      xterm_InitMouse() warning: this `linux' terminal\n"
		   "      can only report click, drag and release, not motion.\n");
	    
	    mouse_motion_seq = NULL;
	}
    } else if (!strncmp(term, "xterm", 5) ||
	       !strncmp(term, "rxvt", 4) ||
	       !strncmp(term, "Eterm", 5)) {
	mouse_start_seq = "\033[?1001s\033[?1000h";
	mouse_end_seq = "\033[?1000l\033[?1001r";
	mouse_motion_seq = NULL;
    } else {
	printk("      xterm_InitMouse() failed: terminal `%."STR(SMALLBUFF)"s' is not supported.\n", term);
	return FALSE;
    }

    fputs(mouse_start_seq, stdOUT);
    setFlush();
    
    HW->mouse_slot = NOSLOT; /* shared with keyboard */
    
    HW->MouseEvent = xterm_MouseEvent;
    HW->ConfigureMouse = xterm_ConfigureMouse;
    HW->QuitMouse = xterm_QuitMouse;
    
    HW->FlagsHW &= ~FlHWSoftMouse; /* no need to Hide/Show it */
    HW->ShowMouse = HW->HideMouse = NoOp; /* override the ones set by InitVideo() */
    
    return TRUE;
}

static void xterm_QuitMouse(void) {
    fputs(mouse_end_seq, stdOUT);
    HW->QuitMouse = NoOp;
}

static void xterm_ConfigureMouse(udat resource, byte todefault, udat value) {
    switch (resource) {
      case HW_MOUSEMOTIONEVENTS:
	/* either enable new style + mouse motion, or switch back to new style */
	fputs(value && !todefault ? mouse_motion_seq : mouse_start_seq, stdOUT);
	setFlush();
	break;
      default:
	break;
    }
}


static void xterm_MouseEvent(int fd, display_hw hw) {
    static dat prev_x, prev_y;
    udat Buttons = 0, Id;
    byte *s = xterm_MouseData;
    dat x, y, dx, dy;
    
    if (*s == '\033' && *++s == '[') {
	if (*++s == 'M' && (Id=*++s) <= '#') {
	    /* classic xterm mouse reporting (X11 specs) */
	    if (Id == ' ')
		Buttons |= HOLD_LEFT;
	    else if (Id == '!')
		Buttons |= HOLD_MIDDLE;
	    else if (Id == '\"')
		Buttons |= HOLD_RIGHT;
	    x = *++s - '!'; /* (*s) must be unsigned char */
	    y = *++s - '!';
	} else if (*s == '5' && *++s == 'M' && (Id = *++s) >= ' ' && (Id -= ' ') <= (HOLD_ANY>>HOLD_BITSHIFT)) {
	    /* enhanced xterm-style reporting (twin specs) */
	    Buttons = Id << HOLD_BITSHIFT;
	    s++;
	    x = (udat)((s[0]-'!') & 0x7f) | (udat)((udat)((s[1]-'!') & 0x7f) << 7);
	    if (x & ((udat)1 << 13))
		/* top bit is set, set also higher ones */
		x |= (udat)~0 << 14;
	    
	    s+=2;
	    y = (udat)((s[0]-'!') & 0x7f) | (udat)((udat)((s[1]-'!') & 0x7f) << 7);
	    if (y & ((udat)1 << 13))
		y |= (udat)~0 << 14;
	} else
	    s = NULL;
    } else
	s = NULL;

    if (s) {
	x = Max2(x, 0);
	x = Min2(x, DisplayWidth - 1);
    
	y = Max2(y, 0);
	y = Min2(y, DisplayHeight - 1);
    
	if (x == 0 && prev_x == 0)
	    dx = -1;
	else if (x == DisplayWidth - 1 && prev_x == DisplayWidth - 1)
	    dx = 1;
	else
	    dx = 0;
	if (y == 0 && prev_y == 0)
	    dy = -1;
	else if (y == DisplayHeight - 1 && prev_y == DisplayHeight - 1)
	    dy = 1;
	else
	    dy = 0;
	
	prev_x = x;
	prev_y = y;
	
	MouseEventCommon(x, y, dx, dy, Buttons);
    }
}

