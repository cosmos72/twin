
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

