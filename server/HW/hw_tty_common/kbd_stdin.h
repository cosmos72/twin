
/* plain Unix-style tty keyboard input */

/* it can't update FullShiftFlags :( */


static void stdin_QuitKeyboard(void);
static void stdin_KeyboardEvent(int fd, display_hw hw);
static udat linux_LookupKey(udat *ShiftFlags, byte *slen, byte *s, byte *retlen, byte **ret);
static void xterm_MouseEvent(int, display_hw);

static byte stdin_TestTty(void) {
    struct termios ttyb;
    byte buf[16], *s = buf+3, c;
    int i;

    ttyb = ttysave;
    /* NL=='\n'==^J; CR=='\r'==^M */
    ttyb.c_iflag &= ~(IXON|IXOFF|IGNCR|INLCR|ICRNL);
    /*ttyb.c_oflag &= ~OPOST;*/
    ttyb.c_lflag &= ~(ECHO|ICANON);
    ttyb.c_cc[VTIME] = 0;
    ttyb.c_cc[VMIN] = 1;
    /* disable special handling of suspend key (^Z), quit key (^\), break key (^C) */
    ttyb.c_cc[VSUSP] = 0;
    ttyb.c_cc[VQUIT] = 0;
    ttyb.c_cc[VINTR] = 0;
    tty_setioctl(tty_fd, &ttyb);

    write(tty_fd, "\033Z", 2); /* request ID */
    /* ensure we CAN read from the tty */
    do {
	i = read(tty_fd, buf, 15);
    } while (i < 0 && (errno == EWOULDBLOCK || errno == EINTR));
    if (i <= 0) {
        tty_setioctl(tty_fd, &ttysave);
	return FALSE;
    }
    buf[i] = '\0';
    ttypar[0] = ttypar[1] = ttypar[2] = i = 0;

    while (i<3 && (c=*s)) {
	if (c>='0' && c<='9') {
	    ttypar[i] *= 10; ttypar[i] += c-'0';
	} else if (*s==';')
	    i++;
	else
	    break;
	s++;
    }
    return TRUE;
}

/* return FALSE if failed */
static byte stdin_InitKeyboard(void) {

    if (!stdin_TestTty()) {
	printk("      stdin_InitKeyboard() failed: unable to read from the terminal!\n");
        return FALSE;
    }

    HW->keyboard_slot = RegisterRemote(tty_fd, (obj)HW, stdin_KeyboardEvent);
    if (HW->keyboard_slot == NOSLOT) {
	stdin_QuitKeyboard();
	return FALSE;
    }
    HW->KeyboardEvent = stdin_KeyboardEvent;
    HW->QuitKeyboard = stdin_QuitKeyboard;

    LookupKey = linux_LookupKey;
    
    return TRUE;
}

static void stdin_QuitKeyboard(void) {
    tty_setioctl(tty_fd, &ttysave);
    
    UnRegisterRemote(HW->keyboard_slot);
    HW->keyboard_slot = NOSLOT;
    
    HW->QuitKeyboard = NoOp;
}


