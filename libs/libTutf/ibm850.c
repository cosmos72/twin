
hwfont Tutf_IBM850_to_UTF_16[0x100] = {
#define EL(x) T_UTF(UTF_16,x),
	T_LIST(IBM850,EL)
#undef EL
};

static byte flag_IBM850;

static utf_to_ch array_IBM850 [] = {
#define EL(x) { T_UTF(UTF_16,x), T_UTF(IBM850,x) },
    T_NLIST(IBM850,EL)
#undef EL
    { T_UTF_16_IDENTICAL_TO, '=' },
    { T_UTF_16_CHECK_MARK, 'V' },
};

hwfont Tutf_UTF_16_to_IBM850(hwfont c) {
    utf_to_ch key, *res;
    
    /* Codepage 850 (VGA) obviously cannot contain all unicode chars. this is just a best effort. */
    if (!flag_IBM850) {
	flag_IBM850 = TRUE;
	QSORT(array_IBM850);
    }
    key.utf = c;
    res = BSEARCH(&key, array_IBM850);
    
    if (res)
	c = res->ch;
    else if (c > 0x7E /*'~'*/)
	/* try to approximate */
	c = T_CAT(Tutf_IBM437_to_,T_MAP(ASCII)) [ Tutf_UTF_16_to_IBM437(c) ];
    /* else c = c; */

    return c;
}

