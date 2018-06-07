/*
 * xft version of requesting the X server to allocate a color
 *
 * rgb values have been set in xcolor by the caller
 */
static int X11_AllocColor(int idx, Display *display, Visual *xvisual, Colormap colormap,
        XRenderColor *xcolor) {
    XftColor *cur_xft_color;
    xcolor->alpha = 65535;
    if (!(cur_xft_color = (XftColor *)AllocMem(sizeof(XftColor)))) {
        printk("      X11_InitHW(): Out of memory!\n");
        return 0;
    }
    WriteMem(cur_xft_color, 0, sizeof(XftColor));
    if (!XftColorAllocValue(display, xvisual, colormap, xcolor, cur_xft_color)) {
        printk("      X11_InitHW() failed to allocate colors\n");
        return 0;
    }
    xcol[idx] = cur_xft_color->pixel;
    xftcolors[idx] = cur_xft_color;
    return 1;
}
