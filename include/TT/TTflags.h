
#ifndef _TT_FLAGS_H
#define _TT_FLAGS_H

/** ttcallback flag indicating the function will receive the component generating the event as first arg */
#define ttcallback_lflags_arg0_component	1
/** ttcallback flag indicating the function will receive the event itself as first arg */
#define ttcallback_lflags_arg0_event		2
/** ttcallback flag indicating the function should be called AFTER other callbacks */
#define ttcallback_lflags_after			4
/** ttcallback flag indicating the function has alternate syntax */
#define ttcallback_lflags_function_plain	8
/** mask of ttcallback flags */
#define ttcallback_lflags_any		      0xC

/** arguments to pass to a ttvisible_repaint_fn function to repaint the whole ttvisible */
#define ttvisible_repaint_args_WHOLE	0, 0, MAXDAT, MAXDAT

/** ttvisible flag indicating it is visible */
#define ttvisible_vflags_visible	1

/** ttwidget flag indicating the style used to hold its contents (fill repeating with a single ttattr) */
#define ttwidget_vflags_style_fill	2
/** ttwidget flag indicating the style used to hold its contents (use expose events) */
#define ttwidget_vflags_style_expose	4
/** ttwidget flag indicating the style used to hold its contents (use expose events and cache result) */
#define ttwidget_vflags_style_cache	8
/** ttwidget flag indicating the style used to hold its contents (store in a bi-dimensional array of ttattr:s) */
#define ttwidget_vflags_style_contents	0x10
/** ttwidget flag indicating the style used to hold its contents (store each row in a one-dimensional array of ttattr:s) */
#define ttwidget_vflags_style_rows	0x20

/** ttanybutton flag indicating it is in normal state */
#define ttanybutton_vflags_normal	0x00
/** ttanybutton flag indicating it is in prelight state */
#define ttanybutton_vflags_prelight	0x10
/** ttanybutton flag indicating it is in pressed state */
#define ttanybutton_vflags_pressed	0x20
/** ttanybutton flag indicating it is in disabled state */
#define ttanybutton_vflags_disabled	0x30
/** mask of ttanybutton state flags */
#define ttanybutton_vflags_statemask	0x30

/** ttbutton index to access shape for normal state */
#define ttbutton_shape_normal		0
/** ttbutton index to access shape for prelight state */
#define ttbutton_shape_prelight		1
/** ttbutton index to access shape for pressed state */
#define ttbutton_shape_pressed		2
/** ttbutton index to access shape for disabled state */
#define ttbutton_shape_disabled		3

/** ttevent value of `evtype' field indicating `del' (delete) event */
#define ttevent_evtype_del		0
/** ttevent value of `evtype' field indicating `askclose' event */
#define ttevent_evtype_askclose		1
/** ttevent value of `evtype' field indicating `key' event */
#define ttevent_evtype_key		2
/** ttevent value of `evtype' field indicating `mouse' event */
#define ttevent_evtype_mouse		3
/** ttevent value of `evtype' field indicating `activate' (user click) event */
#define ttevent_evtype_activate		4 /* button, menu, ... */
/** ttevent value of `evtype' field indicating `change' (resize, ...) event */
#define ttevent_evtype_change		5 /* resize, ... */
/** ttevent value of `evtype' field indicating `expose' event */
#define ttevent_evtype_expose		6

/** ttevent value of `evtype' field indicating the first user-defined event */
#define ttevent_evtype_user_first    0x1000
/** ttevent value of `evtype' field indicating the first user-defined control event */
#define ttevent_evtype_control       0x1000
/** ttevent value of `evtype' field indicating the first user-defined inter-client message */
#define ttevent_evtype_clientmsg     0x2000


#endif /* _TT_FLAGS_H */
