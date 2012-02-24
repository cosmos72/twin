
/*
 * This file is derived from linux/drivers/char/keyboard.c :
 *
 * 31-03-03: adapted to twin, Massimiliano Ghilardi
 *
 * ********************************************************
 *
 * linux/drivers/char/keyboard.c
 *
 * Written for linux by Johan Myreen as a translation from
 * the assembly version by Linus (with diacriticals added)
 *
 * Some additional features added by Christoph Niemann (ChN), March 1993
 *
 * Loadable keymaps by Risto Kankkunen, May 1993
 *
 * Diacriticals redone & other small changes, aeb@cwi.nl, June 1993
 * Added decr/incr_console, dynamic keymaps, Unicode support,
 * dynamic function/string keys, led setting,  Sept 1994
 * `Sticky' modifier keys, 951006.
 *
 * 11-11-96: SAK should now work in the raw mode (Martin Mares)
 *
 * Modified to provide 'generic' keyboard support by Hamish Macdonald
 * Merge with the m68k keyboard driver and split-off of the PC low-level
 * parts by Geert Uytterhoeven, May 1997
 *
 * 27-05-97: Added support for the Magic SysRq Key (Martin Mares)
 * 30-07-98: Dead keys redone, aeb@cwi.nl.
 *
 * ********************************************************
 *
 */

#undef WNOHANG
#undef WUNTRACED

#include <linux/kd.h>
#include <linux/keyboard.h>
#include <linux/vt.h>

#define SIZE(array)		(sizeof(array)/sizeof(array[0]))
#define BITS_PER_ULDAT		(sizeof(uldat)*8)

#include "kbd_bitops.h"



#define VC_APPLIC	0	/* application key mode */
#define VC_CKMODE	1	/* cursor key mode */
#define VC_REPEAT	2	/* keyboard repeat */
#define VC_CRLF		3	/* 0 - enter sends CR, 1 - enter sends CRLF */
#define VC_META		4	/* 0 - meta, 1 - meta=prefix with ESC */

#define     vc_kbd_mode(kbd, bit)	(lrawkbd_config >> (bit) & 1)
#define set_vc_kbd_mode(kbd, bit)	(lrawkbd_config |= 1L << (bit))
#define clr_vc_kbd_mode(kbd, bit)	(lrawkbd_config &= ~(1L << (bit)))
#define chg_vc_kbd_mode(kbd, bit)	(lrawkbd_config ^= 1L << (bit))

#define VC_SCROLLOCK	0	/* scroll-lock led mode */
#define VC_NUMLOCK	1	/* numeric lock led mode */
#define VC_CAPSLOCK	2	/* capslock led mode */

#define     vc_kbd_led(kbd, bit)	(lrawkbd_leds >> (bit) & 1)
#define set_vc_kbd_led(kbd, bit)	set_lights(lrawkbd_leds |= 1L << (bit))
#define clr_vc_kbd_led(kbd, bit)	set_lights(lrawkbd_leds &= ~(1L << (bit)))
#define chg_vc_kbd_led(kbd, bit)	set_lights(lrawkbd_leds ^= 1L << (bit))

#define chg_vc_kbd_lock(kbd, bit)	(lrawkbd_lockstate ^= 1L << (bit))

#define chg_vc_kbd_slock(kbd, bit)	(lrawkbd_slockstate ^= 1L << (bit))


#define U(x) ((x) ^ 0xf000)



/* only one display at time can be in raw-keyboard mode... results in much simpler code */


static int lrawkbd_shiftstate;
static int lrawkbd_config = 1<<VC_REPEAT|1<<VC_META;
static int lrawkbd_leds;
static int lrawkbd_lockstate;
static int lrawkbd_slockstate;


/* shift state counters.. */
static byte k_down[NR_SHIFT];
/* keyboard key bitmap */
static uldat key_down[256/BITS_PER_ULDAT];

static ldat dead_key_next;


static ldat npadch = -1;		/* -1 or number assembled on pad */
static byte diacr;
static byte rep;			/* flag telling character repeat */


