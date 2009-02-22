



/* This file was automatically generated from m4/getset.m4, do not edit! */

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
 *  getset_m4.h  --  implementation of TTGet*_* methods
 *
 */

/*  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */























/* TTGet*_* methods */











 

/* TTGet*_ttobj methods */
         

/* TTGet*_ttclass methods */
             

/* TTGet*_ttobject methods */
    
ttopaque TTGetRefcount_ttobject(TT_ARG_READ tt_obj o) {
    ttobject a0;
    if ((a0 = ID2(ttobject, o)))
	return a0->refcount;
    return 0;
}    
ttuint TTGetOflags_ttobject(TT_ARG_READ tt_obj o) {
    ttobject a0;
    if ((a0 = ID2(ttobject, o)))
	return a0->oflags;
    return 0;
}            
ttany TTGetUserData_ttobject(TT_ARG_READ tt_obj o) {
    ttobject a0;
    if ((a0 = ID2(ttobject, o)))
	return a0->user_data;
    return 0;
}    
ttopaque TTGetEventsInprogress_ttobject(TT_ARG_READ tt_obj o) {
    ttobject a0;
    if ((a0 = ID2(ttobject, o)))
	return a0->events_inprogress;
    return 0;
}    
ttopaque TTGetEventsInformed_ttobject(TT_ARG_READ tt_obj o) {
    ttobject a0;
    if ((a0 = ID2(ttobject, o)))
	return a0->events_informed;
    return 0;
} 

/* TTGet*_ttvector methods */
    
ttopaque TTGetArraySize_ttvector(TT_ARG_READ tt_obj o) {
    ttvector a0;
    if ((a0 = ID2(ttvector, o)))
	return a0->array_size;
    return 0;
}    
ttopaque TTGetArrayN_ttvector(TT_ARG_READ tt_obj o) {
    ttvector a0;
    if ((a0 = ID2(ttvector, o)))
	return a0->array_n;
    return 0;
}    
TT_ARG_ARRAY((_F(array_n))) ttany * TTGetArray_ttvector(TT_ARG_READ tt_obj o) {
    ttvector a0;
    if ((a0 = ID2(ttvector, o)))
	return a0->array;
    return 0;
} 

/* TTGet*_ttfield methods */
    
ttopaque TTGetNameLen_ttfield(TT_ARG_READ tt_obj o) {
    ttfield a0;
    if ((a0 = ID2(ttfield, o)))
	return a0->name_len;
    return 0;
}    
TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * TTGetName_ttfield(TT_ARG_READ tt_obj o) {
    ttfield a0;
    if ((a0 = ID2(ttfield, o)))
	return a0->name;
    return 0;
}    
ttopaque TTGetType_ttfield(TT_ARG_READ tt_obj o) {
    ttfield a0;
    if ((a0 = ID2(ttfield, o)))
	return a0->type;
    return 0;
} 

/* TTGet*_ttmethod methods */
    
ttopaque TTGetNameLen_ttmethod(TT_ARG_READ tt_obj o) {
    ttmethod a0;
    if ((a0 = ID2(ttmethod, o)))
	return a0->name_len;
    return 0;
}    
TT_ARG_READ TT_ARG_ARRAY((_F(name_len))) ttbyte * TTGetName_ttmethod(TT_ARG_READ tt_obj o) {
    ttmethod a0;
    if ((a0 = ID2(ttmethod, o)))
	return a0->name;
    return 0;
}    
ttfunction_fn TTGetAddress_ttmethod(TT_ARG_READ tt_obj o) {
    ttmethod a0;
    if ((a0 = ID2(ttmethod, o)))
	return a0->address;
    return 0;
}    
ttopaque TTGetReturnType_ttmethod(TT_ARG_READ tt_obj o) {
    ttmethod a0;
    if ((a0 = ID2(ttmethod, o)))
	return a0->return_type;
    return 0;
}    
tt_obj TTGetParameterType_ttmethod(TT_ARG_READ tt_obj o) {
    ttmethod a0;
    if ((a0 = ID2(ttmethod, o)))
	return (tt_obj)OBJ2ID(a0->parameter_type);
    return 0;
} 

/* TTGet*_ttevent methods */
    
tt_obj TTGetComponent_ttevent(TT_ARG_READ tt_obj o) {
    ttevent a0;
    if ((a0 = ID2(ttevent, o)))
	return (tt_obj)OBJ2ID(a0->component);
    return 0;
}    
ttuint TTGetEvtype_ttevent(TT_ARG_READ tt_obj o) {
    ttevent a0;
    if ((a0 = ID2(ttevent, o)))
	return a0->evtype;
    return 0;
}    
ttuint TTGetEvcode_ttevent(TT_ARG_READ tt_obj o) {
    ttevent a0;
    if ((a0 = ID2(ttevent, o)))
	return a0->evcode;
    return 0;
}    
ttuint TTGetEvflags_ttevent(TT_ARG_READ tt_obj o) {
    ttevent a0;
    if ((a0 = ID2(ttevent, o)))
	return a0->evflags;
    return 0;
}    
tt_obj TTGetPrev_ttevent(TT_ARG_READ tt_obj o) {
    ttevent a0;
    if ((a0 = ID2(ttevent, o)))
	return (tt_obj)OBJ2ID(a0->prev);
    return 0;
}    
tt_obj TTGetNext_ttevent(TT_ARG_READ tt_obj o) {
    ttevent a0;
    if ((a0 = ID2(ttevent, o)))
	return (tt_obj)OBJ2ID(a0->next);
    return 0;
} 

/* TTGet*_tteventbig methods */
    
ttshort TTGetX_tteventbig(TT_ARG_READ tt_obj o) {
    tteventbig a0;
    if ((a0 = ID2(tteventbig, o)))
	return a0->x;
    return 0;
}    
ttshort TTGetY_tteventbig(TT_ARG_READ tt_obj o) {
    tteventbig a0;
    if ((a0 = ID2(tteventbig, o)))
	return a0->y;
    return 0;
}    
ttshort TTGetW_tteventbig(TT_ARG_READ tt_obj o) {
    tteventbig a0;
    if ((a0 = ID2(tteventbig, o)))
	return a0->w;
    return 0;
}    
ttshort TTGetH_tteventbig(TT_ARG_READ tt_obj o) {
    tteventbig a0;
    if ((a0 = ID2(tteventbig, o)))
	return a0->h;
    return 0;
}    
ttany TTGetValue_tteventbig(TT_ARG_READ tt_obj o) {
    tteventbig a0;
    if ((a0 = ID2(tteventbig, o)))
	return a0->value;
    return 0;
}    
ttany TTGetOldValue_tteventbig(TT_ARG_READ tt_obj o) {
    tteventbig a0;
    if ((a0 = ID2(tteventbig, o)))
	return a0->old_value;
    return 0;
}    
ttopaque TTGetDataLen_tteventbig(TT_ARG_READ tt_obj o) {
    tteventbig a0;
    if ((a0 = ID2(tteventbig, o)))
	return a0->data_len;
    return 0;
}    
TT_ARG_READ TT_ARG_ARRAY((_F(data_len))) ttbyte * TTGetData_tteventbig(TT_ARG_READ tt_obj o) {
    tteventbig a0;
    if ((a0 = ID2(tteventbig, o)))
	return a0->data;
    return 0;
} 

/* TTGet*_ttbitmask methods */
                 

/* TTGet*_tteventmask methods */
    
