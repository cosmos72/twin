/* basic X-11 version of X11_UTF_32_to_charset_function */
static Tutf_function X11_UTF_32_to_charset_function(CONST byte *charset) {
    XFontProp *fp;
    unsigned long prop;
    CONST byte *s, *fontname = NULL;
    uldat i;
    
    if (!charset) {
	/* attempt to autodetect encoding from fontname */
        if (XGetFontProperty(xsfont, XA_FONT, &prop))
            fontname = XGetAtomName(xdisplay, (Atom)prop);

	if (fontname && !strcmp(fontname, "vga")) {
	    charset = T_NAME_CP437;
	} else if (fontname) {
	    i = 2;
	    for (s = fontname + strlen(fontname) - 1; i && s >= fontname; s--) {
		if (*s == '-')
		    i--;
	    }
	    if (!i)
		charset = s + 2; /* skip current char and '-' */
	}
        
	if (!charset) {
	    if (xsfont->min_byte1 < xsfont->max_byte1) {
		/* font is more than just 8-bit. For now, assume it's unicode */
		printk("    X11_InitHW: font `%."STR(TW_SMALLBUFF)"s\' has no known charset encoding,\n"
		       "                assuming Unicode.\n", fontname);
		return NULL;
	    }
	    /* else assume codepage437. gross. */
	    printk("    X11_InitHW: font `%."STR(TW_SMALLBUFF)"s\' has no known charset encoding,\n"
		   "                assuming CP437 codepage (\"VGA\").\n", fontname);
	    return Tutf_UTF_32_to_CP437;
	}
    }
    
    i = Tutf_charset_id(charset);
    s = Tutf_charset_name(i);
    if (s && !strcmp(s, T_NAME_UTF_32)) {
	/* this is an Unicode font. good. */
	return NULL;
    }
    
    if (i == (uldat)-1) {
	printk("      X11_InitHW(): libTutf warning: unknown charset `%." STR(TW_SMALLBUFF) "s', assuming `CP437'\n", charset);
	return Tutf_UTF_32_to_CP437;
    }
    
    return Tutf_UTF_32_to_charset_function(i);
}
