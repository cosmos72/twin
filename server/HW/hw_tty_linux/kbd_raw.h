
/* Linux console keyboard input */

#ifdef HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif

#include <signal.h>


#include "kbd_raw1.h"


/* only one display at time can be in raw-keyboard mode... results in much simpler code */
static display_hw lrawkbd_HW;


static int lrawkbd_mode_save;



static void lrawkbd_QuitKeyboard(void);
static void lrawkbd_ConfigureKeyboard(udat resource, byte todefault, udat value);

static udat lrawkbd_LookupKey(udat *ShiftFlags, byte *slen, byte *s, byte *retlen, byte **ret);
static void lrawkbd_KeyboardEvent(int fd, display_hw hw);

static byte lrawkbd_GetKeyboard(void);
static void lrawkbd_SetKeyboard(void);
static void lrawkbd_RestoreKeyboard(void);

static void lrawkbd_LoadKeymaps(void);
static void lrawkbd_FreeKeymaps(void);

static void lrawkbd_GrabConsole(void);
static void lrawkbd_ReleaseConsole(void);

static void lrawkbd_InitSignals(void);
static void lrawkbd_QuitSignals(void);



/* return FALSE if failed */
static byte lrawkbd_InitKeyboard(void) {
    struct termios ttyb;

    if (lrawkbd_HW) {
	printk("lrawkbd_InitKeyboard(): error: another display is already using raw-keyboard mode.\n");
	return FALSE;
    } else if (!lrawkbd_GetKeyboard()) {
	printk("lrawkbd_InitKeyboard(): error: tty is not a Linux console: ioctl(KDGKBMODE) failed!\n");
	return FALSE;
    }

    lrawkbd_HW = HW;
    
    HW->keyboard_slot = RegisterRemote(tty_fd, (obj)HW, lrawkbd_KeyboardEvent);
    if (HW->keyboard_slot == NOSLOT)
	return FALSE;

    HW->KeyboardEvent = lrawkbd_KeyboardEvent;
    HW->ConfigureKeyboard = lrawkbd_ConfigureKeyboard;
    HW->QuitKeyboard = lrawkbd_QuitKeyboard;

    ttyb = ttysave;
    /* NL=='\n'==^J; CR=='\r'==^M */
    ttyb.c_iflag &= ~(IXON|IXOFF|IGNCR|INLCR|ICRNL);
    ttyb.c_cflag |= CS8;
    /*ttyb.c_oflag &= ~OPOST;*/
    ttyb.c_lflag &= ~(ECHO|ICANON);
    ttyb.c_cc[VTIME] = 0;
    ttyb.c_cc[VMIN] = 0;
    /* disable special handling of suspend key (^Z), quit key (^\), break key (^C) */
    ttyb.c_cc[VSUSP] = 0;
    /* set non-blocking mode */
    ttyb.c_cc[VQUIT] = 0;
    ttyb.c_cc[VINTR] = 0;
    tty_setioctl(tty_fd, &ttyb);

    lrawkbd_InitSignals();
    lrawkbd_GrabConsole();
    lrawkbd_LoadKeymaps();
    lrawkbd_SetKeyboard();

    return TRUE;
}

static void lrawkbd_QuitKeyboard(void) {

    lrawkbd_RestoreKeyboard();
    lrawkbd_FreeKeymaps();
    lrawkbd_ReleaseConsole();
    lrawkbd_QuitSignals();
    
    tty_setioctl(tty_fd, &ttysave);

    UnRegisterRemote(HW->keyboard_slot);
    HW->keyboard_slot = NOSLOT;
    
    HW->QuitKeyboard = NoOp;

    lrawkbd_HW = NULL;
}

static void lrawkbd_ConfigureKeyboard(udat resource, byte todefault, udat value) {
    uldat flag = 0;
    
    switch (resource) {
      case HW_KBDAPPLIC:
	flag = VC_APPLIC;
	break;
      case HW_ALTCURSKEYS:
	flag = VC_CKMODE;
	break;
      default:
	return;
    }

    if (todefault || !value)
	clr_vc_kbd_mode(kbd, flag);
    else
	set_vc_kbd_mode(kbd, flag);
}

#ifdef DEBUG_HW_TTY_LRAWKBD
static void dump_bytes(byte * s, uldat len) {
    uldat i;
    byte c;
    
    printk("lrawkbd: received `");
    
    for (i = 0; (c = s[i]) && i<len; i++) {
	if (c >= ' ' && c <= '~')
	    printk("%c", (int)c);
	else
	    printk("\\x%02X", (int)c);
    }
    printk("'\n");
}
#endif // DEBUG_HW_TTY_LRAWKBD


