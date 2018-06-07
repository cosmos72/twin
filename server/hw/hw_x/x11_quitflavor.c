/* Does the "flavor-specific" X11_QuitHW processing.  This one is for the basic X11 flavor. */
static void X11_QuitFlavor(void) {
    if (xsfont) XFreeFont(xdisplay, xsfont);
}
