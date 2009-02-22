/*
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 */


#define t(c) HWATTR(f, c)
#define s(c) HWATTR(b, c)


/***********************************/


#define f    COL(HIGH|WHITE,GREEN)
#define b    COL(BLACK,WHITE)


static ttattr t_button_normal[] = {
    t(' '), 0,
    0, 0
};
static ttattr s_button_normal[] = {
    0, 0, s(T_UTF_16_LOWER_HALF_BLOCK),
    0, 0, s(T_UTF_16_FULL_BLOCK),
    s(' '), s(T_UTF_16_UPPER_HALF_BLOCK), s(T_UTF_16_UPPER_HALF_BLOCK)
};

static ttattr t_checkbutton_normal[] = {
    t('['), t(' '), t(']'), t(' '), 0,
    0, 0, 0, 0, 0
};
static ttattr t_checkbutton_checked_normal[] = {
    t('['), t(T_UTF_16_CHECK_MARK), t(']'), t(' '), 0,
    0, 0, 0, 0, 0
};
#define s_checkbutton_normal         s_button_normal
#define s_checkbutton_checked_normal s_button_normal

static ttattr t_radiobutton_normal[] = {
    t('('), t(' '), t(')'), t(' '), 0,
    0, 0, 0, 0, 0
};
static ttattr t_radiobutton_checked_normal[] = {
    t('('), t('*'), t(')'), t(' '), 0,
    0, 0, 0, 0, 0
};
#define s_radiobutton_normal         s_button_normal
#define s_radiobutton_checked_normal s_button_normal


/***********************************/


#undef  f
#undef  b
#define f    COL(HIGH|WHITE,GREEN)
#define b    COL(HIGH|BLACK,WHITE)


#define t_button_prelight t_button_normal
static ttattr s_button_prelight[] = {
    0, 0, s(T_UTF_16_LOWER_HALF_BLOCK),
    0, 0, s(T_UTF_16_FULL_BLOCK),
    s(' '), s(T_UTF_16_UPPER_HALF_BLOCK), s(T_UTF_16_UPPER_HALF_BLOCK)
};

#define t_checkbutton_prelight         t_checkbutton_normal
#define t_checkbutton_checked_prelight t_checkbutton_checked_normal
#define s_checkbutton_prelight         s_button_prelight
#define s_checkbutton_checked_prelight s_button_prelight

#define t_radiobutton_prelight         t_radiobutton_normal
#define t_radiobutton_checked_prelight t_radiobutton_checked_normal
#define s_radiobutton_prelight         s_button_prelight
#define s_radiobutton_checked_prelight s_button_prelight


/***********************************/


#undef  f
#undef  b
#define f    COL(HIGH|WHITE,GREEN)
#define b    COL(BLACK,WHITE)


static ttattr t_button_pressed[] = {
    0, t(' '),
    0, 0
};
static ttattr s_button_pressed[] = {
    s(' '), 0,
    s(' '), s(' ')
};

static ttattr t_checkbutton_pressed[] = {
    0, t('['), t(' '), t(']'), t(' '), 
    0, 0, 0, 0, 0, 0
};
static ttattr t_checkbutton_checked_pressed[] = {
    0, t('['), t(T_UTF_16_CHECK_MARK), t(']'), t(' '),
    0, 0, 0, 0, 0
};
#define s_checkbutton_pressed         s_button_pressed
#define s_checkbutton_checked_pressed s_button_pressed

static ttattr t_radiobutton_pressed[] = {
    0, t('('), t(' '), t(')'), t(' '),
    0, 0, 0, 0, 0
};
static ttattr t_radiobutton_checked_pressed[] = {
    0, t('('), t('*'), t(')'), t(' '),
    0, 0, 0, 0, 0
};
#define s_radiobutton_pressed         s_button_pressed
#define s_radiobutton_checked_pressed s_button_pressed


/***********************************/


