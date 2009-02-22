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

#ifndef _TT_THEME_H
#define _TT_THEME_H

#define myTheme(o) ((o)->theme ? (o)->theme : GetDefault_tttheme())


TT_INLINE tobj _myMenubar_id(ttmenubar m) {
    return (m || (m = GetDefault_ttmenubar())) ? m->id : 0;
}
TT_INLINE ttuint _myMenubar_n(ttmenubar m) {
    return (m || (m = GetDefault_ttmenubar())) ? m->native : 0;
}

#define myMenubar(o) ((o)->menubar ? (o)->menubar : GetDefault_ttmenubar())
#define myMenubar_id(o) _myMenubar_id((o)->menubar)
#define myMenubar_n(o) _myMenubar_n((o)->menubar)



#endif /* _TT_THEME_H */

