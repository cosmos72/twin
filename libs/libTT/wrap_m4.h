



/* This file was automatically generated from m4/wrap.m4, do not edit! */


/*
 *  wrap_m4.h  --  wrappers around exported libTT methods
 *
 */













/* public and exported methods */



/* ttobj methods */
                                
ttbyte TTGetValue_ttobj(tt_obj a1, ttuint a2, ttany * a3) {
    ttobj o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttobj,a1)))
	a0 = o->FN->GetValue(o, a2, a3);
    UNLK;
    return a0;
}    
ttbyte TTSetValue_ttobj(tt_obj a1, ttuint a2, ttany a3) {
    ttobj o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttobj,a1)))
	a0 = o->FN->SetValue(o, a2, a3);
    UNLK;
    return a0;
}    
ttbyte TTChangeValue_ttobj(tt_obj a1, ttuint a2, ttany a3, ttany a4) {
    ttobj o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttobj,a1)))
	a0 = o->FN->ChangeValue(o, a2, a3, a4);
    UNLK;
    return a0;
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
tt_obj TTCreateV_ttcallback(tt_obj a1, tt_obj a2, ttuint a3, ttuint a4, ttuint a5, ttuint a6, ttcallback_fn a7, va_list * a8) {
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
void TTSetXlYl_ttwidget(tt_obj a1, ttbyte a2, ttint a3, ttint a4) {
    ttwidget o;
    LOCK;
    if ((o = ID2(ttwidget,a1)))
	 o->FN->SetXlYl(o, a2, a3, a4);
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
void TTSetXl_ttwidget(tt_obj a1, ttint a2) {
    ttwidget o;
    LOCK;
    if ((o = ID2(ttwidget,a1)))
	 SetXl_ttwidget(o, a2);
    UNLK;
    return ;
}    
void TTSetYl_ttwidget(tt_obj a1, ttint a2) {
    ttwidget o;
    LOCK;
    if ((o = ID2(ttwidget,a1)))
	 SetYl_ttwidget(o, a2);
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







