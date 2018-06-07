/* xft implementaiton of the X11_AutodetectFont function */

/* return name of selected font in allocated (char *) */
static char * X11_AutodetectFont(udat fontwidth, udat fontheight) {
    FcFontSet *fontset;
    XftFont *fontp;
    char *fontname = NULL;
    char *t_fontname = NULL;
    int t_fontname_len = -1;
    ldat score, best_score = TW_MINLDAT;
    int xscreen = DefaultScreen(xdisplay);

    // find a usable font as follows
    //    an xft font (outline=true, scalable=true)
    //    monospace (spacing=100)
    //    not italic (slant=0)
    //    medium weight (75 <= weight <= 100)
    //    highest font score (closest to fontwidth X fontheight)
    fontset = XftListFonts (xdisplay, DefaultScreen(xdisplay),
            XFT_OUTLINE, XftTypeBool, FcTrue,
            XFT_SCALABLE, XftTypeBool, FcTrue,
            XFT_SPACING, XftTypeInteger, 100,
            XFT_SLANT, XftTypeInteger, 0,
            (char *)0,
            XFT_WEIGHT, XFT_FILE, (char *)0);
    if (fontset) {
        for (int i = 0; i < fontset->nfont; i++) {
            int weight;
            int len;
            FcChar8 *file;

            if (FcPatternGetInteger (fontset->fonts[i], XFT_WEIGHT, 0, &weight) != FcResultMatch) {
                continue;
            }
            if ((weight < FC_WEIGHT_BOOK) || (weight > FC_WEIGHT_MEDIUM)) {
                continue;
            }
            if (FcPatternGetString (fontset->fonts[i], XFT_FILE, 0, &file) != FcResultMatch) {
                continue;
            }

            // reuse existing t_fontname if possible, otherwise allocate a new one
            len = LenStr(file) + LenStr(":file=") + 1;
            if (!t_fontname || (len > t_fontname_len)) {
                if (t_fontname) {
                    FreeMem(t_fontname);
                }
                t_fontname = AllocMem(t_fontname_len = len);
                if (!t_fontname) {
                    printk("      X11_AutodetectFont(): Out of memory!\n");
                    break;
                }
            }
            sprintf(t_fontname, ":file=%s", file);

            fontp = XftFontOpenName(xdisplay, xscreen, t_fontname);
            if (fontp) {
                score = calcFontScore(fontwidth, fontheight, (ldat)fontp->max_advance_width,
                        (ldat)fontp->ascent + fontp->descent);
                XftFontClose(xdisplay, fontp);

                if (!fontname || (score > best_score)) {
                    best_score = score;
                    if (fontname) {
                        FreeMem(fontname);
                    }
                    fontname = t_fontname;
                    t_fontname = NULL;
                }
            }
        }
        FcFontSetDestroy(fontset);
    }

    if (t_fontname) {
        FreeMem(t_fontname);
        t_fontname = NULL;
    }

    return fontname;
}
