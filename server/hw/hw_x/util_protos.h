/*
 * Function prototypes for the functions defined in util.h to minimize ordering dependencies
 * when #include-ing into hw_xyz.c.
 */

static void X11_FillWindowTitle(char *title, int maxlen);
static void X11_HideCursor(dat x, dat y);
static void X11_ShowCursor(uldat type, dat x, dat y);
static void X11_FlushVideo(void);
static void X11_FlushHW(void);
static void X11_DetectSize(dat *x, dat *y);
static void X11_CheckResize(dat *x, dat *y);
static void X11_Resize(dat x, dat y);
static byte X11_SelectionImport_X11(void);
static void X11_SelectionExport_X11(void);
static void X11_SelectionNotify_X11(uldat ReqPrivate, uldat Magic, CONST char MIME[MAX_MIMELEN],
                                    uldat Len, CONST char *Data);
static void X11_SelectionNotify_up(Window win, Atom prop);
static void X11_SelectionRequest_X11(obj Requestor, uldat ReqPrivate);
static void X11_SelectionRequest_up(XSelectionRequestEvent *req);
static byte X11_CanDragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp);
static void X11_DragArea(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp);
static int X11_Die(Display *d);
static Tutf_function X11_UTF_32_to_charset_function(CONST char *charset);
static hwfont X11_UTF_32_to_UCS_2(hwfont c);