#undef  f
#undef  b
#define f    COL(HIGH|BLACK,GREEN)
#define b    COL(BLACK,WHITE)


static ttattr t_button_disabled[] = {
    t(' '), 0,
    0, 0
};
#define s_button_disabled         s_button_normal

static ttattr t_checkbutton_disabled[] = {
    t('['), t(' '), t(']'), t(' '), 0,
    0, 0, 0, 0, 0
};
static ttattr t_checkbutton_checked_disabled[] = {
    t('['), t(T_UTF_16_CHECK_MARK), t(']'), t(' '), 0,
    0, 0, 0, 0, 0
};
#define s_checkbutton_disabled         s_button_disabled
#define s_checkbutton_checked_disabled s_button_disabled

static ttattr t_radiobutton_disabled[] = {
    t('('), t(' '), t(')'), t(' '), 0,
    0, 0, 0, 0, 0
};
static ttattr t_radiobutton_checked_disabled[] = {
    t('('), t('*'), t(')'), t(' '), 0,
    0, 0, 0, 0, 0
};
#define s_radiobutton_disabled         s_button_disabled
#define s_radiobutton_checked_disabled s_button_disabled


/***********************************/


#undef  f
#undef  b
#define f    COL(HIGH|GREEN,BLUE)
#define b    COL(HIGH|WHITE,BLUE)


static ttattr t_scrollbar_x_normal[] = {
    s(T_UTF_16_MEDIUM_SHADE), t(T_UTF_16_BLACK_LEFT_POINTING_TRIANGLE), t(T_UTF_16_BLACK_RIGHT_POINTING_TRIANGLE),
};
static ttattr t_scrollbar_y_normal[] = {
    s(T_UTF_16_MEDIUM_SHADE), t(T_UTF_16_BLACK_UP_POINTING_TRIANGLE), t(T_UTF_16_BLACK_DOWN_POINTING_TRIANGLE),
};
static ttattr s_scrollbar_x_normal[] = {
    s(T_UTF_16_FULL_BLOCK),
};
#define s_scrollbar_y_normal s_scrollbar_x_normal


/***********************************/


#undef  f
#undef  b
#define f    COL(HIGH|GREEN,HIGH|BLUE)
#define b    COL(HIGH|WHITE,HIGH|BLUE)


static ttattr t_scrollbar_x_prelight[] = {
    s(T_UTF_16_MEDIUM_SHADE), t(T_UTF_16_BLACK_LEFT_POINTING_TRIANGLE), t(T_UTF_16_BLACK_RIGHT_POINTING_TRIANGLE),
};
static ttattr t_scrollbar_y_prelight[] = {
    s(T_UTF_16_MEDIUM_SHADE), t(T_UTF_16_BLACK_UP_POINTING_TRIANGLE), t(T_UTF_16_BLACK_DOWN_POINTING_TRIANGLE),
};
static ttattr s_scrollbar_x_prelight[] = {
    s(T_UTF_16_FULL_BLOCK),
};
#define s_scrollbar_y_prelight s_scrollbar_x_prelight


/***********************************/


#undef  f
#undef  b
#define f    COL(HIGH|BLUE,HIGH|GREEN)
#define b    COL(HIGH|BLUE,HIGH|WHITE)


static ttattr t_scrollbar_x_pressed[] = {
    s(T_UTF_16_MEDIUM_SHADE), t(T_UTF_16_BLACK_LEFT_POINTING_TRIANGLE), t(T_UTF_16_BLACK_RIGHT_POINTING_TRIANGLE),
};
static ttattr t_scrollbar_y_pressed[] = {
    s(T_UTF_16_MEDIUM_SHADE), t(T_UTF_16_BLACK_UP_POINTING_TRIANGLE), t(T_UTF_16_BLACK_DOWN_POINTING_TRIANGLE),
};
static ttattr s_scrollbar_x_pressed[] = {
    s(T_UTF_16_FULL_BLOCK),
};
#define s_scrollbar_y_pressed s_scrollbar_x_pressed


