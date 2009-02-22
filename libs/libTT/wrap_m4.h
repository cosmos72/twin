



/* This file was automatically generated from m4/wrap.m4, do not edit! */
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
 *  wrap_m4.h  --  wrappers around final libTT methods
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























/* public, public_set and final methods */


 
/* ttobj methods */
                    
 
/* ttclass methods */

 
/* ttobject methods */
                
void TTRef_ttobject(tt_obj o) {
    ttobject a1;
    LOCK;
    if ((a1 = ID2(ttobject,o)))
	Ref_ttobject(a1);
    UNLK;
    return ;
}
void TTUnref_ttobject(tt_obj o) {
    ttobject a1;
    LOCK;
    if ((a1 = ID2(ttobject,o)))
	Unref_ttobject(a1);
    UNLK;
    return ;
}
ttbyte TTSetUserData_ttobject(tt_obj o, ttany user_data) {
    ttobject a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttobject,o)))
	a0 = SetUserData_ttobject(a1, user_data);
    UNLK;
    return a0;
}
 
/* ttvector methods */
    
ttany TTGetAt_ttvector(TT_ARG_READ tt_obj o, ttopaque pos) {
    ttvector a1;
    ttany a0 = (ttany)0;
    LOCK;
    if ((a1 = ID2(ttvector,o)))
	a0 = a1->Class->GetAt(a1, pos);
    UNLK;
    return a0;
}    
ttbyte TTSetAt_ttvector(tt_obj o, ttopaque pos, ttany value) {
    ttvector a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttvector,o)))
	a0 = a1->Class->SetAt(a1, pos, value);
    UNLK;
    return a0;
}
tt_obj TTCreate1_ttvector(ttany value) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create1_ttvector(value));
    UNLK;
    return a0;
}
tt_obj TTCreateA_ttvector(ttopaque value_n, TT_ARG_READ TT_ARG_ARRAY((_P(1))) ttarg * values) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(CreateA_ttvector(value_n, values));
    UNLK;
    return a0;
}
tt_obj TTCreateR_ttvector(TT_ARG_READ tt_obj v) {
    ttvector a1;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((a1 = ID2(ttvector,v)))
	a0 = (tt_obj)OBJ2ID(CreateR_ttvector(a1));
    UNLK;
    return a0;
}
tt_obj TTCreateV_ttvector(ttopaque value_n, TT_ARG_ARRAY((1)) va_list * values) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(CreateV_ttvector(value_n, values));
    UNLK;
    return a0;
}
tt_obj TTCreateY_ttvector(ttopaque value_n, TT_ARG_READ TT_ARG_ARRAY((_P(1))) ttany * values) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(CreateY_ttvector(value_n, values));
    UNLK;
    return a0;
}
ttbyte TTAppend_ttvector(tt_obj o, ttany value) {
    ttvector a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttvector,o)))
	a0 = Append_ttvector(a1, value);
    UNLK;
    return a0;
}
ttbyte TTAdd_ttvector(tt_obj o, ttopaque pos, ttany value) {
    ttvector a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttvector,o)))
	a0 = Add_ttvector(a1, pos, value);
    UNLK;
    return a0;
}
ttbyte TTAddA_ttvector(tt_obj o, ttopaque pos, ttopaque value_n, TT_ARG_READ TT_ARG_ARRAY((_P(2))) ttarg * values) {
    ttvector a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttvector,o)))
	a0 = AddA_ttvector(a1, pos, value_n, values);
    UNLK;
    return a0;
}
ttbyte TTAddR_ttvector(tt_obj o, ttopaque pos, TT_ARG_READ tt_obj v) {
    ttvector a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttvector,o)))
	a0 = AddR_ttvector(a1, pos, ID2(ttvector,v));
    UNLK;
    return a0;
}
ttbyte TTAddV_ttvector(tt_obj o, ttopaque pos, ttopaque value_n, TT_ARG_ARRAY((1)) va_list * values) {
    ttvector a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttvector,o)))
	a0 = AddV_ttvector(a1, pos, value_n, values);
    UNLK;
    return a0;
}    
ttbyte TTAddY_ttvector(tt_obj o, ttopaque pos, ttopaque value_n, TT_ARG_READ TT_ARG_ARRAY((_P(2))) ttany * values) {
    ttvector a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttvector,o)))
	a0 = a1->Class->AddY(a1, pos, value_n, values);
    UNLK;
    return a0;
}    
ttopaque TTContainsValue_ttvector(TT_ARG_READ tt_obj o, ttopaque pos_start, ttany value) {
    ttvector a1;
    ttopaque a0 = (ttopaque)0;
    LOCK;
    if ((a1 = ID2(ttvector,o)))
	a0 = a1->Class->ContainsValue(a1, pos_start, value);
    UNLK;
    return a0;
}
ttbyte TTRemoveAt_ttvector(tt_obj o, ttopaque pos) {
    ttvector a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttvector,o)))
	a0 = RemoveAt_ttvector(a1, pos);
    UNLK;
    return a0;
}    
ttbyte TTRemoveRange_ttvector(tt_obj o, ttopaque pos_start, ttopaque pos_n) {
    ttvector a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttvector,o)))
	a0 = a1->Class->RemoveRange(a1, pos_start, pos_n);
    UNLK;
    return a0;
}
 
