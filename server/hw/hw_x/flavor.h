/* The various X11-based flavors of hw_xyz. */
#define HW_X11 1
#define HW_XFT 2
#define HW_GFX 3

/*
 * The X11-based flavor currently being compiled.  Must be redefined in each X11-based hw_xyz.c
 * The default setting of "invalid" enforces this rule, because '#if HW_X_FLAVOR == blah' will fail
 * to compile if HW_X_FLAVOR is a string.
 */
#define HW_X_FLAVOR "invalid"