static byte  queue_buf[512];
static byte *queue;			/* the ASCII sequence of the current keycode */
static uldat queue_len;			/* its length */


static int compute_shiftstate(void);
static udat get_shiftstate_tw(void);
static void set_lights(int lights);
static udat get_kbentry(byte keycode, byte table);
static byte *get_kbsentry(byte keysym);

static udat do_spec(byte value, byte up_flag);
static udat do_cur(byte value, byte up_flag);


static void put_queue(byte ch)
{
    queue[queue_len++] = ch;
}

static void puts_queue(byte *cp)
{
    while (*cp) {
	queue[queue_len++] = *cp++;
    }
}



static void applkey(byte key, byte mode)
{
    static byte buf[] = { 0x1b, 'O', 0x00, 0x00 };
    
    buf[1] = (mode ? 'O' : '[');
    buf[2] = key;
    puts_queue(buf);
}

static void noop_fn(void)
{
}

static void enter(void)
{
    if (diacr) {
	put_queue(diacr);
	diacr = 0;
    }
    put_queue(13);
    if (vc_kbd_mode(kbd,VC_CRLF))
	put_queue(10);
}

static void caps_toggle(void)
{
    if (!rep)
	chg_vc_kbd_led(kbd, VC_CAPSLOCK);
}

static void caps_on(void)
{
    if (!rep)
	set_vc_kbd_led(kbd, VC_CAPSLOCK);
}

static void hold(void)
{
#if 1
    if (!rep)
	chg_vc_kbd_led(kbd, VC_SCROLLOCK);
#else
    if (rep)
	return;
    
    /*
     * Note: SCROLLOCK will be set (cleared) by stop_tty (start_tty);
     * these routines are also activated by ^S/^Q.
     * (And SCROLLOCK can also be set by the ioctl KDSKBLED.)
     */
    if (tty_stopped)
	tty_start();
    else
	tty_stop();
#endif /* 1 */
}

/*
 * Bind this to Shift-NumLock if you work in application keypad mode
 * but want to be able to change the NumLock flag.
 * Bind this to NumLock if you prefer that the NumLock key always
 * changes the NumLock flag.
 */
static void bare_num(void)
{
    if (!rep)
	chg_vc_kbd_led(kbd,VC_NUMLOCK);
}

static void num(void)
{
    if (vc_kbd_mode(kbd,VC_APPLIC))
	applkey('P', 1);
    else
	bare_num();
}

static void set_console(byte nr) {
    ioctl(tty_fd, VT_ACTIVATE, nr & 63);

    /* we will receive SIGUSR1 if kernel needs our permission to switch */
}

static udat get_console_bitmap(void) {
    struct vt_stat vs;
#if 0
    struct vt_stat {
	udat v_active;	/* active vt */
	udat v_signal;	/* signal to send */
	udat v_state;		/* vt bitmask */
    };
#endif
    
    if (ioctl(tty_fd, VT_GETSTATE, &vs) >= 0)
	return vs.v_state & ~1; /* skip tty0 */
    return 0xFFFE;
}

#define b16 (sizeof(udat)*8)


static void set_console_ifthere(uldat k) {
    if (k >= b16 /* get_console_bitmap() cannot tell beyond b16 */ ||
	get_console_bitmap() >> k & 1) {

	set_console(k);
    }
}

/* get the last used console */
static uldat get_console_last(void) {
    uldat b = get_console_bitmap();
    uldat k = 0;
    
    while (b >>= 1)
	k++;
    
    return k;
}


static void set_console_last(void)
{
    /* switch to the last used console, ChN */
    set_console(get_console_last());
}


static void decr_console(void) {
    uldat k = tty_number - 1;
    udat b;
    
    if (k >= b16) /* get_console_bitmap() cannot tell beyond b16 */
	k = b16 - 1;

    b = get_console_bitmap();
    b = b>>(k+1) | b<<(b16-k-1);
    
    while (b && !(b & 1<<(b16-1))) {
	b <<= 1;
	if (--k >= b16)
	    k = b16 - 1;
    }
    
    if (b)
	set_console(k);
}