/* kludge! this is ok for linux terminals only... */
static udat linux_LookupKey(udat *ShiftFlags, byte *slen, byte *s, byte *retlen, byte **ret) {
    byte used = 0, len = *slen;

    *ShiftFlags = 0;
    
    if (len == 0) {
	*retlen = len;
	return TW_Null;
    }

    *ret = s;
    
    if (len > 1 && *s == '\x1B') {

	++used, --len;
	++s;

	if (len == 1) {
	    /* try to handle ALT + <some key> */
	    *slen = *retlen = ++used;
	    *ShiftFlags = KBD_ALT_FL;
	    return (udat)*s;
	}

	switch (*s) {
	    
#define K(sym)     *slen = *retlen = ++used; return TW_##sym
#define IS(sym, c) case c: K(sym)
	
	  case '[':
	    if (++used, --len) switch (*++s) {
		/* ESC [ */
		IS(Up,    'A');
		IS(Down,  'B');
		IS(Right, 'C');
		IS(Left,  'D');
		IS(KP_5,  'G');  /* returned when NumLock is off */
		IS(Pause, 'P');
		
		/* FALLTHROUGH */
		
	      case '[':
		/* ESC [ [ */
		if (++used, --len) switch (*++s) {
		    IS(F1, 'A');
		    IS(F2, 'B');
		    IS(F3, 'C');
		    IS(F4, 'D');
		    IS(F5, 'E');
		}
		break;
		
	      case '1':
		/* ESC [ 1 */
		if (++used, --len) {
		    ++s;
		    
		    if (len == 1 && *s == '~') {
			/* ESC [ 1 ~ */
			K(Home);
		    }
		    ++used, --len;
		    if (len && s[1] == '~')
			/* ESC [ 1 ? ~ */
			switch (*s) {
			    IS(F6, '7');
			    IS(F7, '8');
			    IS(F8, '9');
			}
		}
		break;
		
	      case '2':
		/* ESC [ 2 */
		if (++used, --len) {
		    ++s;
		    
		    if (len == 1 && *s == '~') {
			/* ESC 2 ~ */
			K(Insert);
		    }
		    ++used, --len;
		    if (len && s[1] == '~')
			/* ESC [ 2 ? ~ */
			switch (*s) {
			    IS(F9,  '0');
			    IS(F10, '1');
			    IS(F11, '3');
			    IS(F12, '4');
			}
		}
		break;
		
	      case '3':
		/* ESC [ 3 */
		if (++used, --len) switch (*++s) {
		    IS(Delete, '~');
		}
		break;
		
	      case '4':
		/* ESC [ 4 */
		if (++used, --len) switch (*++s) {
		    IS(End, '~');
		}
		break;
		
	      case '5':
		/* ESC [ 5 */
		if (++used, --len) switch (*++s) {
		    IS(Prior, '~');
		}
		break;

	      case '6':
		/* ESC [ 6 */
		if (++used, --len) switch (*++s) {
		    IS(Next, '~');
		}
		break;
		
	    }
	    break;
	    
	  case 'O':
	    /* ESC O */
	    if (++used, --len) switch (*++s) {
		
		/* alternate cursor keys... */
		IS(Up,    'A');
		IS(Down,  'B');
		IS(Right, 'C');
		IS(Left,  'D');
		IS(KP_5,  'G');
		
		IS(KP_Enter, 'M');
		IS(Num_Lock, 'P');
		IS(KP_Divide,'Q');
		IS(KP_Multiply,'R');
		IS(KP_Subtract,'S');
		
		IS(KP_Add, 'l');
		IS(KP_Separator, 'm');
		IS(KP_Delete, 'n');
		IS(KP_Equal, 'o');
		
		IS(KP_0, 'p');
		IS(KP_1, 'q');
		IS(KP_2, 'r');
		IS(KP_3, 's');
		IS(KP_4, 't');
		IS(KP_5, 'u');
		IS(KP_6, 'v');
		IS(KP_7, 'w');
		IS(KP_8, 'x');
		IS(KP_9, 'y');
	    }
	    break;

	  default:
	    break;
	}

#undef IS
#undef K
	
    }

    /* undo the increments above */
    s -= used;
    len += used;
    used = 0;

    *slen = *retlen = ++used;
    
    switch (*s) {
      case TW_Tab:
      case TW_Linefeed:
      case TW_Return:
      case TW_Escape:
	return (udat)*s;
      default:
	if (*s < ' ') {
	    /* try to handle CTRL + <some key> */
	    *ShiftFlags = KBD_CTRL_FL;
	    return (udat)*s | 0x40;
	}
	return (udat)*s;
    }
    
    /* NOTREACHED */
    return TW_Null;
}



static byte xterm_MouseData[10] = "\033[M#!!!!";

static void stdin_KeyboardEvent(int fd, display_hw hw) {
    static byte buf[SMALLBUFF];
    static fd_set rfds;
    static struct timeval t;
    byte *s = buf, *ret = buf+sizeof(buf)-1;
    udat Code, ShiftFlags;
    byte got, chunk, retlen;
    SaveHW;
    
    SetHW(hw);

    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
    t.tv_sec = 0; t.tv_usec = 1000;
    
    do {
	got = read(fd, s, ret-s);

    } while (got > 0 && (s += got) < ret &&
	     select(fd+1, &rfds, NULL, NULL, &t) == 1);

    
    if (got == (byte)-1 && errno != EINTR && errno != EWOULDBLOCK) {
	/* BIG troubles */
	HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
	return;
    }

    got = s - buf;
    s = buf;
    
    while (got > 0) {
	if (HW->MouseEvent == xterm_MouseEvent) {
	    while (got) {
		if (got >= 6 && !memcmp(s, "\033[M", 3)) {
		    /* classic xterm mouse reporting (X11 specs) */
		    CopyMem(s, xterm_MouseData, chunk = 6);
		} else if (got >= 9 && !memcmp(s, "\033[5M", 4)) {
		    /* enhanced xterm-style reporting (twin specs) */
		    CopyMem(s, xterm_MouseData, chunk = 9);
		} else
		    break;

		s += chunk, got -= chunk;
		xterm_MouseEvent(fd, HW);
	    }
	}

	/* ok, now try to find out the correct KeyCode for the ASCII sequence we got */
	
	if (!(chunk = got))
	    break;
	
	Code = LookupKey(&ShiftFlags, &chunk, s, &retlen, &ret);
	s += chunk, got -= chunk;

	KeyboardEventCommon(Code, ShiftFlags, retlen, ret);
    }
    
    RestoreHW;
}

