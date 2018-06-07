/* Basic X11 implementation of managing foregroun/background colors. */

static void X11_SetColors(hwcol col) {
    if (xsgc.foreground != xcol[COLFG(col)])
        XSetForeground(xdisplay, xgc, xsgc.foreground = xcol[COLFG(col)]);
    if (xsgc.background != xcol[COLBG(col)])
        XSetBackground(xdisplay, xgc, xsgc.background = xcol[COLBG(col)]);
}
