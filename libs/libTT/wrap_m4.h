



/* This file was automatically generated from m4/wrap.m4, do not edit! */


/*
 *  wrap_m4.h  --  wrappers around exported libTT methods
 *
 */



















/* public, public_set and exported methods */



/* ttobj methods */
                                
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
void TTFire_ttevent(tt_obj a1, tt_obj a2) {
    ttevent o;
    LOCK;
    if ((o = ID2(ttevent,a1)))
	 Fire_ttevent(o, ID2(ttcomponent,a2));
    UNLK;
    return ;
}
 
/* tteventbig methods */


tt_obj TTCreate8_tteventbig(ttuint a1, ttuint a2, ttuint a3, ttshort a4, ttshort a5, ttshort a6, ttshort a7, ttany a8, ttany a9, ttuint a10, TT_CONST ttbyte * a11) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create8_tteventbig(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11));
    UNLK;
    return a0;
}
tt_obj TTCreate4_tteventbig(ttuint a1, ttuint a2, ttuint a3, ttshort a4, ttshort a5, ttshort a6, ttshort a7) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create4_tteventbig(a1, a2, a3, a4, a5, a6, a7));
    UNLK;
    return a0;
}
tt_obj TTCreate4s4_tteventbig(ttuint a1, ttuint a2, ttuint a3, ttany a4, ttany a5, ttuint a6, TT_CONST ttbyte * a7) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create4s4_tteventbig(a1, a2, a3, a4, a5, a6, a7));
    UNLK;
    return a0;
}
 
/* ttbitmask methods */

tt_obj TTClone_ttbitmask(tt_obj a1) {
    ttbitmask o;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((o = ID2(ttbitmask,a1)))
	a0 = (tt_obj)OBJ2ID(Clone_ttbitmask(o));
    UNLK;
    return a0;
}
tt_obj TTCreate_ttbitmask(ttany a1) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttbitmask(a1));
    UNLK;
    return a0;
}
tt_obj TTCreateA_ttbitmask(ttopaque a1, TT_ARG_READ ttany * a2) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(CreateA_ttbitmask(a1, a2));
    UNLK;
    return a0;
}
void TTClear_ttbitmask(tt_obj a1) {
    ttbitmask o;
    LOCK;
    if ((o = ID2(ttbitmask,a1)))
	 Clear_ttbitmask(o);
    UNLK;
    return ;
}
void TTFill_ttbitmask(tt_obj a1) {
    ttbitmask o;
    LOCK;
    if ((o = ID2(ttbitmask,a1)))
	 Fill_ttbitmask(o);
    UNLK;
    return ;
}
void TTNegate_ttbitmask(tt_obj a1) {
    ttbitmask o;
    LOCK;
    if ((o = ID2(ttbitmask,a1)))
	 Negate_ttbitmask(o);
    UNLK;
    return ;
}
void TTChange_ttbitmask(tt_obj a1, ttuint a2) {
    ttbitmask o;
    LOCK;
    if ((o = ID2(ttbitmask,a1)))
	 Change_ttbitmask(o, a2);
    UNLK;
    return ;
}
ttbyte TTIsMember_ttbitmask(tt_obj a1, ttany a2) {
    ttbitmask o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttbitmask,a1)))
	a0 = IsMember_ttbitmask(o, a2);
    UNLK;
    return a0;
}
ttbyte TTAdd_ttbitmask(tt_obj a1, ttany a2) {
    ttbitmask o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttbitmask,a1)))
	a0 = Add_ttbitmask(o, a2);
    UNLK;
    return a0;
}
ttbyte TTAddA_ttbitmask(tt_obj a1, ttopaque a2, TT_ARG_READ ttany * a3) {
    ttbitmask o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttbitmask,a1)))
	a0 = AddA_ttbitmask(o, a2, a3);
    UNLK;
    return a0;
}
ttbyte TTRemove_ttbitmask(tt_obj a1, ttany a2) {
    ttbitmask o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttbitmask,a1)))
	a0 = Remove_ttbitmask(o, a2);
    UNLK;
    return a0;
}
ttbyte TTRemoveA_ttbitmask(tt_obj a1, ttopaque a2, TT_ARG_READ ttany * a3) {
    ttbitmask o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttbitmask,a1)))
	a0 = RemoveA_ttbitmask(o, a2, a3);
    UNLK;
    return a0;
}
ttbyte TTCombine_ttbitmask(tt_obj a1, ttany a2, ttuint a3) {
    ttbitmask o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttbitmask,a1)))
	a0 = Combine_ttbitmask(o, a2, a3);
    UNLK;
    return a0;
}
ttbyte TTCombineA_ttbitmask(tt_obj a1, ttopaque a2, TT_ARG_READ ttany * a3, ttuint a4) {
    ttbitmask o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttbitmask,a1)))
	a0 = CombineA_ttbitmask(o, a2, a3, a4);
    UNLK;
    return a0;
}
ttbyte TTCombineB_ttbitmask(tt_obj a1, tt_obj a2, ttuint a3) {
    ttbitmask o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttbitmask,a1)))
	a0 = CombineB_ttbitmask(o, ID2(ttbitmask,a2), a3);
    UNLK;
    return a0;
}
 
