



/* This file was automatically generated from m4/handy.m4, do not edit! */


/*
 *  handy_m4.h  --  wrappers around handy libTT methods
 *
 */



























/* handy public, public_set and exported methods */


























/* handy ttobj methods */
            
ttopaque TTGetId_ttobj(tt_obj o) {
    ttobj a0;
    if ((a0 = ID2(ttobj, o)))
	return a0->id;
    return (ttopaque)0;

}    
ttuint TTGetRefcount_ttobj(tt_obj o) {
    ttobj a0;
    if ((a0 = ID2(ttobj, o)))
	return a0->refcount;
    return (ttuint)0;

}    
ttuint TTGetOflags_ttobj(tt_obj o) {
    ttobj a0;
    if ((a0 = ID2(ttobj, o)))
	return a0->oflags;
    return (ttuint)0;

}            
ttany TTGetUserData_ttobj(tt_obj o) {
    ttobj a0;
    if ((a0 = ID2(ttobj, o)))
	return a0->user_data;
    return (ttany)0;

}    
ttopaque TTGetEventsInprogress_ttobj(tt_obj o) {
    ttobj a0;
    if ((a0 = ID2(ttobj, o)))
	return a0->events_inprogress;
    return (ttopaque)0;

}    
ttopaque TTGetEventsInformed_ttobj(tt_obj o) {
    ttobj a0;
    if ((a0 = ID2(ttobj, o)))
	return a0->events_informed;
    return (ttopaque)0;

}  

/* handy ttevent methods */
        
tt_obj TTGetCallback_ttevent(tt_obj o) {
    ttevent a0;
    if ((a0 = ID2(ttevent, o)))
	return (tt_obj)OBJ2ID(a0->callback);
    return (tt_obj)(opaque)0;

}    
tt_obj TTGetComponent_ttevent(tt_obj o) {
    ttevent a0;
    if ((a0 = ID2(ttevent, o)))
	return (tt_obj)OBJ2ID(a0->component);
    return (tt_obj)(opaque)0;

}    
ttuint TTGetEvtype_ttevent(tt_obj o) {
    ttevent a0;
    if ((a0 = ID2(ttevent, o)))
	return a0->evtype;
    return (ttuint)0;

}    
ttuint TTGetEvcode_ttevent(tt_obj o) {
    ttevent a0;
    if ((a0 = ID2(ttevent, o)))
	return a0->evcode;
    return (ttuint)0;

}    
ttuint TTGetEvflags_ttevent(tt_obj o) {
    ttevent a0;
    if ((a0 = ID2(ttevent, o)))
	return a0->evflags;
    return (ttuint)0;

}            
void TTConsume_ttevent(tt_obj o) {
    TTChangeField_ttobj(o, ttevent_evflags, ttevent_evflags_consumed, ttevent_evflags_consumed);
} 

/* handy tteventbig methods */
        
ttshort TTGetX_tteventbig(tt_obj o) {
    tteventbig a0;
    if ((a0 = ID2(tteventbig, o)))
	return a0->x;
    return (ttshort)0;

}    
ttshort TTGetY_tteventbig(tt_obj o) {
    tteventbig a0;
    if ((a0 = ID2(tteventbig, o)))
	return a0->y;
    return (ttshort)0;

}    
ttshort TTGetW_tteventbig(tt_obj o) {
    tteventbig a0;
    if ((a0 = ID2(tteventbig, o)))
	return a0->w;
    return (ttshort)0;

}    
ttshort TTGetH_tteventbig(tt_obj o) {
    tteventbig a0;
    if ((a0 = ID2(tteventbig, o)))
	return a0->h;
    return (ttshort)0;

}    
ttany TTGetValue_tteventbig(tt_obj o) {
    tteventbig a0;
    if ((a0 = ID2(tteventbig, o)))
	return a0->value;
    return (ttany)0;

}    
ttany TTGetOldValue_tteventbig(tt_obj o) {
    tteventbig a0;
    if ((a0 = ID2(tteventbig, o)))
	return a0->old_value;
    return (ttany)0;

}    
ttuint TTGetLen_tteventbig(tt_obj o) {
    tteventbig a0;
    if ((a0 = ID2(tteventbig, o)))
	return a0->len;
    return (ttuint)0;

}    
TT_CONST ttbyte * TTGetData_tteventbig(tt_obj o) {
    tteventbig a0;
    if ((a0 = ID2(tteventbig, o)))
	return a0->data;
    return (TT_CONST ttbyte *)(opaque)0;

} 

/* handy ttbitmask methods */
                     

/* handy tteventmask methods */
        
tt_obj TTGetEvtypeMask_tteventmask(tt_obj o) {
    tteventmask a0;
    if ((a0 = ID2(tteventmask, o)))
	return (tt_obj)OBJ2ID(a0->evtype_mask);
    return (tt_obj)(opaque)0;

}    
tt_obj TTGetEvcodeMask_tteventmask(tt_obj o) {
    tteventmask a0;
    if ((a0 = ID2(tteventmask, o)))
	return (tt_obj)OBJ2ID(a0->evcode_mask);
    return (tt_obj)(opaque)0;

}    
tt_obj TTGetComponentMask_tteventmask(tt_obj o) {
    tteventmask a0;
    if ((a0 = ID2(tteventmask, o)))
	return (tt_obj)OBJ2ID(a0->component_mask);
    return (tt_obj)(opaque)0;

}    
ttuint TTGetTruthTable_tteventmask(tt_obj o) {
    tteventmask a0;
    if ((a0 = ID2(tteventmask, o)))
	return a0->truth_table;
    return (ttuint)0;

} 

/* handy ttcallback methods */
        
ttuint TTGetLflags_ttcallback(tt_obj o) {
    ttcallback a0;
    if ((a0 = ID2(ttcallback, o)))
	return a0->lflags;
    return (ttuint)0;

}    
tt_obj TTGetComponent_ttcallback(tt_obj o) {
    ttcallback a0;
    if ((a0 = ID2(ttcallback, o)))
	return (tt_obj)OBJ2ID(a0->component);
    return (tt_obj)(opaque)0;

}    
tt_obj TTGetPrev_ttcallback(tt_obj o) {
    ttcallback a0;
    if ((a0 = ID2(ttcallback, o)))
	return (tt_obj)OBJ2ID(a0->prev);
    return (tt_obj)(opaque)0;

}    
tt_obj TTGetNext_ttcallback(tt_obj o) {
    ttcallback a0;
    if ((a0 = ID2(ttcallback, o)))
	return (tt_obj)OBJ2ID(a0->next);
    return (tt_obj)(opaque)0;

} 