/***********************************/


#undef  f
#undef  b
#define f    COL(HIGH|BLACK,BLUE)
#define b    COL(HIGH|BLACK,BLUE)


static ttattr t_scrollbar_x_disabled[] = {
    s(T_UTF_16_MEDIUM_SHADE), t(T_UTF_16_BLACK_LEFT_POINTING_TRIANGLE), t(T_UTF_16_BLACK_RIGHT_POINTING_TRIANGLE),
};
static ttattr t_scrollbar_y_disabled[] = {
    s(T_UTF_16_MEDIUM_SHADE), t(T_UTF_16_BLACK_UP_POINTING_TRIANGLE), t(T_UTF_16_BLACK_DOWN_POINTING_TRIANGLE),
};
static ttattr s_scrollbar_x_disabled[] = {
    s(T_UTF_16_FULL_BLOCK),
};
#define s_scrollbar_y_disabled s_scrollbar_x_disabled


/***********************************/


#undef  f
#undef  b
#define f    COL(HIGH|GREEN,BLUE)
#define b    COL(HIGH|WHITE,BLUE)


static ttattr t_slider_x_normal[] = {
    s(T_UTF_16_MEDIUM_SHADE),
};
#define t_slider_y_normal t_slider_x_normal
static ttattr s_slider_x_normal[] = {
    t(T_UTF_16_BLACK_SQUARE),
};
#define s_slider_y_normal s_slider_x_normal


/***********************************/


#undef  f
#undef  b
#define f    COL(HIGH|GREEN,HIGH|BLUE)
#define b    COL(HIGH|WHITE,HIGH|BLUE)


static ttattr t_slider_x_prelight[] = {
    s(T_UTF_16_MEDIUM_SHADE),
};
#define t_slider_y_prelight t_slider_x_prelight
static ttattr s_slider_x_prelight[] = {
    t(T_UTF_16_BLACK_SQUARE),
};
#define s_slider_y_prelight s_slider_x_prelight


/***********************************/


#undef  f
#undef  b
#define f    COL(HIGH|BLUE,HIGH|GREEN)
#define b    COL(HIGH|BLUE,HIGH|WHITE)


static ttattr t_slider_x_pressed[] = {
    s(T_UTF_16_MEDIUM_SHADE),
};
#define t_slider_y_pressed t_slider_x_pressed
static ttattr s_slider_x_pressed[] = {
    t(T_UTF_16_BLACK_SQUARE),
};
#define s_slider_y_pressed s_slider_x_pressed


/***********************************/


#undef  f
#undef  b
#define f    COL(HIGH|BLACK,BLUE)
#define b    COL(HIGH|BLACK,BLUE)


static ttattr t_slider_x_disabled[] = {
    s(T_UTF_16_MEDIUM_SHADE),
};
#define t_slider_y_disabled t_slider_x_disabled
static ttattr s_slider_x_disabled[] = {
    t(T_UTF_16_BLACK_SQUARE),
};
#define s_slider_y_disabled s_slider_x_disabled


/***********************************/


#undef  f
#undef  b
#define f    COL(HIGH|GREEN,BLUE)
#define b    COL(HIGH|WHITE,BLUE)


