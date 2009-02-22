



/* This file was automatically generated from m4/field.m4, do not edit! */

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

/*
 *  field_m4.h  --  implementation of TT*_ttfield functions,
 *                  reflective ttfield for all existing objects and
 *                  TT*Field_ttobj handy functions
 */

/*  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */





















#define A	TClass_ttfield
#define B(f)	((order_ttfield<<TT_MAGIC_SHIFT)|(f))


#define E(f)	    A,    B(f),

#define F(f) TT_STR(f),
#define G(t,a) id_ttclass_(t)|(a),



#if !defined(e_FFP)
# error internal error: undefined e_FFP !
#endif

/* array of existing fields */

static TT_CONST struct s_ttfield field_array[] = {

 
    /* ttobj fields */    
    { E(ttobj_field_first)	17, F(ttobj_field_first) G(void, 0) },                
    { E(ttobj_field_last)	16, F(ttobj_field_last) G(void, 0) }, 
    /* ttclass fields */    
    { E(ttclass_field_first)	19, F(ttclass_field_first) G(void, 0) },                    
    { E(ttclass_field_last)	18, F(ttclass_field_last) G(void, 0) }, 
    /* ttobject fields */    
    { E(ttobject_field_first)	20, F(ttobject_field_first) G(void, 0) },        
    { E(ttobject_refcount)	17, F(ttobject_refcount) G(ttopaque, 0) },    
    { E(ttobject_oflags)	15, F(ttobject_oflags) G(ttuint, 0) },            
    { E(ttobject_user_data)	18, F(ttobject_user_data) G(ttany, 0) },    
    { E(ttobject_events_inprogress)	26, F(ttobject_events_inprogress) G(ttopaque, 0) },    
    { E(ttobject_events_informed)	24, F(ttobject_events_informed) G(ttopaque, 0) },    
    { E(ttobject_field_last)	19, F(ttobject_field_last) G(void, 0) }, 
    /* ttvector fields */    
    { E(ttvector_field_first)	20, F(ttvector_field_first) G(void, 0) },        
    { E(ttvector_array_size)	19, F(ttvector_array_size) G(ttopaque, 0) },    
    { E(ttvector_array_n)	16, F(ttvector_array_n) G(ttopaque, 0) },    
    { E(ttvector_array)	14, F(ttvector_array) G(ttany, 0|e_A(e_F(ttvector, array_n)|e_OP(e_SET))) },
        /*TT_ARG_ARRAY((_F(array_n)))*/
#if e_FFP <= ttvector_array - ttvector_field_first
# error internal error: ttvector_array - ttvector_field_first overflows e_FFP !
#endif    
    { E(ttvector_field_last)	19, F(ttvector_field_last) G(void, 0) }, 
    /* ttfield fields */    
    { E(ttfield_field_first)	19, F(ttfield_field_first) G(void, 0) },        
    { E(ttfield_name_len)	16, F(ttfield_name_len) G(ttopaque, 0) },    
    { E(ttfield_name)	12, F(ttfield_name) G(ttbyte, 0|e_Z) },
        /*TT_ARG_ARRAY_Z*/
#if e_FFP <= ttfield_name - ttfield_field_first
# error internal error: ttfield_name - ttfield_field_first overflows e_FFP !
#endif    
    { E(ttfield_type)	12, F(ttfield_type) G(ttopaque, 0) },    
    { E(ttfield_field_last)	18, F(ttfield_field_last) G(void, 0) }, 
    /* ttmethod fields */    
    { E(ttmethod_field_first)	20, F(ttmethod_field_first) G(void, 0) },        
    { E(ttmethod_name_len)	17, F(ttmethod_name_len) G(ttopaque, 0) },    
    { E(ttmethod_name)	13, F(ttmethod_name) G(ttbyte, 0|e_A(e_F(ttmethod, name_len)|e_OP(e_SET))) },
        /*TT_ARG_ARRAY((_F(name_len)))*/
#if e_FFP <= ttmethod_name - ttmethod_field_first
# error internal error: ttmethod_name - ttmethod_field_first overflows e_FFP !
#endif    
    { E(ttmethod_address)	16, F(ttmethod_address) G(ttfunction_fn, 0) },    
    { E(ttmethod_return_type)	20, F(ttmethod_return_type) G(ttopaque, 0) },    
    { E(ttmethod_parameter_type)	23, F(ttmethod_parameter_type) G(ttvector, 0) },    
    { E(ttmethod_field_last)	19, F(ttmethod_field_last) G(void, 0) }, 
    /* ttevent fields */    
    { E(ttevent_field_first)	19, F(ttevent_field_first) G(void, 0) },        
    { E(ttevent_component)	17, F(ttevent_component) G(ttcomponent, 0) },    
    { E(ttevent_evtype)	14, F(ttevent_evtype) G(ttuint, 0) },    
    { E(ttevent_evcode)	14, F(ttevent_evcode) G(ttuint, 0) },    
    { E(ttevent_evflags)	15, F(ttevent_evflags) G(ttuint, 0) },    
    { E(ttevent_prev)	12, F(ttevent_prev) G(ttevent, 0) },    
    { E(ttevent_next)	12, F(ttevent_next) G(ttevent, 0) },    
    { E(ttevent_field_last)	18, F(ttevent_field_last) G(void, 0) }, 
    /* tteventbig fields */    
    { E(tteventbig_field_first)	22, F(tteventbig_field_first) G(void, 0) },        
    { E(tteventbig_x)	12, F(tteventbig_x) G(ttshort, 0) },    
    { E(tteventbig_y)	12, F(tteventbig_y) G(ttshort, 0) },    
    { E(tteventbig_w)	12, F(tteventbig_w) G(ttshort, 0) },    
    { E(tteventbig_h)	12, F(tteventbig_h) G(ttshort, 0) },    
    { E(tteventbig_value)	16, F(tteventbig_value) G(ttany, 0) },    
    { E(tteventbig_old_value)	20, F(tteventbig_old_value) G(ttany, 0) },    
    { E(tteventbig_data_len)	19, F(tteventbig_data_len) G(ttopaque, 0) },    
    { E(tteventbig_data)	15, F(tteventbig_data) G(ttbyte, 0|e_A(e_F(tteventbig, data_len)|e_OP(e_SET))) },
        /*TT_ARG_ARRAY((_F(data_len)))*/
#if e_FFP <= tteventbig_data - tteventbig_field_first
# error internal error: tteventbig_data - tteventbig_field_first overflows e_FFP !
#endif    
    { E(tteventbig_field_last)	21, F(tteventbig_field_last) G(void, 0) }, 
    /* ttbitmask fields */    
    { E(ttbitmask_field_first)	21, F(ttbitmask_field_first) G(void, 0) },                        
    { E(ttbitmask_field_last)	20, F(ttbitmask_field_last) G(void, 0) }, 
    /* tteventmask fields */    
    { E(tteventmask_field_first)	23, F(tteventmask_field_first) G(void, 0) },        
    { E(tteventmask_evtype_mask)	23, F(tteventmask_evtype_mask) G(ttbitmask, 0) },    
    { E(tteventmask_evcode_mask)	23, F(tteventmask_evcode_mask) G(ttbitmask, 0) },    
    { E(tteventmask_component_mask)	26, F(tteventmask_component_mask) G(ttbitmask, 0) },    
    { E(tteventmask_truth_table)	23, F(tteventmask_truth_table) G(ttuint, 0) },    
    { E(tteventmask_field_last)	22, F(tteventmask_field_last) G(void, 0) }, 
    /* ttcallback fields */    
    { E(ttcallback_field_first)	22, F(ttcallback_field_first) G(void, 0) },        
    { E(ttcallback_lflags)	17, F(ttcallback_lflags) G(ttuint, 0) },    
    { E(ttcallback_component)	20, F(ttcallback_component) G(ttcomponent, 0) },    
    { E(ttcallback_prev)	15, F(ttcallback_prev) G(ttcallback, 0) },    
    { E(ttcallback_next)	15, F(ttcallback_next) G(ttcallback, 0) },    
    { E(ttcallback_field_last)	21, F(ttcallback_field_last) G(void, 0) }, 
    /* ttlistener fields */    
    { E(ttlistener_field_first)	22, F(ttlistener_field_first) G(void, 0) },            
    { E(ttlistener_event)	16, F(ttlistener_event) G(ttevent, 0) },    
    { E(ttlistener_function)	19, F(ttlistener_function) G(ttfunction_fn, 0) },    
    { E(ttlistener_arg_component_n)	26, F(ttlistener_arg_component_n) G(ttopaque, 0) },    
    { E(ttlistener_arg_event_n)	22, F(ttlistener_arg_event_n) G(ttopaque, 0) },    
    { E(ttlistener_args)	15, F(ttlistener_args) G(ttvector, 0) },    
    { E(ttlistener_event_mask)	21, F(ttlistener_event_mask) G(tteventmask, 0) },            
    { E(ttlistener_field_last)	21, F(ttlistener_field_last) G(void, 0) }, 
    /* tttimer fields */    
    { E(tttimer_field_first)	19, F(tttimer_field_first) G(void, 0) },        
    { E(tttimer_delay_t)	15, F(tttimer_delay_t) G(ttany, 0) },    
    { E(tttimer_delay_f)	15, F(tttimer_delay_f) G(ttany, 0) },    
    { E(tttimer_timer_prev)	18, F(tttimer_timer_prev) G(tttimer, 0) },    
    { E(tttimer_timer_next)	18, F(tttimer_timer_next) G(tttimer, 0) },    
    { E(tttimer_field_last)	18, F(tttimer_field_last) G(void, 0) }, 
    /* ttcomponent fields */    
    { E(ttcomponent_field_first)	23, F(ttcomponent_field_first) G(void, 0) },        
    { E(ttcomponent_callbacks)	21, F(ttcomponent_callbacks) G(ttcallback, 0) },    
    { E(ttcomponent_listeners)	21, F(ttcomponent_listeners) G(ttlistener, 0) },    
    { E(ttcomponent_timers)	18, F(ttcomponent_timers) G(tttimer, 0) },    
    { E(ttcomponent_datas)	17, F(ttcomponent_datas) G(ttdata, 0) },    
    { E(ttcomponent_field_last)	22, F(ttcomponent_field_last) G(void, 0) }, 
    /* ttdata fields */    
    { E(ttdata_field_first)	18, F(ttdata_field_first) G(void, 0) },            
    { E(ttdata_component)	16, F(ttdata_component) G(ttcomponent, 0) },    
    { E(ttdata_key_len)	14, F(ttdata_key_len) G(ttopaque, 0) },    
    { E(ttdata_key)	10, F(ttdata_key) G(ttbyte, 0|e_A(e_F(ttdata, key_len)|e_OP(e_SET))) },
        /*TT_ARG_ARRAY((_F(key_len)))*/
#if e_FFP <= ttdata_key - ttdata_field_first
# error internal error: ttdata_key - ttdata_field_first overflows e_FFP !
#endif    
    { E(ttdata_data)	11, F(ttdata_data) G(ttany, 0) },    
    { E(ttdata_field_last)	17, F(ttdata_field_last) G(void, 0) }, 
    /* ttvisible fields */    
    { E(ttvisible_field_first)	21, F(ttvisible_field_first) G(void, 0) },        
    { E(ttvisible_vflags)	16, F(ttvisible_vflags) G(ttuint, 0) },    
    { E(ttvisible_prev)	14, F(ttvisible_prev) G(ttvisible, 0) },    
    { E(ttvisible_next)	14, F(ttvisible_next) G(ttvisible, 0) },    
    { E(ttvisible_parent)	16, F(ttvisible_parent) G(ttvisible, 0) },    
    { E(ttvisible_child_first)	21, F(ttvisible_child_first) G(ttvisible, 0) },    
    { E(ttvisible_child_last)	20, F(ttvisible_child_last) G(ttvisible, 0) },    
    { E(ttvisible_constraint)	20, F(ttvisible_constraint) G(ttany, 0) },    
    { E(ttvisible_theme)	15, F(ttvisible_theme) G(tttheme, 0) },    
    { E(ttvisible_repaint)	17, F(ttvisible_repaint) G(ttfunction_fn, 0) },    
    { E(ttvisible_field_last)	20, F(ttvisible_field_last) G(void, 0) }, 
    /* ttlayout fields */    
    { E(ttlayout_field_first)	20, F(ttlayout_field_first) G(void, 0) },        
    { E(ttlayout_widget)	15, F(ttlayout_widget) G(ttwidget, 0) },    
    { E(ttlayout_field_last)	19, F(ttlayout_field_last) G(void, 0) }, 
    /* ttboxlayout fields */    
    { E(ttboxlayout_field_first)	23, F(ttboxlayout_field_first) G(void, 0) },        
    { E(ttboxlayout_orientation)	23, F(ttboxlayout_orientation) G(ttuint, 0) },    
    { E(ttboxlayout_field_last)	22, F(ttboxlayout_field_last) G(void, 0) }, 
    /* ttborderlayout fields */    
    { E(ttborderlayout_field_first)	26, F(ttborderlayout_field_first) G(void, 0) },        
    { E(ttborderlayout_field_last)	25, F(ttborderlayout_field_last) G(void, 0) }, 
    /* ttnative fields */    
    { E(ttnative_field_first)	20, F(ttnative_field_first) G(void, 0) },        
    { E(ttnative_field_last)	19, F(ttnative_field_last) G(void, 0) }, 
    /* ttwidget fields */    
    { E(ttwidget_field_first)	20, F(ttwidget_field_first) G(void, 0) },        
    { E(ttwidget_x)	10, F(ttwidget_x) G(ttshort, 0) },    
    { E(ttwidget_y)	10, F(ttwidget_y) G(ttshort, 0) },    
    { E(ttwidget_w)	10, F(ttwidget_w) G(ttshort, 0) },    
    { E(ttwidget_h)	10, F(ttwidget_h) G(ttshort, 0) },    
    { E(ttwidget_layout)	15, F(ttwidget_layout) G(ttlayout, 0) },    
    { E(ttwidget_xl)	11, F(ttwidget_xl) G(ttint, 0) },    
    { E(ttwidget_yl)	11, F(ttwidget_yl) G(ttint, 0) },    
    { E(ttwidget_wl)	11, F(ttwidget_wl) G(ttint, 0) },    
    { E(ttwidget_hl)	11, F(ttwidget_hl) G(ttint, 0) },    
    { E(ttwidget_col)	12, F(ttwidget_col) G(ttcol, 0) },    
    { E(ttwidget_tooltip)	16, F(ttwidget_tooltip) G(tttooltip, 0) },    
    { E(ttwidget_field_last)	19, F(ttwidget_field_last) G(void, 0) }, 
    /* ttlabel fields */    
    { E(ttlabel_field_first)	19, F(ttlabel_field_first) G(void, 0) },        
    { E(ttlabel_text_len)	16, F(ttlabel_text_len) G(ttopaque, 0) },    
    { E(ttlabel_text)	12, F(ttlabel_text) G(ttfont, 0|e_A(e_F(ttlabel, text_len)|e_OP(e_SET))) },
        /*TT_ARG_ARRAY((_F(text_len)))*/
#if e_FFP <= ttlabel_text - ttlabel_field_first
# error internal error: ttlabel_text - ttlabel_field_first overflows e_FFP !
#endif    
    { E(ttlabel_field_last)	18, F(ttlabel_field_last) G(void, 0) }, 
    /* tttooltip fields */    
    { E(tttooltip_field_first)	21, F(tttooltip_field_first) G(void, 0) },            
    { E(tttooltip_widget)	16, F(tttooltip_widget) G(ttwidget, 0) },    
    { E(tttooltip_field_last)	20, F(tttooltip_field_last) G(void, 0) }, 
    /* ttbuttongroup fields */    
    { E(ttbuttongroup_field_first)	25, F(ttbuttongroup_field_first) G(void, 0) },        
    { E(ttbuttongroup_group_first)	25, F(ttbuttongroup_group_first) G(ttradiobutton, 0) },    
    { E(ttbuttongroup_group_last)	24, F(ttbuttongroup_group_last) G(ttradiobutton, 0) },    
    { E(ttbuttongroup_checked)	21, F(ttbuttongroup_checked) G(ttradiobutton, 0) },    
    { E(ttbuttongroup_field_last)	24, F(ttbuttongroup_field_last) G(void, 0) }, 
    /* ttanybutton fields */    
    { E(ttanybutton_field_first)	23, F(ttanybutton_field_first) G(void, 0) },        
    { E(ttanybutton_text_width)	22, F(ttanybutton_text_width) G(ttshort, 0) },    
    { E(ttanybutton_text_height)	23, F(ttanybutton_text_height) G(ttshort, 0) },    
    { E(ttanybutton_text)	16, F(ttanybutton_text) G(ttattr, 0|e_A(e_F(ttanybutton, text_width)|(e_F(ttanybutton, text_height))<<e_WFP|e_OP(e_MUL))) },
        /*TT_ARG_ARRAY((_F(text_width),*,_F(text_height)))*/
#if e_FFP <= ttanybutton_text - ttanybutton_field_first
# error internal error: ttanybutton_text - ttanybutton_field_first overflows e_FFP !
#endif        
    { E(ttanybutton_field_last)	22, F(ttanybutton_field_last) G(void, 0) }, 
    /* ttbutton fields */    
    { E(ttbutton_field_first)	20, F(ttbutton_field_first) G(void, 0) },        
    { E(ttbutton_field_last)	19, F(ttbutton_field_last) G(void, 0) }, 
    /* ttcheckbutton fields */    
    { E(ttcheckbutton_field_first)	25, F(ttcheckbutton_field_first) G(void, 0) },        
    { E(ttcheckbutton_field_last)	24, F(ttcheckbutton_field_last) G(void, 0) }, 
    /* ttradiobutton fields */    
    { E(ttradiobutton_field_first)	25, F(ttradiobutton_field_first) G(void, 0) },        
    { E(ttradiobutton_group)	19, F(ttradiobutton_group) G(ttbuttongroup, 0) },    
    { E(ttradiobutton_group_prev)	24, F(ttradiobutton_group_prev) G(ttradiobutton, 0) },    
    { E(ttradiobutton_group_next)	24, F(ttradiobutton_group_next) G(ttradiobutton, 0) },    
    { E(ttradiobutton_field_last)	24, F(ttradiobutton_field_last) G(void, 0) }, 
    /* ttanyscroll fields */    
    { E(ttanyscroll_field_first)	23, F(ttanyscroll_field_first) G(void, 0) },        
    { E(ttanyscroll_orientation)	23, F(ttanyscroll_orientation) G(ttbyte, 0) },    
    { E(ttanyscroll_size)	16, F(ttanyscroll_size) G(ttint, 0) },    
    { E(ttanyscroll_real_size)	21, F(ttanyscroll_real_size) G(ttint, 0) },    
    { E(ttanyscroll_view_size)	21, F(ttanyscroll_view_size) G(ttint, 0) },        
    { E(ttanyscroll_position)	20, F(ttanyscroll_position) G(ttint, 0) },    
    { E(ttanyscroll_real_position)	25, F(ttanyscroll_real_position) G(ttint, 0) },    
    { E(ttanyscroll_state)	17, F(ttanyscroll_state) G(ttuint, 0) },    
    { E(ttanyscroll_field_last)	22, F(ttanyscroll_field_last) G(void, 0) }, 
    /* ttscrollbar fields */    
    { E(ttscrollbar_field_first)	23, F(ttscrollbar_field_first) G(void, 0) },            
    { E(ttscrollbar_field_last)	22, F(ttscrollbar_field_last) G(void, 0) }, 
    /* ttslider fields */    
    { E(ttslider_field_first)	20, F(ttslider_field_first) G(void, 0) },        
    { E(ttslider_slide_min)	18, F(ttslider_slide_min) G(ttint, 0) },    
    { E(ttslider_slide_max)	18, F(ttslider_slide_max) G(ttint, 0) },    
    { E(ttslider_slide_value)	20, F(ttslider_slide_value) G(ttint, 0) },    
    { E(ttslider_field_last)	19, F(ttslider_field_last) G(void, 0) }, 
    /* ttprogressbar fields */    
    { E(ttprogressbar_field_first)	25, F(ttprogressbar_field_first) G(void, 0) },                
    { E(ttprogressbar_field_last)	24, F(ttprogressbar_field_last) G(void, 0) }, 
    /* ttscrollpane fields */    
    { E(ttscrollpane_field_first)	24, F(ttscrollpane_field_first) G(void, 0) },        
    { E(ttscrollpane_bar_x)	18, F(ttscrollpane_bar_x) G(ttscrollbar, 0) },    
    { E(ttscrollpane_bar_y)	18, F(ttscrollpane_bar_y) G(ttscrollbar, 0) },    
    { E(ttscrollpane_field_last)	23, F(ttscrollpane_field_last) G(void, 0) }, 
    /* ttwindow fields */    
    { E(ttwindow_field_first)	20, F(ttwindow_field_first) G(void, 0) },        
    { E(ttwindow_title_len)	18, F(ttwindow_title_len) G(ttopaque, 0) },    
    { E(ttwindow_title)	14, F(ttwindow_title) G(ttbyte, 0|e_A(e_F(ttwindow, title_len)|e_OP(e_SET))) },
        /*TT_ARG_ARRAY((_F(title_len)))*/
#if e_FFP <= ttwindow_title - ttwindow_field_first
# error internal error: ttwindow_title - ttwindow_field_first overflows e_FFP !
#endif    
    { E(ttwindow_field_last)	19, F(ttwindow_field_last) G(void, 0) }, 
    /* ttframe fields */    
    { E(ttframe_field_first)	19, F(ttframe_field_first) G(void, 0) },        
    { E(ttframe_menubar)	15, F(ttframe_menubar) G(ttmenubar, 0) },    
    { E(ttframe_field_last)	18, F(ttframe_field_last) G(void, 0) }, 
    /* ttscroller fields */    
    { E(ttscroller_field_first)	22, F(ttscroller_field_first) G(void, 0) },        
    { E(ttscroller_scrollpane)	21, F(ttscroller_scrollpane) G(ttscrollpane, 0) },    
    { E(ttscroller_field_last)	21, F(ttscroller_field_last) G(void, 0) }, 
    /* ttmenuitem fields */    
    { E(ttmenuitem_field_first)	22, F(ttmenuitem_field_first) G(void, 0) },        
    { E(ttmenuitem_name_len)	19, F(ttmenuitem_name_len) G(ttopaque, 0) },    
    { E(ttmenuitem_name)	15, F(ttmenuitem_name) G(ttbyte, 0|e_A(e_F(ttmenuitem, name_len)|e_OP(e_SET))) },
        /*TT_ARG_ARRAY((_F(name_len)))*/
#if e_FFP <= ttmenuitem_name - ttmenuitem_field_first
# error internal error: ttmenuitem_name - ttmenuitem_field_first overflows e_FFP !
#endif    
    { E(ttmenuitem_field_last)	21, F(ttmenuitem_field_last) G(void, 0) }, 
    /* ttcheckmenuitem fields */    
    { E(ttcheckmenuitem_field_first)	27, F(ttcheckmenuitem_field_first) G(void, 0) },        
    { E(ttcheckmenuitem_field_last)	26, F(ttcheckmenuitem_field_last) G(void, 0) }, 
    /* ttradiomenuitem fields */    
    { E(ttradiomenuitem_field_first)	27, F(ttradiomenuitem_field_first) G(void, 0) },        
    { E(ttradiomenuitem_field_last)	26, F(ttradiomenuitem_field_last) G(void, 0) }, 
    /* ttmenu fields */    
    { E(ttmenu_field_first)	18, F(ttmenu_field_first) G(void, 0) },        
    { E(ttmenu_menubar)	14, F(ttmenu_menubar) G(ttmenubar, 0) },    
    { E(ttmenu_field_last)	17, F(ttmenu_field_last) G(void, 0) }, 
    /* ttmenubar fields */    
    { E(ttmenubar_field_first)	21, F(ttmenubar_field_first) G(void, 0) },        
    { E(ttmenubar_field_last)	20, F(ttmenubar_field_last) G(void, 0) }, 
    /* ttanytext fields */    
    { E(ttanytext_field_first)	21, F(ttanytext_field_first) G(void, 0) },        
    { E(ttanytext_text_len)	18, F(ttanytext_text_len) G(ttopaque, 0) },    
    { E(ttanytext_text)	14, F(ttanytext_text) G(ttattr, 0|e_A(e_F(ttanytext, text_len)|e_OP(e_SET))) },
        /*TT_ARG_ARRAY((_F(text_len)))*/
#if e_FFP <= ttanytext_text - ttanytext_field_first
# error internal error: ttanytext_text - ttanytext_field_first overflows e_FFP !
#endif    
    { E(ttanytext_field_last)	20, F(ttanytext_field_last) G(void, 0) }, 
    /* tttextfield fields */    
    { E(tttextfield_field_first)	23, F(tttextfield_field_first) G(void, 0) },        
    { E(tttextfield_field_last)	22, F(tttextfield_field_last) G(void, 0) }, 
    /* tttextarea fields */    
    { E(tttextarea_field_first)	22, F(tttextarea_field_first) G(void, 0) },        
    { E(tttextarea_field_last)	21, F(tttextarea_field_last) G(void, 0) }, 
    /* tttheme fields */    
    { E(tttheme_field_first)	19, F(tttheme_field_first) G(void, 0) },        
    { E(tttheme_name_len)	16, F(tttheme_name_len) G(ttopaque, 0) },    
    { E(tttheme_name)	12, F(tttheme_name) G(ttbyte, 0|e_A(e_F(tttheme, name_len)|e_OP(e_SET))) },
        /*TT_ARG_ARRAY((_F(name_len)))*/
#if e_FFP <= tttheme_name - tttheme_field_first
# error internal error: tttheme_name - tttheme_field_first overflows e_FFP !
#endif                    
    { E(tttheme_field_last)	18, F(tttheme_field_last) G(void, 0) }, 
    /* ttapplication fields */    
    { E(ttapplication_field_first)	25, F(ttapplication_field_first) G(void, 0) },        
    { E(ttapplication_name_len)	22, F(ttapplication_name_len) G(ttopaque, 0) },    
    { E(ttapplication_name)	18, F(ttapplication_name) G(ttbyte, 0|e_A(e_F(ttapplication, name_len)|e_OP(e_SET))) },
        /*TT_ARG_ARRAY((_F(name_len)))*/
#if e_FFP <= ttapplication_name - ttapplication_field_first
# error internal error: ttapplication_name - ttapplication_field_first overflows e_FFP !
#endif    
    { E(ttapplication_field_last)	24, F(ttapplication_field_last) G(void, 0) },

};

#undef A
#undef B
#undef C
#undef D
#undef E
#undef F

#define field_array_n (sizeof(field_array)/sizeof(field_array[0]))