/* tteventmask methods */

tt_obj TTCreate_tteventmask(tt_obj a1, tt_obj a2, tt_obj a3) {
    ttbitmask o;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((o = ID2(ttbitmask,a1)))
	a0 = (tt_obj)OBJ2ID(Create_tteventmask(o, ID2(ttbitmask,a2), ID2(ttbitmask,a3)));
    UNLK;
    return a0;
}
void TTSetEvtypeMask_tteventmask(tt_obj a1, tt_obj a2) {
    tteventmask o;
    LOCK;
    if ((o = ID2(tteventmask,a1)))
	 SetEvtypeMask_tteventmask(o, ID2(ttbitmask,a2));
    UNLK;
    return ;
}
void TTSetEvcodeMask_tteventmask(tt_obj a1, tt_obj a2) {
    tteventmask o;
    LOCK;
    if ((o = ID2(tteventmask,a1)))
	 SetEvcodeMask_tteventmask(o, ID2(ttbitmask,a2));
    UNLK;
    return ;
}
void TTSetComponentMask_tteventmask(tt_obj a1, tt_obj a2) {
    tteventmask o;
    LOCK;
    if ((o = ID2(tteventmask,a1)))
	 SetComponentMask_tteventmask(o, ID2(ttbitmask,a2));
    UNLK;
    return ;
}
void TTSetTruthTable_tteventmask(tt_obj a1, ttuint a2) {
    tteventmask o;
    LOCK;
    if ((o = ID2(tteventmask,a1)))
	 SetTruthTable_tteventmask(o, a2);
    UNLK;
    return ;
}
 
/* ttcallback methods */
    
 
/* ttlistener methods */

tt_obj TTCreate_ttlistener(tt_obj a1, ttuint a2, ttuint a3, ttlistener_fn a4, ttany a5) {
    ttcomponent o;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((o = ID2(ttcomponent,a1)))
	a0 = (tt_obj)OBJ2ID(Create_ttlistener(o, a2, a3, a4, a5));
    UNLK;
    return a0;
}
tt_obj TTCreateA_ttlistener(tt_obj a1, tt_obj a2, ttuint a3, ttuint a4, ttuint a5, ttuint a6, ttlistener_fn a7, TT_ARG_READ ttany * a8) {
    ttcomponent o;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((o = ID2(ttcomponent,a1)))
	a0 = (tt_obj)OBJ2ID(CreateA_ttlistener(o, ID2(ttevent,a2), a3, a4, a5, a6, a7, a8));
    UNLK;
    return a0;
}
tt_obj TTCreateV_ttlistener(tt_obj a1, tt_obj a2, ttuint a3, ttuint a4, ttuint a5, ttuint a6, ttlistener_fn a7, va_list * a8) {
    ttcomponent o;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((o = ID2(ttcomponent,a1)))
	a0 = (tt_obj)OBJ2ID(CreateV_ttlistener(o, ID2(ttevent,a2), a3, a4, a5, a6, a7, a8));
    UNLK;
    return a0;
}

