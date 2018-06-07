/* Does the "flavor-specific" X11_QuitHW processing.  This one is for the xft flavor. */
static void X11_QuitFlavor(void) {
    int xscreen;
    Colormap colormap;
    Visual *xvisual;

    if (!xdisplay) {
        return;
    }

    xscreen = DefaultScreen(xdisplay);
    colormap = DefaultColormap(xdisplay, xscreen);
    xvisual = DefaultVisual(xdisplay, xscreen);

    if (xsfont) XftFontClose(xdisplay, xsfont);
    if (xftdraw) XftDrawDestroy(xftdraw);
    for (int i = 0; i < MAXCOL; i++) {
        if (xftcolors[i] == NULL) {
            break;
        }
        XftColorFree (xdisplay, xvisual, colormap, xftcolors[i]);
        FreeMem(xftcolors[i]);
        xftcolors[i] = NULL;
    }
}