tt_obj TTGetEvtypeMask_tteventmask(TT_ARG_READ tt_obj o) {
    tteventmask a0;
    if ((a0 = ID2(tteventmask, o)))
	return (tt_obj)OBJ2ID(a0->evtype_mask);
    return 0;
}    
tt_obj TTGetEvcodeMask_tteventmask(TT_ARG_READ tt_obj o) {
    tteventmask a0;
    if ((a0 = ID2(tteventmask, o)))
	return (tt_obj)OBJ2ID(a0->evcode_mask);
    return 0;
}    
tt_obj TTGetComponentMask_tteventmask(TT_ARG_READ tt_obj o) {
    tteventmask a0;
    if ((a0 = ID2(tteventmask, o)))
	return (tt_obj)OBJ2ID(a0->component_mask);
    return 0;
}    
ttuint TTGetTruthTable_tteventmask(TT_ARG_READ tt_obj o) {
    tteventmask a0;
    if ((a0 = ID2(tteventmask, o)))
	return a0->truth_table;
    return 0;
} 

/* TTGet*_ttcallback methods */
    
ttuint TTGetLflags_ttcallback(TT_ARG_READ tt_obj o) {
    ttcallback a0;
    if ((a0 = ID2(ttcallback, o)))
	return a0->lflags;
    return 0;
}    
tt_obj TTGetComponent_ttcallback(TT_ARG_READ tt_obj o) {
    ttcallback a0;
    if ((a0 = ID2(ttcallback, o)))
	return (tt_obj)OBJ2ID(a0->component);
    return 0;
}    
tt_obj TTGetPrev_ttcallback(TT_ARG_READ tt_obj o) {
    ttcallback a0;
    if ((a0 = ID2(ttcallback, o)))
	return (tt_obj)OBJ2ID(a0->prev);
    return 0;
}    
tt_obj TTGetNext_ttcallback(TT_ARG_READ tt_obj o) {
    ttcallback a0;
    if ((a0 = ID2(ttcallback, o)))
	return (tt_obj)OBJ2ID(a0->next);
    return 0;
} 

/* TTGet*_ttlistener methods */
        
tt_obj TTGetEvent_ttlistener(TT_ARG_READ tt_obj o) {
    ttlistener a0;
    if ((a0 = ID2(ttlistener, o)))
	return (tt_obj)OBJ2ID(a0->event);
    return 0;
}    
ttlistener_fn TTGetFunction_ttlistener(TT_ARG_READ tt_obj o) {
    ttlistener a0;
    if ((a0 = ID2(ttlistener, o)))
	return a0->function;
    return 0;
}    
ttopaque TTGetArgComponentN_ttlistener(TT_ARG_READ tt_obj o) {
    ttlistener a0;
    if ((a0 = ID2(ttlistener, o)))
	return a0->arg_component_n;
    return 0;
}    
ttopaque TTGetArgEventN_ttlistener(TT_ARG_READ tt_obj o) {
    ttlistener a0;
    if ((a0 = ID2(ttlistener, o)))
	return a0->arg_event_n;
    return 0;
}    
tt_obj TTGetArgs_ttlistener(TT_ARG_READ tt_obj o) {
    ttlistener a0;
    if ((a0 = ID2(ttlistener, o)))
	return (tt_obj)OBJ2ID(a0->args);
    return 0;
}    
tt_obj TTGetEventMask_ttlistener(TT_ARG_READ tt_obj o) {
    ttlistener a0;
    if ((a0 = ID2(ttlistener, o)))
	return (tt_obj)OBJ2ID(a0->event_mask);
    return 0;
}         

/* TTGet*_tttimer methods */
    
ttany TTGetDelayT_tttimer(TT_ARG_READ tt_obj o) {
    tttimer a0;
    if ((a0 = ID2(tttimer, o)))
	return a0->delay_t;
    return 0;
}    
ttany TTGetDelayF_tttimer(TT_ARG_READ tt_obj o) {
    tttimer a0;
    if ((a0 = ID2(tttimer, o)))
	return a0->delay_f;
    return 0;
}    
tt_obj TTGetTimerPrev_tttimer(TT_ARG_READ tt_obj o) {
    tttimer a0;
    if ((a0 = ID2(tttimer, o)))
	return (tt_obj)OBJ2ID(a0->timer_prev);
    return 0;
}    
tt_obj TTGetTimerNext_tttimer(TT_ARG_READ tt_obj o) {
    tttimer a0;
    if ((a0 = ID2(tttimer, o)))
	return (tt_obj)OBJ2ID(a0->timer_next);
    return 0;
} 

/* TTGet*_ttcomponent methods */
    
tt_obj TTGetCallbacks_ttcomponent(TT_ARG_READ tt_obj o) {
    ttcomponent a0;
    if ((a0 = ID2(ttcomponent, o)))
	return (tt_obj)OBJ2ID(a0->callbacks);
    return 0;
}    
tt_obj TTGetListeners_ttcomponent(TT_ARG_READ tt_obj o) {
    ttcomponent a0;
    if ((a0 = ID2(ttcomponent, o)))
	return (tt_obj)OBJ2ID(a0->listeners);
    return 0;
}    
tt_obj TTGetTimers_ttcomponent(TT_ARG_READ tt_obj o) {
    ttcomponent a0;
    if ((a0 = ID2(ttcomponent, o)))
	return (tt_obj)OBJ2ID(a0->timers);
    return 0;
}    
tt_obj TTGetDatas_ttcomponent(TT_ARG_READ tt_obj o) {
    ttcomponent a0;
    if ((a0 = ID2(ttcomponent, o)))
	return (tt_obj)OBJ2ID(a0->datas);
    return 0;
} 

/* TTGet*_ttdata methods */
        
tt_obj TTGetComponent_ttdata(TT_ARG_READ tt_obj o) {
    ttdata a0;
    if ((a0 = ID2(ttdata, o)))
	return (tt_obj)OBJ2ID(a0->component);
    return 0;
}    
ttopaque TTGetKeyLen_ttdata(TT_ARG_READ tt_obj o) {
    ttdata a0;
    if ((a0 = ID2(ttdata, o)))
	return a0->key_len;
    return 0;
}    
TT_ARG_ARRAY((_F(key_len))) ttbyte * TTGetKey_ttdata(TT_ARG_READ tt_obj o) {
    ttdata a0;
    if ((a0 = ID2(ttdata, o)))
	return a0->key;
    return 0;
}    
ttany TTGetData_ttdata(TT_ARG_READ tt_obj o) {
    ttdata a0;
    if ((a0 = ID2(ttdata, o)))
	return a0->data;
    return 0;
} 

/* TTGet*_ttvisible methods */
    
ttuint TTGetVflags_ttvisible(TT_ARG_READ tt_obj o) {
    ttvisible a0;
    if ((a0 = ID2(ttvisible, o)))
	return a0->vflags;
    return 0;
}    
tt_obj TTGetPrev_ttvisible(TT_ARG_READ tt_obj o) {
    ttvisible a0;
    if ((a0 = ID2(ttvisible, o)))
	return (tt_obj)OBJ2ID(a0->prev);
    return 0;
}    
tt_obj TTGetNext_ttvisible(TT_ARG_READ tt_obj o) {
    ttvisible a0;
    if ((a0 = ID2(ttvisible, o)))
	return (tt_obj)OBJ2ID(a0->next);
    return 0;
}    
tt_obj TTGetParent_ttvisible(TT_ARG_READ tt_obj o) {
    ttvisible a0;
    if ((a0 = ID2(ttvisible, o)))
	return (tt_obj)OBJ2ID(a0->parent);
    return 0;
}    
tt_obj TTGetChildFirst_ttvisible(TT_ARG_READ tt_obj o) {
    ttvisible a0;
    if ((a0 = ID2(ttvisible, o)))
	return (tt_obj)OBJ2ID(a0->child_first);
    return 0;
}    
tt_obj TTGetChildLast_ttvisible(TT_ARG_READ tt_obj o) {
    ttvisible a0;
    if ((a0 = ID2(ttvisible, o)))
	return (tt_obj)OBJ2ID(a0->child_last);
    return 0;
}    
ttany TTGetConstraint_ttvisible(TT_ARG_READ tt_obj o) {
    ttvisible a0;
    if ((a0 = ID2(ttvisible, o)))
	return a0->constraint;
    return 0;
}    
tt_obj TTGetTheme_ttvisible(TT_ARG_READ tt_obj o) {
    ttvisible a0;
    if ((a0 = ID2(ttvisible, o)))
	return (tt_obj)OBJ2ID(a0->theme);
    return 0;
}    
ttvisible_repaint_fn TTGetRepaint_ttvisible(TT_ARG_READ tt_obj o) {
    ttvisible a0;
    if ((a0 = ID2(ttvisible, o)))
	return a0->repaint;
    return 0;
} 

