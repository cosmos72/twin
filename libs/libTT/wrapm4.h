



/* This file was automatically generated from m4/wrap.m4, do not edit! */


/*
 *  wrapm4.h  --  wrappers around exported libTT methods
 *
 */





















/* public and exported methods */



/* ttobj methods */
                                
ttany TTGetValue_ttobj(tt_obj a1, ttuint a2) {
    ttobj o;
    ttany a0 = (ttany)0;
    LOCK;
    if ((o = ID2(ttobj,a1)))
	a0 = o->FN->GetValue(o, a2);
    UNLK;
    return a0;
}    
void TTSetValue_ttobj(tt_obj a1, ttuint a2, ttany a3) {
    ttobj o;
    LOCK;
    if ((o = ID2(ttobj,a1)))
	 o->FN->SetValue(o, a2, a3);
    UNLK;
    return ;
}    
void TTChangeValue_ttobj(tt_obj a1, ttuint a2, ttany a3, ttany a4) {
    ttobj o;
    LOCK;
    if ((o = ID2(ttobj,a1)))
	 o->FN->ChangeValue(o, a2, a3, a4);
    UNLK;
    return ;
}    
void TTRef_ttobj(tt_obj a1) {
    ttobj o;
    LOCK;
    if ((o = ID2(ttobj,a1)))
	 Ref_ttobj(o);
    UNLK;
    return ;
}    
void TTUnref_ttobj(tt_obj a1) {
    ttobj o;
    LOCK;
    if ((o = ID2(ttobj,a1)))
	 Unref_ttobj(o);
    UNLK;
    return ;
}
  
/* ttevent methods */
    
tt_obj TTCreate_ttevent(ttuint a1, ttuint a2, ttuint a3) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttevent(a1, a2, a3));
    UNLK;
    return a0;
}
 
/* tteventbig methods */

    
tt_obj TTCreate_tteventbig(ttuint a1, ttuint a2, ttuint a3, ttshort a4, ttshort a5, ttshort a6, ttshort a7, ttuint a8, TT_CONST ttbyte * a9) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_tteventbig(a1, a2, a3, a4, a5, a6, a7, a8, a9));
    UNLK;
    return a0;
}
 
/* ttlistener methods */

    
 
/* ttcallback methods */
    
tt_obj TTCreate_ttcallback(tt_obj a1, ttuint a2, ttuint a3, ttcallback_fn a4, ttany a5) {
    ttcomponent o;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((o = ID2(ttcomponent,a1)))
	a0 = (tt_obj)OBJ2ID(Create_ttcallback(o, a2, a3, a4, a5));
    UNLK;
    return a0;
}    
tt_obj TTCreateA_ttcallback(tt_obj a1, tt_obj a2, ttuint a3, ttuint a4, ttuint a5, ttuint a6, ttcallback_fn a7, ttany TT_CONST * a8) {
    ttcomponent o;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((o = ID2(ttcomponent,a1)))
	a0 = (tt_obj)OBJ2ID(CreateA_ttcallback(o, ID2(ttevent,a2), a3, a4, a5, a6, a7, a8));
    UNLK;
    return a0;
}    
tt_obj TTCreateV_ttcallback(tt_obj a1, tt_obj a2, ttuint a3, ttuint a4, ttuint a5, ttuint a6, ttcallback_fn a7, va_list a8) {
    ttcomponent o;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((o = ID2(ttcomponent,a1)))
	a0 = (tt_obj)OBJ2ID(CreateV_ttcallback(o, ID2(ttevent,a2), a3, a4, a5, a6, a7, a8));
    UNLK;
    return a0;
}
 
/* ttcomponent methods */

 
/* ttvisible methods */
    
void TTAddTo_ttvisible(tt_obj a1, tt_obj a2) {
    ttvisible o;
    LOCK;
    if ((o = ID2(ttvisible,a1)))
	 o->FN->AddTo(o, ID2(ttvisible,a2));
    UNLK;
    return ;
}    
void TTRemove_ttvisible(tt_obj a1) {
    ttvisible o;
    LOCK;
    if ((o = ID2(ttvisible,a1)))
	 o->FN->Remove(o);
    UNLK;
    return ;
}    
void TTSetVisible_ttvisible(tt_obj a1, ttbyte a2) {
    ttvisible o;
    LOCK;
    if ((o = ID2(ttvisible,a1)))
	 o->FN->SetVisible(o, a2);
    UNLK;
    return ;
}    
void TTSetTheme_ttvisible(tt_obj a1, tt_obj a2) {
    ttvisible o;
    LOCK;
    if ((o = ID2(ttvisible,a1)))
	 o->FN->SetTheme(o, ID2(tttheme,a2));
    UNLK;
    return ;
}    
void TTBuiltinRepaint_ttvisible(tt_obj a1, ttshort a2, ttshort a3, ttshort a4, ttshort a5) {
    ttvisible o;
    LOCK;
    if ((o = ID2(ttvisible,a1)))
	 o->FN->BuiltinRepaint(o, a2, a3, a4, a5);
    UNLK;
    return ;
}    
void TTSetRepaint_ttvisible(tt_obj a1, ttvisible_repaint_fn a2) {
    ttvisible o;
    LOCK;
    if ((o = ID2(ttvisible,a1)))
	 SetRepaint_ttvisible(o, a2);
    UNLK;
    return ;
}    
void TTAdd_ttvisible(tt_obj a1, tt_obj a2) {
    ttvisible o;
    LOCK;
    if ((o = ID2(ttvisible,a1)))
	 Add_ttvisible(o, ID2(ttvisible,a2));
    UNLK;
    return ;
}    
void TTExpose_ttvisible(tt_obj a1, ttshort a2, ttshort a3, ttshort a4, ttshort a5) {
    ttvisible o;
    LOCK;
    if ((o = ID2(ttvisible,a1)))
	 Expose_ttvisible(o, a2, a3, a4, a5);
    UNLK;
    return ;
}
 
/* ttnative methods */
    
