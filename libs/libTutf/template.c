

#define T_TEMPLATE T_MAP(TEMPLATE)

hwfont T_CAT(T_CAT(Tutf_,T_TEMPLATE),_to_UTF_16)[0x100] = {
#define EL(x) T_UTF(UTF_16,x),
    T_LIST(T_TEMPLATE,EL)
#undef EL
};

static byte T_CAT(flag_,T_TEMPLATE);

static utf_to_ch T_CAT(array_,T_TEMPLATE) [] = {
#define EL(x) { T_UTF(UTF_16,x), T_UTF(T_TEMPLATE,x) },
    T_NLIST(T_TEMPLATE,EL)
#undef EL
};

hwfont T_CAT(Tutf_UTF_16_to_,T_TEMPLATE)(hwfont c) {
    static utf_to_ch key;
    utf_to_ch *res;
    
    /* T_TEMPLATE obviously cannot contain all unicode chars. this is just a best effort. */
    if (!T_CAT(flag_,T_TEMPLATE)) {
	T_CAT(flag_,T_TEMPLATE) = TRUE;
	QSORT(T_CAT(array_,T_TEMPLATE));
    }
    if (c == key.utf)
	return key.ch;
    if ((c & ~0x00ff) == 0xf000 ||
	/* direct-to-font area */
	(c >= ' ' && c <= '~') ||
	/* ASCII area */
	(c > '~' && c < 0x100 && T_CAT(T_CAT(Tutf_,T_TEMPLATE),_to_UTF_16)[c] == c))
	/* c has the same meaning in Unicode and this charset... sheer luck! */

	return c & 0x00ff;
    
    key.utf = c;
    res = BSEARCH(&key, T_CAT(array_,T_TEMPLATE));
    
    if (res)
	c = res->ch;
    else if (c > '~')
	/* try to approximate */
	c = T_CAT(Tutf_IBM437_to_,T_MAP(ASCII)) [ Tutf_UTF_16_to_IBM437(c) ];
    /* else c = c; */

    return key.ch = c;
}

#undef T_TEMPLATE