/* handy ttlistener methods */
            
tt_obj TTGetEvent_ttlistener(tt_obj o) {
    ttlistener a0;
    if ((a0 = ID2(ttlistener, o)))
	return (tt_obj)OBJ2ID(a0->event);
    return (tt_obj)(opaque)0;

}    
ttuint TTGetNargComponent_ttlistener(tt_obj o) {
    ttlistener a0;
    if ((a0 = ID2(ttlistener, o)))
	return a0->narg_component;
    return (ttuint)0;

}    
ttuint TTGetNargEvent_ttlistener(tt_obj o) {
    ttlistener a0;
    if ((a0 = ID2(ttlistener, o)))
	return a0->narg_event;
    return (ttuint)0;

}    
ttuint TTGetNargs_ttlistener(tt_obj o) {
    ttlistener a0;
    if ((a0 = ID2(ttlistener, o)))
	return a0->nargs;
    return (ttuint)0;

}    
ttlistener_fn TTGetFunction_ttlistener(tt_obj o) {
    ttlistener a0;
    if ((a0 = ID2(ttlistener, o)))
	return a0->function;
    return (ttlistener_fn)(opaque)0;

}    
ttany * TTGetArgs_ttlistener(tt_obj o) {
    ttlistener a0;
    if ((a0 = ID2(ttlistener, o)))
	return a0->args;
    return (ttany *)(opaque)0;

}    
tt_obj TTGetEventMask_ttlistener(tt_obj o) {
    ttlistener a0;
    if ((a0 = ID2(ttlistener, o)))
	return (tt_obj)OBJ2ID(a0->event_mask);
    return (tt_obj)(opaque)0;

}         

/* handy tttimer methods */
                     

/* handy ttcomponent methods */
        
tt_obj TTGetCallbacks_ttcomponent(tt_obj o) {
    ttcomponent a0;
    if ((a0 = ID2(ttcomponent, o)))
	return (tt_obj)OBJ2ID(a0->callbacks);
    return (tt_obj)(opaque)0;

}    
tt_obj TTGetListeners_ttcomponent(tt_obj o) {
    ttcomponent a0;
    if ((a0 = ID2(ttcomponent, o)))
	return (tt_obj)OBJ2ID(a0->listeners);
    return (tt_obj)(opaque)0;

}    
tt_obj TTGetTimers_ttcomponent(tt_obj o) {
    ttcomponent a0;
    if ((a0 = ID2(ttcomponent, o)))
	return (tt_obj)OBJ2ID(a0->timers);
    return (tt_obj)(opaque)0;

}    
tt_obj TTGetDatas_ttcomponent(tt_obj o) {
    ttcomponent a0;
    if ((a0 = ID2(ttcomponent, o)))
	return (tt_obj)OBJ2ID(a0->datas);
    return (tt_obj)(opaque)0;

} 

/* handy ttdata methods */
            
tt_obj TTGetComponent_ttdata(tt_obj o) {
    ttdata a0;
    if ((a0 = ID2(ttdata, o)))
	return (tt_obj)OBJ2ID(a0->component);
    return (tt_obj)(opaque)0;

}    
ttbyte * TTGetKey_ttdata(tt_obj o) {
    ttdata a0;
    if ((a0 = ID2(ttdata, o)))
	return a0->key;
    return (ttbyte *)(opaque)0;

}    
ttopaque TTGetKeyLen_ttdata(tt_obj o) {
    ttdata a0;
    if ((a0 = ID2(ttdata, o)))
	return a0->key_len;
    return (ttopaque)0;

}    
ttany TTGetData_ttdata(tt_obj o) {
    ttdata a0;
    if ((a0 = ID2(ttdata, o)))
	return a0->data;
    return (ttany)0;

} 

/* handy ttvisible methods */
        
ttuint TTGetVflags_ttvisible(tt_obj o) {
    ttvisible a0;
    if ((a0 = ID2(ttvisible, o)))
	return a0->vflags;
    return (ttuint)0;

}    
tt_obj TTGetPrev_ttvisible(tt_obj o) {
    ttvisible a0;
    if ((a0 = ID2(ttvisible, o)))
	return (tt_obj)OBJ2ID(a0->prev);
    return (tt_obj)(opaque)0;

}    
tt_obj TTGetNext_ttvisible(tt_obj o) {
    ttvisible a0;
    if ((a0 = ID2(ttvisible, o)))
	return (tt_obj)OBJ2ID(a0->next);
    return (tt_obj)(opaque)0;

}    
tt_obj TTGetParent_ttvisible(tt_obj o) {
    ttvisible a0;
    if ((a0 = ID2(ttvisible, o)))
	return (tt_obj)OBJ2ID(a0->parent);
    return (tt_obj)(opaque)0;

}    
tt_obj TTGetChildFirst_ttvisible(tt_obj o) {
    ttvisible a0;
    if ((a0 = ID2(ttvisible, o)))
	return (tt_obj)OBJ2ID(a0->child_first);
    return (tt_obj)(opaque)0;

}    
tt_obj TTGetChildLast_ttvisible(tt_obj o) {
    ttvisible a0;
    if ((a0 = ID2(ttvisible, o)))
	return (tt_obj)OBJ2ID(a0->child_last);
    return (tt_obj)(opaque)0;

}    
tt_obj TTGetTheme_ttvisible(tt_obj o) {
    ttvisible a0;
    if ((a0 = ID2(ttvisible, o)))
	return (tt_obj)OBJ2ID(a0->theme);
    return (tt_obj)(opaque)0;

}    
ttvisible_repaint_fn TTGetRepaint_ttvisible(tt_obj o) {
    ttvisible a0;
    if ((a0 = ID2(ttvisible, o)))
	return a0->repaint;
    return (ttvisible_repaint_fn)(opaque)0;

} 