tt_obj TTGetRoot_ttnative(void) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(TFN_ttnative->GetRoot());
    UNLK;
    return a0;
}    
tt_obj TTCreate_ttnative(ttany a1) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttnative(a1));
    UNLK;
    return a0;
}
 
/* ttwidget methods */
    
void TTSetXYWH_ttwidget(tt_obj a1, ttbyte a2, ttshort a3, ttshort a4, ttshort a5, ttshort a6) {
    ttwidget o;
    LOCK;
    if ((o = ID2(ttwidget,a1)))
	 o->FN->SetXYWH(o, a2, a3, a4, a5, a6);
    UNLK;
    return ;
}    
void TTSetXY_ttwidget(tt_obj a1, ttshort a2, ttshort a3) {
    ttwidget o;
    LOCK;
    if ((o = ID2(ttwidget,a1)))
	 SetXY_ttwidget(o, a2, a3);
    UNLK;
    return ;
}    
void TTSetWH_ttwidget(tt_obj a1, ttshort a2, ttshort a3) {
    ttwidget o;
    LOCK;
    if ((o = ID2(ttwidget,a1)))
	 SetWH_ttwidget(o, a2, a3);
    UNLK;
    return ;
}
 
/* ttlabel methods */
    
ttbyte TTSetText_ttlabel(tt_obj a1, TT_CONST ttbyte * a2) {
    ttlabel o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttlabel,a1)))
	a0 = o->FN->SetText(o, a2);
    UNLK;
    return a0;
}    
tt_obj TTCreate_ttlabel(TT_CONST ttbyte * a1) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttlabel(a1));
    UNLK;
    return a0;
}
 
/* ttanybutton methods */
    
ttbyte TTSetText_ttanybutton(tt_obj a1, TT_CONST ttfont * a2, ttshort a3, ttshort a4, ttshort a5) {
    ttanybutton o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttanybutton,a1)))
	a0 = o->FN->SetText(o, a2, a3, a4, a5);
    UNLK;
    return a0;
}    
tt_obj TTCreate_ttanybutton(TT_CONST ttfont * a1, ttshort a2, ttshort a3, ttshort a4) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttanybutton(a1, a2, a3, a4));
    UNLK;
    return a0;
}
 
/* ttbutton methods */
    
void TTSetPressed_ttbutton(tt_obj a1, ttbyte a2) {
    ttbutton o;
    LOCK;
    if ((o = ID2(ttbutton,a1)))
	 o->FN->SetPressed(o, a2);
    UNLK;
    return ;
}    
tt_obj TTCreate_ttbutton(TT_CONST ttfont * a1, ttshort a2, ttshort a3, ttshort a4) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttbutton(a1, a2, a3, a4));
    UNLK;
    return a0;
}    
ttbyte TTIsPressed_ttbutton(tt_obj a1) {
    ttbutton o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttbutton,a1)))
	a0 = IsPressed_ttbutton(o);
    UNLK;
    return a0;
}
 
/* ttcheckbutton methods */

 
/* ttradiobutton methods */

 
/* ttscrollbar methods */

 
/* ttbuttongroup methods */

 
/* ttwindow methods */

 
/* ttframe methods */

 
/* ttscroller methods */

 
/* ttmenuitem methods */

 
/* ttcheckmenuitem methods */

 
/* ttradiomenuitem methods */

 
/* ttmenu methods */

 
/* ttmenubar methods */
    
tt_obj TTGetDefault_ttmenubar(void) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(GetDefault_ttmenubar());
    UNLK;
    return a0;
}
 
/* ttanytext methods */

 
/* tttextfield methods */

 
/* tttextarea methods */

 
/* tttheme methods */
        
tt_obj TTGetDefault_tttheme(void) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(GetDefault_tttheme());
    UNLK;
    return a0;
}
 
/* ttapplication methods */
    
tt_obj TTSet_ttapplication(TT_CONST ttbyte * a1) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Set_ttapplication(a1));
    UNLK;
    return a0;
}







/* handy public and exported methods */

























/* handy ttobj methods */
        
ttopaque TTGetId_ttobj(tt_obj o) {
    return (ttopaque)TTGetValue_ttobj(o, ttobj_id);
}    
ttuint TTGetRefcount_ttobj(tt_obj o) {
    return (ttuint)TTGetValue_ttobj(o, ttobj_refcount);
}    
ttuint TTGetOflags_ttobj(tt_obj o) {
    return (ttuint)TTGetValue_ttobj(o, ttobj_oflags);
}            
ttany TTGetUserData_ttobj(tt_obj o) {
    return (ttany)TTGetValue_ttobj(o, ttobj_user_data);
}
void TTSetUserData_ttobj(tt_obj o, ttany user_data) {
    TTSetValue_ttobj(o, ttobj_user_data, (ttany)user_data);
}    
ttuint TTGetEventsInprogress_ttobj(tt_obj o) {
    return (ttuint)TTGetValue_ttobj(o, ttobj_events_inprogress);
}  

/* handy ttevent methods */
        
tt_obj TTGetCallback_ttevent(tt_obj o) {
    return (tt_obj)(opaque)TTGetValue_ttobj(o, ttevent_callback);
}    
tt_obj TTGetComponent_ttevent(tt_obj o) {
    return (tt_obj)(opaque)TTGetValue_ttobj(o, ttevent_component);
}    
ttuint TTGetEvtype_ttevent(tt_obj o) {
    return (ttuint)TTGetValue_ttobj(o, ttevent_evtype);
}    
ttuint TTGetEvcode_ttevent(tt_obj o) {
    return (ttuint)TTGetValue_ttobj(o, ttevent_evcode);
}    
ttuint TTGetEvflags_ttevent(tt_obj o) {
    return (ttuint)TTGetValue_ttobj(o, ttevent_evflags);
}    
void TTConsume_ttevent(tt_obj o) {
    TTChangeValue_ttobj(o, ttevent_evflags, ttevent_evflags_consumed, ttevent_evflags_consumed);
} 

/* handy tteventbig methods */
        