/* TTGet*_ttlayout methods */
    
tt_obj TTGetWidget_ttlayout(TT_ARG_READ tt_obj o) {
    ttlayout a0;
    if ((a0 = ID2(ttlayout, o)))
	return (tt_obj)OBJ2ID(a0->widget);
    return 0;
} 

/* TTGet*_ttboxlayout methods */
    
ttuint TTGetOrientation_ttboxlayout(TT_ARG_READ tt_obj o) {
    ttboxlayout a0;
    if ((a0 = ID2(ttboxlayout, o)))
	return a0->orientation;
    return 0;
} 

/* TTGet*_ttborderlayout methods */
 

/* TTGet*_ttnative methods */
 

/* TTGet*_ttwidget methods */
    
ttshort TTGetX_ttwidget(TT_ARG_READ tt_obj o) {
    ttwidget a0;
    if ((a0 = ID2(ttwidget, o)))
	return a0->x;
    return 0;
}    
ttshort TTGetY_ttwidget(TT_ARG_READ tt_obj o) {
    ttwidget a0;
    if ((a0 = ID2(ttwidget, o)))
	return a0->y;
    return 0;
}    
ttshort TTGetW_ttwidget(TT_ARG_READ tt_obj o) {
    ttwidget a0;
    if ((a0 = ID2(ttwidget, o)))
	return a0->w;
    return 0;
}    
ttshort TTGetH_ttwidget(TT_ARG_READ tt_obj o) {
    ttwidget a0;
    if ((a0 = ID2(ttwidget, o)))
	return a0->h;
    return 0;
}    
tt_obj TTGetLayout_ttwidget(TT_ARG_READ tt_obj o) {
    ttwidget a0;
    if ((a0 = ID2(ttwidget, o)))
	return (tt_obj)OBJ2ID(a0->layout);
    return 0;
}    
ttint TTGetXl_ttwidget(TT_ARG_READ tt_obj o) {
    ttwidget a0;
    if ((a0 = ID2(ttwidget, o)))
	return a0->xl;
    return 0;
}    
ttint TTGetYl_ttwidget(TT_ARG_READ tt_obj o) {
    ttwidget a0;
    if ((a0 = ID2(ttwidget, o)))
	return a0->yl;
    return 0;
}    
ttint TTGetWl_ttwidget(TT_ARG_READ tt_obj o) {
    ttwidget a0;
    if ((a0 = ID2(ttwidget, o)))
	return a0->wl;
    return 0;
}    
ttint TTGetHl_ttwidget(TT_ARG_READ tt_obj o) {
    ttwidget a0;
    if ((a0 = ID2(ttwidget, o)))
	return a0->hl;
    return 0;
}    
ttcol TTGetCol_ttwidget(TT_ARG_READ tt_obj o) {
    ttwidget a0;
    if ((a0 = ID2(ttwidget, o)))
	return a0->col;
    return 0;
}    
tt_obj TTGetTooltip_ttwidget(TT_ARG_READ tt_obj o) {
    ttwidget a0;
    if ((a0 = ID2(ttwidget, o)))
	return (tt_obj)OBJ2ID(a0->tooltip);
    return 0;
} 

/* TTGet*_ttlabel methods */
    
ttopaque TTGetTextLen_ttlabel(TT_ARG_READ tt_obj o) {
    ttlabel a0;
    if ((a0 = ID2(ttlabel, o)))
	return a0->text_len;
    return 0;
}    
TT_ARG_ARRAY((_F(text_len))) ttfont * TTGetText_ttlabel(TT_ARG_READ tt_obj o) {
    ttlabel a0;
    if ((a0 = ID2(ttlabel, o)))
	return a0->text;
    return 0;
} 

/* TTGet*_tttooltip methods */
        
tt_obj TTGetWidget_tttooltip(TT_ARG_READ tt_obj o) {
    tttooltip a0;
    if ((a0 = ID2(tttooltip, o)))
	return (tt_obj)OBJ2ID(a0->widget);
    return 0;
} 

/* TTGet*_ttbuttongroup methods */
    
tt_obj TTGetGroupFirst_ttbuttongroup(TT_ARG_READ tt_obj o) {
    ttbuttongroup a0;
    if ((a0 = ID2(ttbuttongroup, o)))
	return (tt_obj)OBJ2ID(a0->group_first);
    return 0;
}    
tt_obj TTGetGroupLast_ttbuttongroup(TT_ARG_READ tt_obj o) {
    ttbuttongroup a0;
    if ((a0 = ID2(ttbuttongroup, o)))
	return (tt_obj)OBJ2ID(a0->group_last);
    return 0;
}    
tt_obj TTGetChecked_ttbuttongroup(TT_ARG_READ tt_obj o) {
    ttbuttongroup a0;
    if ((a0 = ID2(ttbuttongroup, o)))
	return (tt_obj)OBJ2ID(a0->checked);
    return 0;
} 

/* TTGet*_ttanybutton methods */
    
ttshort TTGetTextWidth_ttanybutton(TT_ARG_READ tt_obj o) {
    ttanybutton a0;
    if ((a0 = ID2(ttanybutton, o)))
	return a0->text_width;
    return 0;
}    
ttshort TTGetTextHeight_ttanybutton(TT_ARG_READ tt_obj o) {
    ttanybutton a0;
    if ((a0 = ID2(ttanybutton, o)))
	return a0->text_height;
    return 0;
}    
TT_ARG_ARRAY((_F(text_width),*,_F(text_height))) ttattr * TTGetText_ttanybutton(TT_ARG_READ tt_obj o) {
    ttanybutton a0;
    if ((a0 = ID2(ttanybutton, o)))
	return a0->text;
    return 0;
}     

/* TTGet*_ttbutton methods */
 

/* TTGet*_ttcheckbutton methods */
 

/* TTGet*_ttradiobutton methods */
    
tt_obj TTGetGroup_ttradiobutton(TT_ARG_READ tt_obj o) {
    ttradiobutton a0;
    if ((a0 = ID2(ttradiobutton, o)))
	return (tt_obj)OBJ2ID(a0->group);
    return 0;
}    
tt_obj TTGetGroupPrev_ttradiobutton(TT_ARG_READ tt_obj o) {
    ttradiobutton a0;
    if ((a0 = ID2(ttradiobutton, o)))
	return (tt_obj)OBJ2ID(a0->group_prev);
    return 0;
}    
tt_obj TTGetGroupNext_ttradiobutton(TT_ARG_READ tt_obj o) {
    ttradiobutton a0;
    if ((a0 = ID2(ttradiobutton, o)))
	return (tt_obj)OBJ2ID(a0->group_next);
    return 0;
} 

