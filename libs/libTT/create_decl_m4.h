



/* This file was automatically generated from m4/create_decl.m4, do not edit! */


/*
 *  create_decl_m4.h  --  prototype for libTT internal Create* function pointers
 *
 */























                                  
    ttevent (*Create_ttevent)(ttuint, ttuint, ttuint); 

    tteventbig (*Create8_tteventbig)(ttuint, ttuint, ttuint, ttshort, ttshort, ttshort, ttshort, ttany, ttany, ttuint, TT_CONST ttbyte *);
    tteventbig (*Create4_tteventbig)(ttuint, ttuint, ttuint, ttshort, ttshort, ttshort, ttshort);
    tteventbig (*Create4s4_tteventbig)(ttuint, ttuint, ttuint, ttany, ttany, ttuint, TT_CONST ttbyte *); 
    ttbitmask (*Create_ttbitmask)(ttany);
    ttbitmask (*CreateA_ttbitmask)(ttopaque, TT_ARG_READ ttany *); 
    tteventmask (*Create_tteventmask)(ttbitmask, ttbitmask, ttbitmask);      
    ttlistener (*Create_ttlistener)(ttcomponent, ttuint, ttuint, ttlistener_fn, ttany);
    ttlistener (*CreateA_ttlistener)(ttcomponent, ttevent, ttuint, ttuint, ttuint, ttuint, ttlistener_fn, TT_ARG_READ ttany *);
    ttlistener (*CreateV_ttlistener)(ttcomponent, ttevent, ttuint, ttuint, ttuint, ttuint, ttlistener_fn, va_list *);

    ttlistener (*CreateDel_ttlistener)(ttcomponent, ttuint, ttlistener_fn);
    ttlistener (*CreateAskclose_ttlistener)(ttcomponent, ttuint, ttlistener_fn);
    ttlistener (*CreateKey_ttlistener)(ttcomponent, ttuint, ttuint, ttuint, ttlistener_fn);
    ttlistener (*CreateMouse_ttlistener)(ttcomponent, ttuint, ttuint, ttuint, ttlistener_fn);
    ttlistener (*CreateActivate_ttlistener)(ttcomponent, ttuint, ttlistener_fn);
    ttlistener (*CreateChange_ttlistener)(ttcomponent, ttuint, ttuint, ttlistener_fn);
    ttlistener (*CreateKeyData_ttlistener)(ttcomponent, TT_ARG_READ ttbyte *, ttuint, ttlistener_fn);
    ttlistener (*CreateExpose_ttlistener)(ttcomponent, ttuint, ttlistener_fn);
         
    tttimer (*CreateA_tttimer)(ttcomponent, ttuint, ttuint, ttuint, ttuint, ttlistener_fn, TT_ARG_READ ttany *, ttuint, ttuint);
    tttimer (*Create_tttimer)(ttcomponent, ttuint, ttlistener_fn, ttany, ttuint, ttuint);                                                            
    ttnative (*Create_ttnative)(ttany);                                  
    ttlabel (*Create_ttlabel)(TT_ARG_READ ttbyte *);                           
    ttanybutton (*Create_ttanybutton)(TT_ARG_READ ttbyte *, ttshort, ttshort, ttshort);     
    ttbutton (*Create_ttbutton)(TT_ARG_READ ttbyte *, ttshort, ttshort, ttshort);     
    ttcheckbutton (*Create_ttcheckbutton)(TT_ARG_READ ttbyte *, ttshort, ttshort, ttshort); 
    ttradiobutton (*Create_ttradiobutton)(TT_ARG_READ ttbyte *, ttshort, ttshort, ttshort);                              
    ttscrollbar (*Create1_ttscrollbar)(ttbyte);
    ttscrollbar (*Create4_ttscrollbar)(ttbyte, ttint, ttint, ttint);             
    ttslider (*Create_ttslider)(ttbyte, ttint); 
    ttprogressbar (*Create_ttprogressbar)(ttbyte); 
    ttscrollpane (*Create_ttscrollpane)(ttshort, ttshort);                  
    ttframe (*Create_ttframe)(TT_ARG_READ ttbyte *); 
    ttscroller (*Create_ttscroller)(ttshort, ttshort);          