static void incr_console(void) {
    uldat k = tty_number + 1;
    udat b;
    
    k &= (udat)~0; /* get_console_bitmap() cannot tell beyond b16 */

    b = get_console_bitmap();
    b = b>>k | b<<(b16-k);
    
    while (b && !(b & 1)) {
	b >>= 1;
	k = (k + 1) & (udat)~0;
    }
    
    if (b)
	set_console(k);
}


static void send_intr(void)
{
#if 0
    tty_break();
#endif
}

static void scroll_forw(void)
{
    /* FIXME: finish this! */
}

static void scroll_back(void)
{
    /* FIXME: finish this! */
}

static void boot_it(void)
{
    /* HW->NeedHW |= NEEDPanicHW, NeedHW |= NEEDPanicHW; */
}

static void compose(void)
{
    dead_key_next = 1;
}

static void spawn_console(void)
{
    /* no way to reproduce this from user level */
}

static void SAK(void)
{
    /*
     * SAK should also work in all raw modes and reset
     * them properly.
     * 
     * So what here?
     */
}

static udat do_ignore(byte value, byte up_flag)
{
    return TW_Null;
}

static void do_null(void)
{
    compute_shiftstate();
}

static udat do_lowercase(byte value, byte up_flag)
{
    printk("twin: HW/linux/kbd_low.h: do_lowercase() called - kernel bug!\n");
    return TW_Null;
}

/*
 * We have a combining character DIACR here, followed by the character CH.
 * If the combination occurs in the table, return the corresponding value.
 * Otherwise, if CH is a space or equals DIACR, return DIACR.
 * Otherwise, conclude that DIACR was not combining after all,
 * queue it and return CH.
 */
static byte handle_diacr(byte ch)
{
    static struct kbdiacrs accent_tables;
    static byte accent_tables_init;
    
    struct kbdiacr *accent_table;
    byte d = diacr;
    ldat i;
    
    diacr = 0;
    
    if (!accent_tables_init) {
	accent_tables_init = TRUE;
	ioctl(tty_fd, KDGKBDIACR, &accent_tables);
    }
    
    accent_table = accent_tables.kbdiacr;
    
    for (i = 0; i < accent_tables.kb_cnt; i++) {
	if (accent_table[i].diacr == d && accent_table[i].base == ch)
	    return accent_table[i].result;
    }
    
    if (ch == ' ' || ch == d)
	return d;
    
    KeyboardEventCommon(d, get_shiftstate_tw(), 1, &d);

    return ch;
}


static udat post_latin(byte value)
{
    if (value < 32) {
	udat shifts = get_shiftstate_tw();
	if (shifts & KBD_CTRL_FL)
	    value |= shifts & (KBD_SHIFT_FL|KBD_CAPS_LOCK) ? 0x40 : 0x60;
    }
    return value;
}

static udat do_latin(byte value, byte up_flag)
{
    if (up_flag)
	return TW_Null;	/* no action, if this is a key release */
    
    if (diacr)
	value = handle_diacr(value);
    
    if (dead_key_next) {
	dead_key_next = 0;
	diacr = value;
	return TW_Null;
    }
    
    put_queue(value);
    
    return post_latin(value);
}

#define A_GRAVE  '`'
#define A_ACUTE  '\''
#define A_CFLEX  '^'
#define A_TILDE  '~'
#define A_DIAER  '"'
#define A_CEDIL  ','
static byte map_DIACR[NR_DEAD] = {
    A_GRAVE, A_ACUTE, A_CFLEX, A_TILDE, A_DIAER, A_CEDIL
};


/*
 * Handle dead key. Note that we now may have several
 * dead keys modifying the same character. Very useful
 * for Vietnamese.
 */
static udat do_dead2(byte value, byte up_flag)
{
    if (!up_flag)
	diacr = (diacr ? handle_diacr(value) : value);
    return TW_Null;
}

/* Obsolete - for backwards compatibility only */
static udat do_dead(byte value, byte up_flag)
{
    value = map_DIACR[value];
    do_dead2(value, up_flag);
    return TW_Null;
}


static udat do_cons(byte value, byte up_flag)
{
    if (!up_flag)
	set_console_ifthere(value+1);
    return TW_Null;
}