static void lrawkbd_KeyboardEvent(int fd, display_hw hw) {
    byte buf[16], *s, *ret;
    udat Code, ShiftFlags;
    byte got, chunk, retlen;
    SaveHW;
    
    SetHW(hw);
    
    got = read(fd, s=buf, 16);
    
    if (got == 0 || (got == (byte)-1 && errno != EINTR && errno != EWOULDBLOCK)) {
	/* BIG troubles */
	HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW;
	return;
    }
    
    while (got > 0) {
	/* ok, now try to find out the correct KeyCode for the scancodes we got */
	
	chunk = got;

#ifdef DEBUG_HW_TTY_LRAWKBD
        dump_bytes(s, got);
#endif // DEBUG_HW_TTY_LRAWKBD
	
        Code = lrawkbd_LookupKey(&ShiftFlags, &chunk, s, &retlen, &ret);

	if (Code != TW_Null)
	    KeyboardEventCommon(Code, ShiftFlags, retlen, ret);
	
	s += chunk, got -= chunk;
    }
    
    RestoreHW;
}




static byte lrawkbd_GetKeyboard(void) {
    return
	/* get original (probably XLATE) mode */
	ioctl(tty_fd, KDGKBMODE, &lrawkbd_mode_save) >= 0;
}

static void lrawkbd_SetKeyboard(void) {
    /* set LED mode */
    char leds = 0;
    ioctl(tty_fd, KDGETLED, &leds);
    ioctl(tty_fd, KDSETLED, lrawkbd_leds = leds & 7);

    /* set MEDIUMRAW mode */
    ioctl(tty_fd, KDSKBMODE, K_MEDIUMRAW);
}

static void lrawkbd_RestoreKeyboard(void) {
    /* restore LED mode */
    ioctl(tty_fd, KDSETLED, lrawkbd_leds | 8);
    
    /* restore original (probably XLATE) mode */
    ioctl(tty_fd, KDSKBMODE, lrawkbd_mode_save);
}




static void lrawkbd_GrabConsole(void) {
    struct vt_mode vt;

#if 0
    struct vt_mode {
	char mode;		/* vt mode */
	char waitv;		/* if set, hang on writes if not active */
	short relsig;		/* signal to raise on release req */
	short acqsig;		/* signal to raise on acquisition */
	short frsig;		/* unused (set to 0) */
    };
# define VT_GETMODE	0x5601	/* get mode of active vt */
# define VT_SETMODE	0x5602	/* set mode of active vt */
# define VT_AUTO	0x00	/* auto vt switching */
# define VT_PROCESS	0x01	/* process controls switching */
# define VT_ACKACQ	0x02	/* acknowledge switch */
#endif /* 0 */

    vt.mode   = VT_PROCESS;
    vt.waitv  = 0;
    vt.relsig = SIGUSR2;
    vt.acqsig = SIGUSR1;
    vt.frsig  = 0;
    
    ioctl(tty_fd, VT_SETMODE, &vt);
}


static void lrawkbd_ReleaseConsole(void) {
    struct vt_mode vt;

    vt.mode   = VT_AUTO;
    vt.waitv  = 0;
    vt.relsig = 0;
    vt.acqsig = 0;
    vt.frsig  = 0;
    
    ioctl(tty_fd, VT_SETMODE, &vt);
}



static RETSIGTYPE lrawkbd_ReactSignalIn(int sig);

static RETSIGTYPE lrawkbd_ReactSignalOut(int sig) {
    /* HW may not be set here... use lrawkbd_HW */
    SaveHW;
    SetHW(lrawkbd_HW);

    /* we just got SIGUSR1. restore settings and tell kernel we allow the switch */

    signal(sig, lrawkbd_ReactSignalOut);

    lrawkbd_RestoreKeyboard();

    ioctl(tty_fd, VT_RELDISP, 1);
    
    RestoreHW;
    
    RETFROMSIGNAL(0);
}


static RETSIGTYPE lrawkbd_ReactSignalIn(int sig) {
    /* HW may not be set here... use lrawkbd_HW */
    SaveHW;
    SetHW(lrawkbd_HW);

    /* we just got SIGUSR2. tell kernel we allow the switch and initialize settings */

    signal(sig, lrawkbd_ReactSignalIn);
    
    ioctl(tty_fd, VT_RELDISP, 2);

    lrawkbd_SetKeyboard();

    RestoreHW;
    
    RETFROMSIGNAL(0);
}

static void lrawkbd_InitSignals(void) {
    signal(SIGUSR1, lrawkbd_ReactSignalIn);
    signal(SIGUSR2, lrawkbd_ReactSignalOut);
}

static void lrawkbd_QuitSignals(void) {
    signal(SIGUSR1, SIG_DFL);
    signal(SIGUSR2, SIG_DFL);
}


