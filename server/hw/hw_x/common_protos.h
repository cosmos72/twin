/*
 * Function prototypes for the functions defined in common.c to minimize ordering dependencies
 * when #include-ing into hw_xyz.c.
 */

INLINE void X11_Mogrify(dat x, dat y, uldat len);
INLINE ldat diff(ldat x, ldat y);
static ldat calcFontScore(udat fontwidth, udat fontheight, ldat width, ldat height);
static byte X11_LoadFont(CONST char * fontname, udat fontwidth, udat fontheight);
static void X11_QuitHW(void);
static byte X11_InitHW(void);
void QuitModule(module Module);