static udat map_FN [] = {
	/* K_F1 */	TW_F1,
	/* K_F2 */	TW_F2,
	/* K_F3 */	TW_F3,
	/* K_F4 */	TW_F4,
	/* K_F5 */	TW_F5,
	/* K_F6 */	TW_F6,
	/* K_F7 */	TW_F7,
	/* K_F8 */	TW_F8,
	/* K_F9 */	TW_F9,
	/* K_F10 */	TW_F10,
	/* K_F11 */	TW_F11,
	/* K_F12 */	TW_F12,
	/* K_F13 */	TW_F13,
	/* K_F14 */	TW_F14,
	/* K_F15 */	TW_F15,
	/* K_F16 */	TW_F16,
	/* K_F17 */	TW_F17,
	/* K_F18 */	TW_F18,
	/* K_F19 */	TW_F19,
	/* K_F20 */	TW_F20,
	/* K_FIND */	TW_Home,
	/* K_INSERT */	TW_Insert,
	/* K_REMOVE */	TW_Delete,
	/* K_SELECT */	TW_End,
	/* K_PGUP */	TW_Prior,
	/* K_PGDN */	TW_Next,
	/* K_MACRO */	TW_Null,
	/* K_HELP */	TW_Help,
	/* K_DO */	TW_Redo,
	/* K_PAUSE */	TW_Pause,
};

static udat do_fn(byte value, byte up_flag)
{
    if (up_flag)
	return TW_Null;
    puts_queue(get_kbsentry(value));
    
    return value < SIZE(map_FN) ? map_FN[value] : 0;
}

static udat map_PAD [] = {
	/* K_P0 */	TW_KP_0,
	/* K_P1 */	TW_KP_1,
	/* K_P2 */	TW_KP_2,
	/* K_P3 */	TW_KP_3,
	/* K_P4 */	TW_KP_4,
	/* K_P5 */	TW_KP_5,
	/* K_P6 */	TW_KP_6,
	/* K_P7 */	TW_KP_7,
	/* K_P8 */	TW_KP_8,
	/* K_P9 */	TW_KP_9,
	/* K_PPLUS */	TW_KP_Add,
	/* K_PMINUS */	TW_KP_Subtract,
	/* K_PSTAR */	TW_KP_Multiply,
	/* K_PSLASH */	TW_KP_Divide,
	/* K_PENTER */	TW_KP_Enter,
	/* K_PCOMMA */	TW_KP_Decimal,
	/* K_PDOT */	TW_KP_Decimal,
	/* K_PPLUSMINUS */ TW_Null,
	/* K_PPARENL */	TW_Null,
	/* K_PPARENR */	TW_Null,
};

static udat do_pad(byte value, byte up_flag)
{
    static const byte * const pad_chars = "0123456789+-*/\015,.?()";
    static const byte * const app_map = "pqrstuvwxylSRQMnnmPQ";
    
    if (up_flag)
	return TW_Null;	/* no action, if this is a key release */
    
    /* kludge... shift forces cursor/number keys */
    if (vc_kbd_mode(kbd,VC_APPLIC) && !k_down[KG_SHIFT]) {
	applkey(app_map[value], 1);
	return map_PAD[value];
    }
    
    if (!vc_kbd_led(kbd,VC_NUMLOCK)) {
	switch (value) {
	  case KVAL(K_PCOMMA):
	  case KVAL(K_PDOT):
	    return do_fn(KVAL(K_REMOVE), 0);
	  case KVAL(K_P0):
	    return do_fn(KVAL(K_INSERT), 0);
	  case KVAL(K_P1):
	    return do_fn(KVAL(K_SELECT), 0);
	  case KVAL(K_P2):
	    return do_cur(KVAL(K_DOWN), 0);
	  case KVAL(K_P3):
	    return do_fn(KVAL(K_PGDN), 0);
	  case KVAL(K_P4):
	    return do_cur(KVAL(K_LEFT), 0);
	  case KVAL(K_P6):
	    return do_cur(KVAL(K_RIGHT), 0);
	  case KVAL(K_P7):
	    return do_fn(KVAL(K_FIND), 0);
	  case KVAL(K_P8):
	    return do_cur(KVAL(K_UP), 0);
	  case KVAL(K_P9):
	    return do_fn(KVAL(K_PGUP), 0);
	  case KVAL(K_P5):
	    applkey('G', vc_kbd_mode(kbd, VC_APPLIC));
	    return TW_KP_5;
	}
    }
    
    put_queue(pad_chars[value]);
    if (value == KVAL(K_PENTER) && vc_kbd_mode(kbd, VC_CRLF))
	put_queue(10);

    return map_PAD[value];
}

