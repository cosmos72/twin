
#ifndef _TT_FLAGS_H

#include <TT/TTflags.h>

#ifndef _TT_FLAGS_H
#define _TT_FLAGS_H
#endif


#define ttmask(n)			((ttuint)1<<(n))


/*
 * from <Tw/datatypes.h> :
 * MINS(t) = ((t)((t)1<<(8*sizeof(t)-1)))
 */
#define ttobj_refcount_alive		MINS(ttopaque)
#define ttobj_oflags_static		1

#define ttevent_evflags_consumed	MINS(ttuint)		

#define ttcallback_lflags_builtin	MINS(ttuint)
#define ttcallback_lflags_args_swallow	(MINS(ttuint)>>1)

#define ttbutton_shape_max		4

#define tttheme_bg_normal		0
#define tttheme_bg_disable		1
#define tttheme_bg_menu			2
#define tttheme_bg_menu_select		3
#define tttheme_bg_menu_disabled	4
#define tttheme_bg_menu_selectdisabled	5
#define tttheme_bg_menu_shcut		6
#define tttheme_bg_menu_selectshcut	7
#define tttheme_bg_max			8

#define tttheme_shape_ttbutton_normal	ttbutton_shape_normal
#define tttheme_shape_ttbutton_prelight	ttbutton_shape_prelight
#define tttheme_shape_ttbutton_pressed	ttbutton_shape_pressed
#define tttheme_shape_ttbutton_disabled	ttbutton_shape_disabled
#define tttheme_shape_max		ttbutton_shape_max

#define tt_x_left			0
#define tt_y_up				1
#define tt_x_right			2
#define tt_y_down			3


#define DispatchEvent_dflags_before	1
#define DispatchEvent_dflags_after	2
#define DispatchEvent_dflags_both	3


#endif /* _TT_FLAGS_H */
