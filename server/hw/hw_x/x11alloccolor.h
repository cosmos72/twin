/*
 * basic X11 version of requesting the X server to allocate a color
 *
 * rgb values have been set in xcolor by the caller
 */
static int X11_AllocColor(int idx, Display *display, Visual *xvisual, Colormap colormap,
        XColor *xcolor) {
    if (!XAllocColor(display, colormap, xcolor)) {
        printk("      X11_InitHW() failed to allocate colors\n");
        return 0;
    }
    xcol[idx] = xcolor->pixel;
    return 1;
}