tt_obj TTCreateDel_ttlistener(tt_obj a1, ttuint a2, ttlistener_fn a3) {
    ttcomponent o;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((o = ID2(ttcomponent,a1)))
	a0 = (tt_obj)OBJ2ID(CreateDel_ttlistener(o, a2, a3));
    UNLK;
    return a0;
}
tt_obj TTCreateAskclose_ttlistener(tt_obj a1, ttuint a2, ttlistener_fn a3) {
    ttcomponent o;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((o = ID2(ttcomponent,a1)))
	a0 = (tt_obj)OBJ2ID(CreateAskclose_ttlistener(o, a2, a3));
    UNLK;
    return a0;
}
tt_obj TTCreateKey_ttlistener(tt_obj a1, ttuint a2, ttuint a3, ttuint a4, ttlistener_fn a5) {
    ttcomponent o;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((o = ID2(ttcomponent,a1)))
	a0 = (tt_obj)OBJ2ID(CreateKey_ttlistener(o, a2, a3, a4, a5));
    UNLK;
    return a0;
}
tt_obj TTCreateMouse_ttlistener(tt_obj a1, ttuint a2, ttuint a3, ttuint a4, ttlistener_fn a5) {
    ttcomponent o;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((o = ID2(ttcomponent,a1)))
	a0 = (tt_obj)OBJ2ID(CreateMouse_ttlistener(o, a2, a3, a4, a5));
    UNLK;
    return a0;
}
tt_obj TTCreateActivate_ttlistener(tt_obj a1, ttuint a2, ttlistener_fn a3) {
    ttcomponent o;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((o = ID2(ttcomponent,a1)))
	a0 = (tt_obj)OBJ2ID(CreateActivate_ttlistener(o, a2, a3));
    UNLK;
    return a0;
}
tt_obj TTCreateChange_ttlistener(tt_obj a1, ttuint a2, ttuint a3, ttlistener_fn a4) {
    ttcomponent o;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((o = ID2(ttcomponent,a1)))
	a0 = (tt_obj)OBJ2ID(CreateChange_ttlistener(o, a2, a3, a4));
    UNLK;
    return a0;
}
tt_obj TTCreateKeyData_ttlistener(tt_obj a1, TT_ARG_READ ttbyte * a2, ttuint a3, ttlistener_fn a4) {
    ttcomponent o;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((o = ID2(ttcomponent,a1)))
	a0 = (tt_obj)OBJ2ID(CreateKeyData_ttlistener(o, a2, a3, a4));
    UNLK;
    return a0;
}
tt_obj TTCreateExpose_ttlistener(tt_obj a1, ttuint a2, ttlistener_fn a3) {
    ttcomponent o;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((o = ID2(ttcomponent,a1)))
	a0 = (tt_obj)OBJ2ID(CreateExpose_ttlistener(o, a2, a3));
    UNLK;
    return a0;
}
    
void TTAddTo_ttlistener(tt_obj a1, tt_obj a2) {
    ttlistener o;
    LOCK;
    if ((o = ID2(ttlistener,a1)))
	 o->FN->AddTo(o, ID2(ttcomponent,a2));
    UNLK;
    return ;
}    
void TTRemove_ttlistener(tt_obj a1) {
    ttlistener o;
    LOCK;
    if ((o = ID2(ttlistener,a1)))
	 o->FN->Remove(o);
    UNLK;
    return ;
}
ttbyte TTSetArgs_ttlistener(tt_obj a1, ttuint a2, ttuint a3, TT_ARG_READ ttany * a4) {
    ttlistener o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttlistener,a1)))
	a0 = SetArgs_ttlistener(o, a2, a3, a4);
    UNLK;
    return a0;
}
void TTSetEventMask_ttlistener(tt_obj a1, tt_obj a2) {
    ttlistener o;
    LOCK;
    if ((o = ID2(ttlistener,a1)))
	 SetEventMask_ttlistener(o, ID2(tteventmask,a2));
    UNLK;
    return ;
}
 