/* handy ttnative methods */
     

/* handy ttwidget methods */
        
ttshort TTGetX_ttwidget(tt_obj o) {
    ttwidget a0;
    if ((a0 = ID2(ttwidget, o)))
	return a0->x;
    return (ttshort)0;

}    
ttshort TTGetY_ttwidget(tt_obj o) {
    ttwidget a0;
    if ((a0 = ID2(ttwidget, o)))
	return a0->y;
    return (ttshort)0;

}    
ttshort TTGetW_ttwidget(tt_obj o) {
    ttwidget a0;
    if ((a0 = ID2(ttwidget, o)))
	return a0->w;
    return (ttshort)0;

}    
ttshort TTGetH_ttwidget(tt_obj o) {
    ttwidget a0;
    if ((a0 = ID2(ttwidget, o)))
	return a0->h;
    return (ttshort)0;

}    
ttint TTGetXl_ttwidget(tt_obj o) {
    ttwidget a0;
    if ((a0 = ID2(ttwidget, o)))
	return a0->xl;
    return (ttint)0;

}    
ttint TTGetYl_ttwidget(tt_obj o) {
    ttwidget a0;
    if ((a0 = ID2(ttwidget, o)))
	return a0->yl;
    return (ttint)0;

}    
ttint TTGetWl_ttwidget(tt_obj o) {
    ttwidget a0;
    if ((a0 = ID2(ttwidget, o)))
	return a0->wl;
    return (ttint)0;

}    
ttint TTGetHl_ttwidget(tt_obj o) {
    ttwidget a0;
    if ((a0 = ID2(ttwidget, o)))
	return a0->hl;
    return (ttint)0;

}        
tt_obj TTGetTooltip_ttwidget(tt_obj o) {
    ttwidget a0;
    if ((a0 = ID2(ttwidget, o)))
	return (tt_obj)OBJ2ID(a0->tooltip);
    return (tt_obj)(opaque)0;

} 

/* handy ttlabel methods */
        
ttopaque TTGetTextLen_ttlabel(tt_obj o) {
    ttlabel a0;
    if ((a0 = ID2(ttlabel, o)))
	return a0->text_len;
    return (ttopaque)0;

}    
ttfont * TTGetText_ttlabel(tt_obj o) {
    ttlabel a0;
    if ((a0 = ID2(ttlabel, o)))
	return a0->text;
    return (ttfont *)(opaque)0;

} 

/* handy tttooltip methods */
            
tt_obj TTGetWidget_tttooltip(tt_obj o) {
    tttooltip a0;
    if ((a0 = ID2(tttooltip, o)))
	return (tt_obj)OBJ2ID(a0->widget);
    return (tt_obj)(opaque)0;

}     

/* handy ttbuttongroup methods */
        
tt_obj TTGetGroupFirst_ttbuttongroup(tt_obj o) {
    ttbuttongroup a0;
    if ((a0 = ID2(ttbuttongroup, o)))
	return (tt_obj)OBJ2ID(a0->group_first);
    return (tt_obj)(opaque)0;

}    
tt_obj TTGetGroupLast_ttbuttongroup(tt_obj o) {
    ttbuttongroup a0;
    if ((a0 = ID2(ttbuttongroup, o)))
	return (tt_obj)OBJ2ID(a0->group_last);
    return (tt_obj)(opaque)0;

}    
tt_obj TTGetChecked_ttbuttongroup(tt_obj o) {
    ttbuttongroup a0;
    if ((a0 = ID2(ttbuttongroup, o)))
	return (tt_obj)OBJ2ID(a0->checked);
    return (tt_obj)(opaque)0;

} 

/* handy ttanybutton methods */
        
ttattr * TTGetText_ttanybutton(tt_obj o) {
    ttanybutton a0;
    if ((a0 = ID2(ttanybutton, o)))
	return a0->text;
    return (ttattr *)(opaque)0;

}    
ttshort TTGetTextWidth_ttanybutton(tt_obj o) {
    ttanybutton a0;
    if ((a0 = ID2(ttanybutton, o)))
	return a0->text_width;
    return (ttshort)0;

}    
ttshort TTGetTextHeight_ttanybutton(tt_obj o) {
    ttanybutton a0;
    if ((a0 = ID2(ttanybutton, o)))
	return a0->text_height;
    return (ttshort)0;

}     

/* handy ttbutton methods */
     

/* handy ttcheckbutton methods */
     

/* handy ttradiobutton methods */
        
tt_obj TTGetGroup_ttradiobutton(tt_obj o) {
    ttradiobutton a0;
    if ((a0 = ID2(ttradiobutton, o)))
	return (tt_obj)OBJ2ID(a0->group);
    return (tt_obj)(opaque)0;

}    
tt_obj TTGetGroupPrev_ttradiobutton(tt_obj o) {
    ttradiobutton a0;
    if ((a0 = ID2(ttradiobutton, o)))
	return (tt_obj)OBJ2ID(a0->group_prev);
    return (tt_obj)(opaque)0;

}    
tt_obj TTGetGroupNext_ttradiobutton(tt_obj o) {
    ttradiobutton a0;
    if ((a0 = ID2(ttradiobutton, o)))
	return (tt_obj)OBJ2ID(a0->group_next);
    return (tt_obj)(opaque)0;

} 

/* handy ttanyscroll methods */
        
ttbyte TTGetOrientation_ttanyscroll(tt_obj o) {
    ttanyscroll a0;
    if ((a0 = ID2(ttanyscroll, o)))
	return a0->orientation;
    return (ttbyte)0;

}    
ttint TTGetSize_ttanyscroll(tt_obj o) {
    ttanyscroll a0;
    if ((a0 = ID2(ttanyscroll, o)))
	return a0->size;
    return (ttint)0;

}    
ttint TTGetRealSize_ttanyscroll(tt_obj o) {
    ttanyscroll a0;
    if ((a0 = ID2(ttanyscroll, o)))
	return a0->real_size;
    return (ttint)0;

}    
ttint TTGetViewSize_ttanyscroll(tt_obj o) {
    ttanyscroll a0;
    if ((a0 = ID2(ttanyscroll, o)))
	return a0->view_size;
    return (ttint)0;

}        
ttint TTGetPosition_ttanyscroll(tt_obj o) {
    ttanyscroll a0;
    if ((a0 = ID2(ttanyscroll, o)))
	return a0->position;
    return (ttint)0;

}    
ttint TTGetRealPosition_ttanyscroll(tt_obj o) {
    ttanyscroll a0;
    if ((a0 = ID2(ttanyscroll, o)))
	return a0->real_position;
    return (ttint)0;

}     