/* ttfield methods */

 
/* ttmethod methods */

 
/* ttevent methods */

tt_obj TTCreate_ttevent(ttuint evtype, ttuint evcode, ttuint evflags) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttevent(evtype, evcode, evflags));
    UNLK;
    return a0;
}
void TTFire_ttevent(TT_ARG_DIE tt_obj ev, TT_ARG_READ tt_obj o) {
    ttevent a1;
    LOCK;
    if ((a1 = ID2(ttevent,ev)))
	Fire_ttevent(a1, ID2(ttcomponent,o));
    UNLK;
    return ;
}
 
/* tteventbig methods */

tt_obj TTCreate8_tteventbig(ttuint evtype, ttuint evcode, ttuint evflags, ttshort x, ttshort y, ttshort w, ttshort h, ttany value, ttany old_value, ttopaque data_len, TT_ARG_READ TT_ARG_ARRAY((_P(10))) ttbyte * data) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create8_tteventbig(evtype, evcode, evflags, x, y, w, h, value, old_value, data_len, data));
    UNLK;
    return a0;
}
tt_obj TTCreate4_tteventbig(ttuint evtype, ttuint evcode, ttuint evflags, ttshort x, ttshort y, ttshort w, ttshort h) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create4_tteventbig(evtype, evcode, evflags, x, y, w, h));
    UNLK;
    return a0;
}
tt_obj TTCreate4s4_tteventbig(ttuint evtype, ttuint evcode, ttuint evflags, ttany value, ttany old_value, ttopaque data_len, TT_ARG_READ TT_ARG_ARRAY((_P(6))) ttbyte * data) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create4s4_tteventbig(evtype, evcode, evflags, value, old_value, data_len, data));
    UNLK;
    return a0;
}
 
/* ttbitmask methods */

tt_obj TTCreate_ttbitmask(ttany value) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttbitmask(value));
    UNLK;
    return a0;
}
tt_obj TTCreateB2_ttbitmask(TT_ARG_READ tt_obj o) {
    ttbitmask a1;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((a1 = ID2(ttbitmask,o)))
	a0 = (tt_obj)OBJ2ID(CreateB2_ttbitmask(a1));
    UNLK;
    return a0;
}
tt_obj TTCreateR_ttbitmask(TT_ARG_DIE tt_obj value) {
    ttvector a1;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((a1 = ID2(ttvector,value)))
	a0 = (tt_obj)OBJ2ID(CreateR_ttbitmask(a1));
    UNLK;
    return a0;
}
tt_obj TTCreateR2_ttbitmask(TT_ARG_READ tt_obj value) {
    ttvector a1;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((a1 = ID2(ttvector,value)))
	a0 = (tt_obj)OBJ2ID(CreateR2_ttbitmask(a1));
    UNLK;
    return a0;
}
void TTClear_ttbitmask(tt_obj o) {
    ttbitmask a1;
    LOCK;
    if ((a1 = ID2(ttbitmask,o)))
	Clear_ttbitmask(a1);
    UNLK;
    return ;
}
void TTFill_ttbitmask(tt_obj o) {
    ttbitmask a1;
    LOCK;
    if ((a1 = ID2(ttbitmask,o)))
	Fill_ttbitmask(a1);
    UNLK;
    return ;
}
void TTNegate_ttbitmask(tt_obj o) {
    ttbitmask a1;
    LOCK;
    if ((a1 = ID2(ttbitmask,o)))
	Negate_ttbitmask(a1);
    UNLK;
    return ;
}
void TTChange_ttbitmask(tt_obj o, ttuint truth_table) {
    ttbitmask a1;
    LOCK;
    if ((a1 = ID2(ttbitmask,o)))
	Change_ttbitmask(a1, truth_table);
    UNLK;
    return ;
}
ttbyte TTContains_ttbitmask(TT_ARG_READ tt_obj o, ttany value) {
    ttbitmask a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttbitmask,o)))
	a0 = Contains_ttbitmask(a1, value);
    UNLK;
    return a0;
}
ttbyte TTAdd_ttbitmask(tt_obj o, ttany value) {
    ttbitmask a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttbitmask,o)))
	a0 = Add_ttbitmask(a1, value);
    UNLK;
    return a0;
}
ttbyte TTAddB_ttbitmask(tt_obj o, TT_ARG_READ tt_obj value) {
    ttbitmask a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttbitmask,o)))
	a0 = AddB_ttbitmask(a1, ID2(ttbitmask,value));
    UNLK;
    return a0;
}
ttbyte TTAddR_ttbitmask(tt_obj o, TT_ARG_READ tt_obj value) {
    ttbitmask a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttbitmask,o)))
	a0 = AddR_ttbitmask(a1, ID2(ttvector,value));
    UNLK;
    return a0;
}
ttbyte TTRemove_ttbitmask(tt_obj o, ttany value) {
    ttbitmask a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttbitmask,o)))
	a0 = Remove_ttbitmask(a1, value);
    UNLK;
    return a0;
}
ttbyte TTRemoveB_ttbitmask(tt_obj o, TT_ARG_READ tt_obj value) {
    ttbitmask a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttbitmask,o)))
	a0 = RemoveB_ttbitmask(a1, ID2(ttbitmask,value));
    UNLK;
    return a0;
}
ttbyte TTRemoveR_ttbitmask(tt_obj o, TT_ARG_READ tt_obj value) {
    ttbitmask a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttbitmask,o)))
	a0 = RemoveR_ttbitmask(a1, ID2(ttvector,value));
    UNLK;
    return a0;
}
ttbyte TTCombine_ttbitmask(tt_obj o, ttany value, ttuint truth_table) {
    ttbitmask a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttbitmask,o)))
	a0 = Combine_ttbitmask(a1, value, truth_table);
    UNLK;
    return a0;
}
ttbyte TTCombineR_ttbitmask(tt_obj o, TT_ARG_READ tt_obj value, ttuint truth_table) {
    ttbitmask a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttbitmask,o)))
	a0 = CombineR_ttbitmask(a1, ID2(ttvector,value), truth_table);
    UNLK;
    return a0;
}
ttbyte TTCombineB_ttbitmask(tt_obj o, TT_ARG_READ tt_obj value, ttuint truth_table) {
    ttbitmask a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttbitmask,o)))
	a0 = CombineB_ttbitmask(a1, ID2(ttbitmask,value), truth_table);
    UNLK;
    return a0;
}
 