/* TTGet*_ttanyscroll methods */
    
ttbyte TTGetOrientation_ttanyscroll(TT_ARG_READ tt_obj o) {
    ttanyscroll a0;
    if ((a0 = ID2(ttanyscroll, o)))
	return a0->orientation;
    return 0;
}    
ttint TTGetSize_ttanyscroll(TT_ARG_READ tt_obj o) {
    ttanyscroll a0;
    if ((a0 = ID2(ttanyscroll, o)))
	return a0->size;
    return 0;
}    
ttint TTGetRealSize_ttanyscroll(TT_ARG_READ tt_obj o) {
    ttanyscroll a0;
    if ((a0 = ID2(ttanyscroll, o)))
	return a0->real_size;
    return 0;
}    
ttint TTGetViewSize_ttanyscroll(TT_ARG_READ tt_obj o) {
    ttanyscroll a0;
    if ((a0 = ID2(ttanyscroll, o)))
	return a0->view_size;
    return 0;
}        
ttint TTGetPosition_ttanyscroll(TT_ARG_READ tt_obj o) {
    ttanyscroll a0;
    if ((a0 = ID2(ttanyscroll, o)))
	return a0->position;
    return 0;
}    
ttint TTGetRealPosition_ttanyscroll(TT_ARG_READ tt_obj o) {
    ttanyscroll a0;
    if ((a0 = ID2(ttanyscroll, o)))
	return a0->real_position;
    return 0;
}    
ttuint TTGetState_ttanyscroll(TT_ARG_READ tt_obj o) {
    ttanyscroll a0;
    if ((a0 = ID2(ttanyscroll, o)))
	return a0->state;
    return 0;
} 

/* TTGet*_ttscrollbar methods */
     

/* TTGet*_ttslider methods */
    
ttint TTGetSlideMin_ttslider(TT_ARG_READ tt_obj o) {
    ttslider a0;
    if ((a0 = ID2(ttslider, o)))
	return a0->slide_min;
    return 0;
}    
ttint TTGetSlideMax_ttslider(TT_ARG_READ tt_obj o) {
    ttslider a0;
    if ((a0 = ID2(ttslider, o)))
	return a0->slide_max;
    return 0;
}    
ttint TTGetSlideValue_ttslider(TT_ARG_READ tt_obj o) {
    ttslider a0;
    if ((a0 = ID2(ttslider, o)))
	return a0->slide_value;
    return 0;
} 

/* TTGet*_ttprogressbar methods */
         

/* TTGet*_ttscrollpane methods */
    
tt_obj TTGetBarX_ttscrollpane(TT_ARG_READ tt_obj o) {
    ttscrollpane a0;
    if ((a0 = ID2(ttscrollpane, o)))
	return (tt_obj)OBJ2ID(a0->bar_x);
    return 0;
}    
tt_obj TTGetBarY_ttscrollpane(TT_ARG_READ tt_obj o) {
    ttscrollpane a0;
    if ((a0 = ID2(ttscrollpane, o)))
	return (tt_obj)OBJ2ID(a0->bar_y);
    return 0;
} 

/* TTGet*_ttwindow methods */
    
ttopaque TTGetTitleLen_ttwindow(TT_ARG_READ tt_obj o) {
    ttwindow a0;
    if ((a0 = ID2(ttwindow, o)))
	return a0->title_len;
    return 0;
}    
TT_ARG_ARRAY((_F(title_len))) ttbyte * TTGetTitle_ttwindow(TT_ARG_READ tt_obj o) {
    ttwindow a0;
    if ((a0 = ID2(ttwindow, o)))
	return a0->title;
    return 0;
} 

/* TTGet*_ttframe methods */
    
tt_obj TTGetMenubar_ttframe(TT_ARG_READ tt_obj o) {
    ttframe a0;
    if ((a0 = ID2(ttframe, o)))
	return (tt_obj)OBJ2ID(a0->menubar);
    return 0;
} 

/* TTGet*_ttscroller methods */
    
tt_obj TTGetScrollpane_ttscroller(TT_ARG_READ tt_obj o) {
    ttscroller a0;
    if ((a0 = ID2(ttscroller, o)))
	return (tt_obj)OBJ2ID(a0->scrollpane);
    return 0;
} 

/* TTGet*_ttmenuitem methods */
    
ttopaque TTGetNameLen_ttmenuitem(TT_ARG_READ tt_obj o) {
    ttmenuitem a0;
    if ((a0 = ID2(ttmenuitem, o)))
	return a0->name_len;
    return 0;
}    
TT_ARG_ARRAY((_F(name_len))) ttbyte * TTGetName_ttmenuitem(TT_ARG_READ tt_obj o) {
    ttmenuitem a0;
    if ((a0 = ID2(ttmenuitem, o)))
	return a0->name;
    return 0;
} 

/* TTGet*_ttcheckmenuitem methods */
 

/* TTGet*_ttradiomenuitem methods */
 

/* TTGet*_ttmenu methods */
    
tt_obj TTGetMenubar_ttmenu(TT_ARG_READ tt_obj o) {
    ttmenu a0;
    if ((a0 = ID2(ttmenu, o)))
	return (tt_obj)OBJ2ID(a0->menubar);
    return 0;
} 

/* TTGet*_ttmenubar methods */
 

/* TTGet*_ttanytext methods */
    
ttopaque TTGetTextLen_ttanytext(TT_ARG_READ tt_obj o) {
    ttanytext a0;
    if ((a0 = ID2(ttanytext, o)))
	return a0->text_len;
    return 0;
}    
TT_ARG_ARRAY((_F(text_len))) ttattr * TTGetText_ttanytext(TT_ARG_READ tt_obj o) {
    ttanytext a0;
    if ((a0 = ID2(ttanytext, o)))
	return a0->text;
    return 0;
} 

/* TTGet*_tttextfield methods */
 

/* TTGet*_tttextarea methods */
 

/* TTGet*_tttheme methods */
    
ttopaque TTGetNameLen_tttheme(TT_ARG_READ tt_obj o) {
    tttheme a0;
    if ((a0 = ID2(tttheme, o)))
	return a0->name_len;
    return 0;
}    
TT_ARG_ARRAY((_F(name_len))) ttbyte * TTGetName_tttheme(TT_ARG_READ tt_obj o) {
    tttheme a0;
    if ((a0 = ID2(tttheme, o)))
	return a0->name;
    return 0;
}                 

/* TTGet*_ttapplication methods */
    
ttopaque TTGetNameLen_ttapplication(TT_ARG_READ tt_obj o) {
    ttapplication a0;
    if ((a0 = ID2(ttapplication, o)))
	return a0->name_len;
    return 0;
}    
TT_ARG_ARRAY((_F(name_len))) ttbyte * TTGetName_ttapplication(TT_ARG_READ tt_obj o) {
    ttapplication a0;
    if ((a0 = ID2(ttapplication, o)))
	return a0->name;
    return 0;
}






/* back-end functions for TTGetField_ttobj() */

 
        