/* handy ttscrollbar methods */
         

/* handy ttslider methods */
        
ttint TTGetSlideMin_ttslider(tt_obj o) {
    ttslider a0;
    if ((a0 = ID2(ttslider, o)))
	return a0->slide_min;
    return (ttint)0;

}    
ttint TTGetSlideMax_ttslider(tt_obj o) {
    ttslider a0;
    if ((a0 = ID2(ttslider, o)))
	return a0->slide_max;
    return (ttint)0;

}    
ttint TTGetSlideValue_ttslider(tt_obj o) {
    ttslider a0;
    if ((a0 = ID2(ttslider, o)))
	return a0->slide_value;
    return (ttint)0;

} 

/* handy ttprogressbar methods */
             

/* handy ttscrollpane methods */
        
tt_obj TTGetBarX_ttscrollpane(tt_obj o) {
    ttscrollpane a0;
    if ((a0 = ID2(ttscrollpane, o)))
	return (tt_obj)OBJ2ID(a0->bar_x);
    return (tt_obj)(opaque)0;

}    
tt_obj TTGetBarY_ttscrollpane(tt_obj o) {
    ttscrollpane a0;
    if ((a0 = ID2(ttscrollpane, o)))
	return (tt_obj)OBJ2ID(a0->bar_y);
    return (tt_obj)(opaque)0;

} 

/* handy ttwindow methods */
        
ttopaque TTGetTitleLen_ttwindow(tt_obj o) {
    ttwindow a0;
    if ((a0 = ID2(ttwindow, o)))
	return a0->title_len;
    return (ttopaque)0;

}    
ttbyte * TTGetTitle_ttwindow(tt_obj o) {
    ttwindow a0;
    if ((a0 = ID2(ttwindow, o)))
	return a0->title;
    return (ttbyte *)(opaque)0;

} 

/* handy ttframe methods */
        
tt_obj TTGetMenubar_ttframe(tt_obj o) {
    ttframe a0;
    if ((a0 = ID2(ttframe, o)))
	return (tt_obj)OBJ2ID(a0->menubar);
    return (tt_obj)(opaque)0;

} 

/* handy ttscroller methods */
        
tt_obj TTGetScrollpane_ttscroller(tt_obj o) {
    ttscroller a0;
    if ((a0 = ID2(ttscroller, o)))
	return (tt_obj)OBJ2ID(a0->scrollpane);
    return (tt_obj)(opaque)0;

} 

/* handy ttmenuitem methods */
        
ttbyte * TTGetName_ttmenuitem(tt_obj o) {
    ttmenuitem a0;
    if ((a0 = ID2(ttmenuitem, o)))
	return a0->name;
    return (ttbyte *)(opaque)0;

} 

/* handy ttcheckmenuitem methods */
     

/* handy ttradiomenuitem methods */
     

/* handy ttmenu methods */
    
    
tt_obj TTGetMenubar_ttmenu(tt_obj o) {
    ttmenu a0;
    if ((a0 = ID2(ttmenu, o)))
	return (tt_obj)OBJ2ID(a0->menubar);
    return (tt_obj)(opaque)0;

} 

/* handy ttmenubar methods */
     

/* handy ttanytext methods */
        
ttopaque TTGetTextLen_ttanytext(tt_obj o) {
    ttanytext a0;
    if ((a0 = ID2(ttanytext, o)))
	return a0->text_len;
    return (ttopaque)0;

}    
ttattr * TTGetText_ttanytext(tt_obj o) {
    ttanytext a0;
    if ((a0 = ID2(ttanytext, o)))
	return a0->text;
    return (ttattr *)(opaque)0;

} 

/* handy tttextfield methods */
     

/* handy tttextarea methods */
     

/* handy tttheme methods */
    
    
ttbyte * TTGetName_tttheme(tt_obj o) {
    tttheme a0;
    if ((a0 = ID2(tttheme, o)))
	return a0->name;
    return (ttbyte *)(opaque)0;

}                 

/* handy ttapplication methods */
        
ttbyte * TTGetName_ttapplication(tt_obj o) {
    ttapplication a0;
    if ((a0 = ID2(ttapplication, o)))
	return a0->name;
    return (ttbyte *)(opaque)0;

}








/* back-end functions for TTGetField_ttobj() */


                                    
static tt_fn handy_GetField_ttobj(ttobj o, ttuint which, ttany *value) {
    if (which && which < ttobj_field_last && o && TTAssert(IS(ttobj,o))) {
	if (which > ttobj_field_first) switch (which) {
        
	  case ttobj_id:
	    *value = (ttany)o->id; return TTFN_ttopaque;    
	  case ttobj_refcount:
	    *value = (ttany)o->refcount; return TTFN_ttuint;    
	  case ttobj_oflags:
	    *value = (ttany)o->oflags; return TTFN_ttuint;            
	  case ttobj_user_data:
	    *value = (ttany)o->user_data; return TTFN_ttany;    
	  case ttobj_events_inprogress:
	    *value = (ttany)o->events_inprogress; return TTFN_ttopaque;    
	  case ttobj_events_informed:
	    *value = (ttany)o->events_informed; return TTFN_ttopaque;
	}
	
    }
    return (tt_fn)0;
}  
                            
