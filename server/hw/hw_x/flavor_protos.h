/* Function prototypes for functions that each X11-based hw_xyz.c must implement. */

/* Prepare to print text using the foreground/background colors defined by 'col'. */
static void X11_SetColors(hwcol col);

/*
 * Auto-detect the best font available, returning the name of the selected font in
 * allocated (char *).  Returns NULL if not usable font was found.
 */
static char *X11_AutodetectFont(udat fontwidth, udat fontheight);

/*
 * Cause the X11 server to allocate the given color and store it in 'pixel'.  Return 1 on
 * success, -1 on failure.
 *
 * Note: color_num is passed for the benefit of the xft driver which needs to store extra
 * information associated with the given color...
 */
static int X11_AllocColor(Display *display, Visual *xvisual, Colormap colormap, XColor *xcolor,
                          unsigned long *pixel, int color_num);

/* Do quit processing specific to the x11-flavor driver. */
static void X11_FlavorQuitHW(void);

/*
 * Check whether the arg (which contains HW->Name) is a valid -hw=xyz  If it is valid, return
 * the length of -hw=xyz.  If it is not valid, log an error and return -1.
 */
static int check_hw_name(char *hw_name);