/* tttimer methods */

tt_obj TTCreateA_tttimer(tt_obj a1, ttuint a2, ttuint a3, ttuint a4, ttuint a5, ttlistener_fn a6, TT_ARG_READ ttany * a7, ttuint a8, ttuint a9) {
    ttcomponent o;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((o = ID2(ttcomponent,a1)))
	a0 = (tt_obj)OBJ2ID(CreateA_tttimer(o, a2, a3, a4, a5, a6, a7, a8, a9));
    UNLK;
    return a0;
}
tt_obj TTCreate_tttimer(tt_obj a1, ttuint a2, ttlistener_fn a3, ttany a4, ttuint a5, ttuint a6) {
    ttcomponent o;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((o = ID2(ttcomponent,a1)))
	a0 = (tt_obj)OBJ2ID(Create_tttimer(o, a2, a3, a4, a5, a6));
    UNLK;
    return a0;
}
void TTSetDelay_tttimer(tt_obj a1, ttuint a2, ttuint a3) {
    tttimer o;
    LOCK;
    if ((o = ID2(tttimer,a1)))
	 SetDelay_tttimer(o, a2, a3);
    UNLK;
    return ;
}    
void TTSetEnabled_tttimer(tt_obj a1, ttbyte a2) {
    tttimer o;
    LOCK;
    if ((o = ID2(tttimer,a1)))
	 o->FN->SetEnabled(o, a2);
    UNLK;
    return ;
}    
ttbyte TTIsEnabled_tttimer(tt_obj a1) {
    tttimer o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(tttimer,a1)))
	a0 = o->FN->IsEnabled(o);
    UNLK;
    return a0;
}
 
/* ttcomponent methods */

void TTAddListener_ttcomponent(tt_obj a1, tt_obj a2) {
    ttcomponent o;
    LOCK;
    if ((o = ID2(ttcomponent,a1)))
	 AddListener_ttcomponent(o, ID2(ttlistener,a2));
    UNLK;
    return ;
}
void TTRemoveListener_ttcomponent(tt_obj a1, tt_obj a2) {
    ttcomponent o;
    LOCK;
    if ((o = ID2(ttcomponent,a1)))
	 RemoveListener_ttcomponent(o, ID2(ttlistener,a2));
    UNLK;
    return ;
}
tt_obj TTSetKeyData_ttcomponent(tt_obj a1, TT_ARG_READ ttbyte * a2, ttany a3) {
    ttcomponent o;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((o = ID2(ttcomponent,a1)))
	a0 = (tt_obj)OBJ2ID(SetKeyData_ttcomponent(o, a2, a3));
    UNLK;
    return a0;
}
ttany TTGetKeyData_ttcomponent(tt_obj a1, TT_ARG_READ ttbyte * a2) {
    ttcomponent o;
    ttany a0 = (ttany)0;
    LOCK;
    if ((o = ID2(ttcomponent,a1)))
	a0 = GetKeyData_ttcomponent(o, a2);
    UNLK;
    return a0;
}
void TTUnsetKeyData_ttcomponent(tt_obj a1, TT_ARG_READ ttbyte * a2) {
    ttcomponent o;
    LOCK;
    if ((o = ID2(ttcomponent,a1)))
	 UnsetKeyData_ttcomponent(o, a2);
    UNLK;
    return ;
}
tt_obj TTGetData_ttcomponent(tt_obj a1, TT_ARG_READ ttbyte * a2) {
    ttcomponent o;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((o = ID2(ttcomponent,a1)))
	a0 = (tt_obj)OBJ2ID(GetData_ttcomponent(o, a2));
    UNLK;
    return a0;
}
 
/* ttdata methods */
        
 
/* ttvisible methods */
    