ttshort TTGetX_tteventbig(tt_obj o) {
    return (ttshort)TTGetValue_ttobj(o, tteventbig_x);
}    
ttshort TTGetY_tteventbig(tt_obj o) {
    return (ttshort)TTGetValue_ttobj(o, tteventbig_y);
}    
ttshort TTGetW_tteventbig(tt_obj o) {
    return (ttshort)TTGetValue_ttobj(o, tteventbig_w);
}    
ttshort TTGetH_tteventbig(tt_obj o) {
    return (ttshort)TTGetValue_ttobj(o, tteventbig_h);
}    
ttuint TTGetLen_tteventbig(tt_obj o) {
    return (ttuint)TTGetValue_ttobj(o, tteventbig_len);
}    
TT_CONST ttbyte * TTGetData_tteventbig(tt_obj o) {
    return (TT_CONST ttbyte *)(opaque)TTGetValue_ttobj(o, tteventbig_data);
} 

/* handy ttlistener methods */
        
ttuint TTGetLflags_ttlistener(tt_obj o) {
    return (ttuint)TTGetValue_ttobj(o, ttlistener_lflags);
}    
tt_obj TTGetComponent_ttlistener(tt_obj o) {
    return (tt_obj)(opaque)TTGetValue_ttobj(o, ttlistener_component);
}    
tt_obj TTGetPrev_ttlistener(tt_obj o) {
    return (tt_obj)(opaque)TTGetValue_ttobj(o, ttlistener_prev);
}    
tt_obj TTGetNext_ttlistener(tt_obj o) {
    return (tt_obj)(opaque)TTGetValue_ttobj(o, ttlistener_next);
} 

/* handy ttcallback methods */
    
tt_obj TTGetAvlLeft_ttcallback(tt_obj o) {
    return (tt_obj)(opaque)TTGetValue_ttobj(o, ttcallback_avl_left);
}    
tt_obj TTGetAvlRight_ttcallback(tt_obj o) {
    return (tt_obj)(opaque)TTGetValue_ttobj(o, ttcallback_avl_right);
}    
tt_obj TTGetAvlParent_ttcallback(tt_obj o) {
    return (tt_obj)(opaque)TTGetValue_ttobj(o, ttcallback_avl_parent);
}            
tt_obj TTGetEvent_ttcallback(tt_obj o) {
    return (tt_obj)(opaque)TTGetValue_ttobj(o, ttcallback_event);
}    
ttuint TTGetNargComponent_ttcallback(tt_obj o) {
    return (ttuint)TTGetValue_ttobj(o, ttcallback_narg_component);
}    
ttuint TTGetNargEvent_ttcallback(tt_obj o) {
    return (ttuint)TTGetValue_ttobj(o, ttcallback_narg_event);
}    
ttuint TTGetNargs_ttcallback(tt_obj o) {
    return (ttuint)TTGetValue_ttobj(o, ttcallback_nargs);
}    
ttcallback_fn TTGetFunction_ttcallback(tt_obj o) {
    return (ttcallback_fn)(opaque)TTGetValue_ttobj(o, ttcallback_function);
}        
TT_CONST ttany * TTGetArgs_ttcallback(tt_obj o) {
    return (TT_CONST ttany *)(opaque)TTGetValue_ttobj(o, ttcallback_args);
} 

/* handy ttcomponent methods */
        
tt_obj TTGetListeners_ttcomponent(tt_obj o) {
    return (tt_obj)(opaque)TTGetValue_ttobj(o, ttcomponent_listeners);
}    
tt_obj TTGetCallbacks_ttcomponent(tt_obj o) {
    return (tt_obj)(opaque)TTGetValue_ttobj(o, ttcomponent_callbacks);
} 

/* handy ttvisible methods */
        
ttuint TTGetVflags_ttvisible(tt_obj o) {
    return (ttuint)TTGetValue_ttobj(o, ttvisible_vflags);
}    
tt_obj TTGetPrev_ttvisible(tt_obj o) {
    return (tt_obj)(opaque)TTGetValue_ttobj(o, ttvisible_prev);
}    
tt_obj TTGetNext_ttvisible(tt_obj o) {
    return (tt_obj)(opaque)TTGetValue_ttobj(o, ttvisible_next);
}    
tt_obj TTGetParent_ttvisible(tt_obj o) {
    return (tt_obj)(opaque)TTGetValue_ttobj(o, ttvisible_parent);
}    
tt_obj TTGetChildFirst_ttvisible(tt_obj o) {
    return (tt_obj)(opaque)TTGetValue_ttobj(o, ttvisible_child_first);
}    
tt_obj TTGetChildLast_ttvisible(tt_obj o) {
    return (tt_obj)(opaque)TTGetValue_ttobj(o, ttvisible_child_last);
}    
tt_obj TTGetTheme_ttvisible(tt_obj o) {
    return (tt_obj)(opaque)TTGetValue_ttobj(o, ttvisible_theme);
}    
ttvisible_repaint_fn TTGetRepaint_ttvisible(tt_obj o) {
    return (ttvisible_repaint_fn)(opaque)TTGetValue_ttobj(o, ttvisible_repaint);
} 

/* handy ttnative methods */
     

/* handy ttwidget methods */
        
ttshort TTGetX_ttwidget(tt_obj o) {
    return (ttshort)TTGetValue_ttobj(o, ttwidget_x);
}    
ttshort TTGetY_ttwidget(tt_obj o) {
    return (ttshort)TTGetValue_ttobj(o, ttwidget_y);
}    
ttshort TTGetW_ttwidget(tt_obj o) {
    return (ttshort)TTGetValue_ttobj(o, ttwidget_w);
}    
ttshort TTGetH_ttwidget(tt_obj o) {
    return (ttshort)TTGetValue_ttobj(o, ttwidget_h);
} 

/* handy ttlabel methods */
        
ttshort TTGetTextLen_ttlabel(tt_obj o) {
    return (ttshort)TTGetValue_ttobj(o, ttlabel_text_len);
}        
TT_CONST ttfont * TTGetText_ttlabel(tt_obj o) {
    return (TT_CONST ttfont *)(opaque)TTGetValue_ttobj(o, ttlabel_text);
} 