static ttattr t_progressbar_x_normal[] = {
    s(' '),
    s(T_UTF_16_LEFT_ONE_EIGHTH_BLOCK),
    s(T_UTF_16_LEFT_ONE_QUARTER_BLOCK),
    s(T_UTF_16_LEFT_THREE_EIGHTHS_BLOCK),
    s(T_UTF_16_LEFT_HALF_BLOCK),
    s(T_UTF_16_LEFT_FIVE_EIGHTHS_BLOCK),
    s(T_UTF_16_LEFT_THREE_QUARTERS_BLOCK),
    s(T_UTF_16_LEFT_SEVEN_EIGHTHS_BLOCK),
    s(T_UTF_16_FULL_BLOCK),
};
static ttattr t_progressbar_y_normal[] = {
    s(' '),
    s(T_UTF_16_LOWER_ONE_EIGHTH_BLOCK),
    s(T_UTF_16_LOWER_ONE_QUARTER_BLOCK),
    s(T_UTF_16_LOWER_THREE_EIGHTHS_BLOCK),
    s(T_UTF_16_LOWER_HALF_BLOCK),
    s(T_UTF_16_LOWER_FIVE_EIGHTHS_BLOCK),
    s(T_UTF_16_LOWER_THREE_QUARTERS_BLOCK),
    s(T_UTF_16_LOWER_SEVEN_EIGHTHS_BLOCK),
    s(T_UTF_16_FULL_BLOCK),
};

static ttattr s_progressbar_x_normal[] = {
    t(' '),
};
#define s_progressbar_y_normal s_progressbar_x_normal


/***********************************/



#undef  f
#undef  b
#define f    COL(HIGH|BLACK,BLUE)
#define b    COL(HIGH|BLACK,BLUE)


static ttattr t_progressbar_x_disabled[] = {
    s(' '),
    s(T_UTF_16_LEFT_ONE_EIGHTH_BLOCK),
    s(T_UTF_16_LEFT_ONE_QUARTER_BLOCK),
    s(T_UTF_16_LEFT_THREE_EIGHTHS_BLOCK),
    s(T_UTF_16_LEFT_HALF_BLOCK),
    s(T_UTF_16_LEFT_FIVE_EIGHTHS_BLOCK),
    s(T_UTF_16_LEFT_THREE_QUARTERS_BLOCK),
    s(T_UTF_16_LEFT_SEVEN_EIGHTHS_BLOCK),
    s(T_UTF_16_FULL_BLOCK),
};
static ttattr t_progressbar_y_disabled[] = {
    s(' '),
    s(T_UTF_16_LOWER_ONE_EIGHTH_BLOCK),
    s(T_UTF_16_LOWER_ONE_QUARTER_BLOCK),
    s(T_UTF_16_LOWER_THREE_EIGHTHS_BLOCK),
    s(T_UTF_16_LOWER_HALF_BLOCK),
    s(T_UTF_16_LOWER_FIVE_EIGHTHS_BLOCK),
    s(T_UTF_16_LOWER_THREE_QUARTERS_BLOCK),
    s(T_UTF_16_LOWER_SEVEN_EIGHTHS_BLOCK),
    s(T_UTF_16_FULL_BLOCK),
};

static ttattr s_progressbar_x_disabled[] = {
    t(' '),
};
#define s_progressbar_y_disabled s_progressbar_x_disabled


/***********************************/


#undef  f
#undef  b


/***********************************/