void TTAddTo_ttvisible(tt_obj a1, tt_obj a2) {
    ttvisible o;
    LOCK;
    if ((o = ID2(ttvisible,a1)))
	 AddTo_ttvisible(o, ID2(ttvisible,a2));
    UNLK;
    return ;
}    
void TTRemove_ttvisible(tt_obj a1) {
    ttvisible o;
    LOCK;
    if ((o = ID2(ttvisible,a1)))
	 Remove_ttvisible(o);
    UNLK;
    return ;
}    
void TTInvalidate_ttvisible(tt_obj a1) {
    ttvisible o;
    LOCK;
    if ((o = ID2(ttvisible,a1)))
	 o->FN->Invalidate(o);
    UNLK;
    return ;
}    
void TTSetVisible_ttvisible(tt_obj a1, ttbyte a2) {
    ttvisible o;
    LOCK;
    if ((o = ID2(ttvisible,a1)))
	 SetVisible_ttvisible(o, a2);
    UNLK;
    return ;
}    
void TTSetTheme_ttvisible(tt_obj a1, tt_obj a2) {
    ttvisible o;
    LOCK;
    if ((o = ID2(ttvisible,a1)))
	 SetTheme_ttvisible(o, ID2(tttheme,a2));
    UNLK;
    return ;
}    
void TTDraw_ttvisible(tt_obj a1, ttshort a2, ttshort a3, ttshort a4, ttshort a5, ttshort a6, TT_ARG_READ ttbyte * a7, TT_ARG_READ ttfont * a8, TT_ARG_READ ttattr * a9) {
    ttvisible o;
    LOCK;
    if ((o = ID2(ttvisible,a1)))
	 o->FN->Draw(o, a2, a3, a4, a5, a6, a7, a8, a9);
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
ttshort TTGetW_ttnative(tt_obj a1) {
    ttnative o;
    ttshort a0 = (ttshort)0;
    LOCK;
    if ((o = ID2(ttnative,a1)))
	a0 = o->FN->GetW(o);
    UNLK;
    return a0;
}    
ttshort TTGetH_ttnative(tt_obj a1) {
    ttnative o;
    ttshort a0 = (ttshort)0;
    LOCK;
    if ((o = ID2(ttnative,a1)))
	a0 = o->FN->GetH(o);
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
void TTSetWl_ttwidget(tt_obj a1, ttint a2) {
    ttwidget o;
    LOCK;
    if ((o = ID2(ttwidget,a1)))
	 SetWl_ttwidget(o, a2);
    UNLK;
    return ;
}    
void TTSetHl_ttwidget(tt_obj a1, ttint a2) {
    ttwidget o;
    LOCK;
    if ((o = ID2(ttwidget,a1)))
	 SetHl_ttwidget(o, a2);
    UNLK;
    return ;
}    
void TTSetTooltip_ttwidget(tt_obj a1, tt_obj a2) {
    ttwidget o;
    LOCK;
    if ((o = ID2(ttwidget,a1)))
	 SetTooltip_ttwidget(o, ID2(tttooltip,a2));
    UNLK;
    return ;
}
void TTSetXlYl_ttwidget(tt_obj a1, ttbyte a2, ttint a3, ttint a4) {
    ttwidget o;
    LOCK;
    if ((o = ID2(ttwidget,a1)))
	 SetXlYl_ttwidget(o, a2, a3, a4);
    UNLK;
    return ;
}
void TTSetWlHl_ttwidget(tt_obj a1, ttbyte a2, ttint a3, ttint a4) {
    ttwidget o;
    LOCK;
    if ((o = ID2(ttwidget,a1)))
	 SetWlHl_ttwidget(o, a2, a3, a4);
    UNLK;
    return ;
}
void TTSetXYWH_ttwidget(tt_obj a1, ttbyte a2, ttshort a3, ttshort a4, ttshort a5, ttshort a6) {
    ttwidget o;
    LOCK;
    if ((o = ID2(ttwidget,a1)))
	 SetXYWH_ttwidget(o, a2, a3, a4, a5, a6);
    UNLK;
    return ;
}
void TTSetX_ttwidget(tt_obj a1, ttshort a2) {
    ttwidget o;
    LOCK;
    if ((o = ID2(ttwidget,a1)))
	 SetX_ttwidget(o, a2);
    UNLK;
    return ;
}
void TTSetY_ttwidget(tt_obj a1, ttshort a2) {
    ttwidget o;
    LOCK;
    if ((o = ID2(ttwidget,a1)))
	 SetY_ttwidget(o, a2);
    UNLK;
    return ;
}
void TTSetW_ttwidget(tt_obj a1, ttshort a2) {
    ttwidget o;
    LOCK;
    if ((o = ID2(ttwidget,a1)))
	 SetW_ttwidget(o, a2);
    UNLK;
    return ;
}
void TTSetH_ttwidget(tt_obj a1, ttshort a2) {
    ttwidget o;
    LOCK;
    if ((o = ID2(ttwidget,a1)))
	 SetH_ttwidget(o, a2);
    UNLK;
    return ;
}
 
/* ttlabel methods */
    
ttbyte TTDoSetFont_ttlabel(tt_obj a1, TT_ARG_DIE ttfont * a2, ttopaque a3) {
    ttlabel o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttlabel,a1)))
	a0 = DoSetFont_ttlabel(o, a2, a3);
    UNLK;
    return a0;
}
ttbyte TTSetFont_ttlabel(tt_obj a1, TT_ARG_READ ttfont * a2) {
    ttlabel o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttlabel,a1)))
	a0 = SetFont_ttlabel(o, a2);
    UNLK;
    return a0;
}
ttbyte TTSetText_ttlabel(tt_obj a1, TT_ARG_READ ttbyte * a2) {
    ttlabel o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttlabel,a1)))
	a0 = SetText_ttlabel(o, a2);
    UNLK;
    return a0;
}
tt_obj TTCreate_ttlabel(TT_ARG_READ ttbyte * a1) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttlabel(a1));
    UNLK;
    return a0;
}
 