static tt_fn handy_GetField_ttevent(ttevent o, ttuint which, ttany *value) {
    if (which && which < ttevent_field_last && o && TTAssert(IS(ttevent,o))) {
	if (which > ttevent_field_first) switch (which) {
    
	  case ttevent_callback:
	    *value = (ttany)(tt_obj)OBJ2ID(o->callback); return TTFN_ttlistener;    
	  case ttevent_component:
	    *value = (ttany)(tt_obj)OBJ2ID(o->component); return TTFN_ttcomponent;    
	  case ttevent_evtype:
	    *value = (ttany)o->evtype; return TTFN_ttuint;    
	  case ttevent_evcode:
	    *value = (ttany)o->evcode; return TTFN_ttuint;    
	  case ttevent_evflags:
	    *value = (ttany)o->evflags; return TTFN_ttuint;        
	}
	else
	    return handy_GetField_ttobj((ttobj)o, which, value);
    }
    return (tt_fn)0;
} 
                                
static tt_fn handy_GetField_tteventbig(tteventbig o, ttuint which, ttany *value) {
    if (which && which < tteventbig_field_last && o && TTAssert(IS(tteventbig,o))) {
	if (which > tteventbig_field_first) switch (which) {
    
	  case tteventbig_x:
	    *value = (ttany)o->x; return TTFN_ttshort;    
	  case tteventbig_y:
	    *value = (ttany)o->y; return TTFN_ttshort;    
	  case tteventbig_w:
	    *value = (ttany)o->w; return TTFN_ttshort;    
	  case tteventbig_h:
	    *value = (ttany)o->h; return TTFN_ttshort;    
	  case tteventbig_value:
	    *value = (ttany)o->value; return TTFN_ttany;    
	  case tteventbig_old_value:
	    *value = (ttany)o->old_value; return TTFN_ttany;    
	  case tteventbig_len:
	    *value = (ttany)o->len; return TTFN_ttuint;    
	  case tteventbig_data:
	    *value = (ttany)(opaque)o->data; return TTFN_ttstring;
	}
	else
	    return handy_GetField_ttevent((ttevent)o, which, value);
    }
    return (tt_fn)0;
} 
                 
                
static tt_fn handy_GetField_tteventmask(tteventmask o, ttuint which, ttany *value) {
    if (which && which < tteventmask_field_last && o && TTAssert(IS(tteventmask,o))) {
	if (which > tteventmask_field_first) switch (which) {
    
	  case tteventmask_evtype_mask:
	    *value = (ttany)(tt_obj)OBJ2ID(o->evtype_mask); return TTFN_ttbitmask;    
	  case tteventmask_evcode_mask:
	    *value = (ttany)(tt_obj)OBJ2ID(o->evcode_mask); return TTFN_ttbitmask;    
	  case tteventmask_component_mask:
	    *value = (ttany)(tt_obj)OBJ2ID(o->component_mask); return TTFN_ttbitmask;    
	  case tteventmask_truth_table:
	    *value = (ttany)o->truth_table; return TTFN_ttuint;
	}
	else
	    return handy_GetField_ttobj((ttobj)o, which, value);
    }
    return (tt_fn)0;
} 
                
static tt_fn handy_GetField_ttcallback(ttcallback o, ttuint which, ttany *value) {
    if (which && which < ttcallback_field_last && o && TTAssert(IS(ttcallback,o))) {
	if (which > ttcallback_field_first) switch (which) {
    
	  case ttcallback_lflags:
	    *value = (ttany)o->lflags; return TTFN_ttuint;    
	  case ttcallback_component:
	    *value = (ttany)(tt_obj)OBJ2ID(o->component); return TTFN_ttcomponent;    
	  case ttcallback_prev:
	    *value = (ttany)(tt_obj)OBJ2ID(o->prev); return TTFN_ttcallback;    
	  case ttcallback_next:
	    *value = (ttany)(tt_obj)OBJ2ID(o->next); return TTFN_ttcallback;
	}
	else
	    return handy_GetField_ttobj((ttobj)o, which, value);
    }
    return (tt_fn)0;
} 
                                        
static tt_fn handy_GetField_ttlistener(ttlistener o, ttuint which, ttany *value) {
    if (which && which < ttlistener_field_last && o && TTAssert(IS(ttlistener,o))) {
	if (which > ttlistener_field_first) switch (which) {
        
	  case ttlistener_event:
	    *value = (ttany)(tt_obj)OBJ2ID(o->event); return TTFN_ttevent;    
	  case ttlistener_narg_component:
	    *value = (ttany)o->narg_component; return TTFN_ttuint;    
	  case ttlistener_narg_event:
	    *value = (ttany)o->narg_event; return TTFN_ttuint;    
	  case ttlistener_nargs:
	    *value = (ttany)o->nargs; return TTFN_ttuint;    
	  case ttlistener_function:
	    *value = (ttany)(opaque)o->function; return TTFN_ttfunction;    
	  case ttlistener_args:
	    *value = (ttany)(opaque)o->args; return TTFN_ttpointer;    
	  case ttlistener_event_mask:
	    *value = (ttany)(tt_obj)OBJ2ID(o->event_mask); return TTFN_tteventmask;        
	}
	else
	    return handy_GetField_ttcallback((ttcallback)o, which, value);
    }
    return (tt_fn)0;
} 
                 
                
static tt_fn handy_GetField_ttcomponent(ttcomponent o, ttuint which, ttany *value) {
    if (which && which < ttcomponent_field_last && o && TTAssert(IS(ttcomponent,o))) {
	if (which > ttcomponent_field_first) switch (which) {
    
	  case ttcomponent_callbacks:
	    *value = (ttany)(tt_obj)OBJ2ID(o->callbacks); return TTFN_ttcallback;    
	  case ttcomponent_listeners:
	    *value = (ttany)(tt_obj)OBJ2ID(o->listeners); return TTFN_ttlistener;    
	  case ttcomponent_timers:
	    *value = (ttany)(tt_obj)OBJ2ID(o->timers); return TTFN_tttimer;    
	  case ttcomponent_datas:
	    *value = (ttany)(tt_obj)OBJ2ID(o->datas); return TTFN_ttdata;
	}
	else
	    return handy_GetField_ttobj((ttobj)o, which, value);
    }
    return (tt_fn)0;
} 
                    