static ttbyte getset_GetField_ttobj(TT_ARG_READ ttobj o, ttopaque which, ttany *value) {
    if (which && which < ttobj_field_last && o && TTAssert(IS(ttobj,o))) {
	if (which >= ttobj_field_first) {
	    switch (which) {
        
	      default:
		return TT_FALSE;
	    }
	    return TT_TRUE;
	}
	
    }
    return TT_FALSE;
} 
             
                            
static ttbyte getset_GetField_ttobject(TT_ARG_READ ttobject o, ttopaque which, ttany *value) {
    if (which && which < ttobject_field_last && o && TTAssert(IS(ttobject,o))) {
	if (which >= ttobject_field_first) {
	    switch (which) {
    
	      case ttobject_refcount:
		*value = (ttany)o->refcount;
		break;    
	      case ttobject_oflags:
		*value = (ttany)o->oflags;
		break;            
	      case ttobject_user_data:
		*value = (ttany)o->user_data;
		break;    
	      case ttobject_events_inprogress:
		*value = (ttany)o->events_inprogress;
		break;    
	      case ttobject_events_informed:
		*value = (ttany)o->events_informed;
		break;
	      default:
		return TT_FALSE;
	    }
	    return TT_TRUE;
	}
	else
	    return getset_GetField_ttobj((TT_ARG_READ ttobj)o, which, value);
    }
    return TT_FALSE;
} 
            
static ttbyte getset_GetField_ttvector(TT_ARG_READ ttvector o, ttopaque which, ttany *value) {
    if (which && which < ttvector_field_last && o && TTAssert(IS(ttvector,o))) {
	if (which >= ttvector_field_first) {
	    switch (which) {
    
	      case ttvector_array_size:
		*value = (ttany)o->array_size;
		break;    
	      case ttvector_array_n:
		*value = (ttany)o->array_n;
		break;    
	      case ttvector_array:
		*value = (ttany)(opaque)o->array;
		break;
	      default:
		return TT_FALSE;
	    }
	    return TT_TRUE;
	}
	else
	    return getset_GetField_ttobject((TT_ARG_READ ttobject)o, which, value);
    }
    return TT_FALSE;
} 
             
                    
static ttbyte getset_GetField_ttmethod(TT_ARG_READ ttmethod o, ttopaque which, ttany *value) {
    if (which && which < ttmethod_field_last && o && TTAssert(IS(ttmethod,o))) {
	if (which >= ttmethod_field_first) {
	    switch (which) {
    
	      case ttmethod_name_len:
		*value = (ttany)o->name_len;
		break;    
	      case ttmethod_name:
		*value = (ttany)(opaque)o->name;
		break;    
	      case ttmethod_address:
		*value = (ttany)(opaque)o->address;
		break;    
	      case ttmethod_return_type:
		*value = (ttany)o->return_type;
		break;    
	      case ttmethod_parameter_type:
		*value = (ttany)OBJ2ID(o->parameter_type);
		break;
	      default:
		return TT_FALSE;
	    }
	    return TT_TRUE;
	}
	else
	    return getset_GetField_ttobj((TT_ARG_READ ttobj)o, which, value);
    }
    return TT_FALSE;
} 
                        
static ttbyte getset_GetField_ttevent(TT_ARG_READ ttevent o, ttopaque which, ttany *value) {
    if (which && which < ttevent_field_last && o && TTAssert(IS(ttevent,o))) {
	if (which >= ttevent_field_first) {
	    switch (which) {
    
	      case ttevent_component:
		*value = (ttany)OBJ2ID(o->component);
		break;    
	      case ttevent_evtype:
		*value = (ttany)o->evtype;
		break;    
	      case ttevent_evcode:
		*value = (ttany)o->evcode;
		break;    
	      case ttevent_evflags:
		*value = (ttany)o->evflags;
		break;    
	      case ttevent_prev:
		*value = (ttany)OBJ2ID(o->prev);
		break;    
	      case ttevent_next:
		*value = (ttany)OBJ2ID(o->next);
		break;
	      default:
		return TT_FALSE;
	    }
	    return TT_TRUE;
	}
	else
	    return getset_GetField_ttobject((TT_ARG_READ ttobject)o, which, value);
    }
    return TT_FALSE;
} 
                                 
                 
                
static ttbyte getset_GetField_tteventmask(TT_ARG_READ tteventmask o, ttopaque which, ttany *value) {
    if (which && which < tteventmask_field_last && o && TTAssert(IS(tteventmask,o))) {
	if (which >= tteventmask_field_first) {
	    switch (which) {
    
	      case tteventmask_evtype_mask:
		*value = (ttany)OBJ2ID(o->evtype_mask);
		break;    
	      case tteventmask_evcode_mask:
		*value = (ttany)OBJ2ID(o->evcode_mask);
		break;    
	      case tteventmask_component_mask:
		*value = (ttany)OBJ2ID(o->component_mask);
		break;    
	      case tteventmask_truth_table:
		*value = (ttany)o->truth_table;
		break;
	      default:
		return TT_FALSE;
	    }
	    return TT_TRUE;
	}
	else
	    return getset_GetField_ttobject((TT_ARG_READ ttobject)o, which, value);
    }
    return TT_FALSE;
} 
                
static ttbyte getset_GetField_ttcallback(TT_ARG_READ ttcallback o, ttopaque which, ttany *value) {
    if (which && which < ttcallback_field_last && o && TTAssert(IS(ttcallback,o))) {
	if (which >= ttcallback_field_first) {
	    switch (which) {
    
	      case ttcallback_lflags:
		*value = (ttany)o->lflags;
		break;    
	      case ttcallback_component:
		*value = (ttany)OBJ2ID(o->component);
		break;    
	      case ttcallback_prev:
		*value = (ttany)OBJ2ID(o->prev);
		break;    
	      case ttcallback_next:
		*value = (ttany)OBJ2ID(o->next);
		break;
	      default:
		return TT_FALSE;
	    }
	    return TT_TRUE;
	}
	else
	    return getset_GetField_ttobject((TT_ARG_READ ttobject)o, which, value);
    }
    return TT_FALSE;
} 
                                    
static ttbyte getset_GetField_ttlistener(TT_ARG_READ ttlistener o, ttopaque which, ttany *value) {
    if (which && which < ttlistener_field_last && o && TTAssert(IS(ttlistener,o))) {
	if (which >= ttlistener_field_first) {
	    switch (which) {
        
	      case ttlistener_event:
		*value = (ttany)OBJ2ID(o->event);
		break;    
	      case ttlistener_function:
		*value = (ttany)(opaque)o->function;
		break;    
	      case ttlistener_arg_component_n:
		*value = (ttany)o->arg_component_n;
		break;    
	      case ttlistener_arg_event_n:
		*value = (ttany)o->arg_event_n;
		break;    
	      case ttlistener_args:
		*value = (ttany)OBJ2ID(o->args);
		break;    
	      case ttlistener_event_mask:
		*value = (ttany)OBJ2ID(o->event_mask);
		break;        
	      default:
		return TT_FALSE;
	    }
	    return TT_TRUE;
	}
	else
	    return getset_GetField_ttcallback((TT_ARG_READ ttcallback)o, which, value);
    }
    return TT_FALSE;
} 
                
static ttbyte getset_GetField_tttimer(TT_ARG_READ tttimer o, ttopaque which, ttany *value) {
    if (which && which < tttimer_field_last && o && TTAssert(IS(tttimer,o))) {
	if (which >= tttimer_field_first) {
	    switch (which) {
    
	      case tttimer_delay_t:
		*value = (ttany)o->delay_t;
		break;    
	      case tttimer_delay_f:
		*value = (ttany)o->delay_f;
		break;    
	      case tttimer_timer_prev:
		*value = (ttany)OBJ2ID(o->timer_prev);
		break;    
	      case tttimer_timer_next:
		*value = (ttany)OBJ2ID(o->timer_next);
		break;
	      default:
		return TT_FALSE;
	    }
	    return TT_TRUE;
	}
	else
	    return getset_GetField_ttlistener((TT_ARG_READ ttlistener)o, which, value);
    }
    return TT_FALSE;
} 
                
