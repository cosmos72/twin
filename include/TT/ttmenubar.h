

/* This file was automatically generated from m4/tt.m4hh, do not edit! */

/*
 *  ttmenubar.h  --  public header for libTT declarations related to "ttmenubar"
 *
 *  Copyright (C) 2001-2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef TT_TTMENUBAR_H
#define TT_TTMENUBAR_H

/* object structure */
#ifndef s_ttmenubar

/** ttmenubar: a complete menu bar */
struct s_ttmenubar;
typedef struct s_ttmenubar *ttmenubar;

#endif /* s_ttmenubar */

/* global runtime class type; returned by TTCLASSOF(<some object>) */
/** class of ttmenubar */
extern ttclass TTClass_ttmenubar;

/* ttmenubar methods */

/** get the default ttmenubar used by ttwidget:s */
ttmenubar TTGetDefault_ttmenubar(void);

/* TTGet*_ttmenubar and TTSet*_ttmenubar methods */

#endif /* TT_TTMENUBAR_H */
