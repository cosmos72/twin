

hwfont T_CAT(T_CAT(Tutf_,T_MAP(ISO_8859_X)),_to_UTF_16)[0x100] = {
#define EL(x) T_UTF(UTF_16,x),
    T_LIST(ISO_8859_X,EL)
#undef EL
};

static byte T_CAT(flag_,T_MAP(ISO_8859_X));

static utf_to_ch T_CAT(array_,T_MAP(ISO_8859_X)) [] = {
#define EL(x) { T_UTF(UTF_16,x), T_UTF(ISO_8859_X,x) },
    T_NLIST(ISO_8859_X,EL)
#undef EL

#define BOX_(x,y) { T_UTF(UTF_16,T_CAT(_BOX_DRAWINGS_LIGHT,x)), y }, \
    { T_UTF(UTF_16,T_CAT(_BOX_DRAWINGS_HEAVY,x)), y }, \
    { T_UTF(UTF_16,T_CAT(_BOX_DRAWINGS_DOUBLE,x)), y }
	
    BOX_(_UP_AND_LEFT, '+'),
    BOX_(_DOWN_AND_LEFT, '+'),
    BOX_(_DOWN_AND_RIGHT, '+'),
    BOX_(_UP_AND_RIGHT, '+'),
    BOX_(_VERTICAL_AND_HORIZONTAL, '+'),
    BOX_(_VERTICAL_AND_RIGHT, '+'),
    BOX_(_VERTICAL_AND_LEFT, '+'),
    BOX_(_UP_AND_HORIZONTAL, '+'),
    BOX_(_DOWN_AND_HORIZONTAL, '+'),
    BOX_(_VERTICAL, '|'),

#undef BOX_
    
    { T_UTF_16_LIGHT_SHADE,	'+' },
    { T_UTF_16_MEDIUM_SHADE,	'*' },
    { T_UTF_16_DARK_SHADE,	'@' },
    { T_UTF_16_FULL_BLOCK,	'#' },
    { T_UTF_16_BOX_DRAWINGS_LIGHT_HORIZONTAL,	'-' },
    { T_UTF_16_BOX_DRAWINGS_HEAVY_HORIZONTAL,	'-' },
    { T_UTF_16_BOX_DRAWINGS_DOUBLE_HORIZONTAL,	'=' },
    { T_UTF_16_UP_DOWN_ARROW,	'|' },
    { T_UTF_16_BLACK_RIGHT_POINTING_TRIANGLE,	'>' },
    { T_UTF_16_BLACK_LEFT_POINTING_TRIANGLE,	'<' },
    { T_UTF_16_BLACK_UP_POINTING_TRIANGLE,	'^' },
    { T_UTF_16_BLACK_DOWN_POINTING_TRIANGLE,	'v' },
    { T_UTF_16_UPPER_HALF_BLOCK,	'"' },
    { T_UTF_16_LOWER_HALF_BLOCK,	'_' },
};

hwfont T_CAT(Tutf_UTF_16_to_,T_MAP(ISO_8859_X))(hwfont c) {
    utf_to_ch key, *res;
    
    /* ISO_8859_X obviously cannot contain all unicode chars. this is just a best effort. */
    if (!T_CAT(flag_,T_MAP(ISO_8859_X))) {
	T_CAT(flag_,T_MAP(ISO_8859_X)) = TRUE;
	QSORT(T_CAT(array_,T_MAP(ISO_8859_X)));
    }

    key.utf = c;
    res = BSEARCH(&key, T_CAT(array_,T_MAP(ISO_8859_X)));
    
    if (res)
	c = res->ch;
    else if (c > 0xFF /*'~'*/)
	/* try to approximate */
	c = T_CAT(Tutf_IBM437_to_,T_MAP(ASCII)) [ Tutf_UTF_16_to_IBM437(c) ];
    /* else c = c; */

    return c;
}

