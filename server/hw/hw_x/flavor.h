/* The various flavors of X11-based drivers. */
#define HW_X11 1
#define HW_XFT 2
#define HW_GFX 3

/*
 * The X11-based driver currently being compiled.  Must be redefined in each X11-based hw_xyz.c
 * The default setting of "invalid" enforces this rule, because '#if HW_X_DRIVER == blah' will fail
 * to compile if HW_X_DRIVER is a string.
 */
#define HW_X_DRIVER "invalid"