/* tttooltip methods */
    
void TTSetWidget_tttooltip(tt_obj a1, tt_obj a2) {
    tttooltip o;
    LOCK;
    if ((o = ID2(tttooltip,a1)))
	 SetWidget_tttooltip(o, ID2(ttwidget,a2));
    UNLK;
    return ;
}
 
/* ttbuttongroup methods */
    
void TTAdd_ttbuttongroup(tt_obj a1, tt_obj a2) {
    ttbuttongroup o;
    LOCK;
    if ((o = ID2(ttbuttongroup,a1)))
	 Add_ttbuttongroup(o, ID2(ttradiobutton,a2));
    UNLK;
    return ;
}    
void TTRemove_ttbuttongroup(tt_obj a1, tt_obj a2) {
    ttbuttongroup o;
    LOCK;
    if ((o = ID2(ttbuttongroup,a1)))
	 Remove_ttbuttongroup(o, ID2(ttradiobutton,a2));
    UNLK;
    return ;
}    
void TTSetChecked_ttbuttongroup(tt_obj a1, tt_obj a2) {
    ttbuttongroup o;
    LOCK;
    if ((o = ID2(ttbuttongroup,a1)))
	 SetChecked_ttbuttongroup(o, ID2(ttradiobutton,a2));
    UNLK;
    return ;
}
 
/* ttanybutton methods */
        