static tt_fn handy_GetField_ttdata(ttdata o, ttuint which, ttany *value) {
    if (which && which < ttdata_field_last && o && TTAssert(IS(ttdata,o))) {
	if (which > ttdata_field_first) switch (which) {
        
	  case ttdata_component:
	    *value = (ttany)(tt_obj)OBJ2ID(o->component); return TTFN_ttcomponent;    
	  case ttdata_key:
	    *value = (ttany)(opaque)o->key; return TTFN_ttstring;    
	  case ttdata_key_len:
	    *value = (ttany)o->key_len; return TTFN_ttopaque;    
	  case ttdata_data:
	    *value = (ttany)o->data; return TTFN_ttany;
	}
	else
	    return handy_GetField_ttcomponent((ttcomponent)o, which, value);
    }
    return (tt_fn)0;
} 
                                
static tt_fn handy_GetField_ttvisible(ttvisible o, ttuint which, ttany *value) {
    if (which && which < ttvisible_field_last && o && TTAssert(IS(ttvisible,o))) {
	if (which > ttvisible_field_first) switch (which) {
    
	  case ttvisible_vflags:
	    *value = (ttany)o->vflags; return TTFN_ttuint;    
	  case ttvisible_prev:
	    *value = (ttany)(tt_obj)OBJ2ID(o->prev); return TTFN_ttvisible;    
	  case ttvisible_next:
	    *value = (ttany)(tt_obj)OBJ2ID(o->next); return TTFN_ttvisible;    
	  case ttvisible_parent:
	    *value = (ttany)(tt_obj)OBJ2ID(o->parent); return TTFN_ttvisible;    
	  case ttvisible_child_first:
	    *value = (ttany)(tt_obj)OBJ2ID(o->child_first); return TTFN_ttvisible;    
	  case ttvisible_child_last:
	    *value = (ttany)(tt_obj)OBJ2ID(o->child_last); return TTFN_ttvisible;    
	  case ttvisible_theme:
	    *value = (ttany)(tt_obj)OBJ2ID(o->theme); return TTFN_tttheme;    
	  case ttvisible_repaint:
	    *value = (ttany)(opaque)o->repaint; return TTFN_ttfunction;
	}
	else
	    return handy_GetField_ttcomponent((ttcomponent)o, which, value);
    }
    return (tt_fn)0;
} 
 
                                        
static tt_fn handy_GetField_ttwidget(ttwidget o, ttuint which, ttany *value) {
    if (which && which < ttwidget_field_last && o && TTAssert(IS(ttwidget,o))) {
	if (which > ttwidget_field_first) switch (which) {
    
	  case ttwidget_x:
	    *value = (ttany)o->x; return TTFN_ttshort;    
	  case ttwidget_y:
	    *value = (ttany)o->y; return TTFN_ttshort;    
	  case ttwidget_w:
	    *value = (ttany)o->w; return TTFN_ttshort;    
	  case ttwidget_h:
	    *value = (ttany)o->h; return TTFN_ttshort;    
	  case ttwidget_xl:
	    *value = (ttany)o->xl; return TTFN_ttint;    
	  case ttwidget_yl:
	    *value = (ttany)o->yl; return TTFN_ttint;    
	  case ttwidget_wl:
	    *value = (ttany)o->wl; return TTFN_ttint;    
	  case ttwidget_hl:
	    *value = (ttany)o->hl; return TTFN_ttint;        
	  case ttwidget_tooltip:
	    *value = (ttany)(tt_obj)OBJ2ID(o->tooltip); return TTFN_tttooltip;
	}
	else
	    return handy_GetField_ttvisible((ttvisible)o, which, value);
    }
    return (tt_fn)0;
} 
        
static tt_fn handy_GetField_ttlabel(ttlabel o, ttuint which, ttany *value) {
    if (which && which < ttlabel_field_last && o && TTAssert(IS(ttlabel,o))) {
	if (which > ttlabel_field_first) switch (which) {
    
	  case ttlabel_text_len:
	    *value = (ttany)o->text_len; return TTFN_ttopaque;    
	  case ttlabel_text:
	    *value = (ttany)(opaque)o->text; return TTFN_ttpointer;
	}
	else
	    return handy_GetField_ttwidget((ttwidget)o, which, value);
    }
    return (tt_fn)0;
} 
            
static tt_fn handy_GetField_tttooltip(tttooltip o, ttuint which, ttany *value) {
    if (which && which < tttooltip_field_last && o && TTAssert(IS(tttooltip,o))) {
	if (which > tttooltip_field_first) switch (which) {
        
	  case tttooltip_widget:
	    *value = (ttany)(tt_obj)OBJ2ID(o->widget); return TTFN_ttwidget;    
	}
	else
	    return handy_GetField_ttlabel((ttlabel)o, which, value);
    }
    return (tt_fn)0;
} 
            
static tt_fn handy_GetField_ttbuttongroup(ttbuttongroup o, ttuint which, ttany *value) {
    if (which && which < ttbuttongroup_field_last && o && TTAssert(IS(ttbuttongroup,o))) {
	if (which > ttbuttongroup_field_first) switch (which) {
    
	  case ttbuttongroup_group_first:
	    *value = (ttany)(tt_obj)OBJ2ID(o->group_first); return TTFN_ttradiobutton;    
	  case ttbuttongroup_group_last:
	    *value = (ttany)(tt_obj)OBJ2ID(o->group_last); return TTFN_ttradiobutton;    
	  case ttbuttongroup_checked:
	    *value = (ttany)(tt_obj)OBJ2ID(o->checked); return TTFN_ttradiobutton;
	}
	else
	    return handy_GetField_ttcomponent((ttcomponent)o, which, value);
    }
    return (tt_fn)0;
} 
                
static tt_fn handy_GetField_ttanybutton(ttanybutton o, ttuint which, ttany *value) {
    if (which && which < ttanybutton_field_last && o && TTAssert(IS(ttanybutton,o))) {
	if (which > ttanybutton_field_first) switch (which) {
    
	  case ttanybutton_text:
	    *value = (ttany)(opaque)o->text; return TTFN_ttpointer;    
	  case ttanybutton_text_width:
	    *value = (ttany)o->text_width; return TTFN_ttshort;    
	  case ttanybutton_text_height:
	    *value = (ttany)o->text_height; return TTFN_ttshort;    
	}
	else
	    return handy_GetField_ttwidget((ttwidget)o, which, value);
    }
    return (tt_fn)0;
} 
 
 
            