/* handy ttanybutton methods */
            
ttshort TTGetTextWidth_ttanybutton(tt_obj o) {
    return (ttshort)TTGetValue_ttobj(o, ttanybutton_text_width);
}    
ttshort TTGetTextHeight_ttanybutton(tt_obj o) {
    return (ttshort)TTGetValue_ttobj(o, ttanybutton_text_height);
}        
TT_CONST ttfont * TTGetText_ttanybutton(tt_obj o) {
    return (TT_CONST ttfont *)(opaque)TTGetValue_ttobj(o, ttanybutton_text);
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
    return (tt_obj)(opaque)TTGetValue_ttobj(o, ttscroller_scrollx);
}    
tt_obj TTGetScrolly_ttscroller(tt_obj o) {
    return (tt_obj)(opaque)TTGetValue_ttobj(o, ttscroller_scrolly);
} 

/* handy ttmenuitem methods */
         

/* handy ttcheckmenuitem methods */
     

/* handy ttradiomenuitem methods */
     

/* handy ttmenu methods */
    
    
tt_obj TTGetMenubar_ttmenu(tt_obj o) {
    return (tt_obj)(opaque)TTGetValue_ttobj(o, ttmenu_menubar);
} 

/* handy ttmenubar methods */
     

/* handy ttanytext methods */
        
ttshort TTGetTextLen_ttanytext(tt_obj o) {
    return (ttshort)TTGetValue_ttobj(o, ttanytext_text_len);
}        
TT_CONST ttfont * TTGetText_ttanytext(tt_obj o) {
    return (TT_CONST ttfont *)(opaque)TTGetValue_ttobj(o, ttanytext_text);
} 

/* handy tttextfield methods */
     

/* handy tttextarea methods */
     

/* handy tttheme methods */
    
            
        
TT_CONST ttbyte * TTGetName_tttheme(tt_obj o) {
    return (TT_CONST ttbyte *)(opaque)TTGetValue_ttobj(o, tttheme_name);
} 

/* handy ttapplication methods */
        








/* enum for method callbacks */

typedef enum e_order_methods {
    /* generic functions */
    order_BAD = -1,
    order_ExitMainLoop,
    order_New,
    order_Del,



    /* ttobj methods */                                
    order_GetValue_ttobj,    
    order_SetValue_ttobj,    
    order_ChangeValue_ttobj,    
    order_Ref_ttobj,    
    order_Unref_ttobj,  
    /* ttevent methods */    
    order_Create_ttevent, 
    /* tteventbig methods */
    
    order_Create_tteventbig, 
    /* ttlistener methods */
     
    /* ttcallback methods */    
    order_Create_ttcallback,    
    order_CreateA_ttcallback,    
    order_CreateV_ttcallback, 
    /* ttcomponent methods */ 
    /* ttvisible methods */    
    order_AddTo_ttvisible,    
    order_Remove_ttvisible,    
    order_SetVisible_ttvisible,    
    order_SetTheme_ttvisible,    
    order_BuiltinRepaint_ttvisible,    
    order_SetRepaint_ttvisible,    
    order_Add_ttvisible,    
    order_Expose_ttvisible, 
    /* ttnative methods */    
    order_GetRoot_ttnative,    
    order_Create_ttnative, 
    /* ttwidget methods */    
    order_SetXYWH_ttwidget,    
    order_SetXY_ttwidget,    
    order_SetWH_ttwidget, 
    /* ttlabel methods */    
    order_SetText_ttlabel,    
    order_Create_ttlabel, 
    /* ttanybutton methods */    
    order_SetText_ttanybutton,    
    order_Create_ttanybutton, 
    /* ttbutton methods */    
    order_SetPressed_ttbutton,    
    order_Create_ttbutton,    
    order_IsPressed_ttbutton, 
    /* ttcheckbutton methods */ 
    /* ttradiobutton methods */ 
    /* ttscrollbar methods */ 
    /* ttbuttongroup methods */ 
    /* ttwindow methods */ 
    /* ttframe methods */ 
    /* ttscroller methods */ 
    /* ttmenuitem methods */ 
    /* ttcheckmenuitem methods */ 
    /* ttradiomenuitem methods */ 
    /* ttmenu methods */ 
    /* ttmenubar methods */    
    order_GetDefault_ttmenubar, 
    /* ttanytext methods */ 
    /* tttextfield methods */ 
    /* tttextarea methods */ 
    /* tttheme methods */        
    order_GetDefault_tttheme, 
    /* ttapplication methods */    
    order_Set_ttapplication,




    order_FN_handy,







    /* handy ttobj methods */        
    order_GetId_ttobj,    
    order_GetRefcount_ttobj,    
    order_GetOflags_ttobj,            
    order_GetUserData_ttobj,
    order_SetUserData_ttobj,    
    order_GetEventsInprogress_ttobj,  
    /* handy ttevent methods */        
    order_GetCallback_ttevent,    
    order_GetComponent_ttevent,    
    order_GetEvtype_ttevent,    
    order_GetEvcode_ttevent,    
    order_GetEvflags_ttevent,    
    order_Consume_ttevent, 
    /* handy tteventbig methods */        
    order_GetX_tteventbig,    
    order_GetY_tteventbig,    
    order_GetW_tteventbig,    
    order_GetH_tteventbig,    
    order_GetLen_tteventbig,    
    order_GetData_tteventbig, 
    /* handy ttlistener methods */        
    order_GetLflags_ttlistener,    
    order_GetComponent_ttlistener,    
    order_GetPrev_ttlistener,    
    order_GetNext_ttlistener, 
    /* handy ttcallback methods */    
    order_GetAvlLeft_ttcallback,    
    order_GetAvlRight_ttcallback,    
    order_GetAvlParent_ttcallback,            
    order_GetEvent_ttcallback,    
    order_GetNargComponent_ttcallback,    
    order_GetNargEvent_ttcallback,    
    order_GetNargs_ttcallback,    
    order_GetFunction_ttcallback,        
    order_GetArgs_ttcallback, 
    /* handy ttcomponent methods */        
    order_GetListeners_ttcomponent,    
    order_GetCallbacks_ttcomponent, 
    /* handy ttvisible methods */        
    order_GetVflags_ttvisible,    
    order_GetPrev_ttvisible,    
    order_GetNext_ttvisible,    
    order_GetParent_ttvisible,    
    order_GetChildFirst_ttvisible,    
    order_GetChildLast_ttvisible,    
    order_GetTheme_ttvisible,    
    order_GetRepaint_ttvisible, 
    /* handy ttnative methods */     
    /* handy ttwidget methods */        
    order_GetX_ttwidget,    
    order_GetY_ttwidget,    
    order_GetW_ttwidget,    
    order_GetH_ttwidget, 
    /* handy ttlabel methods */        
    order_GetTextLen_ttlabel,        
    order_GetText_ttlabel, 
    /* handy ttanybutton methods */            
    order_GetTextWidth_ttanybutton,    
    order_GetTextHeight_ttanybutton,        
    order_GetText_ttanybutton, 
    /* handy ttbutton methods */     
    /* handy ttcheckbutton methods */     
    /* handy ttradiobutton methods */     
    /* handy ttscrollbar methods */     
    /* handy ttbuttongroup methods */     
    /* handy ttwindow methods */         
    /* handy ttframe methods */     
    /* handy ttscroller methods */        
    order_GetScrollx_ttscroller,    
    order_GetScrolly_ttscroller, 
    /* handy ttmenuitem methods */         
    /* handy ttcheckmenuitem methods */     
    /* handy ttradiomenuitem methods */     
    /* handy ttmenu methods */    
    
    order_GetMenubar_ttmenu, 
    /* handy ttmenubar methods */     
    /* handy ttanytext methods */        
    order_GetTextLen_ttanytext,        
    order_GetText_ttanytext, 
    /* handy tttextfield methods */     
    /* handy tttextarea methods */     
    /* handy tttheme methods */    
            
        
    order_GetName_tttheme, 
    /* handy ttapplication methods */        






    order_FN_n
} e_order_methods;


