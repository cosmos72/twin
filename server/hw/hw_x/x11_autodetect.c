/* The X11_AutodetectFont function for basic X11 along with its support functions. */

/*
 * return ttrue if each font glyph is either 'narrow' (latin, etc.) or 'wide' (CJK...)
 * with 'wide' characters exactly twice as wide as 'narrow' ones
 */
static tbool X11_FontIsDualWidth(CONST XFontStruct *info) {
    XCharStruct * p = info->per_char;
    ldat  wide = info->max_bounds.width,
	narrow = info->min_bounds.width,
	i, n_chars, w;
    if (wide != narrow * 2)
	return tfalse;
    if (p == NULL)
	/*
	 * no way to check per-character bounding box. hope for the best... 
	 * at least GNU unifont falls in this case.
	 */
	return ttrue;

    n_chars = (ldat)(info->max_byte1 - info->min_byte1 + 1) * (info->max_char_or_byte2 - info->min_char_or_byte2 + 1);
    for (i = 0; i < n_chars; i++) {
	w = p[i].width;
	if (w != 0 && w != narrow && w != wide)
	    return tfalse;
    }
    return ttrue;
}

/* if font is monospaced, return its score. otherwise return MINLDAT */
static ldat X11_MonospaceFontScore(CONST XFontStruct *info, udat fontwidth, udat fontheight, ldat best_score) {
    ldat score = TW_MINLDAT,
        width = info->min_bounds.width,
        height = (ldat)info->ascent + info->descent,
        max_width = info->max_bounds.width;
    
    if (width == max_width || X11_FontIsDualWidth(info)) {
        score = calcFontScore(fontwidth, fontheight, width, height);
        if (score > best_score)
            printk("      candidate font %ux%u score %ld\n", (unsigned)width, (unsigned)height, (long)score);
    }
    return score;
}

/* return name of selected font in allocated (char *) */
static char * X11_AutodetectFont(udat fontwidth, udat fontheight) {
    struct {
        CONST char * wildcard;
        ldat score_adj;
    } patterns[] = {
        /* { "-gnu-unifont-medium-r-normal-*-%s?-*-*-*-*-*-iso10646-1",  0 }, double-width chars not supported yet */
        { "-misc-console-medium-r-normal-*-%s?-*-*-*-*-*-iso10646-1", 0 },
        { "-misc-fixed-medium-r-normal-*-%s?-*-*-*-*-*-iso10646-1", 0 },
        { "-*-*-medium-r-normal-*-%s?-*-*-*-*-*-iso10646-1",        0 },
        { "-*-*-medium-r-normal-*-%s?-*-*-*-*-*-*-cp437",          -5 },
        { "-*-*-medium-r-normal-*-%s?-*-*-*-*-*-*-cp850",          -5 },
        { "-*-*-medium-r-normal-*-%s?-*-*-*-*-*-ibm-850",          -5 },
        { "-*-*-medium-r-normal-*-%s?-*-*-*-*-*-iso8859-*",       -10 },
    };
    enum { max_fonts = 1000, n_patterns = sizeof(patterns)/sizeof(patterns[0]) };
    
    XFontStruct *info;
    int i, j, k, n_fonts;
    
    char * pattern = AllocMem(LenStr(patterns[0].wildcard) + 1 + 3 * sizeof(unsigned));
    char digits[1 + 3 * sizeof(unsigned)];
    char ** names = NULL;
    char * best = NULL;
    ldat score, best_score = TW_MINLDAT;
    byte beatable_score = ttrue, look_up = fontheight >= 10 && (fontheight % 10) >= 5;
    if (!pattern)
        return NULL;
    
    for (i = 0; i < n_patterns && beatable_score; i++) {
        ldat score_adj = patterns[i].score_adj;

        beatable_score = best_score <= MAX_FONT_SCORE + score_adj;
        
        for (j = 0; j < 2 && beatable_score; j++) {
            n_fonts = 0;
            info = NULL;
            sprintf(digits, "%u", (unsigned)(fontheight / 10 + (j != 0 ? look_up ? 1 : -1 : 0)));
            sprintf(pattern, patterns[i].wildcard, digits + (digits[0] == '0'));
            names = XListFontsWithInfo(xdisplay, pattern, max_fonts, &n_fonts, &info);

	    /* printk("%4d fonts match '%s'\n", names ? n_fonts : 0, pattern); */
            if (names == NULL)
                continue;

            for (k = 0; k < n_fonts && beatable_score; k++)
            {
		/*
		printk("     font        '%s'\t direction = %u, min_byte1 = %u, max_byte1 = %u, min_char_or_byte2 = %d, max_char_or_byte2 = %d\n",
		       names[k], info[k].direction, info[k].min_byte1, info[k].max_byte1, info[k].min_char_or_byte2, info[k].max_char_or_byte2);
		 */

                if (info[k].direction == FontLeftToRight
                    && info[k].min_byte1 == 0
                    && info[k].min_char_or_byte2 <= 32)
                {
                    score = X11_MonospaceFontScore(&info[k], fontwidth, fontheight, best_score - score_adj) + score_adj;
                    if (score <= best_score)
                        continue;
            
                    best_score = score;
                    beatable_score = best_score < MAX_FONT_SCORE + score_adj;

                    FreeMem(best);
                    best = CloneStr(names[k]);
                }
            }
            XFreeFontInfo(names, info, n_fonts);
        }
    }
    FreeMem(pattern);
    return best;
}
