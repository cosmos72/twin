



/* This file was automatically generated from m4/field.m4, do not edit! */


/*
 *  field_m4.h  --  arrays for field.c conversion functions
 *                  between strings and numeric ids
 */



























/* array for  names */

static TT_CONST struct s_ttfield field_array_c[] = {

    /* ttobj fields */    
    { ttobj_field_first, 17, "ttobj_field_first", },                
    { ttobj_id, 8, "ttobj_id", },    
    { ttobj_refcount, 14, "ttobj_refcount", },    
    { ttobj_oflags, 12, "ttobj_oflags", },            
    { ttobj_user_data, 15, "ttobj_user_data", },    
    { ttobj_events_inprogress, 23, "ttobj_events_inprogress", },    
    { ttobj_events_informed, 21, "ttobj_events_informed", },    
    { ttobj_field_last, 16, "ttobj_field_last", },  
    /* ttevent fields */    
    { ttevent_field_first, 19, "ttevent_field_first", },            
    { ttevent_callback, 16, "ttevent_callback", },    
    { ttevent_component, 17, "ttevent_component", },    
    { ttevent_evtype, 14, "ttevent_evtype", },    
    { ttevent_evcode, 14, "ttevent_evcode", },    
    { ttevent_evflags, 15, "ttevent_evflags", },                
    { ttevent_field_last, 18, "ttevent_field_last", }, 
    /* tteventbig fields */    
    { tteventbig_field_first, 22, "tteventbig_field_first", },            
    { tteventbig_x, 12, "tteventbig_x", },    
    { tteventbig_y, 12, "tteventbig_y", },    
    { tteventbig_w, 12, "tteventbig_w", },    
    { tteventbig_h, 12, "tteventbig_h", },    
    { tteventbig_value, 16, "tteventbig_value", },    
    { tteventbig_old_value, 20, "tteventbig_old_value", },    
    { tteventbig_len, 14, "tteventbig_len", },    
    { tteventbig_data, 15, "tteventbig_data", },    
    { tteventbig_field_last, 21, "tteventbig_field_last", }, 
    /* ttbitmask fields */    
    { ttbitmask_field_first, 21, "ttbitmask_field_first", },                            
    { ttbitmask_field_last, 20, "ttbitmask_field_last", }, 
    /* tteventmask fields */    
    { tteventmask_field_first, 23, "tteventmask_field_first", },            
    { tteventmask_evtype_mask, 23, "tteventmask_evtype_mask", },    
    { tteventmask_evcode_mask, 23, "tteventmask_evcode_mask", },    
    { tteventmask_component_mask, 26, "tteventmask_component_mask", },    
    { tteventmask_truth_table, 23, "tteventmask_truth_table", },    
    { tteventmask_field_last, 22, "tteventmask_field_last", }, 
    /* ttcallback fields */    
    { ttcallback_field_first, 22, "ttcallback_field_first", },            
    { ttcallback_lflags, 17, "ttcallback_lflags", },    
    { ttcallback_component, 20, "ttcallback_component", },    
    { ttcallback_prev, 15, "ttcallback_prev", },    
    { ttcallback_next, 15, "ttcallback_next", },    
    { ttcallback_field_last, 21, "ttcallback_field_last", }, 
    /* ttlistener fields */    
    { ttlistener_field_first, 22, "ttlistener_field_first", },                
    { ttlistener_event, 16, "ttlistener_event", },    
    { ttlistener_narg_component, 25, "ttlistener_narg_component", },    
    { ttlistener_narg_event, 21, "ttlistener_narg_event", },    
    { ttlistener_nargs, 16, "ttlistener_nargs", },    
    { ttlistener_function, 19, "ttlistener_function", },    
    { ttlistener_args, 15, "ttlistener_args", },    
    { ttlistener_event_mask, 21, "ttlistener_event_mask", },            
    { ttlistener_field_last, 21, "ttlistener_field_last", }, 
    /* tttimer fields */    
    { tttimer_field_first, 19, "tttimer_field_first", },                            
    { tttimer_field_last, 18, "tttimer_field_last", }, 
    /* ttcomponent fields */    
    { ttcomponent_field_first, 23, "ttcomponent_field_first", },            
    { ttcomponent_callbacks, 21, "ttcomponent_callbacks", },    
    { ttcomponent_listeners, 21, "ttcomponent_listeners", },    
    { ttcomponent_timers, 18, "ttcomponent_timers", },    
    { ttcomponent_datas, 17, "ttcomponent_datas", },    
    { ttcomponent_field_last, 22, "ttcomponent_field_last", }, 
    /* ttdata fields */    
    { ttdata_field_first, 18, "ttdata_field_first", },                
    { ttdata_component, 16, "ttdata_component", },    
    { ttdata_key, 10, "ttdata_key", },    
    { ttdata_key_len, 14, "ttdata_key_len", },    
    { ttdata_data, 11, "ttdata_data", },    
    { ttdata_field_last, 17, "ttdata_field_last", }, 
    /* ttvisible fields */    
    { ttvisible_field_first, 21, "ttvisible_field_first", },            
    { ttvisible_vflags, 16, "ttvisible_vflags", },    
    { ttvisible_prev, 14, "ttvisible_prev", },    
    { ttvisible_next, 14, "ttvisible_next", },    
    { ttvisible_parent, 16, "ttvisible_parent", },    
    { ttvisible_child_first, 21, "ttvisible_child_first", },    
    { ttvisible_child_last, 20, "ttvisible_child_last", },    
    { ttvisible_theme, 15, "ttvisible_theme", },    
    { ttvisible_repaint, 17, "ttvisible_repaint", },    
    { ttvisible_field_last, 20, "ttvisible_field_last", }, 
    /* ttnative fields */    
    { ttnative_field_first, 20, "ttnative_field_first", },            
    { ttnative_field_last, 19, "ttnative_field_last", }, 
    /* ttwidget fields */    
    { ttwidget_field_first, 20, "ttwidget_field_first", },            
    { ttwidget_x, 10, "ttwidget_x", },    
    { ttwidget_y, 10, "ttwidget_y", },    
    { ttwidget_w, 10, "ttwidget_w", },    
    { ttwidget_h, 10, "ttwidget_h", },    
    { ttwidget_xl, 11, "ttwidget_xl", },    
    { ttwidget_yl, 11, "ttwidget_yl", },    
    { ttwidget_wl, 11, "ttwidget_wl", },    
    { ttwidget_hl, 11, "ttwidget_hl", },        
    { ttwidget_tooltip, 16, "ttwidget_tooltip", },    
    { ttwidget_field_last, 19, "ttwidget_field_last", }, 
    /* ttlabel fields */    
    { ttlabel_field_first, 19, "ttlabel_field_first", },            
    { ttlabel_text_len, 16, "ttlabel_text_len", },    
    { ttlabel_text, 12, "ttlabel_text", },    
    { ttlabel_field_last, 18, "ttlabel_field_last", }, 
    /* tttooltip fields */    
    { tttooltip_field_first, 21, "tttooltip_field_first", },                
    { tttooltip_widget, 16, "tttooltip_widget", },        
    { tttooltip_field_last, 20, "tttooltip_field_last", }, 
    /* ttbuttongroup fields */    
    { ttbuttongroup_field_first, 25, "ttbuttongroup_field_first", },            
    { ttbuttongroup_group_first, 25, "ttbuttongroup_group_first", },    
    { ttbuttongroup_group_last, 24, "ttbuttongroup_group_last", },    
    { ttbuttongroup_checked, 21, "ttbuttongroup_checked", },    
    { ttbuttongroup_field_last, 24, "ttbuttongroup_field_last", }, 
    /* ttanybutton fields */    
    { ttanybutton_field_first, 23, "ttanybutton_field_first", },            
    { ttanybutton_text, 16, "ttanybutton_text", },    
    { ttanybutton_text_width, 22, "ttanybutton_text_width", },    
    { ttanybutton_text_height, 23, "ttanybutton_text_height", },        
    { ttanybutton_field_last, 22, "ttanybutton_field_last", }, 
    /* ttbutton fields */    
    { ttbutton_field_first, 20, "ttbutton_field_first", },            
    { ttbutton_field_last, 19, "ttbutton_field_last", }, 
    /* ttcheckbutton fields */    
    { ttcheckbutton_field_first, 25, "ttcheckbutton_field_first", },            
    { ttcheckbutton_field_last, 24, "ttcheckbutton_field_last", }, 
    /* ttradiobutton fields */    
    { ttradiobutton_field_first, 25, "ttradiobutton_field_first", },            
    { ttradiobutton_group, 19, "ttradiobutton_group", },    
    { ttradiobutton_group_prev, 24, "ttradiobutton_group_prev", },    
    { ttradiobutton_group_next, 24, "ttradiobutton_group_next", },    
    { ttradiobutton_field_last, 24, "ttradiobutton_field_last", }, 
    /* ttanyscroll fields */    
    { ttanyscroll_field_first, 23, "ttanyscroll_field_first", },            
    { ttanyscroll_orientation, 23, "ttanyscroll_orientation", },    
    { ttanyscroll_size, 16, "ttanyscroll_size", },    
    { ttanyscroll_real_size, 21, "ttanyscroll_real_size", },    
    { ttanyscroll_view_size, 21, "ttanyscroll_view_size", },        
    { ttanyscroll_position, 20, "ttanyscroll_position", },    
    { ttanyscroll_real_position, 25, "ttanyscroll_real_position", },        
    { ttanyscroll_field_last, 22, "ttanyscroll_field_last", }, 
    /* ttscrollbar fields */    
    { ttscrollbar_field_first, 23, "ttscrollbar_field_first", },                
    { ttscrollbar_field_last, 22, "ttscrollbar_field_last", }, 
    /* ttslider fields */    
    { ttslider_field_first, 20, "ttslider_field_first", },            
    { ttslider_slide_min, 18, "ttslider_slide_min", },    
    { ttslider_slide_max, 18, "ttslider_slide_max", },    
    { ttslider_slide_value, 20, "ttslider_slide_value", },    
    { ttslider_field_last, 19, "ttslider_field_last", }, 
    /* ttprogressbar fields */    
    { ttprogressbar_field_first, 25, "ttprogressbar_field_first", },                    
    { ttprogressbar_field_last, 24, "ttprogressbar_field_last", }, 
    /* ttscrollpane fields */    
    { ttscrollpane_field_first, 24, "ttscrollpane_field_first", },            
    { ttscrollpane_bar_x, 18, "ttscrollpane_bar_x", },    
    { ttscrollpane_bar_y, 18, "ttscrollpane_bar_y", },    
    { ttscrollpane_field_last, 23, "ttscrollpane_field_last", }, 
    /* ttwindow fields */    
    { ttwindow_field_first, 20, "ttwindow_field_first", },            
    { ttwindow_title_len, 18, "ttwindow_title_len", },    
    { ttwindow_title, 14, "ttwindow_title", },    
    { ttwindow_field_last, 19, "ttwindow_field_last", }, 
    /* ttframe fields */    
    { ttframe_field_first, 19, "ttframe_field_first", },            
    { ttframe_menubar, 15, "ttframe_menubar", },    
    { ttframe_field_last, 18, "ttframe_field_last", }, 
    /* ttscroller fields */    
    { ttscroller_field_first, 22, "ttscroller_field_first", },            
    { ttscroller_scrollpane, 21, "ttscroller_scrollpane", },    
    { ttscroller_field_last, 21, "ttscroller_field_last", }, 
    /* ttmenuitem fields */    
    { ttmenuitem_field_first, 22, "ttmenuitem_field_first", },            
    { ttmenuitem_name, 15, "ttmenuitem_name", },    
    { ttmenuitem_field_last, 21, "ttmenuitem_field_last", }, 
    /* ttcheckmenuitem fields */    
    { ttcheckmenuitem_field_first, 27, "ttcheckmenuitem_field_first", },            
    { ttcheckmenuitem_field_last, 26, "ttcheckmenuitem_field_last", }, 
    /* ttradiomenuitem fields */    
    { ttradiomenuitem_field_first, 27, "ttradiomenuitem_field_first", },            
    { ttradiomenuitem_field_last, 26, "ttradiomenuitem_field_last", }, 
    /* ttmenu fields */    
    { ttmenu_field_first, 18, "ttmenu_field_first", },        
    
    { ttmenu_menubar, 14, "ttmenu_menubar", },    
    { ttmenu_field_last, 17, "ttmenu_field_last", }, 
    /* ttmenubar fields */    
    { ttmenubar_field_first, 21, "ttmenubar_field_first", },            
    { ttmenubar_field_last, 20, "ttmenubar_field_last", }, 
    /* ttanytext fields */    
    { ttanytext_field_first, 21, "ttanytext_field_first", },            
    { ttanytext_text_len, 18, "ttanytext_text_len", },    
    { ttanytext_text, 14, "ttanytext_text", },    
    { ttanytext_field_last, 20, "ttanytext_field_last", }, 
    /* tttextfield fields */    
    { tttextfield_field_first, 23, "tttextfield_field_first", },            
    { tttextfield_field_last, 22, "tttextfield_field_last", }, 
    /* tttextarea fields */    
    { tttextarea_field_first, 22, "tttextarea_field_first", },            
    { tttextarea_field_last, 21, "tttextarea_field_last", }, 
    /* tttheme fields */    
    { tttheme_field_first, 19, "tttheme_field_first", },        
    
    { tttheme_name, 12, "tttheme_name", },                    
    { tttheme_field_last, 18, "tttheme_field_last", }, 
    /* ttapplication fields */    
    { ttapplication_field_first, 25, "ttapplication_field_first", },            
    { ttapplication_name, 18, "ttapplication_name", },    
    { ttapplication_field_last, 24, "ttapplication_field_last", },
};


#define ttobj_field_n (sizeof(field_array_c)/sizeof(field_array_c[0]))

static struct s_ttfield field_array[ttobj_field_n];