typedef struct s_ttmethod s_ttmethod;
typedef struct s_ttmethod *ttmethod;
struct s_ttmethod {
    opaque mtho;
    void *mth;
};

/* array for method callbacks */

static struct s_ttmethod methods_array[] = {
    /* generic functions */
    { order_ExitMainLoop, (void *)TTExitMainLoop, },
    { order_New, (void *)TTNew, },
    { order_Del, (void *)TTDel, }, 



    /* ttobj methods */                                
    { order_GetValue_ttobj, (void *)TTGetValue_ttobj, },    
    { order_SetValue_ttobj, (void *)TTSetValue_ttobj, },    
    { order_ChangeValue_ttobj, (void *)TTChangeValue_ttobj, },    
    { order_Ref_ttobj, (void *)TTRef_ttobj, },    
    { order_Unref_ttobj, (void *)TTUnref_ttobj, },  
    /* ttevent methods */    
    { order_Create_ttevent, (void *)TTCreate_ttevent, }, 
    /* tteventbig methods */
    
    { order_Create_tteventbig, (void *)TTCreate_tteventbig, }, 
    /* ttlistener methods */
     
    /* ttcallback methods */    
    { order_Create_ttcallback, (void *)TTCreate_ttcallback, },    
    { order_CreateA_ttcallback, (void *)TTCreateA_ttcallback, },    
    { order_CreateV_ttcallback, (void *)TTCreateV_ttcallback, }, 
    /* ttcomponent methods */ 
    /* ttvisible methods */    
    { order_AddTo_ttvisible, (void *)TTAddTo_ttvisible, },    
    { order_Remove_ttvisible, (void *)TTRemove_ttvisible, },    
    { order_SetVisible_ttvisible, (void *)TTSetVisible_ttvisible, },    
    { order_SetTheme_ttvisible, (void *)TTSetTheme_ttvisible, },    
    { order_BuiltinRepaint_ttvisible, (void *)TTBuiltinRepaint_ttvisible, },    
    { order_SetRepaint_ttvisible, (void *)TTSetRepaint_ttvisible, },    
    { order_Add_ttvisible, (void *)TTAdd_ttvisible, },    
    { order_Expose_ttvisible, (void *)TTExpose_ttvisible, }, 
    /* ttnative methods */    
    { order_GetRoot_ttnative, (void *)TTGetRoot_ttnative, },    
    { order_Create_ttnative, (void *)TTCreate_ttnative, }, 
    /* ttwidget methods */    
    { order_SetXYWH_ttwidget, (void *)TTSetXYWH_ttwidget, },    
    { order_SetXY_ttwidget, (void *)TTSetXY_ttwidget, },    
    { order_SetWH_ttwidget, (void *)TTSetWH_ttwidget, }, 
    /* ttlabel methods */    
    { order_SetText_ttlabel, (void *)TTSetText_ttlabel, },    
    { order_Create_ttlabel, (void *)TTCreate_ttlabel, }, 
    /* ttanybutton methods */    
    { order_SetText_ttanybutton, (void *)TTSetText_ttanybutton, },    
    { order_Create_ttanybutton, (void *)TTCreate_ttanybutton, }, 
    /* ttbutton methods */    
    { order_SetPressed_ttbutton, (void *)TTSetPressed_ttbutton, },    
    { order_Create_ttbutton, (void *)TTCreate_ttbutton, },    
    { order_IsPressed_ttbutton, (void *)TTIsPressed_ttbutton, }, 
    /* ttcheckbutton methods */ 
    /* ttradiobutton methods */ 
    /* ttscrollbar methods */ 
    /* ttbuttongroup methods */ 
    /* ttwindow methods */ 
    /* ttframe methods */ 
    /* ttscroller methods */ 
    /* ttmenuitem methods */ 
    /* ttcheckmenuitem methods */ 
    /* ttradiomenuitem methods */ 
    /* ttmenu methods */ 
    /* ttmenubar methods */    
    { order_GetDefault_ttmenubar, (void *)TTGetDefault_ttmenubar, }, 
    /* ttanytext methods */ 
    /* tttextfield methods */ 
    /* tttextarea methods */ 
    /* tttheme methods */        
    { order_GetDefault_tttheme, (void *)TTGetDefault_tttheme, }, 
    /* ttapplication methods */    
    { order_Set_ttapplication, (void *)TTSet_ttapplication, },