static udat map_CUR [] = {
	/* K_DOWN */	TW_Down,
	/* K_LEFT */	TW_Left,
	/* K_RIGHT */	TW_Right,
	/* K_UP */	TW_Up,
};

static udat do_cur(byte value, byte up_flag)
{
    static const byte * const cur_chars = "BDCA";
    if (up_flag)
	return TW_Null;
    
    applkey(cur_chars[value], vc_kbd_mode(kbd,VC_CKMODE));
    
    return map_CUR[value];
}


static udat map_SHIFT [] = {
	/* K_SHIFT */	TW_Shift_L,
	/* K_ALTGR */	TW_Alt_R,
	/* K_CTRL */	TW_Control_L,
	/* K_ALT */	TW_Alt_L,
	/* K_SHIFTL */	TW_Shift_L,
	/* K_SHIFTR */	TW_Shift_R,
	/* K_CTRLL */	TW_Control_L,
	/* K_CTRLR */	TW_Control_R,
	/* K_CAPSSHIFT*/TW_Shift_R,
};

static udat do_shift(byte value, byte up_flag)
{
    ldat old_state = lrawkbd_shiftstate;
    
    if (rep)
	return map_SHIFT[value];
    
    /* Mimic typewriter:
     a CapsShift key acts like Shift but undoes CapsLock */
    if (value == KVAL(K_CAPSSHIFT)) {
	value = KVAL(K_SHIFT);
	if (!up_flag)
	    clr_vc_kbd_led(kbd, VC_CAPSLOCK);
    }
    
    if (up_flag) {
	/* handle the case that two shift or control
	 keys are depressed simultaneously */
	if (k_down[value])
	    k_down[value]--;
    } else
	k_down[value]++;
    
    if (k_down[value])
	lrawkbd_shiftstate |= (1 << value);
    else
	lrawkbd_shiftstate &= ~(1 << value);
    
    /* kludge */
    if (up_flag && lrawkbd_shiftstate != old_state && npadch != -1) {
	byte ch = npadch & 0xff;
	KeyboardEventCommon(ch, get_shiftstate_tw(), 1, &ch);
	npadch = -1;
    }
    
    return up_flag ? TW_Null : map_SHIFT[value];
}

/* called after returning from RAW mode or when changing consoles -
 recompute k_down[] and shift_state from key_down[] */
/* maybe called when keymap is undefined, so that shiftkey release is seen */
static int compute_shiftstate(void)
{
    ldat i, j, k, sym, val;
    
    lrawkbd_shiftstate = 0;
    for (i=0; i < SIZE(k_down); i++)
	k_down[i] = 0;
    
    for (i=0; i < SIZE(key_down); i++) {
	if (key_down[i]) {	/* skip this word if not a single bit on */
	    k = i*BITS_PER_ULDAT;
	    for (j=0; j<BITS_PER_ULDAT; j++,k++) {
		if (lrawkbd_test_bit(k, (void *)key_down)) {
		    sym = U(get_kbentry(k, 0));
		    if (KTYP(sym) == KT_SHIFT || KTYP(sym) == KT_SLOCK) {
			val = KVAL(sym);
			if (val == KVAL(K_CAPSSHIFT))
			    val = KVAL(K_SHIFT);
			k_down[val]++;
			lrawkbd_shiftstate |= (1<<val);
		    }
		}
	    }
	}
    }
    return lrawkbd_shiftstate;
}