/* tteventmask methods */

tt_obj TTCreateB_tteventmask(TT_ARG_DIE tt_obj evtype_mask, TT_ARG_DIE tt_obj evcode_mask, TT_ARG_DIE tt_obj component_mask) {
    ttbitmask a1;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((a1 = ID2(ttbitmask,evtype_mask)))
	a0 = (tt_obj)OBJ2ID(CreateB_tteventmask(a1, ID2(ttbitmask,evcode_mask), ID2(ttbitmask,component_mask)));
    UNLK;
    return a0;
}
tt_obj TTCreateB2_tteventmask(TT_ARG_READ tt_obj evtype_mask, TT_ARG_READ tt_obj evcode_mask, TT_ARG_READ tt_obj component_mask) {
    ttbitmask a1;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((a1 = ID2(ttbitmask,evtype_mask)))
	a0 = (tt_obj)OBJ2ID(CreateB2_tteventmask(a1, ID2(ttbitmask,evcode_mask), ID2(ttbitmask,component_mask)));
    UNLK;
    return a0;
}
ttbyte TTSetEvtypeMask_tteventmask(tt_obj o, TT_ARG_DIE tt_obj evtype_mask) {
    tteventmask a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(tteventmask,o)))
	a0 = SetEvtypeMask_tteventmask(a1, ID2(ttbitmask,evtype_mask));
    UNLK;
    return a0;
}
ttbyte TTSetEvcodeMask_tteventmask(tt_obj o, TT_ARG_DIE tt_obj evcode_mask) {
    tteventmask a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(tteventmask,o)))
	a0 = SetEvcodeMask_tteventmask(a1, ID2(ttbitmask,evcode_mask));
    UNLK;
    return a0;
}
ttbyte TTSetComponentMask_tteventmask(tt_obj o, TT_ARG_DIE tt_obj component_mask) {
    tteventmask a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(tteventmask,o)))
	a0 = SetComponentMask_tteventmask(a1, ID2(ttbitmask,component_mask));
    UNLK;
    return a0;
}
ttbyte TTSetTruthTable_tteventmask(tt_obj o, ttuint truth_table) {
    tteventmask a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(tteventmask,o)))
	a0 = SetTruthTable_tteventmask(a1, truth_table);
    UNLK;
    return a0;
}
 
/* ttcallback methods */
    
 
/* ttlistener methods */