ttbyte TTDoSetAttr_ttanybutton(tt_obj a1, TT_ARG_DIE ttattr * a2, ttshort a3, ttshort a4, ttshort a5) {
    ttanybutton o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttanybutton,a1)))
	a0 = DoSetAttr_ttanybutton(o, a2, a3, a4, a5);
    UNLK;
    return a0;
}
ttbyte TTSetAttr_ttanybutton(tt_obj a1, TT_ARG_READ ttattr * a2, ttshort a3, ttshort a4, ttshort a5) {
    ttanybutton o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttanybutton,a1)))
	a0 = SetAttr_ttanybutton(o, a2, a3, a4, a5);
    UNLK;
    return a0;
}
ttbyte TTSetFont_ttanybutton(tt_obj a1, TT_ARG_READ ttfont * a2, ttshort a3, ttshort a4, ttshort a5) {
    ttanybutton o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttanybutton,a1)))
	a0 = SetFont_ttanybutton(o, a2, a3, a4, a5);
    UNLK;
    return a0;
}
ttbyte TTSetText_ttanybutton(tt_obj a1, TT_ARG_READ ttbyte * a2, ttshort a3, ttshort a4, ttshort a5) {
    ttanybutton o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttanybutton,a1)))
	a0 = SetText_ttanybutton(o, a2, a3, a4, a5);
    UNLK;
    return a0;
}
tt_obj TTCreate_ttanybutton(TT_ARG_READ ttbyte * a1, ttshort a2, ttshort a3, ttshort a4) {
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
	 SetPressed_ttbutton(o, a2);
    UNLK;
    return ;
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
tt_obj TTCreate_ttbutton(TT_ARG_READ ttbyte * a1, ttshort a2, ttshort a3, ttshort a4) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttbutton(a1, a2, a3, a4));
    UNLK;
    return a0;
}
 
/* ttcheckbutton methods */
    
void TTSetChecked_ttcheckbutton(tt_obj a1, ttbyte a2) {
    ttcheckbutton o;
    LOCK;
    if ((o = ID2(ttcheckbutton,a1)))
	 SetChecked_ttcheckbutton(o, a2);
    UNLK;
    return ;
}
ttbyte TTIsChecked_ttcheckbutton(tt_obj a1) {
    ttcheckbutton o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttcheckbutton,a1)))
	a0 = IsChecked_ttcheckbutton(o);
    UNLK;
    return a0;
}
tt_obj TTCreate_ttcheckbutton(TT_ARG_READ ttbyte * a1, ttshort a2, ttshort a3, ttshort a4) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttcheckbutton(a1, a2, a3, a4));
    UNLK;
    return a0;
}
 
/* ttradiobutton methods */

void TTAddToGroup_ttradiobutton(tt_obj a1, tt_obj a2) {
    ttradiobutton o;
    LOCK;
    if ((o = ID2(ttradiobutton,a1)))
	 AddToGroup_ttradiobutton(o, ID2(ttbuttongroup,a2));
    UNLK;
    return ;
}
void TTRemoveFromGroup_ttradiobutton(tt_obj a1) {
    ttradiobutton o;
    LOCK;
    if ((o = ID2(ttradiobutton,a1)))
	 RemoveFromGroup_ttradiobutton(o);
    UNLK;
    return ;
}
tt_obj TTCreate_ttradiobutton(TT_ARG_READ ttbyte * a1, ttshort a2, ttshort a3, ttshort a4) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttradiobutton(a1, a2, a3, a4));
    UNLK;
    return a0;
}
 
/* ttanyscroll methods */
            
void TTSetOrientation_ttanyscroll(tt_obj a1, ttbyte a2) {
    ttanyscroll o;
    LOCK;
    if ((o = ID2(ttanyscroll,a1)))
	 SetOrientation_ttanyscroll(o, a2);
    UNLK;
    return ;
}    
void TTSetSize_ttanyscroll(tt_obj a1, ttint a2) {
    ttanyscroll o;
    LOCK;
    if ((o = ID2(ttanyscroll,a1)))
	 SetSize_ttanyscroll(o, a2);
    UNLK;
    return ;
}    
void TTSetRealSize_ttanyscroll(tt_obj a1, ttint a2) {
    ttanyscroll o;
    LOCK;
    if ((o = ID2(ttanyscroll,a1)))
	 SetRealSize_ttanyscroll(o, a2);
    UNLK;
    return ;
}    
void TTSetViewSize_ttanyscroll(tt_obj a1, ttint a2) {
    ttanyscroll o;
    LOCK;
    if ((o = ID2(ttanyscroll,a1)))
	 SetViewSize_ttanyscroll(o, a2);
    UNLK;
    return ;
}    
void TTSetPosition_ttanyscroll(tt_obj a1, ttint a2) {
    ttanyscroll o;
    LOCK;
    if ((o = ID2(ttanyscroll,a1)))
	 SetPosition_ttanyscroll(o, a2);
    UNLK;
    return ;
}
 