static udat get_shiftstate_tw(void) {
    return
	(lrawkbd_shiftstate & (1<<KG_SHIFT|1<<KG_SHIFTL|1<<KG_SHIFTR) ? KBD_SHIFT_FL : 0) |
	(lrawkbd_shiftstate & (1<<KG_CTRL |1<<KG_CTRLL |1<<KG_CTRLR ) ? KBD_CTRL_FL  : 0) |
	(lrawkbd_shiftstate & (1<<KG_ALT  |1<<KG_ALTGR             ) ? KBD_ALT_FL   : 0) |
	(lrawkbd_leds & 1<<VC_CAPSLOCK ? KBD_CAPS_LOCK : 0) |
	(lrawkbd_leds & 1<<VC_NUMLOCK  ? KBD_NUM_LOCK  : 0);
}
    
static udat do_meta(byte value, byte up_flag)
{
    if (up_flag)
	return TW_Null;
    
    if (vc_kbd_mode(kbd, VC_META)) {
	put_queue('\033');
	put_queue(value);
    } else
	put_queue(value | 0x80);
    
    return value;
}

static udat do_ascii(byte value, byte up_flag)
{
    ldat base;
    
    if (up_flag)
	return TW_Null;
    
    if (value < 10)    /* decimal input of code, while Alt depressed */
	base = 10;
    else {       /* hexadecimal input of code, while AltGr depressed */
	value -= 10;
	base = 16;
    }
    
    if (npadch == -1)
	npadch = value;
    else
	npadch = npadch * base + value;
    
    return TW_Null;
}

static udat map_LOCK[] = {
    	/* K_SHIFTLOCK */	TW_Shift_Lock,
	/* K_CTRLLOCK */	TW_Null,
	/* K_ALTLOCK */		TW_Null,
	/* K_ALTGRLOCK */	TW_Null,
	/* K_SHIFTLLOCK */	TW_Shift_Lock,
	/* K_SHIFTRLOCK */	TW_Shift_Lock,
	/* K_CTRLLLOCK */	TW_Null,
	/* K_CTRLRLOCK */	TW_Null,
};

static udat do_lock(byte value, byte up_flag)
{
    if (up_flag || rep)
	return TW_Null;
    chg_vc_kbd_lock(kbd, value);
    return map_LOCK[value];
}

static udat map_SLOCK[] = {
	/* K_SHIFT_SLOCK */	TW_Shift_Lock,
	/* K_CTRL_SLOCK */	TW_Null,
	/* K_ALT_SLOCK */	TW_Null,
	/* K_ALTGR_SLOCK */	TW_Null,
	/* K_SHIFTL_SLOCK */	TW_Shift_Lock,
	/* K_SHIFTR_SLOCK */	TW_Shift_Lock,
	/* K_CTRLL_SLOCK */	TW_Null,
	/* K_CTRLR_SLOCK */	TW_Null,
};

static udat do_slock(byte value, byte up_flag)
{
    do_shift(value,up_flag);
    if (up_flag || rep)
	return TW_Null;
    chg_vc_kbd_slock(kbd, value);

    /* try to make Alt, oops, AltGr and such work */
    if (get_kbentry(0, lrawkbd_lockstate ^ lrawkbd_slockstate) == K_NOSUCHMAP) {
	lrawkbd_slockstate = 0;
	chg_vc_kbd_slock(kbd, value);
    }
    return map_SLOCK[value];
}




typedef void (*void_fnp)(void);

#define show_ptregs	noop_fn
#define show_mem	noop_fn
#define show_state	noop_fn

static void_fnp table_SPEC[] = {
	do_null,	enter,		show_ptregs,	show_mem,
	show_state,	send_intr,   set_console_last,	caps_toggle,
	num,		hold,		scroll_forw,	scroll_back,
	boot_it,	caps_on,	compose,	SAK,
	decr_console,	incr_console,	spawn_console,	bare_num
};

