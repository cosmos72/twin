
#ifndef _TT_FLAGS_H
#define _TT_FLAGS_H

#define ttcallback_lflags_arg0_component	1
#define ttcallback_lflags_arg0_event		2
#define ttcallback_lflags_after			4
#define ttcallback_lflags_function_plain	8
#define ttcallback_lflags_any		      0xC

#define ttvisible_repaint_args_WHOLE	0, 0, MAXDAT, MAXDAT

#define ttvisible_vflags_visible	1

#define ttwidget_vflags_style_fill	2
#define ttwidget_vflags_style_expose	4
#define ttwidget_vflags_style_cache	8
#define ttwidget_vflags_style_contents	0x10
#define ttwidget_vflags_style_rows	0x20

#define ttanybutton_vflags_normal	0x00
#define ttanybutton_vflags_prelight	0x10
#define ttanybutton_vflags_pressed	0x20
#define ttanybutton_vflags_disabled	0x30
#define ttanybutton_vflags_statemask	0x30

#define ttbutton_shape_normal		0
#define ttbutton_shape_prelight		1
#define ttbutton_shape_pressed		2
#define ttbutton_shape_disabled		3

#define ttbutton_vflags_pressed		0x100

#define ttevent_evtype_del		0
#define ttevent_evtype_askclose		1
#define ttevent_evtype_key		2
#define ttevent_evtype_mouse		3
#define ttevent_evtype_activate		4 /* button, menu, ... */
#define ttevent_evtype_change		5 /* resize, ... */
#define ttevent_evtype_expose		6 /* not used */

#define ttevent_evtype_user_first    0x1000
#define ttevent_evtype_control       0x1000
#define ttevent_evtype_clientmsg     0x2000


#endif /* _TT_FLAGS_H */