static ttbyte getset_GetField_ttcomponent(TT_ARG_READ ttcomponent o, ttopaque which, ttany *value) {
    if (which && which < ttcomponent_field_last && o && TTAssert(IS(ttcomponent,o))) {
	if (which >= ttcomponent_field_first) {
	    switch (which) {
    
	      case ttcomponent_callbacks:
		*value = (ttany)OBJ2ID(o->callbacks);
		break;    
	      case ttcomponent_listeners:
		*value = (ttany)OBJ2ID(o->listeners);
		break;    
	      case ttcomponent_timers:
		*value = (ttany)OBJ2ID(o->timers);
		break;    
	      case ttcomponent_datas:
		*value = (ttany)OBJ2ID(o->datas);
		break;
	      default:
		return TT_FALSE;
	    }
	    return TT_TRUE;
	}
	else
	    return getset_GetField_ttobject((TT_ARG_READ ttobject)o, which, value);
    }
    return TT_FALSE;
} 
                     
                                    
static ttbyte getset_GetField_ttvisible(TT_ARG_READ ttvisible o, ttopaque which, ttany *value) {
    if (which && which < ttvisible_field_last && o && TTAssert(IS(ttvisible,o))) {
	if (which >= ttvisible_field_first) {
	    switch (which) {
    
	      case ttvisible_vflags:
		*value = (ttany)o->vflags;
		break;    
	      case ttvisible_prev:
		*value = (ttany)OBJ2ID(o->prev);
		break;    
	      case ttvisible_next:
		*value = (ttany)OBJ2ID(o->next);
		break;    
	      case ttvisible_parent:
		*value = (ttany)OBJ2ID(o->parent);
		break;    
	      case ttvisible_child_first:
		*value = (ttany)OBJ2ID(o->child_first);
		break;    
	      case ttvisible_child_last:
		*value = (ttany)OBJ2ID(o->child_last);
		break;    
	      case ttvisible_constraint:
		*value = (ttany)o->constraint;
		break;    
	      case ttvisible_theme:
		*value = (ttany)OBJ2ID(o->theme);
		break;    
	      case ttvisible_repaint:
		*value = (ttany)(opaque)o->repaint;
		break;
	      default:
		return TT_FALSE;
	    }
	    return TT_TRUE;
	}
	else
	    return getset_GetField_ttcomponent((TT_ARG_READ ttcomponent)o, which, value);
    }
    return TT_FALSE;
} 
     
    
static ttbyte getset_GetField_ttboxlayout(TT_ARG_READ ttboxlayout o, ttopaque which, ttany *value) {
    if (which && which < ttboxlayout_field_last && o && TTAssert(IS(ttboxlayout,o))) {
	if (which >= ttboxlayout_field_first) {
	    switch (which) {
    
	      case ttboxlayout_orientation:
		*value = (ttany)o->orientation;
		break;
	      default:
		return TT_FALSE;
	    }
	    return TT_TRUE;
	}
	else
	    return getset_GetField_ttcomponent((TT_ARG_READ ttcomponent)o, which, value);
    }
    return TT_FALSE;
} 
 
 
                                             
         
         
            
static ttbyte getset_GetField_ttbuttongroup(TT_ARG_READ ttbuttongroup o, ttopaque which, ttany *value) {
    if (which && which < ttbuttongroup_field_last && o && TTAssert(IS(ttbuttongroup,o))) {
	if (which >= ttbuttongroup_field_first) {
	    switch (which) {
    
	      case ttbuttongroup_group_first:
		*value = (ttany)OBJ2ID(o->group_first);
		break;    
	      case ttbuttongroup_group_last:
		*value = (ttany)OBJ2ID(o->group_last);
		break;    
	      case ttbuttongroup_checked:
		*value = (ttany)OBJ2ID(o->checked);
		break;
	      default:
		return TT_FALSE;
	    }
	    return TT_TRUE;
	}
	else
	    return getset_GetField_ttcomponent((TT_ARG_READ ttcomponent)o, which, value);
    }
    return TT_FALSE;
} 
                
static ttbyte getset_GetField_ttanybutton(TT_ARG_READ ttanybutton o, ttopaque which, ttany *value) {
    if (which && which < ttanybutton_field_last && o && TTAssert(IS(ttanybutton,o))) {
	if (which >= ttanybutton_field_first) {
	    switch (which) {
    
	      case ttanybutton_text_width:
		*value = (ttany)o->text_width;
		break;    
	      case ttanybutton_text_height:
		*value = (ttany)o->text_height;
		break;    
	      case ttanybutton_text:
		*value = (ttany)(opaque)o->text;
		break;    
	      default:
		return TT_FALSE;
	    }
	    return TT_TRUE;
	}
	else
	    return getset_GetField_ttvisible((TT_ARG_READ ttvisible)o, which, value);
    }
    return TT_FALSE;
} 
 
 
            
static ttbyte getset_GetField_ttradiobutton(TT_ARG_READ ttradiobutton o, ttopaque which, ttany *value) {
    if (which && which < ttradiobutton_field_last && o && TTAssert(IS(ttradiobutton,o))) {
	if (which >= ttradiobutton_field_first) {
	    switch (which) {
    
	      case ttradiobutton_group:
		*value = (ttany)OBJ2ID(o->group);
		break;    
	      case ttradiobutton_group_prev:
		*value = (ttany)OBJ2ID(o->group_prev);
		break;    
	      case ttradiobutton_group_next:
		*value = (ttany)OBJ2ID(o->group_next);
		break;
	      default:
		return TT_FALSE;
	    }
	    return TT_TRUE;
	}
	else
	    return getset_GetField_ttanybutton((TT_ARG_READ ttanybutton)o, which, value);
    }
    return TT_FALSE;
} 
                                
static ttbyte getset_GetField_ttanyscroll(TT_ARG_READ ttanyscroll o, ttopaque which, ttany *value) {
    if (which && which < ttanyscroll_field_last && o && TTAssert(IS(ttanyscroll,o))) {
	if (which >= ttanyscroll_field_first) {
	    switch (which) {
    
	      case ttanyscroll_orientation:
		*value = (ttany)o->orientation;
		break;    
	      case ttanyscroll_size:
		*value = (ttany)o->size;
		break;    
	      case ttanyscroll_real_size:
		*value = (ttany)o->real_size;
		break;    
	      case ttanyscroll_view_size:
		*value = (ttany)o->view_size;
		break;        
	      case ttanyscroll_position:
		*value = (ttany)o->position;
		break;    
	      case ttanyscroll_real_position:
		*value = (ttany)o->real_position;
		break;    
	      case ttanyscroll_state:
		*value = (ttany)o->state;
		break;
	      default:
		return TT_FALSE;
	    }
	    return TT_TRUE;
	}
	else
	    return getset_GetField_ttanybutton((TT_ARG_READ ttanybutton)o, which, value);
    }
    return TT_FALSE;
} 
    
static ttbyte getset_GetField_ttscrollbar(TT_ARG_READ ttscrollbar o, ttopaque which, ttany *value) {
    if (which && which < ttscrollbar_field_last && o && TTAssert(IS(ttscrollbar,o))) {
	if (which >= ttscrollbar_field_first) {
	    switch (which) {
    
	      default:
		return TT_FALSE;
	    }
	    return TT_TRUE;
	}
	else
	    return getset_GetField_ttanyscroll((TT_ARG_READ ttanyscroll)o, which, value);
    }
    return TT_FALSE;
} 
             
        
static ttbyte getset_GetField_ttprogressbar(TT_ARG_READ ttprogressbar o, ttopaque which, ttany *value) {
    if (which && which < ttprogressbar_field_last && o && TTAssert(IS(ttprogressbar,o))) {
	if (which >= ttprogressbar_field_first) {
	    switch (which) {
        
	      default:
		return TT_FALSE;
	    }
	    return TT_TRUE;
	}
	else
	    return getset_GetField_ttanyscroll((TT_ARG_READ ttanyscroll)o, which, value);
    }
    return TT_FALSE;
} 
        
