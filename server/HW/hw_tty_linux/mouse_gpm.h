
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
static void GPM_ConfigureMouse(udat resource, byte todefault, udat value);

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
    HW->ConfigureMouse = GPM_ConfigureMouse;
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

static void GPM_ConfigureMouse(udat resource, byte todefault, udat value) {
    switch (resource) {
      case HW_MOUSEMOTIONEVENTS:
	/* nothing to do */
	break;
      default:
	break;
    }
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
	if ((left = Gpm_GetEvent(&GPM_EV)) <= 0) {
	    if (loopN == 30) {
		printk("GPM_MouseEvent(): connection to gpm lost. continuing without mouse :-(\n");
		HW->QuitMouse();
		null_InitMouse();
	    }
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
