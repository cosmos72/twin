
#ifndef _TT_FLAGS_H
#define _TT_FLAGS_H



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
/** ttevent value of `evtype' field indicating `field changed value' event */
#define ttevent_evtype_change		5
/** ttevent value of `evtype' field indicating `expose' event */
#define ttevent_evtype_expose		6


/** ttevent value of `evtype' field indicating the first user-defined event */
#define ttevent_evtype_user_first    0x1000
/** ttevent value of `evtype' field indicating the first user-defined control event */
#define ttevent_evtype_control       0x1000
/** ttevent value of `evtype' field indicating the first user-defined inter-client message */
#define ttevent_evtype_clientmsg     0x2000


#define ttevent_evcode_key_first	0x001
#define ttevent_evcode_key_last		0x3FF
#define ttevent_evcode_key_mask		0x3FF /* 0x1FF actually used, but better stay large */


#define ttevent_evcode_mouse_first	0x400
#define ttevent_evcode_mouse_last	0x7FF
#define ttevent_evcode_mouse_mask	0x3FF /* 0x07F actually used, but better stay large */



/* mouse events stuff; loosely designed after libTw */

#define ttevent_evcode_mouse_button_1	0x01
#define ttevent_evcode_mouse_button_2	0x02
#define ttevent_evcode_mouse_button_3	0x03
#define ttevent_evcode_mouse_button_4	0x04
#define ttevent_evcode_mouse_button_5	0x05
#define ttevent_evcode_mouse_button_mask 0x07

#define ttevent_evcode_mouse_hold_1	0x08
#define ttevent_evcode_mouse_hold_2	0x10
#define ttevent_evcode_mouse_hold_3	0x20
#define ttevent_evcode_mouse_hold_4	0x40
#define ttevent_evcode_mouse_hold_5	0x80
#define ttevent_evcode_mouse_hold_mask	0xFF

#define ttevent_evcode_mouse_move	0x400
#define ttevent_evcode_mouse_press	0x500
#define ttevent_evcode_mouse_release	0x600
#define ttevent_evcode_mouse_action_mask 0x700

#define ttevent_mouse_get_hold_mask(code)	((code) & ttevent_evcode_mouse_button_hold_mask)
#define ttevent_mouse_get_action(code)		((code) & ttevent_evcode_mouse_action_mask)
#define ttevent_mouse_get_button(code)		((code) & ttevent_evcode_mouse_button_mask)

#define ttevent_mouse_is_move(code)	(((code) & ttevent_evcode_mouse_action_mask) == ttevent_evcode_mouse_move)
#define ttevent_mouse_is_press(code)	(((code) & ttevent_evcode_mouse_action_mask) == ttevent_evcode_mouse_press)
#define ttevent_mouse_is_release(code)	(((code) & ttevent_evcode_mouse_action_mask) == ttevent_evcode_mouse_release)

/* shortcuts */
#define ttevent_evcode_mouse_press_1	(ttevent_evcode_mouse_press | ttevent_evcode_mouse_button_1)
#define ttevent_evcode_mouse_press_2	(ttevent_evcode_mouse_press | ttevent_evcode_mouse_button_2)
#define ttevent_evcode_mouse_press_3	(ttevent_evcode_mouse_press | ttevent_evcode_mouse_button_3)
#define ttevent_evcode_mouse_press_4	(ttevent_evcode_mouse_press | ttevent_evcode_mouse_button_4)
#define ttevent_evcode_mouse_press_5	(ttevent_evcode_mouse_press | ttevent_evcode_mouse_button_5)

#define ttevent_evcode_mouse_release_1	(ttevent_evcode_mouse_release | ttevent_evcode_mouse_button_1)
#define ttevent_evcode_mouse_release_2	(ttevent_evcode_mouse_release | ttevent_evcode_mouse_button_2)
#define ttevent_evcode_mouse_release_3	(ttevent_evcode_mouse_release | ttevent_evcode_mouse_button_3)
#define ttevent_evcode_mouse_release_4	(ttevent_evcode_mouse_release | ttevent_evcode_mouse_button_4)
#define ttevent_evcode_mouse_release_5	(ttevent_evcode_mouse_release | ttevent_evcode_mouse_button_5)


