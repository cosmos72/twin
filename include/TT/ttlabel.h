




/* This file was automatically generated from m4/tt.m4hh, do not edit! */


/*
 *  ttlabel.h  --  public header for libTT declarations related to "ttlabel"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



#ifndef _TT_TTLABEL_H
#define _TT_TTLABEL_H







/* object structure */
#ifndef s_ttlabel


/** ttlabel: A simple text label */        
        struct s_ttlabel;
typedef struct s_ttlabel *	ttlabel;

#endif /* s_ttlabel */



/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttlabel */
extern ttclass	TTClass_ttlabel;





/* ttlabel methods */

/** set the text of the ttlabel `o' and swallow `text' */    
void TTSetFontD_ttlabel(ttlabel o,ttopaque text_len,TT_ARG_DIE TT_ARG_ARRAY((_P(2))) ttfont * text);

/** set the text of the ttlabel `o' to a copy of `text' */
ttbyte TTSetFont_ttlabel(ttlabel o,TT_ARG_READ TT_ARG_ARRAY_Z ttfont * text);

/** set the text of the ttlabel `o' to a copy of `text' */
ttbyte TTSetText_ttlabel(ttlabel o,TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * text);

/** create a ttlabel with a copy of given text */
ttlabel TTCreate_ttlabel(TT_ARG_READ TT_ARG_ARRAY_Z ttbyte * text);









/* TTGet*_ttlabel and TTSet*_ttlabel methods */
    
/** get `text_len' of ttlabel `o' */
ttopaque 	TTGetTextLen_ttlabel(TT_ARG_READ ttlabel o);
    
/** get `text' of ttlabel `o' */
TT_ARG_ARRAY((_F(text_len))) ttfont * 	TTGetText_ttlabel(TT_ARG_READ ttlabel o);










#endif /* _TT_TTLABEL_H */

