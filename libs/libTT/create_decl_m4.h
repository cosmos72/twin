



/* This file was automatically generated from m4/exports_init.m4, do not edit! */


/*
 *  exports_init_m4.h  --  exported wrappers around libTT methods
 *
 */

















                                                      
    ttevent (*Create_ttevent)(ttuint, ttuint, ttuint); 
    
    tteventbig (*Create_tteventbig)(ttuint, ttuint, ttuint, ttshort, ttshort, ttshort, ttshort, ttuint, TT_CONST ttbyte *); 
         
    ttcallback (*Create_ttcallback)(ttcomponent, ttuint, ttuint, ttcallback_fn, ttany);    
    ttcallback (*CreateA_ttcallback)(ttcomponent, ttevent, ttuint, ttuint, ttuint, ttuint, ttcallback_fn, ttany TT_CONST *);    
    ttcallback (*CreateV_ttcallback)(ttcomponent, ttevent, ttuint, ttuint, ttuint, ttuint, ttcallback_fn, va_list *);                                           
    ttnative (*Create_ttnative)(ttany);                                  
    ttlabel (*Create_ttlabel)(TT_CONST ttbyte *);         
    ttanybutton (*Create_ttanybutton)(TT_CONST ttfont *, ttshort, ttshort, ttshort);         
    ttbutton (*Create_ttbutton)(TT_CONST ttfont *, ttshort, ttshort, ttshort);                                     




