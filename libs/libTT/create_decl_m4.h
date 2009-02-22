



/* This file was automatically generated from m4/create_decl.m4, do not edit! */

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
 *  create_decl_m4.h  --  prototype for libTT internal Create* function pointers
 *
 */











                                                
    ttvector (*Create1_ttvector)(ttany value);
    ttvector (*CreateA_ttvector)(ttopaque value_n, TT_ARG_READ TT_ARG_ARRAY((_P(1))) ttarg * values);
    ttvector (*CreateL_ttvector)(ttopaque value_n, ... );
    ttvector (*CreateR_ttvector)(TT_ARG_READ ttvector v);
    ttvector (*CreateV_ttvector)(ttopaque value_n, TT_ARG_ARRAY((1)) va_list * values);
    ttvector (*CreateY_ttvector)(ttopaque value_n, TT_ARG_READ TT_ARG_ARRAY((_P(1))) ttany * values);               
    ttevent (*Create_ttevent)(ttuint evtype, ttuint evcode, ttuint evflags); 
    tteventbig (*Create8_tteventbig)(ttuint evtype, ttuint evcode, ttuint evflags, ttshort x, ttshort y, ttshort w, ttshort h, ttany value, ttany old_value, ttopaque data_len, TT_ARG_READ TT_ARG_ARRAY((_P(10))) ttbyte * data);
    tteventbig (*Create4_tteventbig)(ttuint evtype, ttuint evcode, ttuint evflags, ttshort x, ttshort y, ttshort w, ttshort h);
    tteventbig (*Create4s4_tteventbig)(ttuint evtype, ttuint evcode, ttuint evflags, ttany value, ttany old_value, ttopaque data_len, TT_ARG_READ TT_ARG_ARRAY((_P(6))) ttbyte * data); 
    ttbitmask (*Create_ttbitmask)(ttany value);
    ttbitmask (*CreateB2_ttbitmask)(TT_ARG_READ ttbitmask o);
    ttbitmask (*CreateR_ttbitmask)(TT_ARG_DIE ttvector value);
    ttbitmask (*CreateR2_ttbitmask)(TT_ARG_READ ttvector value); 
    tteventmask (*CreateB_tteventmask)(TT_ARG_DIE ttbitmask evtype_mask, TT_ARG_DIE ttbitmask evcode_mask, TT_ARG_DIE ttbitmask component_mask);
    tteventmask (*CreateB2_tteventmask)(TT_ARG_READ ttbitmask evtype_mask, TT_ARG_READ ttbitmask evcode_mask, TT_ARG_READ ttbitmask component_mask);      
    ttlistener (*Create_ttlistener)(ttcomponent o, ttuint evtype, ttuint lflags, TT_ARG_READ ttlistener_fn function, ttany arg);
    ttlistener (*CreateE_ttlistener)(ttcomponent o, TT_ARG_DIE ttevent ev, ttuint lflags, TT_ARG_READ ttlistener_fn function, ttany arg);
    ttlistener (*CreateR_ttlistener)(ttcomponent o, TT_ARG_DIE ttevent ev, ttuint lflags, TT_ARG_READ ttlistener_fn function, ttopaque arg_component_n, ttopaque arg_event_n, TT_ARG_DIE ttvector args);
    ttlistener (*CreateDel_ttlistener)(ttcomponent o, ttuint lflags, TT_ARG_READ ttlistener_fn function);
    ttlistener (*CreateAskclose_ttlistener)(ttcomponent o, ttuint lflags, TT_ARG_READ ttlistener_fn function);
    ttlistener (*CreateKey_ttlistener)(ttcomponent o, ttuint evcode, ttuint evflags, ttuint lflags, TT_ARG_READ ttlistener_fn function);
    ttlistener (*CreateMouse_ttlistener)(ttcomponent o, ttuint evcode, ttuint evflags, ttuint lflags, TT_ARG_READ ttlistener_fn function);
    ttlistener (*CreateActivate_ttlistener)(ttcomponent o, ttuint lflags, TT_ARG_READ ttlistener_fn function);
    ttlistener (*CreateChange_ttlistener)(ttcomponent o, ttuint evcode, ttuint lflags, TT_ARG_READ ttlistener_fn function);
    ttlistener (*CreateKeyData_ttlistener)(ttcomponent o, TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * key, ttuint lflags, TT_ARG_READ ttlistener_fn function);
    ttlistener (*CreateExpose_ttlistener)(ttcomponent o, ttuint lflags, TT_ARG_READ ttlistener_fn function);         
    tttimer (*CreateR_tttimer)(ttcomponent r, ttuint lflags, TT_ARG_READ ttlistener_fn function, ttopaque arg_component_n, ttopaque arg_timer_n, TT_ARG_DIE ttvector args, ttany delay_t, ttany delay_f);
    tttimer (*Create_tttimer)(ttcomponent r, ttuint lflags, TT_ARG_READ ttlistener_fn function, ttany arg, ttany delay_t, ttany delay_f);                                                   
          
    ttboxlayout (*Create_ttboxlayout)(ttany orientation);              
    ttnative (*Create_ttnative)(ttany native_value);                                      
    ttlabel (*Create_ttlabel)(TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * text);                           
    ttanybutton (*Create_ttanybutton)(ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(2),*,_P(3))) ttbyte * text);     
    ttbutton (*Create_ttbutton)(ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(2),*,_P(3))) ttbyte * text);     
    ttcheckbutton (*Create_ttcheckbutton)(ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(2),*,_P(3))) ttbyte * text); 
    ttradiobutton (*Create_ttradiobutton)(ttshort w, ttshort h, ttshort pitch, TT_ARG_READ TT_ARG_ARRAY((_P(2),*,_P(3))) ttbyte * text);                              
    ttscrollbar (*Create1_ttscrollbar)(ttbyte orientation);
    ttscrollbar (*Create4_ttscrollbar)(ttbyte orientation, ttint size, ttint real_size, ttint view_size);             
    ttslider (*Create_ttslider)(ttbyte orientation, ttint real_size); 
    ttprogressbar (*Create_ttprogressbar)(ttbyte orientation); 
    ttscrollpane (*Create_ttscrollpane)(ttshort w, ttshort h);                  
    ttframe (*Create_ttframe)(TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * title); 
    ttscroller (*Create_ttscroller)(ttshort w, ttshort h);          
    ttapplication (*Create_ttapplication)(TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * name);




