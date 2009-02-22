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

#ifndef _TT_FLAGS_H

#include <TT/TTflags.h>

#ifndef _TT_FLAGS_H
#define _TT_FLAGS_H
#endif


#define ttmask(n)			((ttany)1<<(n))



/*
 * from <Tw/datatypes.h> :
 * MINS(t) = ((t)((t)1<<(8*sizeof(t)-1)))
 */
#define ttobject_refcount_alive		MINS(ttopaque)
/** object is in non-dynamic memory and must not be free()d */
#define ttobject_oflags_static		1
/** object is in read-only memory and must not be modified */
#define ttobject_oflags_const		2


#define ttevent_evflags_consumed	MINS(ttuint)
#define ttevent_evflags_firing		(MINS(ttuint)>>1)

#define ttbitmask_oflags_mask_reverse	2

#define ttlistener_lflags_ttmethod	MINS(ttuint)
#define ttlistener_lflags_args_swallow	(MINS(ttuint)>>1)



/** ttbutton index to access shape for normal state */
#define ttbutton_shape_normal		0
/** ttbutton index to access shape for prelight state */
#define ttbutton_shape_prelight		1
/** ttbutton index to access shape for pressed state */
#define ttbutton_shape_pressed		2
/** ttbutton index to access shape for disabled state */
#define ttbutton_shape_disabled		3
#define ttbutton_shape_max		4


#define ttcheckbutton_shape_normal		ttbutton_shape_normal
#define ttcheckbutton_shape_prelight		ttbutton_shape_prelight
#define ttcheckbutton_shape_pressed		ttbutton_shape_pressed
#define ttcheckbutton_shape_disabled		ttbutton_shape_disabled
#define ttcheckbutton_shape_checked_normal	4
#define ttcheckbutton_shape_checked_prelight	5
#define ttcheckbutton_shape_checked_pressed	6
#define ttcheckbutton_shape_checked_disabled	7
#define ttcheckbutton_shape_max			8


#define ttradiobutton_shape_normal		ttbutton_shape_normal
#define ttradiobutton_shape_prelight		ttbutton_shape_prelight
#define ttradiobutton_shape_pressed		ttbutton_shape_pressed
#define ttradiobutton_shape_disabled		ttbutton_shape_disabled
#define ttradiobutton_shape_checked_normal	4
#define ttradiobutton_shape_checked_prelight	5
#define ttradiobutton_shape_checked_pressed	6
#define ttradiobutton_shape_checked_disabled	7
#define ttradiobutton_shape_max			8


#define ttscrollbar_shape_normal		0
#define ttscrollbar_shape_prelight		1
#define ttscrollbar_shape_pressed		2
#define ttscrollbar_shape_disabled		3
#define ttscrollbar_shape_max			4


#define ttslider_shape_normal			0
#define ttslider_shape_prelight			1
#define ttslider_shape_pressed			2
#define ttslider_shape_disabled			3
#define ttslider_shape_max			4

#define ttprogressbar_shape_normal		0
#define ttprogressbar_shape_disabled		1
#define ttprogressbar_shape_max			2


#define ttderived_ttanybutton_shape_max		8


#define tttheme_shape_ttbutton_normal			0
#define tttheme_shape_ttbutton_prelight			1
#define tttheme_shape_ttbutton_pressed			2
#define tttheme_shape_ttbutton_disabled			3

#define tttheme_shape_ttcheckbutton_normal		4
#define tttheme_shape_ttcheckbutton_prelight		5
#define tttheme_shape_ttcheckbutton_pressed		6
#define tttheme_shape_ttcheckbutton_disabled		7
#define tttheme_shape_ttcheckbutton_checked_normal	8
#define tttheme_shape_ttcheckbutton_checked_prelight	9
#define tttheme_shape_ttcheckbutton_checked_pressed	0xA
#define tttheme_shape_ttcheckbutton_checked_disabled	0xB

#define tttheme_shape_ttradiobutton_normal		0xC
#define tttheme_shape_ttradiobutton_prelight		0xD
#define tttheme_shape_ttradiobutton_pressed		0xE
#define tttheme_shape_ttradiobutton_disabled		0xF
#define tttheme_shape_ttradiobutton_checked_normal	0x10
#define tttheme_shape_ttradiobutton_checked_prelight	0x11
#define tttheme_shape_ttradiobutton_checked_pressed	0x12
#define tttheme_shape_ttradiobutton_checked_disabled	0x13

#define tttheme_shape_ttscrollbar_x_normal		0x14
#define tttheme_shape_ttscrollbar_x_prelight		0x15
#define tttheme_shape_ttscrollbar_x_pressed		0x16
#define tttheme_shape_ttscrollbar_x_disabled		0x17

#define tttheme_shape_ttscrollbar_y_normal		0x18
#define tttheme_shape_ttscrollbar_y_prelight		0x19
#define tttheme_shape_ttscrollbar_y_pressed		0x1A
#define tttheme_shape_ttscrollbar_y_disabled		0x1B

#define tttheme_shape_ttslider_x_normal			0x1C
#define tttheme_shape_ttslider_x_prelight		0x1D
#define tttheme_shape_ttslider_x_pressed		0x1E
#define tttheme_shape_ttslider_x_disabled		0x1F

#define tttheme_shape_ttslider_y_normal			0x20
#define tttheme_shape_ttslider_y_prelight		0x21
#define tttheme_shape_ttslider_y_pressed		0x22
#define tttheme_shape_ttslider_y_disabled		0x23

#define tttheme_shape_ttprogressbar_x_normal		0x24
#define tttheme_shape_ttprogressbar_x_disabled		0x25

#define tttheme_shape_ttprogressbar_y_normal		0x26
#define tttheme_shape_ttprogressbar_y_disabled		0x27

#define tttheme_shape_max				0x28


#define tttheme_val_ttscrollbar_x_arrow_rev_min		0
#define tttheme_val_ttscrollbar_x_arrow_rev_max		1
#define tttheme_val_ttscrollbar_x_arrow_fwd_min		2
#define tttheme_val_ttscrollbar_x_arrow_fwd_max		3
#define tttheme_val_ttscrollbar_y_arrow_rev_min		4
#define tttheme_val_ttscrollbar_y_arrow_rev_max		5
#define tttheme_val_ttscrollbar_y_arrow_fwd_min		6
#define tttheme_val_ttscrollbar_y_arrow_fwd_max		7

#define tttheme_val_max					8


#define tttheme_bg_normal		0
#define tttheme_bg_disable		1
#define tttheme_bg_menu_normal		2
#define tttheme_bg_menu_select		3
#define tttheme_bg_menu_disabled	4
#define tttheme_bg_menu_selectdisabled	5
#define tttheme_bg_menu_shcut		6
#define tttheme_bg_menu_selectshcut	7
#define tttheme_bg_label		8
#define tttheme_bg_anybutton		9
#define tttheme_bg_max		       10


#define tt_x_left			0
#define tt_y_up				1
#define tt_x_right			2
#define tt_y_down			3


#endif /* _TT_FLAGS_H */