    /* handy ttobj methods */        
    { order_GetId_ttobj, (void *)TTGetId_ttobj, },    
    { order_GetRefcount_ttobj, (void *)TTGetRefcount_ttobj, },    
    { order_GetOflags_ttobj, (void *)TTGetOflags_ttobj, },            
    { order_GetUserData_ttobj, (void *)TTGetUserData_ttobj, },
    { order_SetUserData_ttobj, (void *)TTSetUserData_ttobj, },    
    { order_GetEventsInprogress_ttobj, (void *)TTGetEventsInprogress_ttobj, },  
    /* handy ttevent methods */        
    { order_GetCallback_ttevent, (void *)TTGetCallback_ttevent, },    
    { order_GetComponent_ttevent, (void *)TTGetComponent_ttevent, },    
    { order_GetEvtype_ttevent, (void *)TTGetEvtype_ttevent, },    
    { order_GetEvcode_ttevent, (void *)TTGetEvcode_ttevent, },    
    { order_GetEvflags_ttevent, (void *)TTGetEvflags_ttevent, },    
    { order_Consume_ttevent, (void *)TTConsume_ttevent, }, 
    /* handy tteventbig methods */        
    { order_GetX_tteventbig, (void *)TTGetX_tteventbig, },    
    { order_GetY_tteventbig, (void *)TTGetY_tteventbig, },    
    { order_GetW_tteventbig, (void *)TTGetW_tteventbig, },    
    { order_GetH_tteventbig, (void *)TTGetH_tteventbig, },    
    { order_GetLen_tteventbig, (void *)TTGetLen_tteventbig, },    
    { order_GetData_tteventbig, (void *)TTGetData_tteventbig, }, 
    /* handy ttlistener methods */        
    { order_GetLflags_ttlistener, (void *)TTGetLflags_ttlistener, },    
    { order_GetComponent_ttlistener, (void *)TTGetComponent_ttlistener, },    
    { order_GetPrev_ttlistener, (void *)TTGetPrev_ttlistener, },    
    { order_GetNext_ttlistener, (void *)TTGetNext_ttlistener, }, 
    /* handy ttcallback methods */    
    { order_GetAvlLeft_ttcallback, (void *)TTGetAvlLeft_ttcallback, },    
    { order_GetAvlRight_ttcallback, (void *)TTGetAvlRight_ttcallback, },    
    { order_GetAvlParent_ttcallback, (void *)TTGetAvlParent_ttcallback, },            
    { order_GetEvent_ttcallback, (void *)TTGetEvent_ttcallback, },    
    { order_GetNargComponent_ttcallback, (void *)TTGetNargComponent_ttcallback, },    
    { order_GetNargEvent_ttcallback, (void *)TTGetNargEvent_ttcallback, },    
    { order_GetNargs_ttcallback, (void *)TTGetNargs_ttcallback, },    
    { order_GetFunction_ttcallback, (void *)TTGetFunction_ttcallback, },        
    { order_GetArgs_ttcallback, (void *)TTGetArgs_ttcallback, }, 
    /* handy ttcomponent methods */        
    { order_GetListeners_ttcomponent, (void *)TTGetListeners_ttcomponent, },    
    { order_GetCallbacks_ttcomponent, (void *)TTGetCallbacks_ttcomponent, }, 
    /* handy ttvisible methods */        
    { order_GetVflags_ttvisible, (void *)TTGetVflags_ttvisible, },    
    { order_GetPrev_ttvisible, (void *)TTGetPrev_ttvisible, },    
    { order_GetNext_ttvisible, (void *)TTGetNext_ttvisible, },    
    { order_GetParent_ttvisible, (void *)TTGetParent_ttvisible, },    
    { order_GetChildFirst_ttvisible, (void *)TTGetChildFirst_ttvisible, },    
    { order_GetChildLast_ttvisible, (void *)TTGetChildLast_ttvisible, },    
    { order_GetTheme_ttvisible, (void *)TTGetTheme_ttvisible, },    
    { order_GetRepaint_ttvisible, (void *)TTGetRepaint_ttvisible, }, 
    /* handy ttnative methods */     
    /* handy ttwidget methods */        
    { order_GetX_ttwidget, (void *)TTGetX_ttwidget, },    
    { order_GetY_ttwidget, (void *)TTGetY_ttwidget, },    
    { order_GetW_ttwidget, (void *)TTGetW_ttwidget, },    
    { order_GetH_ttwidget, (void *)TTGetH_ttwidget, }, 
    /* handy ttlabel methods */        
    { order_GetTextLen_ttlabel, (void *)TTGetTextLen_ttlabel, },        
    { order_GetText_ttlabel, (void *)TTGetText_ttlabel, }, 
    /* handy ttanybutton methods */            
    { order_GetTextWidth_ttanybutton, (void *)TTGetTextWidth_ttanybutton, },    
    { order_GetTextHeight_ttanybutton, (void *)TTGetTextHeight_ttanybutton, },        
    { order_GetText_ttanybutton, (void *)TTGetText_ttanybutton, }, 
    /* handy ttbutton methods */     
    /* handy ttcheckbutton methods */     
    /* handy ttradiobutton methods */     
    /* handy ttscrollbar methods */     
    /* handy ttbuttongroup methods */     
    /* handy ttwindow methods */         
    /* handy ttframe methods */     
    /* handy ttscroller methods */        
    { order_GetScrollx_ttscroller, (void *)TTGetScrollx_ttscroller, },    
    { order_GetScrolly_ttscroller, (void *)TTGetScrolly_ttscroller, }, 
    /* handy ttmenuitem methods */         
    /* handy ttcheckmenuitem methods */     
    /* handy ttradiomenuitem methods */     
    /* handy ttmenu methods */    
    
