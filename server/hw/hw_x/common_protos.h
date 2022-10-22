/*
 * Function prototypes for the functions defined in common.c to minimize ordering dependencies
 * when #include-ing into hw_xyz.c.
 */

inline void XSYM(DrawSome)(dat x, dat y, ldat len);
inline ldat diff(ldat x, ldat y) {
  return x >= y ? x - y : y - x;
}

static ldat XSYM(CalcFontScore)(udat fontwidth, udat fontheight, ldat width, ldat height);
static byte XSYM(LoadFont)(const char *fontname, udat fontwidth, udat fontheight);
static void XSYM(QuitHW)(void);
static bool XSYM(InitHW)(void);
EXTERN_C byte InitModule(module Module);
EXTERN_C void QuitModule(module Module);
