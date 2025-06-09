/*
 * Function prototypes for the functions defined in util.h to minimize ordering dependencies
 * when #include-ing into hw_xyz.c.
 */

#include "stl/view.h"

static unsigned long XSYM(ColorToPixel)(trgb rgb);
static void XSYM(FillWindowTitle)(char *title, int maxlen);
static void XSYM(HideCursor)(dat x, dat y);
static void XSYM(ShowCursor)(uldat type, dat x, dat y);
static void XSYM(FlushVideo)(void);
static void XSYM(FlushHW)(void);
static void XSYM(DetectSize)(dat *x, dat *y);
static void XSYM(CheckResize)(dat *x, dat *y);
static void XSYM(Resize)(dat x, dat y);
static byte XSYM(SelectionImport_X11)(void);
static void XSYM(SelectionExport_X11)(void);
static void XSYM(SelectionNotify_X11)(uldat reqprivate, uldat magic, const Chars mime, Chars data);
static void XSYM(SelectionNotify_up)(Window win, Atom prop);
static void XSYM(SelectionRequest_X11)(Tobj Requestor, uldat ReqPrivate);
static void XSYM(SelectionRequest_up)(XSelectionRequestEvent *req);
static byte XSYM(CanDragArea)(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp);
static void XSYM(DragArea)(dat Left, dat Up, dat Rgt, dat Dwn, dat DstLeft, dat DstUp);
static int XSYM(Die)(Display *d);
static Tutf_function XSYM(UTF_32_to_charset_function)(const char *charset);
static trune XSYM(UTF_32_to_UCS_2)(trune c);