    { order_GetMenubar_ttmenu, (void *)TTGetMenubar_ttmenu, }, 
    /* handy ttmenubar methods */     
    /* handy ttanytext methods */        
    { order_GetTextLen_ttanytext, (void *)TTGetTextLen_ttanytext, },        
    { order_GetText_ttanytext, (void *)TTGetText_ttanytext, }, 
    /* handy tttextfield methods */     
    /* handy tttextarea methods */     
    /* handy tttheme methods */    
            
        
    { order_GetName_tttheme, (void *)TTGetName_tttheme, }, 
    /* handy ttapplication methods */        






};

static ttbyte methods_needsort = TRUE;

static int CompareMethods(TT_CONST ttmethod m1, TT_CONST ttmethod m2) {
    return (int)((byte *)m1->mth - (byte *)m2->mth);
}

/*
 * use binary search for efficiency
 */
static opaque Method2Order(void *method) {
    s_ttmethod key, *m;
    
    if (methods_needsort) {
	methods_needsort = FALSE;
	qsort(methods_array, order_FN_n, sizeof(struct s_ttmethod),
	      (int (*)(TT_CONST void *, TT_CONST void *))CompareMethods);
    }
    
    key.mth = method;
    
    if ((m = bsearch(&key, methods_array, order_FN_n, sizeof(struct s_ttmethod),
	             (int (*)(TT_CONST void *, TT_CONST void *))CompareMethods))) {
	return m->mtho;
    }
    return (opaque)order_BAD;
}







/*
 * here and below, we do consider TTGet*() methods specially:
 * they have no side effects, and if called from within a callback they
 * cannot return a useful return value, so we can do without calling them.
 */
