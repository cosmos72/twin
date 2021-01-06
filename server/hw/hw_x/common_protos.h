/*
 * Function prototypes for the functions defined in common.c to minimize ordering dependencies
 * when #include-ing into hw_xyz.c.
 */

inline void X11_Mogrify(dat x, dat y, ldat len);
inline ldat diff(ldat x, ldat y);
static ldat calcFontScore(udat fontwidth, udat fontheight, ldat width, ldat height);
static byte X11_LoadFont(const char *fontname, udat fontwidth, udat fontheight);
static void X11_QuitHW(void);
static byte X11_InitHW(void);
EXTERN_C byte InitModule(module Module);
EXTERN_C void QuitModule(module Module);
