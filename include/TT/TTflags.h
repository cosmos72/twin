
#ifndef _TT_FLAGS_H
#define _TT_FLAGS_H

#define ttvisible_Repaint_args_WHOLE	0, 0, MAXDAT, MAXDAT

#define ttvisible_vflags_visible	1

#define ttanybutton_vflags_normal	0x00
#define ttanybutton_vflags_prelight	0x10
#define ttanybutton_vflags_pressed	0x20
#define ttanybutton_vflags_disabled	0x30
#define ttanybutton_vflags_statemask	0x30

#define ttbutton_shape_normal		0
#define ttbutton_shape_prelight		1
#define ttbutton_shape_pressed		2
#define ttbutton_shape_disabled		3

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

#define tttheme_shape_max		4

#define x_left		0
#define y_up		1
#define x_right		2
#define y_down		3

#endif /* _TT_FLAGS_H */