tt_obj TTCreate_ttlistener(tt_obj o, ttuint evtype, ttuint lflags, TT_ARG_READ ttlistener_fn function, ttany arg) {
    ttcomponent a1;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((a1 = ID2(ttcomponent,o)))
	a0 = (tt_obj)OBJ2ID(Create_ttlistener(a1, evtype, lflags, function, arg));
    UNLK;
    return a0;
}
tt_obj TTCreateE_ttlistener(tt_obj o, TT_ARG_DIE tt_obj ev, ttuint lflags, TT_ARG_READ ttlistener_fn function, ttany arg) {
    ttcomponent a1;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((a1 = ID2(ttcomponent,o)))
	a0 = (tt_obj)OBJ2ID(CreateE_ttlistener(a1, ID2(ttevent,ev), lflags, function, arg));
    UNLK;
    return a0;
}
tt_obj TTCreateR_ttlistener(tt_obj o, TT_ARG_DIE tt_obj ev, ttuint lflags, TT_ARG_READ ttlistener_fn function, ttopaque arg_component_n, ttopaque arg_event_n, TT_ARG_DIE tt_obj args) {
    ttcomponent a1;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((a1 = ID2(ttcomponent,o)))
	a0 = (tt_obj)OBJ2ID(CreateR_ttlistener(a1, ID2(ttevent,ev), lflags, function, arg_component_n, arg_event_n, ID2(ttvector,args)));
    UNLK;
    return a0;
}
tt_obj TTCreateDel_ttlistener(tt_obj o, ttuint lflags, TT_ARG_READ ttlistener_fn function) {
    ttcomponent a1;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((a1 = ID2(ttcomponent,o)))
	a0 = (tt_obj)OBJ2ID(CreateDel_ttlistener(a1, lflags, function));
    UNLK;
    return a0;
}
tt_obj TTCreateAskclose_ttlistener(tt_obj o, ttuint lflags, TT_ARG_READ ttlistener_fn function) {
    ttcomponent a1;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((a1 = ID2(ttcomponent,o)))
	a0 = (tt_obj)OBJ2ID(CreateAskclose_ttlistener(a1, lflags, function));
    UNLK;
    return a0;
}
tt_obj TTCreateKey_ttlistener(tt_obj o, ttuint evcode, ttuint evflags, ttuint lflags, TT_ARG_READ ttlistener_fn function) {
    ttcomponent a1;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((a1 = ID2(ttcomponent,o)))
	a0 = (tt_obj)OBJ2ID(CreateKey_ttlistener(a1, evcode, evflags, lflags, function));
    UNLK;
    return a0;
}
tt_obj TTCreateMouse_ttlistener(tt_obj o, ttuint evcode, ttuint evflags, ttuint lflags, TT_ARG_READ ttlistener_fn function) {
    ttcomponent a1;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((a1 = ID2(ttcomponent,o)))
	a0 = (tt_obj)OBJ2ID(CreateMouse_ttlistener(a1, evcode, evflags, lflags, function));
    UNLK;
    return a0;
}
tt_obj TTCreateActivate_ttlistener(tt_obj o, ttuint lflags, TT_ARG_READ ttlistener_fn function) {
    ttcomponent a1;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((a1 = ID2(ttcomponent,o)))
	a0 = (tt_obj)OBJ2ID(CreateActivate_ttlistener(a1, lflags, function));
    UNLK;
    return a0;
}
tt_obj TTCreateChange_ttlistener(tt_obj o, ttuint evcode, ttuint lflags, TT_ARG_READ ttlistener_fn function) {
    ttcomponent a1;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((a1 = ID2(ttcomponent,o)))
	a0 = (tt_obj)OBJ2ID(CreateChange_ttlistener(a1, evcode, lflags, function));
    UNLK;
    return a0;
}
tt_obj TTCreateKeyData_ttlistener(tt_obj o, TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * key, ttuint lflags, TT_ARG_READ ttlistener_fn function) {
    ttcomponent a1;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((a1 = ID2(ttcomponent,o)))
	a0 = (tt_obj)OBJ2ID(CreateKeyData_ttlistener(a1, key, lflags, function));
    UNLK;
    return a0;
}
tt_obj TTCreateExpose_ttlistener(tt_obj o, ttuint lflags, TT_ARG_READ ttlistener_fn function) {
    ttcomponent a1;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((a1 = ID2(ttcomponent,o)))
	a0 = (tt_obj)OBJ2ID(CreateExpose_ttlistener(a1, lflags, function));
    UNLK;
    return a0;
}    
void TTAddTo_ttlistener(tt_obj o, tt_obj c) {
    ttlistener a1;
    LOCK;
    if ((a1 = ID2(ttlistener,o)))
	a1->Class->AddTo(a1, ID2(ttcomponent,c));
    UNLK;
    return ;
}    
void TTRemove_ttlistener(tt_obj o) {
    ttlistener a1;
    LOCK;
    if ((a1 = ID2(ttlistener,o)))
	a1->Class->Remove(a1);
    UNLK;
    return ;
}
ttbyte TTSetArgsR_ttlistener(tt_obj o, TT_ARG_DIE tt_obj args) {
    ttlistener a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttlistener,o)))
	a0 = SetArgsR_ttlistener(a1, ID2(ttvector,args));
    UNLK;
    return a0;
}
ttbyte TTSetEventMask_ttlistener(tt_obj o, TT_ARG_DIE tt_obj event_mask) {
    ttlistener a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttlistener,o)))
	a0 = SetEventMask_ttlistener(a1, ID2(tteventmask,event_mask));
    UNLK;
    return a0;
}
 
/* tttimer methods */

tt_obj TTCreateR_tttimer(tt_obj r, ttuint lflags, TT_ARG_READ ttlistener_fn function, ttopaque arg_component_n, ttopaque arg_timer_n, TT_ARG_DIE tt_obj args, ttany delay_t, ttany delay_f) {
    ttcomponent a1;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((a1 = ID2(ttcomponent,r)))
	a0 = (tt_obj)OBJ2ID(CreateR_tttimer(a1, lflags, function, arg_component_n, arg_timer_n, ID2(ttvector,args), delay_t, delay_f));
    UNLK;
    return a0;
}
tt_obj TTCreate_tttimer(tt_obj r, ttuint lflags, TT_ARG_READ ttlistener_fn function, ttany arg, ttany delay_t, ttany delay_f) {
    ttcomponent a1;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((a1 = ID2(ttcomponent,r)))
	a0 = (tt_obj)OBJ2ID(Create_tttimer(a1, lflags, function, arg, delay_t, delay_f));
    UNLK;
    return a0;
}
void TTSetDelay_tttimer(tt_obj o, ttany delay_t, ttany delay_s) {
    tttimer a1;
    LOCK;
    if ((a1 = ID2(tttimer,o)))
	SetDelay_tttimer(a1, delay_t, delay_s);
    UNLK;
    return ;
}    
void TTSetEnabled_tttimer(tt_obj o, ttbyte enabled) {
    tttimer a1;
    LOCK;
    if ((a1 = ID2(tttimer,o)))
	a1->Class->SetEnabled(a1, enabled);
    UNLK;
    return ;
}    
ttbyte TTIsEnabled_tttimer(TT_ARG_READ tt_obj o) {
    tttimer a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(tttimer,o)))
	a0 = a1->Class->IsEnabled(a1);
    UNLK;
    return a0;
}
 