static ttbyte getset_GetField_ttscrollpane(TT_ARG_READ ttscrollpane o, ttopaque which, ttany *value) {
    if (which && which < ttscrollpane_field_last && o && TTAssert(IS(ttscrollpane,o))) {
	if (which >= ttscrollpane_field_first) {
	    switch (which) {
    
	      case ttscrollpane_bar_x:
		*value = (ttany)OBJ2ID(o->bar_x);
		break;    
	      case ttscrollpane_bar_y:
		*value = (ttany)OBJ2ID(o->bar_y);
		break;
	      default:
		return TT_FALSE;
	    }
	    return TT_TRUE;
	}
	else
	    return getset_GetField_ttvisible((TT_ARG_READ ttvisible)o, which, value);
    }
    return TT_FALSE;
} 
         
    
static ttbyte getset_GetField_ttframe(TT_ARG_READ ttframe o, ttopaque which, ttany *value) {
    if (which && which < ttframe_field_last && o && TTAssert(IS(ttframe,o))) {
	if (which >= ttframe_field_first) {
	    switch (which) {
    
	      case ttframe_menubar:
		*value = (ttany)OBJ2ID(o->menubar);
		break;
	      default:
		return TT_FALSE;
	    }
	    return TT_TRUE;
	}
	else
	    return getset_GetField_ttvisible((TT_ARG_READ ttvisible)o, which, value);
    }
    return TT_FALSE;
} 
    
static ttbyte getset_GetField_ttscroller(TT_ARG_READ ttscroller o, ttopaque which, ttany *value) {
    if (which && which < ttscroller_field_last && o && TTAssert(IS(ttscroller,o))) {
	if (which >= ttscroller_field_first) {
	    switch (which) {
    
	      case ttscroller_scrollpane:
		*value = (ttany)OBJ2ID(o->scrollpane);
		break;
	      default:
		return TT_FALSE;
	    }
	    return TT_TRUE;
	}
	else
	    return getset_GetField_ttvisible((TT_ARG_READ ttvisible)o, which, value);
    }
    return TT_FALSE;
} 
         
 
 
    
static ttbyte getset_GetField_ttmenu(TT_ARG_READ ttmenu o, ttopaque which, ttany *value) {
    if (which && which < ttmenu_field_last && o && TTAssert(IS(ttmenu,o))) {
	if (which >= ttmenu_field_first) {
	    switch (which) {
    
	      case ttmenu_menubar:
		*value = (ttany)OBJ2ID(o->menubar);
		break;
	      default:
		return TT_FALSE;
	    }
	    return TT_TRUE;
	}
	else
	    return getset_GetField_ttvisible((TT_ARG_READ ttvisible)o, which, value);
    }
    return TT_FALSE;
} 
 
         
 
 
                         
        


typedef ttbyte (*get_fn)(TT_ARG_READ ttobj, ttuint, ttany *);

static ttbyte getset_GetField(TT_ARG_READ ttobj o, tt_obj which, ttany *value) {
    static get_fn fn [] = {
	(get_fn)0,

 
	(get_fn)getset_GetField_ttobj, 
	(get_fn)getset_GetField_ttobj, 
	(get_fn)getset_GetField_ttobject, 
	(get_fn)getset_GetField_ttvector, 
	(get_fn)getset_GetField_ttobj, 
	(get_fn)getset_GetField_ttmethod, 
	(get_fn)getset_GetField_ttevent, 
	(get_fn)getset_GetField_ttevent, 
	(get_fn)getset_GetField_ttobject, 
	(get_fn)getset_GetField_tteventmask, 
	(get_fn)getset_GetField_ttcallback, 
	(get_fn)getset_GetField_ttlistener, 
	(get_fn)getset_GetField_tttimer, 
	(get_fn)getset_GetField_ttcomponent, 
	(get_fn)getset_GetField_ttcomponent, 
	(get_fn)getset_GetField_ttvisible, 
	(get_fn)getset_GetField_ttcomponent, 
	(get_fn)getset_GetField_ttboxlayout, 
	(get_fn)getset_GetField_ttcomponent, 
	(get_fn)getset_GetField_ttvisible, 
	(get_fn)getset_GetField_ttvisible, 
	(get_fn)getset_GetField_ttvisible, 
	(get_fn)getset_GetField_ttvisible, 
	(get_fn)getset_GetField_ttbuttongroup, 
	(get_fn)getset_GetField_ttanybutton, 
	(get_fn)getset_GetField_ttanybutton, 
	(get_fn)getset_GetField_ttanybutton, 
	(get_fn)getset_GetField_ttradiobutton, 
	(get_fn)getset_GetField_ttanyscroll, 
	(get_fn)getset_GetField_ttscrollbar, 
	(get_fn)getset_GetField_ttanyscroll, 
	(get_fn)getset_GetField_ttprogressbar, 
	(get_fn)getset_GetField_ttscrollpane, 
	(get_fn)getset_GetField_ttvisible, 
	(get_fn)getset_GetField_ttframe, 
	(get_fn)getset_GetField_ttscroller, 
	(get_fn)getset_GetField_ttvisible, 
	(get_fn)getset_GetField_ttvisible, 
	(get_fn)getset_GetField_ttvisible, 
	(get_fn)getset_GetField_ttmenu, 
	(get_fn)getset_GetField_ttvisible, 
	(get_fn)getset_GetField_ttvisible, 
	(get_fn)getset_GetField_ttvisible, 
	(get_fn)getset_GetField_ttvisible, 
	(get_fn)getset_GetField_ttcomponent, 
	(get_fn)getset_GetField_ttcomponent,

    };
    ttuint i;
    ttany dummy;
    
    if (!value)
	value = &dummy;

    if (o && (i = o->id >> TT_MAGIC_SHIFT) && i < sizeof(fn)/sizeof(fn[0]) && ID2(ttfield,which))
	return fn[i](o, (ttuint)((ttopaque)which & TT_MAGIC_MASK), value);

    return TT_FALSE;
}

/* back-end functions for TTSetField_ttobj() */

 
        
static ttbyte getset_SetField_ttobj(ttobj o, ttopaque which, ttany value) {
    if (which && which < ttobj_field_last && o && TTAssert(IS(ttobj,o))) {
	if (which >= ttobj_field_first) switch (which) {
        
	  default:
	    return TT_FALSE;
	}
	
    }
    return TT_FALSE;
} 
             
                             
             
             
                     
                         
                                
static ttbyte getset_SetField_tteventbig(tteventbig o, ttopaque which, ttany value) {
    if (which && which < tteventbig_field_last && o && TTAssert(IS(tteventbig,o))) {
	if (which >= tteventbig_field_first) switch (which) {
                                
	  default:
	    return TT_FALSE;
	}
	else
	    return getset_SetField_ttobj((ttobj)o, which, value);
    }
    return TT_FALSE;
} 
                 
                 
                 
                                     
                 
                 
                     
                                     
    
