



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
    { tteventbig_len, 14, "tteventbig_len", },    
    { tteventbig_data, 15, "tteventbig_data", },    
    { tteventbig_field_last, 21, "tteventbig_field_last", }, 
    /* ttlistener fields */    
    { ttlistener_field_first, 22, "ttlistener_field_first", },            
    { ttlistener_lflags, 17, "ttlistener_lflags", },    
    { ttlistener_component, 20, "ttlistener_component", },    
    { ttlistener_prev, 15, "ttlistener_prev", },    
    { ttlistener_next, 15, "ttlistener_next", },    
    { ttlistener_field_last, 21, "ttlistener_field_last", }, 
    /* ttcallback fields */    
    { ttcallback_field_first, 22, "ttcallback_field_first", },        
    { ttcallback_avl_left, 19, "ttcallback_avl_left", },    
    { ttcallback_avl_right, 20, "ttcallback_avl_right", },    
    { ttcallback_avl_parent, 21, "ttcallback_avl_parent", },            
    { ttcallback_event, 16, "ttcallback_event", },    
    { ttcallback_narg_component, 25, "ttcallback_narg_component", },    
    { ttcallback_narg_event, 21, "ttcallback_narg_event", },    
    { ttcallback_nargs, 16, "ttcallback_nargs", },    
    { ttcallback_function, 19, "ttcallback_function", },        
    { ttcallback_args, 15, "ttcallback_args", },    
    { ttcallback_field_last, 21, "ttcallback_field_last", }, 
    /* ttcomponent fields */    
    { ttcomponent_field_first, 23, "ttcomponent_field_first", },            
    { ttcomponent_listeners, 21, "ttcomponent_listeners", },    
    { ttcomponent_callbacks, 21, "ttcomponent_callbacks", },    
    { ttcomponent_field_last, 22, "ttcomponent_field_last", }, 
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
    { ttwidget_field_last, 19, "ttwidget_field_last", }, 
    /* ttlabel fields */    
    { ttlabel_field_first, 19, "ttlabel_field_first", },            
    { ttlabel_text_len, 16, "ttlabel_text_len", },        
    { ttlabel_text, 12, "ttlabel_text", },    
    { ttlabel_field_last, 18, "ttlabel_field_last", }, 
    /* ttanybutton fields */    
    { ttanybutton_field_first, 23, "ttanybutton_field_first", },                
    { ttanybutton_text_width, 22, "ttanybutton_text_width", },    
    { ttanybutton_text_height, 23, "ttanybutton_text_height", },        
    { ttanybutton_text, 16, "ttanybutton_text", },    
    { ttanybutton_field_last, 22, "ttanybutton_field_last", }, 
    /* ttbutton fields */    
    { ttbutton_field_first, 20, "ttbutton_field_first", },            
    { ttbutton_field_last, 19, "ttbutton_field_last", }, 
    /* ttcheckbutton fields */    
    { ttcheckbutton_field_first, 25, "ttcheckbutton_field_first", },            
    { ttcheckbutton_field_last, 24, "ttcheckbutton_field_last", }, 
    /* ttradiobutton fields */    
    { ttradiobutton_field_first, 25, "ttradiobutton_field_first", },            
    { ttradiobutton_field_last, 24, "ttradiobutton_field_last", }, 
    /* ttscrollbar fields */    
    { ttscrollbar_field_first, 23, "ttscrollbar_field_first", },            
    { ttscrollbar_field_last, 22, "ttscrollbar_field_last", }, 
    /* ttbuttongroup fields */    
    { ttbuttongroup_field_first, 25, "ttbuttongroup_field_first", },            
    { ttbuttongroup_field_last, 24, "ttbuttongroup_field_last", }, 
    /* ttwindow fields */    
    { ttwindow_field_first, 20, "ttwindow_field_first", },                
    { ttwindow_field_last, 19, "ttwindow_field_last", }, 
    /* ttframe fields */    
    { ttframe_field_first, 19, "ttframe_field_first", },            
    { ttframe_field_last, 18, "ttframe_field_last", }, 
    /* ttscroller fields */    
    { ttscroller_field_first, 22, "ttscroller_field_first", },            
    { ttscroller_scrollx, 18, "ttscroller_scrollx", },    
    { ttscroller_scrolly, 18, "ttscroller_scrolly", },    
    { ttscroller_field_last, 21, "ttscroller_field_last", }, 
    /* ttmenuitem fields */    
    { ttmenuitem_field_first, 22, "ttmenuitem_field_first", },                
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