/* ttscrollbar methods */

tt_obj TTCreate1_ttscrollbar(ttbyte a1) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create1_ttscrollbar(a1));
    UNLK;
    return a0;
}
tt_obj TTCreate4_ttscrollbar(ttbyte a1, ttint a2, ttint a3, ttint a4) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create4_ttscrollbar(a1, a2, a3, a4));
    UNLK;
    return a0;
}
 
/* ttslider methods */
    
void TTSetSlideMin_ttslider(tt_obj a1, ttint a2) {
    ttslider o;
    LOCK;
    if ((o = ID2(ttslider,a1)))
	 SetSlideMin_ttslider(o, a2);
    UNLK;
    return ;
}    
void TTSetSlideMax_ttslider(tt_obj a1, ttint a2) {
    ttslider o;
    LOCK;
    if ((o = ID2(ttslider,a1)))
	 SetSlideMax_ttslider(o, a2);
    UNLK;
    return ;
}    
void TTSetSlideValue_ttslider(tt_obj a1, ttint a2) {
    ttslider o;
    LOCK;
    if ((o = ID2(ttslider,a1)))
	 SetSlideValue_ttslider(o, a2);
    UNLK;
    return ;
}
tt_obj TTCreate_ttslider(ttbyte a1, ttint a2) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttslider(a1, a2));
    UNLK;
    return a0;
}
 
/* ttprogressbar methods */

tt_obj TTCreate_ttprogressbar(ttbyte a1) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttprogressbar(a1));
    UNLK;
    return a0;
}
 
/* ttscrollpane methods */

tt_obj TTCreate_ttscrollpane(ttshort a1, ttshort a2) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttscrollpane(a1, a2));
    UNLK;
    return a0;
}    
void TTSetBarX_ttscrollpane(tt_obj a1, tt_obj a2) {
    ttscrollpane o;
    LOCK;
    if ((o = ID2(ttscrollpane,a1)))
	 SetBarX_ttscrollpane(o, ID2(ttscrollbar,a2));
    UNLK;
    return ;
}    
void TTSetBarY_ttscrollpane(tt_obj a1, tt_obj a2) {
    ttscrollpane o;
    LOCK;
    if ((o = ID2(ttscrollpane,a1)))
	 SetBarY_ttscrollpane(o, ID2(ttscrollbar,a2));
    UNLK;
    return ;
}
 
/* ttwindow methods */
    
ttbyte TTSetTitle_ttwindow(tt_obj a1, TT_ARG_READ ttbyte * a2) {
    ttwindow o;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((o = ID2(ttwindow,a1)))
	a0 = SetTitle_ttwindow(o, a2);
    UNLK;
    return a0;
}
 
/* ttframe methods */
    
void TTSetMenubar_ttframe(tt_obj a1, tt_obj a2) {
    ttframe o;
    LOCK;
    if ((o = ID2(ttframe,a1)))
	 SetMenubar_ttframe(o, ID2(ttmenubar,a2));
    UNLK;
    return ;
}
tt_obj TTCreate_ttframe(TT_ARG_READ ttbyte * a1) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttframe(a1));
    UNLK;
    return a0;
}
 
/* ttscroller methods */

tt_obj TTCreate_ttscroller(ttshort a1, ttshort a2) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttscroller(a1, a2));
    UNLK;
    return a0;
}
 
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

tt_obj TTSet_ttapplication(TT_ARG_READ ttbyte * a1) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Set_ttapplication(a1));
    UNLK;
    return a0;
}
tt_obj TTGet_ttapplication(void) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Get_ttapplication());
    UNLK;
    return a0;
}