static ttbyte getset_SetField_ttlayout(ttlayout o, ttopaque which, ttany value) {
    if (which && which < ttlayout_field_last && o && TTAssert(IS(ttlayout,o))) {
	if (which >= ttlayout_field_first) switch (which) {
    
	  default:
	    return TT_FALSE;
	}
	else
	    return getset_SetField_ttobj((ttobj)o, which, value);
    }
    return TT_FALSE;
} 
     
 
 
                                            
static ttbyte getset_SetField_ttwidget(ttwidget o, ttopaque which, ttany value) {
    if (which && which < ttwidget_field_last && o && TTAssert(IS(ttwidget,o))) {
	if (which >= ttwidget_field_first) switch (which) {
    
	  case ttwidget_x:
	    return TTSetX_ttwidget((tt_obj)o->id, (ttshort)value);    
	  case ttwidget_y:
	    return TTSetY_ttwidget((tt_obj)o->id, (ttshort)value);    
	  case ttwidget_w:
	    return TTSetW_ttwidget((tt_obj)o->id, (ttshort)value);    
	  case ttwidget_h:
	    return TTSetH_ttwidget((tt_obj)o->id, (ttshort)value);        
	  case ttwidget_xl:
	    return TTSetXl_ttwidget((tt_obj)o->id, (ttint)value);    
	  case ttwidget_yl:
	    return TTSetYl_ttwidget((tt_obj)o->id, (ttint)value);    
	  case ttwidget_wl:
	    return TTSetWl_ttwidget((tt_obj)o->id, (ttint)value);    
	  case ttwidget_hl:
	    return TTSetHl_ttwidget((tt_obj)o->id, (ttint)value);        
	  case ttwidget_tooltip:
	    return TTSetTooltip_ttwidget((tt_obj)o->id, (tt_obj)(opaque)value);
	  default:
	    return TT_FALSE;
	}
	else
	    return getset_SetField_ttobj((ttobj)o, which, value);
    }
    return TT_FALSE;
} 
         
        
static ttbyte getset_SetField_tttooltip(tttooltip o, ttopaque which, ttany value) {
    if (which && which < tttooltip_field_last && o && TTAssert(IS(tttooltip,o))) {
	if (which >= tttooltip_field_first) switch (which) {
        
	  default:
	    return TT_FALSE;
	}
	else
	    return getset_SetField_ttwidget((ttwidget)o, which, value);
    }
    return TT_FALSE;
} 
             
                
static ttbyte getset_SetField_ttanybutton(ttanybutton o, ttopaque which, ttany value) {
    if (which && which < ttanybutton_field_last && o && TTAssert(IS(ttanybutton,o))) {
	if (which >= ttanybutton_field_first) switch (which) {
                
	  default:
	    return TT_FALSE;
	}
	else
	    return getset_SetField_ttwidget((ttwidget)o, which, value);
    }
    return TT_FALSE;
} 
 
 
             
                                
static ttbyte getset_SetField_ttanyscroll(ttanyscroll o, ttopaque which, ttany value) {
    if (which && which < ttanyscroll_field_last && o && TTAssert(IS(ttanyscroll,o))) {
	if (which >= ttanyscroll_field_first) switch (which) {
    
	  case ttanyscroll_orientation:
	    return TTSetOrientation_ttanyscroll((tt_obj)o->id, (ttbyte)value);    
	  case ttanyscroll_size:
	    return TTSetSize_ttanyscroll((tt_obj)o->id, (ttint)value);    
	  case ttanyscroll_real_size:
	    return TTSetRealSize_ttanyscroll((tt_obj)o->id, (ttint)value);    
	  case ttanyscroll_view_size:
	    return TTSetViewSize_ttanyscroll((tt_obj)o->id, (ttint)value);        
	  case ttanyscroll_position:
	    return TTSetPosition_ttanyscroll((tt_obj)o->id, (ttint)value);        
	  default:
	    return TT_FALSE;
	}
	else
	    return getset_SetField_ttanybutton((ttanybutton)o, which, value);
    }
    return TT_FALSE;
} 
     
             
        
static ttbyte getset_SetField_ttprogressbar(ttprogressbar o, ttopaque which, ttany value) {
    if (which && which < ttprogressbar_field_last && o && TTAssert(IS(ttprogressbar,o))) {
	if (which >= ttprogressbar_field_first) switch (which) {
        
	  default:
	    return TT_FALSE;
	}
	else
	    return getset_SetField_ttanyscroll((ttanyscroll)o, which, value);
    }
    return TT_FALSE;
} 
         
         
     
     
         
 
 
     
 
         
 
 
                         
        


typedef ttbyte (*set_fn)(TT_ARG_READ ttobj, ttuint, ttany);

static ttbyte getset_SetField(TT_ARG_READ ttobj o, tt_obj which, TT_ARG_READ TT_ARG_ARRAY((1)) ttarg *value) {
    static set_fn fn [] = {
	(set_fn)0,

 
	(set_fn)getset_SetField_ttobj, 
	(set_fn)getset_SetField_ttobj, 
	(set_fn)getset_SetField_ttobj, 
	(set_fn)getset_SetField_ttobj, 
	(set_fn)getset_SetField_ttobj, 
	(set_fn)getset_SetField_ttobj, 
	(set_fn)getset_SetField_ttobj, 
	(set_fn)getset_SetField_tteventbig, 
	(set_fn)getset_SetField_ttobj, 
	(set_fn)getset_SetField_ttobj, 
	(set_fn)getset_SetField_ttobj, 
	(set_fn)getset_SetField_ttobj, 
	(set_fn)getset_SetField_ttobj, 
	(set_fn)getset_SetField_ttobj, 
	(set_fn)getset_SetField_ttobj, 
	(set_fn)getset_SetField_ttobj, 
	(set_fn)getset_SetField_ttlayout, 
	(set_fn)getset_SetField_ttlayout, 
	(set_fn)getset_SetField_ttlayout, 
	(set_fn)getset_SetField_ttobj, 
	(set_fn)getset_SetField_ttwidget, 
	(set_fn)getset_SetField_ttwidget, 
	(set_fn)getset_SetField_tttooltip, 
	(set_fn)getset_SetField_ttobj, 
	(set_fn)getset_SetField_ttanybutton, 
	(set_fn)getset_SetField_ttanybutton, 
	(set_fn)getset_SetField_ttanybutton, 
	(set_fn)getset_SetField_ttanybutton, 
	(set_fn)getset_SetField_ttanyscroll, 
	(set_fn)getset_SetField_ttanyscroll, 
	(set_fn)getset_SetField_ttanyscroll, 
	(set_fn)getset_SetField_ttprogressbar, 
	(set_fn)getset_SetField_ttwidget, 
	(set_fn)getset_SetField_ttwidget, 
	(set_fn)getset_SetField_ttwidget, 
	(set_fn)getset_SetField_ttwidget, 
	(set_fn)getset_SetField_ttobj, 
	(set_fn)getset_SetField_ttobj, 
	(set_fn)getset_SetField_ttobj, 
	(set_fn)getset_SetField_ttobj, 
	(set_fn)getset_SetField_ttobj, 
	(set_fn)getset_SetField_ttwidget, 
	(set_fn)getset_SetField_ttwidget, 
	(set_fn)getset_SetField_ttwidget, 
	(set_fn)getset_SetField_ttobj, 
	(set_fn)getset_SetField_ttobj,

    };
    ttuint i;
    
    if (o && (i = o->id >> TT_MAGIC_SHIFT) && i < sizeof(fn)/sizeof(fn[0]) &&
	IS(ttobject,o) && !(((ttobject)o)->oflags & ttobject_oflags_const) && ID2(ttfield,which)) {

	return fn[i](o, (ttuint)((ttopaque)which & TT_MAGIC_MASK), value->value);
    }
    return TT_FALSE;
}