/* ttcomponent methods */

void TTAddListener_ttcomponent(tt_obj o, tt_obj l) {
    ttcomponent a1;
    LOCK;
    if ((a1 = ID2(ttcomponent,o)))
	AddListener_ttcomponent(a1, ID2(ttlistener,l));
    UNLK;
    return ;
}
void TTRemoveListener_ttcomponent(tt_obj o, tt_obj l) {
    ttcomponent a1;
    LOCK;
    if ((a1 = ID2(ttcomponent,o)))
	RemoveListener_ttcomponent(a1, ID2(ttlistener,l));
    UNLK;
    return ;
}
tt_obj TTSetKeyData_ttcomponent(tt_obj o, TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * key, ttany data) {
    ttcomponent a1;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((a1 = ID2(ttcomponent,o)))
	a0 = (tt_obj)OBJ2ID(SetKeyData_ttcomponent(a1, key, data));
    UNLK;
    return a0;
}
ttany TTGetKeyData_ttcomponent(TT_ARG_READ tt_obj o, TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * key) {
    ttcomponent a1;
    ttany a0 = (ttany)0;
    LOCK;
    if ((a1 = ID2(ttcomponent,o)))
	a0 = GetKeyData_ttcomponent(a1, key);
    UNLK;
    return a0;
}
void TTUnsetKeyData_ttcomponent(tt_obj o, TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * key) {
    ttcomponent a1;
    LOCK;
    if ((a1 = ID2(ttcomponent,o)))
	UnsetKeyData_ttcomponent(a1, key);
    UNLK;
    return ;
}
tt_obj TTGetData_ttcomponent(TT_ARG_READ tt_obj o, TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * key) {
    ttcomponent a1;
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    if ((a1 = ID2(ttcomponent,o)))
	a0 = (tt_obj)OBJ2ID(GetData_ttcomponent(a1, key));
    UNLK;
    return a0;
}
 
/* ttdata methods */
        
ttbyte TTSetData_ttdata(tt_obj o, ttany data) {
    ttdata a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttdata,o)))
	a0 = SetData_ttdata(a1, data);
    UNLK;
    return a0;
}
 
/* ttvisible methods */
    
void TTAddTo_ttvisible(tt_obj o, tt_obj parent, ttany constraint) {
    ttvisible a1;
    LOCK;
    if ((a1 = ID2(ttvisible,o)))
	AddTo_ttvisible(a1, ID2(ttvisible,parent), constraint);
    UNLK;
    return ;
}    
void TTValidate_ttvisible(tt_obj o) {
    ttvisible a1;
    LOCK;
    if ((a1 = ID2(ttvisible,o)))
	a1->Class->Validate(a1);
    UNLK;
    return ;
}    
void TTRemove_ttvisible(tt_obj o) {
    ttvisible a1;
    LOCK;
    if ((a1 = ID2(ttvisible,o)))
	Remove_ttvisible(a1);
    UNLK;
    return ;
}    
void TTInvalidate_ttvisible(tt_obj o) {
    ttvisible a1;
    LOCK;
    if ((a1 = ID2(ttvisible,o)))
	a1->Class->Invalidate(a1);
    UNLK;
    return ;
}    
void TTSetVisible_ttvisible(tt_obj o, ttbyte visible) {
    ttvisible a1;
    LOCK;
    if ((a1 = ID2(ttvisible,o)))
	SetVisible_ttvisible(a1, visible);
    UNLK;
    return ;
}    
ttbyte TTSetTheme_ttvisible(tt_obj o, TT_ARG_READ tt_obj theme) {
    ttvisible a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttvisible,o)))
	a0 = SetTheme_ttvisible(a1, ID2(tttheme,theme));
    UNLK;
    return a0;
}    
void TTDraw_ttvisible(tt_obj o, ttshort x, ttshort y, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttbyte * asciidata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttfont * fontdata, TT_ARG_READ TT_ARG_ARRAY((_P(5),*,_P(6))) ttattr * attrdata) {
    ttvisible a1;
    LOCK;
    if ((a1 = ID2(ttvisible,o)))
	a1->Class->Draw(a1, x, y, w, h, pitch, asciidata, fontdata, attrdata);
    UNLK;
    return ;
}    
void TTBuiltinRepaint_ttvisible(tt_obj o, ttshort x, ttshort y, ttshort w, ttshort h) {
    ttvisible a1;
    LOCK;
    if ((a1 = ID2(ttvisible,o)))
	a1->Class->BuiltinRepaint(a1, x, y, w, h);
    UNLK;
    return ;
}

ttbyte TTSetRepaint_ttvisible(tt_obj o, TT_ARG_READ ttvisible_repaint_fn repaint) {
    ttvisible a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttvisible,o)))
	a0 = SetRepaint_ttvisible(a1, repaint);
    UNLK;
    return a0;
}
void TTExpose_ttvisible(tt_obj o, ttshort x, ttshort y, ttshort w, ttshort h) {
    ttvisible a1;
    LOCK;
    if ((a1 = ID2(ttvisible,o)))
	Expose_ttvisible(a1, x, y, w, h);
    UNLK;
    return ;
}
void TTAdd_ttvisible(tt_obj o, tt_obj child, ttany constraint) {
    ttvisible a1;
    LOCK;
    if ((a1 = ID2(ttvisible,o)))
	Add_ttvisible(a1, ID2(ttvisible,child), constraint);
    UNLK;
    return ;
}
 