static udat map_SPEC [] = {
	/* K_HOLE */	TW_Null,
	/* K_ENTER */	TW_Return,
	/* K_SH_REGS */	TW_Null,
	/* K_SH_MEM */	TW_Null,
	/* K_SH_STAT */	TW_Null,
	/* K_BREAK */	TW_Break,
	/* K_CONS */	TW_Null,
	/* K_CAPS */	TW_Caps_Lock,
	/* K_NUM */	TW_Num_Lock,
	/* K_HOLD */	TW_Scroll_Lock,
	/* K_SCROLLFORW */	TW_Next,  /* hack! */
	/* K_SCROLLBACK */	TW_Prior, /* hack! */
	/* K_BOOT */	TW_Delete,	  /* hack! */
	/* K_CAPSON */	TW_Null,
	/* K_COMPOSE */	TW_Null,
	/* K_SAK */	TW_Null,
	/* K_DECRCONSOLE */	TW_Null,
	/* K_INCRCONSOLE */	TW_Null,
	/* K_SPAWNCONSOLE */	TW_Up,    /* hack! */
	/* K_BARENUMLOCK */	TW_Caps_Lock,
};

static udat do_spec(byte value, byte up_flag)
{
    if (up_flag)
	return TW_Null;
    table_SPEC[value]();
    return map_SPEC[value];
}







typedef udat (*k_hand)(byte value, byte up_flag);

static k_hand key_handler[] = {
	do_latin, do_fn, do_spec, do_pad,
	do_dead, do_cons, do_cur, do_shift,
	
	do_meta, do_ascii, do_lock, do_lowercase,
	do_slock, do_dead2, do_ignore, do_ignore
};

/* maximum values each key_handler can handle */
static const byte key_handler_maxval[] = {
	255, 255, SIZE(map_SPEC)-1, SIZE(map_PAD)-1,
	SIZE(map_DIACR)-1, 255, SIZE(map_CUR)-1, SIZE(map_SHIFT)-1,
	
	255, NR_ASCII-1, SIZE(map_LOCK)-1, 255,
	SIZE(map_SLOCK)-1, 255, 0, 0
};




static void set_lights(int lights) {
#if 0
    /* this is an identity */
    lights =
	(lights & 1<<VC_SCROLLOCK ? LED_SCR : 0) |
	(lights & 1<<VC_NUMLOCK   ? LED_NUM : 0) |
	(lights & 1<<VC_CAPSLOCK  ? LED_CAP : 0);
#endif
    ioctl(tty_fd, KDSETLED, lights);
}


#define lrawkbd_KEYMAPS_N (1 << (NR_SHIFT <= 8*sizeof(byte) ? NR_SHIFT : 8*sizeof(byte)))

static udat * lrawkbd_keymaps[lrawkbd_KEYMAPS_N];

static void lrawkbd_FreeKeymaps(void) {
    udat * keymap;
    ldat table;
    for (table = 0; table < lrawkbd_KEYMAPS_N; table++) {
        if ((keymap = lrawkbd_keymaps[table]) != NULL) {
            FreeMem(keymap);
            lrawkbd_keymaps[table] = NULL;
        }
    }
}

static void lrawkbd_LoadKeymaps(void) {
    struct kbentry ke;
    ldat table, keycode;

    /*
     * set UNICODE keyboard mode.
     * needed to get accurate results from ioctl(tty_fd, KDGKBENT, ...)
     * 
     * keyboard mode will be overridden in lrawkbd_SetKeyboard(),
     * no need to restore it here.
     */
    ioctl(tty_fd, KDSKBMODE, K_UNICODE);

    for (table = 0; table < lrawkbd_KEYMAPS_N; table++) {
        for (keycode = 0; keycode < 0x80; keycode++) {

            ke.kb_table = table;
            ke.kb_index = keycode;
            ke.kb_value = 0;

            if (ioctl (tty_fd, KDGKBENT, &ke) == 0) {

#ifdef DEBUG_HW_TTY_LRAWKBD
                printk("\t... ioctl(tty_fd = %d, KDGKBENT, {table = 0x%X, index = 0x%X} ) = 0x%X\n",
                       (int)tty_fd, table, keycode, (int)ke.kb_value);
#endif

                if (keycode == 0 && ke.kb_value == K_NOSUCHMAP)
                    break;
                else if (!lrawkbd_keymaps[table] &&
                         !(lrawkbd_keymaps[table] = AllocMem(0x80 * sizeof(udat))))
                    break;
                
                lrawkbd_keymaps[table][keycode] = ke.kb_value;
            }
        }
    }
}