static tt_fn handy_GetField_ttradiobutton(ttradiobutton o, ttuint which, ttany *value) {
    if (which && which < ttradiobutton_field_last && o && TTAssert(IS(ttradiobutton,o))) {
	if (which > ttradiobutton_field_first) switch (which) {
    
	  case ttradiobutton_group:
	    *value = (ttany)(tt_obj)OBJ2ID(o->group); return TTFN_ttbuttongroup;    
	  case ttradiobutton_group_prev:
	    *value = (ttany)(tt_obj)OBJ2ID(o->group_prev); return TTFN_ttradiobutton;    
	  case ttradiobutton_group_next:
	    *value = (ttany)(tt_obj)OBJ2ID(o->group_next); return TTFN_ttradiobutton;
	}
	else
	    return handy_GetField_ttanybutton((ttanybutton)o, which, value);
    }
    return (tt_fn)0;
} 
                                
static tt_fn handy_GetField_ttanyscroll(ttanyscroll o, ttuint which, ttany *value) {
    if (which && which < ttanyscroll_field_last && o && TTAssert(IS(ttanyscroll,o))) {
	if (which > ttanyscroll_field_first) switch (which) {
    
	  case ttanyscroll_orientation:
	    *value = (ttany)o->orientation; return TTFN_ttbyte;    
	  case ttanyscroll_size:
	    *value = (ttany)o->size; return TTFN_ttint;    
	  case ttanyscroll_real_size:
	    *value = (ttany)o->real_size; return TTFN_ttint;    
	  case ttanyscroll_view_size:
	    *value = (ttany)o->view_size; return TTFN_ttint;        
	  case ttanyscroll_position:
	    *value = (ttany)o->position; return TTFN_ttint;    
	  case ttanyscroll_real_position:
	    *value = (ttany)o->real_position; return TTFN_ttint;    
	}
	else
	    return handy_GetField_ttanybutton((ttanybutton)o, which, value);
    }
    return (tt_fn)0;
} 
     
            
static tt_fn handy_GetField_ttslider(ttslider o, ttuint which, ttany *value) {
    if (which && which < ttslider_field_last && o && TTAssert(IS(ttslider,o))) {
	if (which > ttslider_field_first) switch (which) {
    
	  case ttslider_slide_min:
	    *value = (ttany)o->slide_min; return TTFN_ttint;    
	  case ttslider_slide_max:
	    *value = (ttany)o->slide_max; return TTFN_ttint;    
	  case ttslider_slide_value:
	    *value = (ttany)o->slide_value; return TTFN_ttint;
	}
	else
	    return handy_GetField_ttanyscroll((ttanyscroll)o, which, value);
    }
    return (tt_fn)0;
} 
         
        
static tt_fn handy_GetField_ttscrollpane(ttscrollpane o, ttuint which, ttany *value) {
    if (which && which < ttscrollpane_field_last && o && TTAssert(IS(ttscrollpane,o))) {
	if (which > ttscrollpane_field_first) switch (which) {
    
	  case ttscrollpane_bar_x:
	    *value = (ttany)(tt_obj)OBJ2ID(o->bar_x); return TTFN_ttscrollbar;    
	  case ttscrollpane_bar_y:
	    *value = (ttany)(tt_obj)OBJ2ID(o->bar_y); return TTFN_ttscrollbar;
	}
	else
	    return handy_GetField_ttwidget((ttwidget)o, which, value);
    }
    return (tt_fn)0;
} 
        
static tt_fn handy_GetField_ttwindow(ttwindow o, ttuint which, ttany *value) {
    if (which && which < ttwindow_field_last && o && TTAssert(IS(ttwindow,o))) {
	if (which > ttwindow_field_first) switch (which) {
    
	  case ttwindow_title_len:
	    *value = (ttany)o->title_len; return TTFN_ttopaque;    
	  case ttwindow_title:
	    *value = (ttany)(opaque)o->title; return TTFN_ttstring;
	}
	else
	    return handy_GetField_ttwidget((ttwidget)o, which, value);
    }
    return (tt_fn)0;
} 
    
static tt_fn handy_GetField_ttframe(ttframe o, ttuint which, ttany *value) {
    if (which && which < ttframe_field_last && o && TTAssert(IS(ttframe,o))) {
	if (which > ttframe_field_first) switch (which) {
    
	  case ttframe_menubar:
	    *value = (ttany)(tt_obj)OBJ2ID(o->menubar); return TTFN_ttmenubar;
	}
	else
	    return handy_GetField_ttwindow((ttwindow)o, which, value);
    }
    return (tt_fn)0;
} 
    
static tt_fn handy_GetField_ttscroller(ttscroller o, ttuint which, ttany *value) {
    if (which && which < ttscroller_field_last && o && TTAssert(IS(ttscroller,o))) {
	if (which > ttscroller_field_first) switch (which) {
    
	  case ttscroller_scrollpane:
	    *value = (ttany)(tt_obj)OBJ2ID(o->scrollpane); return TTFN_ttscrollpane;
	}
	else
	    return handy_GetField_ttwindow((ttwindow)o, which, value);
    }
    return (tt_fn)0;
} 
    