/* ttevent evflags; same as libTw */
#define ttevent_evflags_shift		0x0001
#define ttevent_evflags_ctrl		0x0002
#define ttevent_evflags_alt		0x0004
#define ttevent_evflags_caps_lock	0x0008
#define ttevent_evflags_num_lock	0x0010
#define ttevent_evflags_super		0x0020
#define ttevent_evflags_hyper		0x0040




/** ttlistener flag indicating the function will receive the component generating the event as first arg */
#define ttlistener_lflags_arg0_component	1
/** ttlistener flag indicating the function will receive the event itself as first arg */
#define ttlistener_lflags_arg0_event		2
#define ttlistener_lflags_arg0_timer		2
/** ttlistener flag indicating the function should be called BEFORE other callbacks */
#define ttlistener_lflags_before		4
/** ttlistener flag indicating the function has alternate syntax */
#define ttlistener_lflags_function_array	8
/** mask of ttlistener flags */
#define ttlistener_lflags_any		      0xC


/** tttimer enabled flag */
#define tttimer_lflags_enabled		     0x10
/** mask of tttimer flags */
#define tttimer_lflags_any		     0x18



/** arguments to pass to a ttvisible_repaint_fn function to repaint the whole ttvisible */
#define ttvisible_repaint_args_WHOLE	0, 0, MAXDAT, MAXDAT

/** ttvisible flag indicating it is visible */
#define ttvisible_vflags_visible	1


#define ttlayout_constraint_root		0

/** ttboxlayout horizontal layout */
#define ttboxlayout_orientation_x		0
/** ttboxlayout vertical layout */
#define ttboxlayout_orientation_y		1

#define ttborderlayout_constraint_north		1
#define ttborderlayout_constraint_west		3
#define ttborderlayout_constraint_east		4
#define ttborderlayout_constraint_center	5
#define ttborderlayout_constraint_south		7

    
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


/** ttprogressbar horizontal orientation */
#define ttprogressbar_orientation_x	ttanyscroll_orientation_x
/** ttprogressbar vertical orientation */
#define ttprogressbar_orientation_y	ttanyscroll_orientation_y

/** ttprogressbar flag indicating it is in normal state */
#define ttprogressbar_vflags_normal	0x00
/** ttprogressbar flag indicating it is in disabled state */
#define ttprogressbar_vflags_disabled	0x10
/** mask of ttprogressbar state flags */
#define ttprogressbar_vflags_statemask	0x10


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



/** ttcheckbutton flag indicating it is checked (i.e. `ON') */
#define ttcheckbutton_vflags_checked	0x40
/** mask of ttcheckbutton state flags */
#define ttcheckbutton_vflags_statemask	0x70


/** ttanyscroll horizontal orientation */
#define ttanyscroll_orientation_x	0
/** ttanyscroll vertical orientation */
#define ttanyscroll_orientation_y	1

/** ttanyscroll normal state */
#define ttanyscroll_state_normal	0
/** ttanyscroll `backward page pressed' state */
#define ttanyscroll_state_page_rev	1
/** ttanyscroll `forward page pressed' state */
#define ttanyscroll_state_page_fwd	2
/** ttanyscroll `tab pressed' state */
#define ttanyscroll_state_tab		5

#define ttslider_state_normal		ttanyscroll_state_normal
#define ttslider_state_page_rev		ttanyscroll_state_page_rev
#define ttslider_state_page_fwd		ttanyscroll_state_page_fwd
#define ttslider_state_tab		ttanyscroll_state_tab

#define ttscrollbar_state_normal	ttanyscroll_state_normal
#define ttscrollbar_state_page_rev	ttanyscroll_state_page_rev
#define ttscrollbar_state_page_fwd	ttanyscroll_state_page_fwd
#define ttscrollbar_state_tab		ttanyscroll_state_tab
/** ttscrollbar `backward arrow pressed' state */
#define ttscrollbar_state_arrow_rev	3
/** ttscrollbar `forward arrow pressed' state */
#define ttscrollbar_state_arrow_fwd	4


#endif /* _TT_FLAGS_H */