static udat get_kbentry(byte keycode, byte table)
{
    if (table < lrawkbd_KEYMAPS_N) {
        udat * keymap = lrawkbd_keymaps[table];
        if (keymap != NULL) {
#ifdef DEBUG_HW_TTY_LRAWKBD
            printk("\tget_kbentry(table = 0x%X, keycode = 0x%X) = 0x%X\n",
                   (int)table, (int)keycode, (int)keymap[keycode & 0x7F]);
#endif
            return keymap[keycode & 0x7F];
        }
    } else
        printk("internal error! invalid arguments in get_kbentry(keycode = 0x%X, table = 0x%X): table must be < 0x%X\n",
               (int)keycode, (int)table, (int)lrawkbd_KEYMAPS_N);
    
    return K_NOSUCHMAP;
}

static byte *get_kbsentry(byte keysym) {
    static struct kbsentry ks;
    /*
     struct kbsentry {
	unsigned char kb_func;
	unsigned char kb_string[512];
     };
     */
    
    ks.kb_func = keysym;
    ks.kb_string[0] = '\0';
    
    ioctl(tty_fd, KDGKBSENT, &ks);
    
    return ks.kb_string;
}

#ifdef DEBUG_HW_TTY_LRAWKBD
static void dump_key(udat twk, ldat keysym, byte *s, uldat len) {
    uldat f;
    byte c;
    
    printk("\tTWsym = 0x%X, ksym = 0x%X, `", (int)twk, (int)keysym);
    
    for (f = 0; (c = s[f]) && f<len; f++) {
	if (c >= ' ' && c <= '~')
	    printk("%c", (int)c);
	else
	    printk("\\x%02X", (int)c);
    }
    printk("'\n");
}
#endif

static udat handle_keycode(byte keycode, byte up)
{
    ldat up_flag = up ? 0x80 : 0;
    udat twk = 0;

    if (up_flag) {
	rep = 0;
	lrawkbd_clear_bit(keycode, (void *)key_down);
    } else
	rep = lrawkbd_test_and_set_bit(keycode, (void *)key_down);

    if (!rep || vc_kbd_mode(kbd,VC_REPEAT)) {
	ldat keysym;
	ldat type, val;

	/* the XOR below used to be an OR */
	int shift_final = (lrawkbd_shiftstate | lrawkbd_slockstate) ^
	    lrawkbd_lockstate;

	keysym = get_kbentry(keycode, shift_final);
	
	type = KTYP(keysym);
	
	if (type == KT_LETTER) {
	    type = KT_LATIN;
	    if (vc_kbd_led(kbd, VC_CAPSLOCK))
		keysym = get_kbentry(keycode, shift_final ^ (1<<KG_SHIFT));
	} else if (type == 0xf0) {
            // treat UNICODE as plain ASCII (should be handled properly)
            type = KT_LATIN;
        }
                
	val = KVAL(keysym);
	if (type < SIZE(key_handler_maxval) && val <= key_handler_maxval[type])
	    twk = (*key_handler[type])(val, up_flag);
	
	if (type != KT_SLOCK)
	    lrawkbd_slockstate = 0;
	
#ifdef DEBUG_HW_TTY_LRAWKBD
	dump_key(twk, keysym, queue, queue_len);
#endif
    }
    
    return twk;
}


static udat lrawkbd_LookupKey(udat *ShiftFlags, byte *slen, byte *s, byte *retlen, byte **ret) {
    udat twk;
    byte k = *s;
    
    queue = queue_buf;
    queue_len = 0;

    twk = handle_keycode(k & ~0x80, k & 0x80);

    *ShiftFlags = get_shiftstate_tw();
    
    /* we ALWAYS process one byte at time */
    *slen = 1;
    *retlen = queue_len;
    *ret = queue_buf;

    /* play it safe */
    queue_len = 0;
    queue = NULL;
    
    return twk;
}