/* ttlayout methods */
        
 
/* ttboxlayout methods */

tt_obj TTCreate_ttboxlayout(ttany orientation) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttboxlayout(orientation));
    UNLK;
    return a0;
}
 
/* ttborderlayout methods */

 
/* ttnative methods */
    
tt_obj TTGetRoot_ttnative(void) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(TClass_ttnative->GetRoot());
    UNLK;
    return a0;
}    
ttshort TTGetW_ttnative(TT_ARG_READ tt_obj o) {
    ttnative a1;
    ttshort a0 = (ttshort)0;
    LOCK;
    if ((a1 = ID2(ttnative,o)))
	a0 = a1->Class->GetW(a1);
    UNLK;
    return a0;
}    
ttshort TTGetH_ttnative(TT_ARG_READ tt_obj o) {
    ttnative a1;
    ttshort a0 = (ttshort)0;
    LOCK;
    if ((a1 = ID2(ttnative,o)))
	a0 = a1->Class->GetH(a1);
    UNLK;
    return a0;
}
tt_obj TTCreate_ttnative(ttany native_value) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttnative(native_value));
    UNLK;
    return a0;
}
 
/* ttwidget methods */
    
void TTSetLayout_ttwidget(tt_obj o, tt_obj l) {
    ttwidget a1;
    LOCK;
    if ((a1 = ID2(ttwidget,o)))
	SetLayout_ttwidget(a1, ID2(ttlayout,l));
    UNLK;
    return ;
}    
ttbyte TTSetXY_ttwidget(tt_obj o, ttshort x, ttshort y) {
    ttwidget a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttwidget,o)))
	a0 = SetXY_ttwidget(a1, x, y);
    UNLK;
    return a0;
}    
ttbyte TTSetWH_ttwidget(tt_obj o, ttshort w, ttshort h) {
    ttwidget a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttwidget,o)))
	a0 = SetWH_ttwidget(a1, w, h);
    UNLK;
    return a0;
}    
ttbyte TTSetXl_ttwidget(tt_obj o, ttint xl) {
    ttwidget a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttwidget,o)))
	a0 = SetXl_ttwidget(a1, xl);
    UNLK;
    return a0;
}    
ttbyte TTSetYl_ttwidget(tt_obj o, ttint yl) {
    ttwidget a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttwidget,o)))
	a0 = SetYl_ttwidget(a1, yl);
    UNLK;
    return a0;
}    
ttbyte TTSetWl_ttwidget(tt_obj o, ttint wl) {
    ttwidget a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttwidget,o)))
	a0 = SetWl_ttwidget(a1, wl);
    UNLK;
    return a0;
}    
ttbyte TTSetHl_ttwidget(tt_obj o, ttint hl) {
    ttwidget a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttwidget,o)))
	a0 = SetHl_ttwidget(a1, hl);
    UNLK;
    return a0;
}    
ttbyte TTSetTooltip_ttwidget(tt_obj o, TT_ARG_DIE tt_obj t) {
    ttwidget a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttwidget,o)))
	a0 = SetTooltip_ttwidget(a1, ID2(tttooltip,t));
    UNLK;
    return a0;
}
ttbyte TTSetXlYl_ttwidget(tt_obj o, ttuint mask, ttint xl, ttint yl) {
    ttwidget a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttwidget,o)))
	a0 = SetXlYl_ttwidget(a1, mask, xl, yl);
    UNLK;
    return a0;
}
ttbyte TTSetWlHl_ttwidget(tt_obj o, ttuint mask, ttint wl, ttint hl) {
    ttwidget a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttwidget,o)))
	a0 = SetWlHl_ttwidget(a1, mask, wl, hl);
    UNLK;
    return a0;
}
ttbyte TTSetXYWH_ttwidget(tt_obj o, ttuint mask, ttshort x, ttshort y, ttshort w, ttshort h) {
    ttwidget a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttwidget,o)))
	a0 = SetXYWH_ttwidget(a1, mask, x, y, w, h);
    UNLK;
    return a0;
}
ttbyte TTSetX_ttwidget(tt_obj o, ttshort x) {
    ttwidget a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttwidget,o)))
	a0 = SetX_ttwidget(a1, x);
    UNLK;
    return a0;
}
ttbyte TTSetY_ttwidget(tt_obj o, ttshort y) {
    ttwidget a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttwidget,o)))
	a0 = SetY_ttwidget(a1, y);
    UNLK;
    return a0;
}
ttbyte TTSetW_ttwidget(tt_obj o, ttshort w) {
    ttwidget a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttwidget,o)))
	a0 = SetW_ttwidget(a1, w);
    UNLK;
    return a0;
}
ttbyte TTSetH_ttwidget(tt_obj o, ttshort h) {
    ttwidget a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttwidget,o)))
	a0 = SetH_ttwidget(a1, h);
    UNLK;
    return a0;
}
 
/* ttlabel methods */
    
