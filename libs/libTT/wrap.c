
/*
 *  wrap.c  --  wrappers around exported libTT methods
 */

#ifdef CONF_SOCKET_PTHREADS
# include <pthread.h>
#endif

#include "TT.h"

#include "mutex.h"

#include "TTextern.h"
#include "TTassert.h"

#include "utils.h"
#include "inlines.h"

#ifdef CONF_SOCKET_PTHREADS
TH_R_MUTEX_HELPER_DEFS(static);
#endif


/* ttobj */
TT_INLINE void GetValue_ttobj(ttobj o) {
    if (o)
	o->refcount++;
}
TT_INLINE void Ref_ttobj(ttobj o) {
    if (o)
	o->refcount++;
}

TT_INLINE void Unref_ttobj(ttobj o) {
    if (o && !--o->refcount)
	Del(o);
}


/* ttvisible */
TT_INLINE void Add_ttvisible(ttvisible o, ttvisible child) {
    if (o && child && TTAssert(IS(ttvisible,o)) && TTAssert(IS(ttvisible,child)))
	child->FN->AddTo(child, o);
}
TT_INLINE void SetRepaint_ttvisible(ttvisible o, ttvisible_repaint_fn repaint) {
    if (o && TTAssert(IS(ttvisible,o)) && o->repaint != repaint) {
	o->repaint = repaint;
	if (o->vflags & ttvisible_vflags_visible)
	    Expose_ttvisible(o, ttvisible_repaint_args_WHOLE);
    }
}


/* ttwidget */
TT_INLINE void SetXY_ttwidget(ttwidget o, ttshort x, ttshort y) {
    if (o && TTAssert(IS(ttwidget,o)))
	o->FN->SetXYWH(o, 0x03, x, y, 0, 0);
}
TT_INLINE void SetWH_ttwidget(ttwidget o, ttshort w, ttshort h) {
    if (o && TTAssert(IS(ttwidget,o)))
	o->FN->SetXYWH(o, 0x0c, 0, 0, w, h);
}
TT_INLINE void SetXl_ttwidget(ttwidget o, ttint xl) {
    if (o && TTAssert(IS(ttwidget,o)))
	o->FN->SetXlYl(o, 0x01, xl, 0);
}
TT_INLINE void SetYl_ttwidget(ttwidget o, ttint yl) {
    if (o && TTAssert(IS(ttwidget,o)))
	o->FN->SetXlYl(o, 0x02, 0, yl);
}


/* ttbutton */
TT_INLINE byte IsPressed_ttbutton(ttbutton o) {
    if (o && TTAssert(IS(ttbutton,o)))
	return !!(o->vflags & ttanybutton_vflags_pressed);
    return FALSE;
}


/* ttapplication */
TT_INLINE ttapplication Set_ttapplication(TT_CONST byte * name) {
    byte *apname;
    
    if (!name)
	return (ttapplication)0;
    
    if (TTD.Application || (TTD.Application = TFN_ttapplication->New(TFN_ttapplication, NULL))) {

	if ((apname = TTCloneStr(name))) {
	    if (TTD.Application->name)
		TTFreeMem(TTD.Application->name);
	    TTD.Application->name = apname;

	    return (ttapplication)Build((ttobj)TTD.Application);
	}
	TDEL(TTD.Application);
    }
    return TTD.Application;
}



#include "wrap_m4.h"



