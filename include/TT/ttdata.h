




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttdata.h  --  public header for libTT declarations related to "ttdata"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTDATA_H
#define _TT_TTDATA_H







/* object structure */
#ifndef s_ttdata


/**
 * ttdata: wrapper for user-defined data in ttcomponents;
 * this class is derived from ttcomponent to allow creating listeners on it,
 * but this implementation has the undesired side effect that you can
 * TT{Set,Get,Unset}KeyData_ttcomponent also on ttdatas themselves.
 */                    
        struct s_ttdata;
typedef struct s_ttdata *	ttdata;

#endif /* s_ttdata */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttdata */
extern ttclass	TTClass_ttdata;





/* ttdata methods */
        
/** set the data in ttdata `o' */
ttbyte TTSetData_ttdata(ttdata o,ttany data);









/* TTGet*_ttdata and TTSet*_ttdata methods */
        
/** get `component' of ttdata `o' */
ttcomponent 	TTGetComponent_ttdata(TT_ARG_READ ttdata o);
    
/** get `key_len' of ttdata `o' */
ttopaque 	TTGetKeyLen_ttdata(TT_ARG_READ ttdata o);
    
/** get `key' of ttdata `o' */
TT_ARG_ARRAY((_F(key_len))) ttbyte * 	TTGetKey_ttdata(TT_ARG_READ ttdata o);
    
/** get `data' of ttdata `o' */
ttany 	TTGetData_ttdata(TT_ARG_READ ttdata o);

/** set `data' of ttdata `o' */
ttbyte 	TTSetData_ttdata(ttdata o, ttany data);










#endif /* _TT_TTDATA_H */