void TTSetFontD_ttlabel(tt_obj o, ttopaque text_len, TT_ARG_DIE TT_ARG_ARRAY((_P(2))) ttfont * text) {
    ttlabel a1;
    LOCK;
    if ((a1 = ID2(ttlabel,o)))
	SetFontD_ttlabel(a1, text_len, text);
    UNLK;
    return ;
}
ttbyte TTSetFont_ttlabel(tt_obj o, TT_ARG_READ TT_ARG_ARRAY_Z ttfont * text) {
    ttlabel a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttlabel,o)))
	a0 = SetFont_ttlabel(a1, text);
    UNLK;
    return a0;
}
ttbyte TTSetText_ttlabel(tt_obj o, TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * text) {
    ttlabel a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttlabel,o)))
	a0 = SetText_ttlabel(a1, text);
    UNLK;
    return a0;
}
tt_obj TTCreate_ttlabel(TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * text) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttlabel(text));
    UNLK;
    return a0;
}
 
/* tttooltip methods */
    
ttbyte TTSetWidget_tttooltip(TT_ARG_DIE tt_obj o, tt_obj w) {
    tttooltip a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(tttooltip,o)))
	a0 = SetWidget_tttooltip(a1, ID2(ttwidget,w));
    UNLK;
    return a0;
}
 
/* ttbuttongroup methods */
    
void TTAdd_ttbuttongroup(tt_obj o, tt_obj r) {
    ttbuttongroup a1;
    LOCK;
    if ((a1 = ID2(ttbuttongroup,o)))
	Add_ttbuttongroup(a1, ID2(ttradiobutton,r));
    UNLK;
    return ;
}    
void TTRemove_ttbuttongroup(tt_obj o, tt_obj r) {
    ttbuttongroup a1;
    LOCK;
    if ((a1 = ID2(ttbuttongroup,o)))
	Remove_ttbuttongroup(a1, ID2(ttradiobutton,r));
    UNLK;
    return ;
}    
void TTSetChecked_ttbuttongroup(tt_obj o, tt_obj r) {
    ttbuttongroup a1;
    LOCK;
    if ((a1 = ID2(ttbuttongroup,o)))
	SetChecked_ttbuttongroup(a1, ID2(ttradiobutton,r));
    UNLK;
    return ;
}
 
/* ttanybutton methods */
        
void TTSetAttrD_ttanybutton(tt_obj o, ttshort w, ttshort h, ttshort pitch, TT_ARG_DIE TT_ARG_ARRAY((_P(3),*,_P(4))) ttattr * text) {
    ttanybutton a1;
    LOCK;
    if ((a1 = ID2(ttanybutton,o)))
	SetAttrD_ttanybutton(a1, w, h, pitch, text);
    UNLK;
    return ;
}
ttbyte TTSetAttr_ttanybutton(tt_obj o, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(3),*,_P(4))) ttattr * text) {
    ttanybutton a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttanybutton,o)))
	a0 = SetAttr_ttanybutton(a1, w, h, pitch, text);
    UNLK;
    return a0;
}
ttbyte TTSetFont_ttanybutton(tt_obj o, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(3),*,_P(4))) ttfont * text) {
    ttanybutton a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttanybutton,o)))
	a0 = SetFont_ttanybutton(a1, w, h, pitch, text);
    UNLK;
    return a0;
}
ttbyte TTSetText_ttanybutton(tt_obj o, ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(3),*,_P(4))) ttbyte * text) {
    ttanybutton a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttanybutton,o)))
	a0 = SetText_ttanybutton(a1, w, h, pitch, text);
    UNLK;
    return a0;
}
tt_obj TTCreate_ttanybutton(ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(2),*,_P(3))) ttbyte * text) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttanybutton(w, h, pitch, text));
    UNLK;
    return a0;
}
 
/* ttbutton methods */
    
void TTSetPressed_ttbutton(tt_obj o, ttbyte pressed) {
    ttbutton a1;
    LOCK;
    if ((a1 = ID2(ttbutton,o)))
	SetPressed_ttbutton(a1, pressed);
    UNLK;
    return ;
}
ttbyte TTIsPressed_ttbutton(TT_ARG_READ tt_obj o) {
    ttbutton a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttbutton,o)))
	a0 = IsPressed_ttbutton(a1);
    UNLK;
    return a0;
}
tt_obj TTCreate_ttbutton(ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(2),*,_P(3))) ttbyte * text) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttbutton(w, h, pitch, text));
    UNLK;
    return a0;
}
 
/* ttcheckbutton methods */
    
void TTSetChecked_ttcheckbutton(tt_obj o, ttbyte checked) {
    ttcheckbutton a1;
    LOCK;
    if ((a1 = ID2(ttcheckbutton,o)))
	SetChecked_ttcheckbutton(a1, checked);
    UNLK;
    return ;
}
ttbyte TTIsChecked_ttcheckbutton(TT_ARG_READ tt_obj o) {
    ttcheckbutton a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttcheckbutton,o)))
	a0 = IsChecked_ttcheckbutton(a1);
    UNLK;
    return a0;
}
tt_obj TTCreate_ttcheckbutton(ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(2),*,_P(3))) ttbyte * text) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttcheckbutton(w, h, pitch, text));
    UNLK;
    return a0;
}
 
/* ttradiobutton methods */

