



/* This file was automatically generated from m4/handy.m4, do not edit! */


/*
 *  handy_m4.h  --  wrappers around handy libTT methods
 *
 */





















/* mixed-internal {Get,Set,Change}Value(ttopaque id, ttuint which, ...) */

static ttbyte GetValue_ttobj(opaque id, ttuint which, ttany *value) {
    ttobj o;
    if ((o = ID2OBJ(id)))
	return o->FN->GetValue(o, which, value);
    return FALSE;
}    

static ttbyte SetValue_ttobj(opaque id, ttuint which, ttany value) {
    ttobj o;
    if ((o = ID2OBJ(id)))
	return o->FN->SetValue(o, which, value);
    return FALSE;
}    

static ttbyte ChangeValue_ttobj(opaque id, ttuint which, ttany nand_value, ttany xor_value) {
    ttobj o;
    if ((o = ID2OBJ(id)))
	return o->FN->ChangeValue(o, which, nand_value, xor_value);
    return FALSE;
}    

/* handy public and exported methods */

























/* handy ttobj methods */
        
ttopaque TTGetId_ttobj(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttobj_id, &a0) ? (ttopaque)a0 : (ttany)0;
}    
ttuint TTGetRefcount_ttobj(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttobj_refcount, &a0) ? (ttuint)a0 : (ttany)0;
}    
ttuint TTGetOflags_ttobj(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttobj_oflags, &a0) ? (ttuint)a0 : (ttany)0;
}            
ttany TTGetUserData_ttobj(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttobj_user_data, &a0) ? (ttany)a0 : (ttany)0;
}
void TTSetUserData_ttobj(tt_obj o, ttany user_data) {
    SetValue_ttobj((opaque)o, ttobj_user_data, (ttany)user_data);
}    
ttuint TTGetEventsInprogress_ttobj(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttobj_events_inprogress, &a0) ? (ttuint)a0 : (ttany)0;
}  

/* handy ttevent methods */
        
tt_obj TTGetCallback_ttevent(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttevent_callback, &a0) ? (tt_obj)(opaque)a0 : (ttany)0;
}    
tt_obj TTGetComponent_ttevent(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttevent_component, &a0) ? (tt_obj)(opaque)a0 : (ttany)0;
}    
ttuint TTGetEvtype_ttevent(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttevent_evtype, &a0) ? (ttuint)a0 : (ttany)0;
}    
ttuint TTGetEvcode_ttevent(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttevent_evcode, &a0) ? (ttuint)a0 : (ttany)0;
}    
ttuint TTGetEvflags_ttevent(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttevent_evflags, &a0) ? (ttuint)a0 : (ttany)0;
}    
void TTConsume_ttevent(tt_obj o) {
    ChangeValue_ttobj((opaque)o, ttevent_evflags, ttevent_evflags_consumed, ttevent_evflags_consumed);
} 

/* handy tteventbig methods */
        
ttshort TTGetX_tteventbig(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, tteventbig_x, &a0) ? (ttshort)a0 : (ttany)0;
}    
ttshort TTGetY_tteventbig(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, tteventbig_y, &a0) ? (ttshort)a0 : (ttany)0;
}    
ttshort TTGetW_tteventbig(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, tteventbig_w, &a0) ? (ttshort)a0 : (ttany)0;
}    
ttshort TTGetH_tteventbig(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, tteventbig_h, &a0) ? (ttshort)a0 : (ttany)0;
}    
ttuint TTGetLen_tteventbig(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, tteventbig_len, &a0) ? (ttuint)a0 : (ttany)0;
}    
TT_CONST ttbyte * TTGetData_tteventbig(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, tteventbig_data, &a0) ? (TT_CONST ttbyte *)(opaque)a0 : (ttany)0;
} 

/* handy ttlistener methods */
        
ttuint TTGetLflags_ttlistener(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttlistener_lflags, &a0) ? (ttuint)a0 : (ttany)0;
}    
tt_obj TTGetComponent_ttlistener(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttlistener_component, &a0) ? (tt_obj)(opaque)a0 : (ttany)0;
}    
tt_obj TTGetPrev_ttlistener(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttlistener_prev, &a0) ? (tt_obj)(opaque)a0 : (ttany)0;
}    
tt_obj TTGetNext_ttlistener(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttlistener_next, &a0) ? (tt_obj)(opaque)a0 : (ttany)0;
} 

/* handy ttcallback methods */
    
tt_obj TTGetAvlLeft_ttcallback(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttcallback_avl_left, &a0) ? (tt_obj)(opaque)a0 : (ttany)0;
}    
tt_obj TTGetAvlRight_ttcallback(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttcallback_avl_right, &a0) ? (tt_obj)(opaque)a0 : (ttany)0;
}    
tt_obj TTGetAvlParent_ttcallback(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttcallback_avl_parent, &a0) ? (tt_obj)(opaque)a0 : (ttany)0;
}            
tt_obj TTGetEvent_ttcallback(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttcallback_event, &a0) ? (tt_obj)(opaque)a0 : (ttany)0;
}    
ttuint TTGetNargComponent_ttcallback(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttcallback_narg_component, &a0) ? (ttuint)a0 : (ttany)0;
}    
ttuint TTGetNargEvent_ttcallback(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttcallback_narg_event, &a0) ? (ttuint)a0 : (ttany)0;
}    
ttuint TTGetNargs_ttcallback(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttcallback_nargs, &a0) ? (ttuint)a0 : (ttany)0;
}    
ttcallback_fn TTGetFunction_ttcallback(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttcallback_function, &a0) ? (ttcallback_fn)(opaque)a0 : (ttany)0;
}        
TT_CONST ttany * TTGetArgs_ttcallback(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttcallback_args, &a0) ? (TT_CONST ttany *)(opaque)a0 : (ttany)0;
} 