static void CallMethod(ttuint order, ttuint nargs, TT_CONST ttany *a) {

    switch (order) {
    
    /* generic functions */
    
      case order_New:
	if (nargs >= 1)
	    TTNew((tt_fn)(opaque)a[0]);
	break;
      case order_Del:
	if (nargs >= 1)
	    TTDel((tt_obj)(opaque)a[0]);
	break;
      case order_ExitMainLoop:
	if (nargs >= 0)
	    TTExitMainLoop();
	break;





    /* ttobj methods */
                                    
      case order_GetValue_ttobj:
	if (nargs >= 2)
	    TTGetValue_ttobj((tt_obj)(opaque)a[0], (ttuint)a[1]);
	break;    
      case order_SetValue_ttobj:
	if (nargs >= 3)
	    TTSetValue_ttobj((tt_obj)(opaque)a[0], (ttuint)a[1], (ttany)a[2]);
	break;    
      case order_ChangeValue_ttobj:
	if (nargs >= 4)
	    TTChangeValue_ttobj((tt_obj)(opaque)a[0], (ttuint)a[1], (ttany)a[2], (ttany)a[3]);
	break;    
      case order_Ref_ttobj:
	if (nargs >= 1)
	    TTRef_ttobj((tt_obj)(opaque)a[0]);
	break;    
      case order_Unref_ttobj:
	if (nargs >= 1)
	    TTUnref_ttobj((tt_obj)(opaque)a[0]);
	break;
  
    /* ttevent methods */
        
      case order_Create_ttevent:
	if (nargs >= 3)
	    TTCreate_ttevent((ttuint)a[0], (ttuint)a[1], (ttuint)a[2]);
	break;
 
    /* tteventbig methods */
    
    
      case order_Create_tteventbig:
	if (nargs >= 9)
	    TTCreate_tteventbig((ttuint)a[0], (ttuint)a[1], (ttuint)a[2], (ttshort)a[3], (ttshort)a[4], (ttshort)a[5], (ttshort)a[6], (ttuint)a[7], (TT_CONST ttbyte *)(opaque)a[8]);
	break;
 
    /* ttlistener methods */
    
    
 
    /* ttcallback methods */
        
      case order_Create_ttcallback:
	if (nargs >= 5)
	    TTCreate_ttcallback((tt_obj)(opaque)a[0], (ttuint)a[1], (ttuint)a[2], (ttcallback_fn)(opaque)a[3], (ttany)a[4]);
	break;    
      case order_CreateA_ttcallback:
	if (nargs >= 8)
	    TTCreateA_ttcallback((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1], (ttuint)a[2], (ttuint)a[3], (ttuint)a[4], (ttuint)a[5], (ttcallback_fn)(opaque)a[6], (ttany TT_CONST *)(opaque)a[7]);
	break;    
      case order_CreateV_ttcallback:
	if (nargs >= 8)
	    TTCreateV_ttcallback((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1], (ttuint)a[2], (ttuint)a[3], (ttuint)a[4], (ttuint)a[5], (ttcallback_fn)(opaque)a[6], (va_list)a[7]);
	break;
 
    /* ttcomponent methods */
    
 
    /* ttvisible methods */
        
      case order_AddTo_ttvisible:
	if (nargs >= 2)
	    TTAddTo_ttvisible((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;    
      case order_Remove_ttvisible:
	if (nargs >= 1)
	    TTRemove_ttvisible((tt_obj)(opaque)a[0]);
	break;    
      case order_SetVisible_ttvisible:
	if (nargs >= 2)
	    TTSetVisible_ttvisible((tt_obj)(opaque)a[0], (ttbyte)a[1]);
	break;    
      case order_SetTheme_ttvisible:
	if (nargs >= 2)
	    TTSetTheme_ttvisible((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;    
      case order_BuiltinRepaint_ttvisible:
	if (nargs >= 5)
	    TTBuiltinRepaint_ttvisible((tt_obj)(opaque)a[0], (ttshort)a[1], (ttshort)a[2], (ttshort)a[3], (ttshort)a[4]);
	break;    
      case order_SetRepaint_ttvisible:
	if (nargs >= 2)
	    TTSetRepaint_ttvisible((tt_obj)(opaque)a[0], (ttvisible_repaint_fn)(opaque)a[1]);
	break;    
      case order_Add_ttvisible:
	if (nargs >= 2)
	    TTAdd_ttvisible((tt_obj)(opaque)a[0], (tt_obj)(opaque)a[1]);
	break;    
      case order_Expose_ttvisible:
	if (nargs >= 5)
	    TTExpose_ttvisible((tt_obj)(opaque)a[0], (ttshort)a[1], (ttshort)a[2], (ttshort)a[3], (ttshort)a[4]);
	break;
 
    /* ttnative methods */
        
      case order_GetRoot_ttnative:
	if (nargs >= 0)
	    TTGetRoot_ttnative();
	break;    
      case order_Create_ttnative:
	if (nargs >= 1)
	    TTCreate_ttnative((ttany)a[0]);
	break;
 
    /* ttwidget methods */
        
      case order_SetXYWH_ttwidget:
	if (nargs >= 6)
	    TTSetXYWH_ttwidget((tt_obj)(opaque)a[0], (ttbyte)a[1], (ttshort)a[2], (ttshort)a[3], (ttshort)a[4], (ttshort)a[5]);
	break;    
      case order_SetXY_ttwidget:
	if (nargs >= 3)
	    TTSetXY_ttwidget((tt_obj)(opaque)a[0], (ttshort)a[1], (ttshort)a[2]);
	break;    
      case order_SetWH_ttwidget:
	if (nargs >= 3)
	    TTSetWH_ttwidget((tt_obj)(opaque)a[0], (ttshort)a[1], (ttshort)a[2]);
	break;
 
    /* ttlabel methods */
        
      case order_SetText_ttlabel:
	if (nargs >= 2)
	    TTSetText_ttlabel((tt_obj)(opaque)a[0], (TT_CONST ttbyte *)(opaque)a[1]);
	break;    
      case order_Create_ttlabel:
	if (nargs >= 1)
	    TTCreate_ttlabel((TT_CONST ttbyte *)(opaque)a[0]);
	break;
 
    /* ttanybutton methods */
        
      case order_SetText_ttanybutton:
	if (nargs >= 5)
	    TTSetText_ttanybutton((tt_obj)(opaque)a[0], (TT_CONST ttfont *)(opaque)a[1], (ttshort)a[2], (ttshort)a[3], (ttshort)a[4]);
	break;    
      case order_Create_ttanybutton:
	if (nargs >= 4)
	    TTCreate_ttanybutton((TT_CONST ttfont *)(opaque)a[0], (ttshort)a[1], (ttshort)a[2], (ttshort)a[3]);
	break;
 
    /* ttbutton methods */
        
      case order_SetPressed_ttbutton:
	if (nargs >= 2)
	    TTSetPressed_ttbutton((tt_obj)(opaque)a[0], (ttbyte)a[1]);
	break;    
      case order_Create_ttbutton:
	if (nargs >= 4)
	    TTCreate_ttbutton((TT_CONST ttfont *)(opaque)a[0], (ttshort)a[1], (ttshort)a[2], (ttshort)a[3]);
	break;    
      case order_IsPressed_ttbutton:
	if (nargs >= 1)
	    TTIsPressed_ttbutton((tt_obj)(opaque)a[0]);
	break;
 
    /* ttcheckbutton methods */
    
 
    /* ttradiobutton methods */
    
 
    /* ttscrollbar methods */
    
 
    /* ttbuttongroup methods */
    
 
    /* ttwindow methods */
    
 
    /* ttframe methods */
    
 
    /* ttscroller methods */
    
 
    /* ttmenuitem methods */
    
 
    /* ttcheckmenuitem methods */
    
 
    /* ttradiomenuitem methods */
    
 
    /* ttmenu methods */
    
 
    /* ttmenubar methods */
        
      case order_GetDefault_ttmenubar:
	if (nargs >= 0)
	    TTGetDefault_ttmenubar();
	break;
 
    /* ttanytext methods */
    
 
    /* tttextfield methods */
    
 
    /* tttextarea methods */
    
 
    /* tttheme methods */
            
      case order_GetDefault_tttheme:
	if (nargs >= 0)
	    TTGetDefault_tttheme();
	break;
 
    /* ttapplication methods */
        
      case order_Set_ttapplication:
	if (nargs >= 1)
	    TTSet_ttapplication((TT_CONST ttbyte *)(opaque)a[0]);
	break;











    /* handy ttobj methods */                            
      case order_SetUserData_ttobj:
	if (nargs >= 2)
	    TTSetUserData_ttobj((tt_obj)(opaque)a[0], (ttany)a[1]);
	break;      
    /* handy ttevent methods */                            
      case order_Consume_ttevent:
	if (nargs >= 1)
	    TTConsume_ttevent((tt_obj)(opaque)a[0]);
	break; 
    /* handy tteventbig methods */                             
    /* handy ttlistener methods */                     
    /* handy ttcallback methods */                                                 
    /* handy ttcomponent methods */             
    /* handy ttvisible methods */                                     
    /* handy ttnative methods */     
    /* handy ttwidget methods */                     
    /* handy ttlabel methods */                 
    /* handy ttanybutton methods */                         
    /* handy ttbutton methods */     
    /* handy ttcheckbutton methods */     
    /* handy ttradiobutton methods */     
    /* handy ttscrollbar methods */     
    /* handy ttbuttongroup methods */     
    /* handy ttwindow methods */         
    /* handy ttframe methods */     
    /* handy ttscroller methods */             
    /* handy ttmenuitem methods */         
    /* handy ttcheckmenuitem methods */     
    /* handy ttradiomenuitem methods */     
    /* handy ttmenu methods */    
     
    /* handy ttmenubar methods */     
    /* handy ttanytext methods */                 
    /* handy tttextfield methods */     
    /* handy tttextarea methods */     
    /* handy tttheme methods */    
            
         
    /* handy ttapplication methods */        






      default:
	break;
    }
}