void TTAddToGroup_ttradiobutton(tt_obj o, tt_obj g) {
    ttradiobutton a1;
    LOCK;
    if ((a1 = ID2(ttradiobutton,o)))
	AddToGroup_ttradiobutton(a1, ID2(ttbuttongroup,g));
    UNLK;
    return ;
}
void TTRemoveFromGroup_ttradiobutton(tt_obj o) {
    ttradiobutton a1;
    LOCK;
    if ((a1 = ID2(ttradiobutton,o)))
	RemoveFromGroup_ttradiobutton(a1);
    UNLK;
    return ;
}
tt_obj TTCreate_ttradiobutton(ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(2),*,_P(3))) ttbyte * text) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttradiobutton(w, h, pitch, text));
    UNLK;
    return a0;
}
 
/* ttanyscroll methods */
            
ttbyte TTSetOrientation_ttanyscroll(tt_obj o, ttbyte orientation) {
    ttanyscroll a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttanyscroll,o)))
	a0 = SetOrientation_ttanyscroll(a1, orientation);
    UNLK;
    return a0;
}    
ttbyte TTSetSize_ttanyscroll(tt_obj o, ttint size) {
    ttanyscroll a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttanyscroll,o)))
	a0 = SetSize_ttanyscroll(a1, size);
    UNLK;
    return a0;
}    
ttbyte TTSetRealSize_ttanyscroll(tt_obj o, ttint real_size) {
    ttanyscroll a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttanyscroll,o)))
	a0 = SetRealSize_ttanyscroll(a1, real_size);
    UNLK;
    return a0;
}    
ttbyte TTSetViewSize_ttanyscroll(tt_obj o, ttint view_size) {
    ttanyscroll a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttanyscroll,o)))
	a0 = SetViewSize_ttanyscroll(a1, view_size);
    UNLK;
    return a0;
}    
ttbyte TTSetPosition_ttanyscroll(tt_obj o, ttint position) {
    ttanyscroll a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttanyscroll,o)))
	a0 = SetPosition_ttanyscroll(a1, position);
    UNLK;
    return a0;
}
 
/* ttscrollbar methods */

tt_obj TTCreate1_ttscrollbar(ttbyte orientation) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create1_ttscrollbar(orientation));
    UNLK;
    return a0;
}
tt_obj TTCreate4_ttscrollbar(ttbyte orientation, ttint size, ttint real_size, ttint view_size) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create4_ttscrollbar(orientation, size, real_size, view_size));
    UNLK;
    return a0;
}
 
/* ttslider methods */
    
ttbyte TTSetSlideMin_ttslider(tt_obj o, ttint slide_min) {
    ttslider a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttslider,o)))
	a0 = SetSlideMin_ttslider(a1, slide_min);
    UNLK;
    return a0;
}    
ttbyte TTSetSlideMax_ttslider(tt_obj o, ttint slide_max) {
    ttslider a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttslider,o)))
	a0 = SetSlideMax_ttslider(a1, slide_max);
    UNLK;
    return a0;
}    
ttbyte TTSetSlideValue_ttslider(tt_obj o, ttint slide_value) {
    ttslider a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttslider,o)))
	a0 = SetSlideValue_ttslider(a1, slide_value);
    UNLK;
    return a0;
}
tt_obj TTCreate_ttslider(ttbyte orientation, ttint real_size) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttslider(orientation, real_size));
    UNLK;
    return a0;
}
 
/* ttprogressbar methods */

tt_obj TTCreate_ttprogressbar(ttbyte orientation) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttprogressbar(orientation));
    UNLK;
    return a0;
}
 
/* ttscrollpane methods */

tt_obj TTCreate_ttscrollpane(ttshort w, ttshort h) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttscrollpane(w, h));
    UNLK;
    return a0;
}    
ttbyte TTSetBarX_ttscrollpane(tt_obj o, tt_obj bar_x) {
    ttscrollpane a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttscrollpane,o)))
	a0 = SetBarX_ttscrollpane(a1, ID2(ttscrollbar,bar_x));
    UNLK;
    return a0;
}    
ttbyte TTSetBarY_ttscrollpane(tt_obj o, tt_obj bar_y) {
    ttscrollpane a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttscrollpane,o)))
	a0 = SetBarY_ttscrollpane(a1, ID2(ttscrollbar,bar_y));
    UNLK;
    return a0;
}
 
/* ttwindow methods */
    
ttbyte TTSetTitle_ttwindow(tt_obj o, TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * title) {
    ttwindow a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttwindow,o)))
	a0 = SetTitle_ttwindow(a1, title);
    UNLK;
    return a0;
}
 
/* ttframe methods */
    
ttbyte TTSetMenubar_ttframe(tt_obj o, TT_ARG_READ tt_obj m) {
    ttframe a1;
    ttbyte a0 = (ttbyte)0;
    LOCK;
    if ((a1 = ID2(ttframe,o)))
	a0 = SetMenubar_ttframe(a1, ID2(ttmenubar,m));
    UNLK;
    return a0;
}
tt_obj TTCreate_ttframe(TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * title) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttframe(title));
    UNLK;
    return a0;
}
 
/* ttscroller methods */

tt_obj TTCreate_ttscroller(ttshort w, ttshort h) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttscroller(w, h));
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

tt_obj TTCreate_ttapplication(TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * name) {
    tt_obj a0 = (tt_obj)(opaque)0;
    LOCK;
    a0 = (tt_obj)OBJ2ID(Create_ttapplication(name));
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








