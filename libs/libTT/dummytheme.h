
static s_ttfn_tttheme dummy_TTFN_tttheme;

#define g    COL(GREEN,WHITE)
#define b    COL(BLACK,BLUE)
#define t(f) HWATTR(g, f)
#define s(f) HWATTR(b, f)

static ttattr t_normalbutton[] = {
    t(' '), 0,
    0, 0
};

static ttattr s_normalbutton[] = {
    0, s(T_UTF_16_LOWER_HALF_BLOCK),
    0, s(T_UTF_16_FULL_BLOCK),
    s(' '), s(T_UTF_16_UPPER_HALF_BLOCK)
};

#undef  g
#undef  b
#define g    COL(HIGH|GREEN,WHITE)
#define b    COL(HIGH|BLACK,BLUE)

static ttattr t_prelightbutton[] = {
    t(' '), 0,
    0, 0
};
static ttattr s_prelightbutton[] = {
    0, s(T_UTF_16_LOWER_HALF_BLOCK),
    0, s(T_UTF_16_FULL_BLOCK),
    s(' '), s(T_UTF_16_UPPER_HALF_BLOCK)
};

#undef  g
#undef  b
#define g    COL(GREEN,WHITE)
#define b    COL(BLACK,BLUE)

static ttattr t_pressedbutton[] = {
    0, t(' '),
    0, 0
};
static ttattr s_pressedbutton[] = {
    s(' '), 0,
    s(' '), s(' ')
};

#undef  g
#undef  b
#define g    COL(HIGH|BLACK,WHITE)
#define b    COL(BLACK,BLUE)

static ttattr t_disabledbutton[] = {
    t(' '), 0,
    0, 0
};
static ttattr s_disabledbutton[] = {
    0, s(' '),
    s(' '), s(' ')
};

#undef  g
#undef  b

static s_tttheme s_DummyTheme = {
    &dummy_TTFN_tttheme,
    TT_NOID,
    (ttuint)0,
    (ttuint)0,
    (ttany)0,
    (ttany)0,
    (ttany)0,
    (ttuint)0,
    (ttlistener)0,
    (ttcallback)0,
    NULL,
    HWFONT(' '),
    {
	COL(WHITE,BLUE),
	COL(HIGH|BLACK,BLUE),
	COL(BLACK,WHITE),
	COL(BLACK,GREEN),
	COL(HIGH|BLACK,WHITE),
	COL(HIGH|BLACK,BLACK),
	COL(RED,WHITE),
	COL(RED,GREEN),
    },
    {
	{ { 2, 2, { 0, 0, 1, 1 }, t_normalbutton   }, { 2, 3, { 0, 1, 1, 1 }, s_normalbutton   }, },
	{ { 2, 2, { 0, 0, 1, 1 }, t_prelightbutton }, { 2, 3, { 0, 1, 1, 1 }, s_prelightbutton }, },
	{ { 2, 2, { 1, 0, 0, 1 }, t_pressedbutton  }, { 2, 2, { 1, 0, 0, 1 }, s_pressedbutton  }, },
	{ { 2, 2, { 0, 0, 1, 1 }, t_disabledbutton }, { 2, 2, { 0, 0, 1, 1 }, s_disabledbutton }, },
    },
};