static tt_fn handy_GetField_ttmenuitem(ttmenuitem o, ttuint which, ttany *value) {
    if (which && which < ttmenuitem_field_last && o && TTAssert(IS(ttmenuitem,o))) {
	if (which > ttmenuitem_field_first) switch (which) {
    
	  case ttmenuitem_name:
	    *value = (ttany)(opaque)o->name; return TTFN_ttstring;
	}
	else
	    return handy_GetField_ttvisible((ttvisible)o, which, value);
    }
    return (tt_fn)0;
} 
 
 

    
static tt_fn handy_GetField_ttmenu(ttmenu o, ttuint which, ttany *value) {
    if (which && which < ttmenu_field_last && o && TTAssert(IS(ttmenu,o))) {
	if (which > ttmenu_field_first) switch (which) {

    
	  case ttmenu_menubar:
	    *value = (ttany)(tt_obj)OBJ2ID(o->menubar); return TTFN_ttmenubar;
	}
	else
	    return handy_GetField_ttmenuitem((ttmenuitem)o, which, value);
    }
    return (tt_fn)0;
} 
 
        
static tt_fn handy_GetField_ttanytext(ttanytext o, ttuint which, ttany *value) {
    if (which && which < ttanytext_field_last && o && TTAssert(IS(ttanytext,o))) {
	if (which > ttanytext_field_first) switch (which) {
    
	  case ttanytext_text_len:
	    *value = (ttany)o->text_len; return TTFN_ttopaque;    
	  case ttanytext_text:
	    *value = (ttany)(opaque)o->text; return TTFN_ttpointer;
	}
	else
	    return handy_GetField_ttwindow((ttwindow)o, which, value);
    }
    return (tt_fn)0;
} 
 
 

                    
static tt_fn handy_GetField_tttheme(tttheme o, ttuint which, ttany *value) {
    if (which && which < tttheme_field_last && o && TTAssert(IS(tttheme,o))) {
	if (which > tttheme_field_first) switch (which) {

    
	  case tttheme_name:
	    *value = (ttany)(opaque)o->name; return TTFN_ttstring;                
	}
	else
	    return handy_GetField_ttcomponent((ttcomponent)o, which, value);
    }
    return (tt_fn)0;
} 
    
static tt_fn handy_GetField_ttapplication(ttapplication o, ttuint which, ttany *value) {
    if (which && which < ttapplication_field_last && o && TTAssert(IS(ttapplication,o))) {
	if (which > ttapplication_field_first) switch (which) {
    
	  case ttapplication_name:
	    *value = (ttany)(opaque)o->name; return TTFN_ttstring;
	}
	else
	    return handy_GetField_ttcomponent((ttcomponent)o, which, value);
    }
    return (tt_fn)0;
}


typedef tt_fn (*get_fn)(TT_ARG_READ ttobj, ttuint, ttany *);

static tt_fn GetField(TT_ARG_READ ttobj o, ttuint which, ttany *value) {
    static get_fn fn [] = {
	(get_fn)0,


	(get_fn)handy_GetField_ttobj,  
	(get_fn)handy_GetField_ttevent, 
	(get_fn)handy_GetField_tteventbig, 
	(get_fn)handy_GetField_ttobj, 
	(get_fn)handy_GetField_tteventmask, 
	(get_fn)handy_GetField_ttcallback, 
	(get_fn)handy_GetField_ttlistener, 
	(get_fn)handy_GetField_ttlistener, 
	(get_fn)handy_GetField_ttcomponent, 
	(get_fn)handy_GetField_ttdata, 
	(get_fn)handy_GetField_ttvisible, 
	(get_fn)handy_GetField_ttvisible, 
	(get_fn)handy_GetField_ttwidget, 
	(get_fn)handy_GetField_ttlabel, 
	(get_fn)handy_GetField_tttooltip, 
	(get_fn)handy_GetField_ttbuttongroup, 
	(get_fn)handy_GetField_ttanybutton, 
	(get_fn)handy_GetField_ttanybutton, 
	(get_fn)handy_GetField_ttanybutton, 
	(get_fn)handy_GetField_ttradiobutton, 
	(get_fn)handy_GetField_ttanyscroll, 
	(get_fn)handy_GetField_ttanyscroll, 
	(get_fn)handy_GetField_ttslider, 
	(get_fn)handy_GetField_ttanyscroll, 
	(get_fn)handy_GetField_ttscrollpane, 
	(get_fn)handy_GetField_ttwindow, 
	(get_fn)handy_GetField_ttframe, 
	(get_fn)handy_GetField_ttscroller, 
	(get_fn)handy_GetField_ttmenuitem, 
	(get_fn)handy_GetField_ttmenuitem, 
	(get_fn)handy_GetField_ttmenuitem, 
	(get_fn)handy_GetField_ttmenu, 
	(get_fn)handy_GetField_ttvisible, 
	(get_fn)handy_GetField_ttanytext, 
	(get_fn)handy_GetField_ttanytext, 
	(get_fn)handy_GetField_ttanytext, 
	(get_fn)handy_GetField_tttheme, 
	(get_fn)handy_GetField_ttapplication,

    };
    ttuint i;
    
    if (TTAssert(o) && (i = o->id >> magic_shift) && i < sizeof(fn)/sizeof(fn[0])) {
	return fn[i]((ttobj)o, which, value);
    }
    return (tt_fn)0;
}

/* back-end function for TTSetField_ttobj() */


                                    
static tt_fn handy_SetField_ttobj(ttobj o, ttuint which, ttany value) {
    if (which && which < ttobj_field_last && o && TTAssert(IS(ttobj,o))) {
	if (which > ttobj_field_first) switch (which) {
                            
	  case ttobj_user_data:
	    return TTSetUserData_ttobj((tt_obj)o->id, value);        
	}
	
    }
    return (tt_fn)0;
}  
                             
                                 
                 
                 
                 
                                         
                 
                 
                    
static tt_fn handy_SetField_ttdata(ttdata o, ttuint which, ttany value) {
    if (which && which < ttdata_field_last && o && TTAssert(IS(ttdata,o))) {
	if (which > ttdata_field_first) switch (which) {
                    
	  case ttdata_data:
	    return TTSetData_ttdata((tt_obj)o->id, value);
	}
	else
	    return handy_SetField_ttobj((ttobj)o, which, value);
    }
    return (tt_fn)0;
} 
                                 
 
                                         
         
             
             
                 
 
 
             
                                 
     
             
         
         
         
     
     
     
 
 

     
 
         
 
 

                     
    


typedef tt_fn (*set_fn)(TT_ARG_READ ttobj, ttuint, ttany);

static tt_fn SetField(TT_ARG_READ ttobj o, ttuint which, ttany value) {
    static set_fn fn [] = {
	(set_fn)0,


	(set_fn)handy_SetField_ttobj,  
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttdata, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj, 
	(set_fn)handy_SetField_ttobj,

    };
    ttuint i;
    
    if (TTAssert(o) && (i = o->id >> magic_shift) && i < sizeof(fn)/sizeof(fn[0])) {
	return fn[i]((ttobj)o, which, value);
    }
    return (tt_fn)0;
}