/* handy ttcomponent methods */
        
tt_obj TTGetListeners_ttcomponent(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttcomponent_listeners, &a0) ? (tt_obj)(opaque)a0 : (ttany)0;
}    
tt_obj TTGetCallbacks_ttcomponent(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttcomponent_callbacks, &a0) ? (tt_obj)(opaque)a0 : (ttany)0;
} 

/* handy ttvisible methods */
        
ttuint TTGetVflags_ttvisible(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttvisible_vflags, &a0) ? (ttuint)a0 : (ttany)0;
}    
tt_obj TTGetPrev_ttvisible(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttvisible_prev, &a0) ? (tt_obj)(opaque)a0 : (ttany)0;
}    
tt_obj TTGetNext_ttvisible(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttvisible_next, &a0) ? (tt_obj)(opaque)a0 : (ttany)0;
}    
tt_obj TTGetParent_ttvisible(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttvisible_parent, &a0) ? (tt_obj)(opaque)a0 : (ttany)0;
}    
tt_obj TTGetChildFirst_ttvisible(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttvisible_child_first, &a0) ? (tt_obj)(opaque)a0 : (ttany)0;
}    
tt_obj TTGetChildLast_ttvisible(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttvisible_child_last, &a0) ? (tt_obj)(opaque)a0 : (ttany)0;
}    
tt_obj TTGetTheme_ttvisible(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttvisible_theme, &a0) ? (tt_obj)(opaque)a0 : (ttany)0;
}    
ttvisible_repaint_fn TTGetRepaint_ttvisible(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttvisible_repaint, &a0) ? (ttvisible_repaint_fn)(opaque)a0 : (ttany)0;
} 

/* handy ttnative methods */
     

/* handy ttwidget methods */
        
ttshort TTGetX_ttwidget(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttwidget_x, &a0) ? (ttshort)a0 : (ttany)0;
}    
ttshort TTGetY_ttwidget(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttwidget_y, &a0) ? (ttshort)a0 : (ttany)0;
}    
ttshort TTGetW_ttwidget(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttwidget_w, &a0) ? (ttshort)a0 : (ttany)0;
}    
ttshort TTGetH_ttwidget(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttwidget_h, &a0) ? (ttshort)a0 : (ttany)0;
}    
ttint TTGetXl_ttwidget(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttwidget_xl, &a0) ? (ttint)a0 : (ttany)0;
}    
ttint TTGetYl_ttwidget(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttwidget_yl, &a0) ? (ttint)a0 : (ttany)0;
} 

/* handy ttlabel methods */
        
ttshort TTGetTextLen_ttlabel(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttlabel_text_len, &a0) ? (ttshort)a0 : (ttany)0;
}        
TT_CONST ttfont * TTGetText_ttlabel(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttlabel_text, &a0) ? (TT_CONST ttfont *)(opaque)a0 : (ttany)0;
} 

/* handy ttanybutton methods */
            
ttshort TTGetTextWidth_ttanybutton(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttanybutton_text_width, &a0) ? (ttshort)a0 : (ttany)0;
}    
ttshort TTGetTextHeight_ttanybutton(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttanybutton_text_height, &a0) ? (ttshort)a0 : (ttany)0;
}        
TT_CONST ttfont * TTGetText_ttanybutton(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttanybutton_text, &a0) ? (TT_CONST ttfont *)(opaque)a0 : (ttany)0;
} 

/* handy ttbutton methods */
     

/* handy ttcheckbutton methods */
     

/* handy ttradiobutton methods */
     

/* handy ttscrollbar methods */
     

/* handy ttbuttongroup methods */
     

/* handy ttwindow methods */
         

/* handy ttframe methods */
     

/* handy ttscroller methods */
        
tt_obj TTGetScrollx_ttscroller(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttscroller_scrollx, &a0) ? (tt_obj)(opaque)a0 : (ttany)0;
}    
tt_obj TTGetScrolly_ttscroller(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttscroller_scrolly, &a0) ? (tt_obj)(opaque)a0 : (ttany)0;
} 

/* handy ttmenuitem methods */
         

/* handy ttcheckmenuitem methods */
     

/* handy ttradiomenuitem methods */
     

/* handy ttmenu methods */
    
    
tt_obj TTGetMenubar_ttmenu(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttmenu_menubar, &a0) ? (tt_obj)(opaque)a0 : (ttany)0;
} 

/* handy ttmenubar methods */
     

/* handy ttanytext methods */
        
ttshort TTGetTextLen_ttanytext(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttanytext_text_len, &a0) ? (ttshort)a0 : (ttany)0;
}        
TT_CONST ttfont * TTGetText_ttanytext(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttanytext_text, &a0) ? (TT_CONST ttfont *)(opaque)a0 : (ttany)0;
} 

/* handy tttextfield methods */
     

/* handy tttextarea methods */
     

/* handy tttheme methods */
    
            
        
TT_CONST ttbyte * TTGetName_tttheme(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, tttheme_name, &a0) ? (TT_CONST ttbyte *)(opaque)a0 : (ttany)0;
} 

/* handy ttapplication methods */
        
ttbyte * TTGetName_ttapplication(tt_obj o) {
    ttany a0;
    return GetValue_ttobj((opaque)o, ttapplication_name, &a0) ? (ttbyte *)(opaque)a0 : (ttany)0;
}