static s_tttheme s_DummyTheme, s_RealDummyTheme = {
    (ttclass_tttheme)0,
    TT_NOID,
    (ttuint)0,
    (ttuint)0,
    (ttany)0,
    (ttany)0,
    (ttany)0,
    (ttopaque)0,
    (ttopaque)0,
	
    (ttcallback)0,
    (ttlistener)0,
    (tttimer)0,
    (ttdata)0,

    (ttopaque)0,
    NULL,
    HWFONT(' '),
    {
	COL(BLACK,WHITE),
	COL(HIGH|BLACK,BLACK),
	COL(BLACK,WHITE),
	COL(BLACK,GREEN),
	COL(HIGH|BLACK,WHITE),
	COL(HIGH|BLACK,BLACK),
	COL(RED,WHITE),
	COL(RED,GREEN),
	COL(HIGH|WHITE,WHITE),
	COL(HIGH|GREEN,WHITE),
    },
    {
	-2, -2, -1, -1, -2, -2, -1, -1,
    },
    {
	/* 0 */
	{ { 2, 2, { 0, 0, 1, 1 }, t_button_normal   }, { 3, 3, { 1, 1, 1, 1 }, s_button_normal   }, },
	{ { 2, 2, { 0, 0, 1, 1 }, t_button_prelight }, { 3, 3, { 1, 1, 1, 1 }, s_button_prelight }, },
	{ { 2, 2, { 1, 0, 0, 1 }, t_button_pressed  }, { 2, 2, { 1, 0, 0, 1 }, s_button_pressed  }, },
	{ { 2, 2, { 0, 0, 1, 1 }, t_button_disabled }, { 2, 2, { 0, 0, 1, 1 }, s_button_disabled }, },
	/* 4 */
	{ { 5, 2, { 3, 0, 1, 1 }, t_checkbutton_normal   }, { 3, 3, { 1, 1, 1, 1 }, s_checkbutton_normal   }, },
	{ { 5, 2, { 3, 0, 1, 1 }, t_checkbutton_prelight }, { 3, 3, { 1, 1, 1, 1 }, s_checkbutton_prelight }, },
	{ { 5, 2, { 4, 0, 0, 1 }, t_checkbutton_pressed  }, { 2, 2, { 1, 0, 0, 1 }, s_checkbutton_pressed  }, },
	{ { 5, 2, { 3, 0, 1, 1 }, t_checkbutton_disabled }, { 2, 2, { 0, 0, 1, 1 }, s_checkbutton_disabled }, },
	/* 8 */
	{ { 5, 2, { 3, 0, 1, 1 }, t_checkbutton_checked_normal   }, { 3, 3, { 1, 1, 1, 1 }, s_checkbutton_checked_normal   }, },
	{ { 5, 2, { 3, 0, 1, 1 }, t_checkbutton_checked_prelight }, { 3, 3, { 1, 1, 1, 1 }, s_checkbutton_checked_prelight }, },
	{ { 5, 2, { 4, 0, 0, 1 }, t_checkbutton_checked_pressed  }, { 2, 2, { 1, 0, 0, 1 }, s_checkbutton_checked_pressed  }, },
	{ { 5, 2, { 3, 0, 1, 1 }, t_checkbutton_checked_disabled }, { 2, 2, { 0, 0, 1, 1 }, s_checkbutton_checked_disabled }, },
	/* 0xC */
	{ { 5, 2, { 3, 0, 1, 1 }, t_radiobutton_normal   }, { 3, 3, { 1, 1, 1, 1 }, s_radiobutton_normal   }, },
	{ { 5, 2, { 3, 0, 1, 1 }, t_radiobutton_prelight }, { 3, 3, { 1, 1, 1, 1 }, s_radiobutton_prelight }, },
	{ { 5, 2, { 4, 0, 0, 1 }, t_radiobutton_pressed  }, { 2, 2, { 1, 0, 0, 1 }, s_radiobutton_pressed  }, },
	{ { 5, 2, { 3, 0, 1, 1 }, t_radiobutton_disabled }, { 2, 2, { 0, 0, 1, 1 }, s_radiobutton_disabled }, },
	/* 0x10 */
	{ { 5, 2, { 3, 0, 1, 1 }, t_radiobutton_checked_normal   }, { 3, 3, { 1, 1, 1, 1 }, s_radiobutton_checked_normal   }, },
	{ { 5, 2, { 3, 0, 1, 1 }, t_radiobutton_checked_prelight }, { 3, 3, { 1, 1, 1, 1 }, s_radiobutton_checked_prelight }, },
	{ { 5, 2, { 4, 0, 0, 1 }, t_radiobutton_checked_pressed  }, { 2, 2, { 1, 0, 0, 1 }, s_radiobutton_checked_pressed  }, },
	{ { 5, 2, { 3, 0, 1, 1 }, t_radiobutton_checked_disabled }, { 2, 2, { 0, 0, 1, 1 }, s_radiobutton_checked_disabled }, },
	/* 0x14 */
	{ { 3, 1, { 0, 0, 2, 0 }, t_scrollbar_x_normal   }, { 1, 1, { 0, 0, 0, 0 }, s_scrollbar_x_normal   }, },
	{ { 3, 1, { 0, 0, 2, 0 }, t_scrollbar_x_prelight }, { 1, 1, { 0, 0, 0, 0 }, s_scrollbar_x_prelight }, },
	{ { 3, 1, { 0, 0, 2, 0 }, t_scrollbar_x_pressed  }, { 1, 1, { 0, 0, 0, 0 }, s_scrollbar_x_pressed  }, },
	{ { 3, 1, { 0, 0, 2, 0 }, t_scrollbar_x_disabled }, { 1, 1, { 0, 0, 0, 0 }, s_scrollbar_x_disabled }, },
	/* 0x18 */
	{ { 1, 3, { 0, 0, 0, 2 }, t_scrollbar_y_normal   }, { 1, 1, { 0, 0, 0, 0 }, s_scrollbar_y_normal   }, },
	{ { 1, 3, { 0, 0, 0, 2 }, t_scrollbar_y_prelight }, { 1, 1, { 0, 0, 0, 0 }, s_scrollbar_y_prelight }, },
	{ { 1, 3, { 0, 0, 0, 2 }, t_scrollbar_y_pressed  }, { 1, 1, { 0, 0, 0, 0 }, s_scrollbar_y_pressed  }, },
	{ { 1, 3, { 0, 0, 0, 2 }, t_scrollbar_y_disabled }, { 1, 1, { 0, 0, 0, 0 }, s_scrollbar_y_disabled }, },
	/* 0x1C */
	{ { 1, 1, { 0, 0, 0, 0 }, t_slider_x_normal   }, { 1, 1, { 0, 0, 0, 0 }, s_slider_x_normal   }, },
	{ { 1, 1, { 0, 0, 0, 0 }, t_slider_x_prelight }, { 1, 1, { 0, 0, 0, 0 }, s_slider_x_prelight }, },
	{ { 1, 1, { 0, 0, 0, 0 }, t_slider_x_pressed  }, { 1, 1, { 0, 0, 0, 0 }, s_slider_x_pressed  }, },
	{ { 1, 1, { 0, 0, 0, 0 }, t_slider_x_disabled }, { 1, 1, { 0, 0, 0, 0 }, s_slider_x_disabled }, },
	/* 0x20 */
	{ { 1, 1, { 0, 0, 0, 0 }, t_slider_y_normal   }, { 1, 1, { 0, 0, 0, 0 }, s_slider_y_normal   }, },
	{ { 1, 1, { 0, 0, 0, 0 }, t_slider_y_prelight }, { 1, 1, { 0, 0, 0, 0 }, s_slider_y_prelight }, },
	{ { 1, 1, { 0, 0, 0, 0 }, t_slider_y_pressed  }, { 1, 1, { 0, 0, 0, 0 }, s_slider_y_pressed  }, },
	{ { 1, 1, { 0, 0, 0, 0 }, t_slider_y_disabled }, { 1, 1, { 0, 0, 0, 0 }, s_slider_y_disabled }, },
	/* 0x24 */
	{ { 8, 1, { 0, 0, 0, 0 }, t_progressbar_x_normal   }, { 1, 1, { 0, 0, 0, 0 }, s_progressbar_x_normal   }, },
	{ { 8, 1, { 0, 0, 0, 0 }, t_progressbar_x_disabled }, { 1, 1, { 0, 0, 0, 0 }, s_progressbar_x_disabled }, },
	{ { 8, 1, { 0, 0, 0, 0 }, t_progressbar_y_normal   }, { 1, 1, { 0, 0, 0, 0 }, s_progressbar_y_normal   }, },
	{ { 8, 1, { 0, 0, 0, 0 }, t_progressbar_y_disabled }, { 1, 1, { 0, 0, 0, 0 }, s_progressbar_y_disabled }, },
	/* 0x28 */
    },
};

