




/* This file was automatically generated from hw_gtk_c by m4/hw.m4h, do not edit! */


/*
 *  hw_gtk_m4.c  --  implementation of libTT methods on "gtk" target display
 *
 *  Copyright (C) 2001,2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */





























#include <gtk/gtk.h>



/* order is crucial here! */

#ifdef CONF_SOCKET_PTHREADS
# include <pthread.h>
#endif

/* include our internal copy of TT.h instead of the  one */
#include "TT.h"
#include <TT/TTerrno.h>

#include "mutex.h"

#include "TTextern.h"
#include "TTassert.h"

#include "utils.h"
#include "inlines.h"
#include "seterrno.h"
#include "theme.h"

static s_ttfns gtk_TTFNs;



/*
 * We need to write down only overloaded methods.
 */
#define TT_GTK_EUNFINISHED 1
 


/* ttobj */
static void gtk_Break_ttobj(ttobj o) {
    if (o) {
	if (o->native) {
	    gtk_object_destroy((GtkObject *)o->native);
	    o->native = (opaque)0;
	}
    }
}


/* ttwidget */
static ttwidget gtk_Build_ttwidget(ttwidget o) {
    if ((o->native = (opaque)gtk_fixed_new())) {
	gtk_widget_realize((GtkWidget *)o->native);
	return o;
    }
    return NULL;
}
static void gtk_AddTo_ttwidget(ttwidget o, ttvisible parent) {
    if (parent && !o->parent) {
	/* prepend to list, cannot use TFNdefault(ttwidget)->AddTo() */
	if ((o->next = parent->child_first))
	    parent->child_first->prev = (ttvisible)o;
	else
	    parent->child_last = (ttvisible)o;
	parent->child_first = (ttvisible)o;
	o->prev = NULL;
	o->parent = parent;

	gtk_widget_reparent((GtkWidget *)o->native, (GtkWidget *)parent->native);
	
	if (o->vflags & ttvisible_vflags_visible)
	    gtk_widget_show((GtkWidget *)o->native);
    }
}
static void gtk_SetVisible_ttwidget(ttwidget o, ttbyte on_off) {
    if (!on_off != !(o->vflags & ttvisible_vflags_visible)) {
	o->vflags ^= ttvisible_vflags_visible;
	if (o->parent) {
	    if (on_off)
		gtk_widget_show((GtkWidget *)o->native);
	    else
		gtk_widget_hide((GtkWidget *)o->native);
	}
    }
}
static void gtk_Remove_ttwidget(ttwidget o) {
    if (o->parent) {
	TFNdefault(ttwidget)->Remove(o);
	
	gtk_widget_reparent((GtkWidget *)o->native, (GtkWidget *)0);
    }
}


static ttbyte gtk_Sync(void) {
    return 1;
}
static ttbyte gtk_Flush(void) {
    return 1;
}
static ttbyte gtk_TimidFlush(void) {
    return 1;
}
#if 0 /* not used */
static ttbyte gtk_MainLoop(void) {
    gtk_main();
    return 1;
}
static void gtk_ExitMainLoop(void) {
    gtk_main_quit();
}
#endif
static ttbyte gtk_MainLoopOnce(ttbyte wait) {
    /* FIXME: this is almost surely wrong */
    gtk_main_quit();
    gtk_main();
    return 1;
}
static void gtk_DeleteCallback(ttcallback o) {
}
static void gtk_Close(void) {
}
static int gtk_ConnectionFd(void) {
  return -1;
}
static ttuint gtk_GetErrno(void) {
  return TT_GTK_EUNFINISHED;
}
static ttuint gtk_GetErrnoDetail(void) {
  return 0;
}
static TT_CONST ttbyte *gtk_StrError(ttuint E) {
    switch (E) {
      case TT_GTK_EUNFINISHED:
	return "gtk support is not finished yet :(";
    }
    return "";
}
static TT_CONST ttbyte *gtk_StrErrorDetail(ttuint E, ttuint S) {
    return "";
}


#ifdef THIS_MODULE
ttfns InitModule(tthw *HW)
#else
ttfns _TT_gtk_InitHW(tthw *HW)
#endif
{
    /*
     * (*HW) must be set even if initialization fails,
     * to report error messages.
     */
    *HW = &gtk_TTFNs.HW;
    
    FAIL(TT_GTK_EUNFINISHED, 0); /* it's unfinished... */
    
    return (ttfns)0;
}




                                      


/*
 * use default values for methods not implemented in hw_gtk_c
 *
 * null display target MUST IMPLEMENT ALL METHODS, even if as stubs.
 */



static s_ttfns gtk_TTFNs = {


  {
    order_ttobj,
    magicmask_ttobj,
    magic_ttobj,
    sizeof(s_ttobj),
    (void *)NULL /* UNDEFINED: New_ttobj  */,
    (void *)NULL /* UNDEFINED: Build_ttobj  */,
    gtk_Break_ttobj,
    (void *)NULL /* UNDEFINED: Del_ttobj  */,

  },
  
  {
    order_ttevent,
    magicmask_ttevent,
    magic_ttevent,
    sizeof(s_ttevent),
    (void *)NULL /* UNDEFINED: New_ttevent  */,
    (void *)NULL /* UNDEFINED: Build_ttevent  */,
    (void *)NULL /* inherits:  Break_ttevent -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttevent  */,

    /* extends ttfn_ttobj */

  },
 
  {
    order_tteventbig,
    magicmask_tteventbig,
    magic_tteventbig,
    sizeof(s_tteventbig),
    (void *)NULL /* UNDEFINED: New_tteventbig  */,
    (void *)NULL /* UNDEFINED: Build_tteventbig  */,
    (void *)NULL /* inherits:  Break_tteventbig -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_tteventbig  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttevent */


  },
 
  {
    order_ttbitmask,
    magicmask_ttbitmask,
    magic_ttbitmask,
    sizeof(s_ttbitmask),
    (void *)NULL /* UNDEFINED: New_ttbitmask  */,
    (void *)NULL /* UNDEFINED: Build_ttbitmask  */,
    (void *)NULL /* inherits:  Break_ttbitmask -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttbitmask  */,

    /* extends ttfn_ttobj */

  },
 
  {
    order_tteventmask,
    magicmask_tteventmask,
    magic_tteventmask,
    sizeof(s_tteventmask),
    (void *)NULL /* UNDEFINED: New_tteventmask  */,
    (void *)NULL /* UNDEFINED: Build_tteventmask  */,
    (void *)NULL /* inherits:  Break_tteventmask -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_tteventmask  */,

    /* extends ttfn_ttobj */

  },
 
  {
    order_ttcallback,
    magicmask_ttcallback,
    magic_ttcallback,
    sizeof(s_ttcallback),
    (void *)NULL /* UNDEFINED: New_ttcallback  */,
    (void *)NULL /* UNDEFINED: Build_ttcallback  */,
    (void *)NULL /* inherits:  Break_ttcallback -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttcallback  */,

    /* extends ttfn_ttobj */
    (void *)NULL /* UNDEFINED: Create_ttcallback  */,

  },
 
  {
    order_ttlistener,
    magicmask_ttlistener,
    magic_ttlistener,
    sizeof(s_ttlistener),
    (void *)NULL /* UNDEFINED: New_ttlistener  */,
    (void *)NULL /* UNDEFINED: Build_ttlistener  */,
    (void *)NULL /* inherits:  Break_ttlistener -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttlistener  */,

    /* extends ttfn_ttobj */
    (void *)NULL /* UNDEFINED: Create_ttlistener  */,

    /* extends ttfn_ttcallback */


    (void *)NULL /* UNDEFINED: AddTo_ttlistener  */,
    (void *)NULL /* UNDEFINED: Remove_ttlistener  */,

  },
 
  {
    order_tttimer,
    magicmask_tttimer,
    magic_tttimer,
    sizeof(s_tttimer),
    (void *)NULL /* UNDEFINED: New_tttimer  */,
    (void *)NULL /* UNDEFINED: Build_tttimer  */,
    (void *)NULL /* inherits:  Break_tttimer -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_tttimer  */,

    /* extends ttfn_ttobj */
    (void *)NULL /* UNDEFINED: Create_tttimer  */,

    /* extends ttfn_ttcallback */


    (void *)NULL /* UNDEFINED: AddTo_tttimer  */,
    (void *)NULL /* UNDEFINED: Remove_tttimer  */,

    /* extends ttfn_ttlistener */
    (void *)NULL /* UNDEFINED: SetEnabled_tttimer  */,
    (void *)NULL /* UNDEFINED: IsEnabled_tttimer  */,

  },
 
  {
    order_ttcomponent,
    magicmask_ttcomponent,
    magic_ttcomponent,
    sizeof(s_ttcomponent),
    (void *)NULL /* UNDEFINED: New_ttcomponent  */,
    (void *)NULL /* UNDEFINED: Build_ttcomponent  */,
    (void *)NULL /* inherits:  Break_ttcomponent -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttcomponent  */,

    /* extends ttfn_ttobj */

  },
 
  {
    order_ttdata,
    magicmask_ttdata,
    magic_ttdata,
    sizeof(s_ttdata),
    (void *)NULL /* UNDEFINED: New_ttdata  */,
    (void *)NULL /* UNDEFINED: Build_ttdata  */,
    (void *)NULL /* inherits:  Break_ttdata -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttdata  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttdata  */,
    (void *)NULL /* UNDEFINED: Remove_ttdata  */,

  },
 
  {
    order_ttvisible,
    magicmask_ttvisible,
    magic_ttvisible,
    sizeof(s_ttvisible),
    (void *)NULL /* UNDEFINED: New_ttvisible  */,
    (void *)NULL /* UNDEFINED: Build_ttvisible  */,
    (void *)NULL /* inherits:  Break_ttvisible -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttvisible  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttvisible  */,
    (void *)NULL /* UNDEFINED: Remove_ttvisible  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttvisible  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttvisible  */,
    (void *)NULL /* UNDEFINED: SetTheme_ttvisible  */,
    (void *)NULL /* UNDEFINED: Draw_ttvisible  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttvisible  */,

  },
 
  {
    order_ttnative,
    magicmask_ttnative,
    magic_ttnative,
    sizeof(s_ttnative),
    (void *)NULL /* UNDEFINED: New_ttnative  */,
    (void *)NULL /* UNDEFINED: Build_ttnative  */,
    (void *)NULL /* inherits:  Break_ttnative -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttnative  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttnative  */,
    (void *)NULL /* UNDEFINED: Remove_ttnative  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttnative  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttnative  */,
    (void *)NULL /* UNDEFINED: SetTheme_ttnative  */,
    (void *)NULL /* UNDEFINED: Draw_ttnative  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttnative  */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* UNDEFINED: GetRoot_ttnative  */,
    (void *)NULL /* UNDEFINED: GetW_ttnative  */,
    (void *)NULL /* UNDEFINED: GetH_ttnative  */,

  },
 
  {
    order_ttwidget,
    magicmask_ttwidget,
    magic_ttwidget,
    sizeof(s_ttwidget),
    (void *)NULL /* UNDEFINED: New_ttwidget  */,
    gtk_Build_ttwidget,
    (void *)NULL /* inherits:  Break_ttwidget -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttwidget  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    gtk_AddTo_ttwidget,
    gtk_Remove_ttwidget,
    (void *)NULL /* UNDEFINED: Invalidate_ttwidget  */,
    gtk_SetVisible_ttwidget,
    (void *)NULL /* UNDEFINED: SetTheme_ttwidget  */,
    (void *)NULL /* UNDEFINED: Draw_ttwidget  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttwidget  */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* UNDEFINED: SetXY_ttwidget  */,
    (void *)NULL /* UNDEFINED: SetWH_ttwidget  */,
    (void *)NULL /* UNDEFINED: SetXl_ttwidget  */,
    (void *)NULL /* UNDEFINED: SetYl_ttwidget  */,
    (void *)NULL /* UNDEFINED: SetWl_ttwidget  */,
    (void *)NULL /* UNDEFINED: SetHl_ttwidget  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttwidget  */,

  },
 
  {
    order_ttlabel,
    magicmask_ttlabel,
    magic_ttlabel,
    sizeof(s_ttlabel),
    (void *)NULL /* UNDEFINED: New_ttlabel  */,
    (void *)NULL /* inherits:  Build_ttlabel -> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttlabel -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttlabel  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttlabel -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttlabel -> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttlabel  */,
    (void *)NULL /* inherits:  SetVisible_ttlabel -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttlabel  */,
    (void *)NULL /* UNDEFINED: Draw_ttlabel  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttlabel  */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* UNDEFINED: SetXY_ttlabel  */,
    (void *)NULL /* UNDEFINED: SetWH_ttlabel  */,
    (void *)NULL /* UNDEFINED: SetXl_ttlabel  */,
    (void *)NULL /* UNDEFINED: SetYl_ttlabel  */,
    (void *)NULL /* UNDEFINED: SetWl_ttlabel  */,
    (void *)NULL /* UNDEFINED: SetHl_ttlabel  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttlabel  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: DoSetFont_ttlabel  */,

  },
 
  {
    order_tttooltip,
    magicmask_tttooltip,
    magic_tttooltip,
    sizeof(s_tttooltip),
    (void *)NULL /* UNDEFINED: New_tttooltip  */,
    (void *)NULL /* inherits:  Build_tttooltip -> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_tttooltip -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_tttooltip  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_tttooltip -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_tttooltip -> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_tttooltip  */,
    (void *)NULL /* inherits:  SetVisible_tttooltip -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_tttooltip  */,
    (void *)NULL /* UNDEFINED: Draw_tttooltip  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_tttooltip  */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* UNDEFINED: SetXY_tttooltip  */,
    (void *)NULL /* UNDEFINED: SetWH_tttooltip  */,
    (void *)NULL /* UNDEFINED: SetXl_tttooltip  */,
    (void *)NULL /* UNDEFINED: SetYl_tttooltip  */,
    (void *)NULL /* UNDEFINED: SetWl_tttooltip  */,
    (void *)NULL /* UNDEFINED: SetHl_tttooltip  */,
    (void *)NULL /* UNDEFINED: SetTooltip_tttooltip  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: DoSetFont_tttooltip  */,

    /* extends ttfn_ttlabel */
    (void *)NULL /* UNDEFINED: SetWidget_tttooltip  */,

  },
 
  {
    order_ttbuttongroup,
    magicmask_ttbuttongroup,
    magic_ttbuttongroup,
    sizeof(s_ttbuttongroup),
    (void *)NULL /* UNDEFINED: New_ttbuttongroup  */,
    (void *)NULL /* UNDEFINED: Build_ttbuttongroup  */,
    (void *)NULL /* inherits:  Break_ttbuttongroup -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttbuttongroup  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: Add_ttbuttongroup  */,
    (void *)NULL /* UNDEFINED: Remove_ttbuttongroup  */,
    (void *)NULL /* UNDEFINED: SetChecked_ttbuttongroup  */,

  },
 
  {
    order_ttanybutton,
    magicmask_ttanybutton,
    magic_ttanybutton,
    sizeof(s_ttanybutton),
    (void *)NULL /* UNDEFINED: New_ttanybutton  */,
    (void *)NULL /* inherits:  Build_ttanybutton -> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttanybutton -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttanybutton  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttanybutton -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttanybutton -> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttanybutton  */,
    (void *)NULL /* inherits:  SetVisible_ttanybutton -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttanybutton  */,
    (void *)NULL /* UNDEFINED: Draw_ttanybutton  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttanybutton  */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* UNDEFINED: SetXY_ttanybutton  */,
    (void *)NULL /* UNDEFINED: SetWH_ttanybutton  */,
    (void *)NULL /* UNDEFINED: SetXl_ttanybutton  */,
    (void *)NULL /* UNDEFINED: SetYl_ttanybutton  */,
    (void *)NULL /* UNDEFINED: SetWl_ttanybutton  */,
    (void *)NULL /* UNDEFINED: SetHl_ttanybutton  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttanybutton  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttanybutton  */,
    (void *)NULL /* UNDEFINED: DoSetAttr_ttanybutton  */,

  },
 
  {
    order_ttbutton,
    magicmask_ttbutton,
    magic_ttbutton,
    sizeof(s_ttbutton),
    (void *)NULL /* UNDEFINED: New_ttbutton  */,
    (void *)NULL /* inherits:  Build_ttbutton -> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttbutton -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttbutton  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttbutton -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttbutton -> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttbutton  */,
    (void *)NULL /* inherits:  SetVisible_ttbutton -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttbutton  */,
    (void *)NULL /* UNDEFINED: Draw_ttbutton  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttbutton  */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* UNDEFINED: SetXY_ttbutton  */,
    (void *)NULL /* UNDEFINED: SetWH_ttbutton  */,
    (void *)NULL /* UNDEFINED: SetXl_ttbutton  */,
    (void *)NULL /* UNDEFINED: SetYl_ttbutton  */,
    (void *)NULL /* UNDEFINED: SetWl_ttbutton  */,
    (void *)NULL /* UNDEFINED: SetHl_ttbutton  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttbutton  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttbutton  */,
    (void *)NULL /* UNDEFINED: DoSetAttr_ttbutton  */,

    /* extends ttfn_ttanybutton */
    (void *)NULL /* UNDEFINED: SetPressed_ttbutton  */,

  },
 
  {
    order_ttcheckbutton,
    magicmask_ttcheckbutton,
    magic_ttcheckbutton,
    sizeof(s_ttcheckbutton),
    (void *)NULL /* UNDEFINED: New_ttcheckbutton  */,
    (void *)NULL /* inherits:  Build_ttcheckbutton -> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttcheckbutton -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttcheckbutton  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttcheckbutton -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttcheckbutton -> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttcheckbutton  */,
    (void *)NULL /* inherits:  SetVisible_ttcheckbutton -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttcheckbutton  */,
    (void *)NULL /* UNDEFINED: Draw_ttcheckbutton  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttcheckbutton  */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* UNDEFINED: SetXY_ttcheckbutton  */,
    (void *)NULL /* UNDEFINED: SetWH_ttcheckbutton  */,
    (void *)NULL /* UNDEFINED: SetXl_ttcheckbutton  */,
    (void *)NULL /* UNDEFINED: SetYl_ttcheckbutton  */,
    (void *)NULL /* UNDEFINED: SetWl_ttcheckbutton  */,
    (void *)NULL /* UNDEFINED: SetHl_ttcheckbutton  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttcheckbutton  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttcheckbutton  */,
    (void *)NULL /* UNDEFINED: DoSetAttr_ttcheckbutton  */,

    /* extends ttfn_ttanybutton */
    (void *)NULL /* UNDEFINED: SetPressed_ttcheckbutton  */,

    /* extends ttfn_ttbutton */
    (void *)NULL /* UNDEFINED: SetChecked_ttcheckbutton  */,

  },
 
  {
    order_ttradiobutton,
    magicmask_ttradiobutton,
    magic_ttradiobutton,
    sizeof(s_ttradiobutton),
    (void *)NULL /* UNDEFINED: New_ttradiobutton  */,
    (void *)NULL /* inherits:  Build_ttradiobutton -> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttradiobutton -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttradiobutton  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttradiobutton -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttradiobutton -> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttradiobutton  */,
    (void *)NULL /* inherits:  SetVisible_ttradiobutton -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttradiobutton  */,
    (void *)NULL /* UNDEFINED: Draw_ttradiobutton  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttradiobutton  */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* UNDEFINED: SetXY_ttradiobutton  */,
    (void *)NULL /* UNDEFINED: SetWH_ttradiobutton  */,
    (void *)NULL /* UNDEFINED: SetXl_ttradiobutton  */,
    (void *)NULL /* UNDEFINED: SetYl_ttradiobutton  */,
    (void *)NULL /* UNDEFINED: SetWl_ttradiobutton  */,
    (void *)NULL /* UNDEFINED: SetHl_ttradiobutton  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttradiobutton  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttradiobutton  */,
    (void *)NULL /* UNDEFINED: DoSetAttr_ttradiobutton  */,

    /* extends ttfn_ttanybutton */
    (void *)NULL /* UNDEFINED: SetPressed_ttradiobutton  */,

    /* extends ttfn_ttbutton */
    (void *)NULL /* UNDEFINED: SetChecked_ttradiobutton  */,

    /* extends ttfn_ttcheckbutton */

  },
 
  {
    order_ttanyscroll,
    magicmask_ttanyscroll,
    magic_ttanyscroll,
    sizeof(s_ttanyscroll),
    (void *)NULL /* UNDEFINED: New_ttanyscroll  */,
    (void *)NULL /* inherits:  Build_ttanyscroll -> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttanyscroll -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttanyscroll  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttanyscroll -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttanyscroll -> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttanyscroll  */,
    (void *)NULL /* inherits:  SetVisible_ttanyscroll -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: Draw_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttanyscroll  */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* UNDEFINED: SetXY_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: SetWH_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: SetXl_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: SetYl_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: SetWl_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: SetHl_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttanyscroll  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: DoSetAttr_ttanyscroll  */,

    /* extends ttfn_ttanybutton */
    (void *)NULL /* UNDEFINED: Recalculate_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: SetState_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: SetOrientation_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: SetSize_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: SetRealSize_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: SetViewSize_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: SetPosition_ttanyscroll  */,

  },
 
  {
    order_ttscrollbar,
    magicmask_ttscrollbar,
    magic_ttscrollbar,
    sizeof(s_ttscrollbar),
    (void *)NULL /* UNDEFINED: New_ttscrollbar  */,
    (void *)NULL /* inherits:  Build_ttscrollbar -> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttscrollbar -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttscrollbar  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttscrollbar -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttscrollbar -> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttscrollbar  */,
    (void *)NULL /* inherits:  SetVisible_ttscrollbar -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: Draw_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttscrollbar  */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* UNDEFINED: SetXY_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: SetWH_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: SetXl_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: SetYl_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: SetWl_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: SetHl_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttscrollbar  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: DoSetAttr_ttscrollbar  */,

    /* extends ttfn_ttanybutton */
    (void *)NULL /* UNDEFINED: Recalculate_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: SetState_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: SetOrientation_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: SetSize_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: SetRealSize_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: SetViewSize_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: SetPosition_ttscrollbar  */,

    /* extends ttfn_ttanyscroll */

  },
 
  {
    order_ttslider,
    magicmask_ttslider,
    magic_ttslider,
    sizeof(s_ttslider),
    (void *)NULL /* UNDEFINED: New_ttslider  */,
    (void *)NULL /* inherits:  Build_ttslider -> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttslider -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttslider  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttslider -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttslider -> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttslider  */,
    (void *)NULL /* inherits:  SetVisible_ttslider -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttslider  */,
    (void *)NULL /* UNDEFINED: Draw_ttslider  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttslider  */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* UNDEFINED: SetXY_ttslider  */,
    (void *)NULL /* UNDEFINED: SetWH_ttslider  */,
    (void *)NULL /* UNDEFINED: SetXl_ttslider  */,
    (void *)NULL /* UNDEFINED: SetYl_ttslider  */,
    (void *)NULL /* UNDEFINED: SetWl_ttslider  */,
    (void *)NULL /* UNDEFINED: SetHl_ttslider  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttslider  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttslider  */,
    (void *)NULL /* UNDEFINED: DoSetAttr_ttslider  */,

    /* extends ttfn_ttanybutton */
    (void *)NULL /* UNDEFINED: Recalculate_ttslider  */,
    (void *)NULL /* UNDEFINED: SetState_ttslider  */,
    (void *)NULL /* UNDEFINED: SetOrientation_ttslider  */,
    (void *)NULL /* UNDEFINED: SetSize_ttslider  */,
    (void *)NULL /* UNDEFINED: SetRealSize_ttslider  */,
    (void *)NULL /* UNDEFINED: SetViewSize_ttslider  */,
    (void *)NULL /* UNDEFINED: SetPosition_ttslider  */,

    /* extends ttfn_ttanyscroll */
    (void *)NULL /* UNDEFINED: SetSlideMin_ttslider  */,
    (void *)NULL /* UNDEFINED: SetSlideMax_ttslider  */,
    (void *)NULL /* UNDEFINED: SetSlideValue_ttslider  */,

  },
 
  {
    order_ttprogressbar,
    magicmask_ttprogressbar,
    magic_ttprogressbar,
    sizeof(s_ttprogressbar),
    (void *)NULL /* UNDEFINED: New_ttprogressbar  */,
    (void *)NULL /* inherits:  Build_ttprogressbar -> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttprogressbar -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttprogressbar  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttprogressbar -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttprogressbar -> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttprogressbar  */,
    (void *)NULL /* inherits:  SetVisible_ttprogressbar -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: Draw_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttprogressbar  */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* UNDEFINED: SetXY_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: SetWH_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: SetXl_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: SetYl_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: SetWl_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: SetHl_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttprogressbar  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: CacheThemeShape_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: DoSetAttr_ttprogressbar  */,

    /* extends ttfn_ttanybutton */
    (void *)NULL /* UNDEFINED: Recalculate_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: SetState_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: SetOrientation_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: SetSize_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: SetRealSize_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: SetViewSize_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: SetPosition_ttprogressbar  */,

    /* extends ttfn_ttanyscroll */

  },
 
  {
    order_ttscrollpane,
    magicmask_ttscrollpane,
    magic_ttscrollpane,
    sizeof(s_ttscrollpane),
    (void *)NULL /* UNDEFINED: New_ttscrollpane  */,
    (void *)NULL /* inherits:  Build_ttscrollpane -> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttscrollpane -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttscrollpane  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttscrollpane -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttscrollpane -> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttscrollpane  */,
    (void *)NULL /* inherits:  SetVisible_ttscrollpane -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttscrollpane  */,
    (void *)NULL /* UNDEFINED: Draw_ttscrollpane  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttscrollpane  */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* UNDEFINED: SetXY_ttscrollpane  */,
    (void *)NULL /* UNDEFINED: SetWH_ttscrollpane  */,
    (void *)NULL /* UNDEFINED: SetXl_ttscrollpane  */,
    (void *)NULL /* UNDEFINED: SetYl_ttscrollpane  */,
    (void *)NULL /* UNDEFINED: SetWl_ttscrollpane  */,
    (void *)NULL /* UNDEFINED: SetHl_ttscrollpane  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttscrollpane  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: SetBarX_ttscrollpane  */,
    (void *)NULL /* UNDEFINED: SetBarY_ttscrollpane  */,

  },
 
  {
    order_ttwindow,
    magicmask_ttwindow,
    magic_ttwindow,
    sizeof(s_ttwindow),
    (void *)NULL /* UNDEFINED: New_ttwindow  */,
    (void *)NULL /* inherits:  Build_ttwindow -> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttwindow -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttwindow  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttwindow -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttwindow -> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttwindow  */,
    (void *)NULL /* inherits:  SetVisible_ttwindow -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttwindow  */,
    (void *)NULL /* UNDEFINED: Draw_ttwindow  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttwindow  */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* UNDEFINED: SetXY_ttwindow  */,
    (void *)NULL /* UNDEFINED: SetWH_ttwindow  */,
    (void *)NULL /* UNDEFINED: SetXl_ttwindow  */,
    (void *)NULL /* UNDEFINED: SetYl_ttwindow  */,
    (void *)NULL /* UNDEFINED: SetWl_ttwindow  */,
    (void *)NULL /* UNDEFINED: SetHl_ttwindow  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttwindow  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: SetTitle_ttwindow  */,

  },
 
  {
    order_ttframe,
    magicmask_ttframe,
    magic_ttframe,
    sizeof(s_ttframe),
    (void *)NULL /* UNDEFINED: New_ttframe  */,
    (void *)NULL /* inherits:  Build_ttframe -> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttframe -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttframe  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttframe -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttframe -> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttframe  */,
    (void *)NULL /* inherits:  SetVisible_ttframe -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttframe  */,
    (void *)NULL /* UNDEFINED: Draw_ttframe  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttframe  */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* UNDEFINED: SetXY_ttframe  */,
    (void *)NULL /* UNDEFINED: SetWH_ttframe  */,
    (void *)NULL /* UNDEFINED: SetXl_ttframe  */,
    (void *)NULL /* UNDEFINED: SetYl_ttframe  */,
    (void *)NULL /* UNDEFINED: SetWl_ttframe  */,
    (void *)NULL /* UNDEFINED: SetHl_ttframe  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttframe  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: SetTitle_ttframe  */,

    /* extends ttfn_ttwindow */
    (void *)NULL /* UNDEFINED: SetMenubar_ttframe  */,

  },
 
  {
    order_ttscroller,
    magicmask_ttscroller,
    magic_ttscroller,
    sizeof(s_ttscroller),
    (void *)NULL /* UNDEFINED: New_ttscroller  */,
    (void *)NULL /* inherits:  Build_ttscroller -> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttscroller -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttscroller  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttscroller -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttscroller -> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttscroller  */,
    (void *)NULL /* inherits:  SetVisible_ttscroller -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttscroller  */,
    (void *)NULL /* UNDEFINED: Draw_ttscroller  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttscroller  */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* UNDEFINED: SetXY_ttscroller  */,
    (void *)NULL /* UNDEFINED: SetWH_ttscroller  */,
    (void *)NULL /* UNDEFINED: SetXl_ttscroller  */,
    (void *)NULL /* UNDEFINED: SetYl_ttscroller  */,
    (void *)NULL /* UNDEFINED: SetWl_ttscroller  */,
    (void *)NULL /* UNDEFINED: SetHl_ttscroller  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttscroller  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: SetTitle_ttscroller  */,

    /* extends ttfn_ttwindow */

  },
 
  {
    order_ttmenuitem,
    magicmask_ttmenuitem,
    magic_ttmenuitem,
    sizeof(s_ttmenuitem),
    (void *)NULL /* UNDEFINED: New_ttmenuitem  */,
    (void *)NULL /* UNDEFINED: Build_ttmenuitem  */,
    (void *)NULL /* inherits:  Break_ttmenuitem -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttmenuitem  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttmenuitem  */,
    (void *)NULL /* UNDEFINED: Remove_ttmenuitem  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttmenuitem  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttmenuitem  */,
    (void *)NULL /* UNDEFINED: SetTheme_ttmenuitem  */,
    (void *)NULL /* UNDEFINED: Draw_ttmenuitem  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttmenuitem  */,

    /* extends ttfn_ttvisible */

  },
 
  {
    order_ttcheckmenuitem,
    magicmask_ttcheckmenuitem,
    magic_ttcheckmenuitem,
    sizeof(s_ttcheckmenuitem),
    (void *)NULL /* UNDEFINED: New_ttcheckmenuitem  */,
    (void *)NULL /* UNDEFINED: Build_ttcheckmenuitem  */,
    (void *)NULL /* inherits:  Break_ttcheckmenuitem -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttcheckmenuitem  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttcheckmenuitem  */,
    (void *)NULL /* UNDEFINED: Remove_ttcheckmenuitem  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttcheckmenuitem  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttcheckmenuitem  */,
    (void *)NULL /* UNDEFINED: SetTheme_ttcheckmenuitem  */,
    (void *)NULL /* UNDEFINED: Draw_ttcheckmenuitem  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttcheckmenuitem  */,

    /* extends ttfn_ttvisible */

    /* extends ttfn_ttmenuitem */

  },
 
  {
    order_ttradiomenuitem,
    magicmask_ttradiomenuitem,
    magic_ttradiomenuitem,
    sizeof(s_ttradiomenuitem),
    (void *)NULL /* UNDEFINED: New_ttradiomenuitem  */,
    (void *)NULL /* UNDEFINED: Build_ttradiomenuitem  */,
    (void *)NULL /* inherits:  Break_ttradiomenuitem -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttradiomenuitem  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttradiomenuitem  */,
    (void *)NULL /* UNDEFINED: Remove_ttradiomenuitem  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttradiomenuitem  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttradiomenuitem  */,
    (void *)NULL /* UNDEFINED: SetTheme_ttradiomenuitem  */,
    (void *)NULL /* UNDEFINED: Draw_ttradiomenuitem  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttradiomenuitem  */,

    /* extends ttfn_ttvisible */

    /* extends ttfn_ttmenuitem */

    /* extends ttfn_ttcheckmenuitem */

  },
 
  {
    order_ttmenu,
    magicmask_ttmenu,
    magic_ttmenu,
    sizeof(s_ttmenu),
    (void *)NULL /* UNDEFINED: New_ttmenu  */,
    (void *)NULL /* UNDEFINED: Build_ttmenu  */,
    (void *)NULL /* inherits:  Break_ttmenu -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttmenu  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttmenu  */,
    (void *)NULL /* UNDEFINED: Remove_ttmenu  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttmenu  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttmenu  */,
    (void *)NULL /* UNDEFINED: SetTheme_ttmenu  */,
    (void *)NULL /* UNDEFINED: Draw_ttmenu  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttmenu  */,

    /* extends ttfn_ttvisible */

    /* extends ttfn_ttmenuitem */

  },
 
  {
    order_ttmenubar,
    magicmask_ttmenubar,
    magic_ttmenubar,
    sizeof(s_ttmenubar),
    (void *)NULL /* UNDEFINED: New_ttmenubar  */,
    (void *)NULL /* UNDEFINED: Build_ttmenubar  */,
    (void *)NULL /* inherits:  Break_ttmenubar -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttmenubar  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttmenubar  */,
    (void *)NULL /* UNDEFINED: Remove_ttmenubar  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttmenubar  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttmenubar  */,
    (void *)NULL /* UNDEFINED: SetTheme_ttmenubar  */,
    (void *)NULL /* UNDEFINED: Draw_ttmenubar  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttmenubar  */,

    /* extends ttfn_ttvisible */

  },
 
  {
    order_ttanytext,
    magicmask_ttanytext,
    magic_ttanytext,
    sizeof(s_ttanytext),
    (void *)NULL /* UNDEFINED: New_ttanytext  */,
    (void *)NULL /* inherits:  Build_ttanytext -> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_ttanytext -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttanytext  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_ttanytext -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_ttanytext -> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_ttanytext  */,
    (void *)NULL /* inherits:  SetVisible_ttanytext -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_ttanytext  */,
    (void *)NULL /* UNDEFINED: Draw_ttanytext  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_ttanytext  */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* UNDEFINED: SetXY_ttanytext  */,
    (void *)NULL /* UNDEFINED: SetWH_ttanytext  */,
    (void *)NULL /* UNDEFINED: SetXl_ttanytext  */,
    (void *)NULL /* UNDEFINED: SetYl_ttanytext  */,
    (void *)NULL /* UNDEFINED: SetWl_ttanytext  */,
    (void *)NULL /* UNDEFINED: SetHl_ttanytext  */,
    (void *)NULL /* UNDEFINED: SetTooltip_ttanytext  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: SetTitle_ttanytext  */,

    /* extends ttfn_ttwindow */

  },
 
  {
    order_tttextfield,
    magicmask_tttextfield,
    magic_tttextfield,
    sizeof(s_tttextfield),
    (void *)NULL /* UNDEFINED: New_tttextfield  */,
    (void *)NULL /* inherits:  Build_tttextfield -> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_tttextfield -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_tttextfield  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_tttextfield -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_tttextfield -> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_tttextfield  */,
    (void *)NULL /* inherits:  SetVisible_tttextfield -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_tttextfield  */,
    (void *)NULL /* UNDEFINED: Draw_tttextfield  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_tttextfield  */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* UNDEFINED: SetXY_tttextfield  */,
    (void *)NULL /* UNDEFINED: SetWH_tttextfield  */,
    (void *)NULL /* UNDEFINED: SetXl_tttextfield  */,
    (void *)NULL /* UNDEFINED: SetYl_tttextfield  */,
    (void *)NULL /* UNDEFINED: SetWl_tttextfield  */,
    (void *)NULL /* UNDEFINED: SetHl_tttextfield  */,
    (void *)NULL /* UNDEFINED: SetTooltip_tttextfield  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: SetTitle_tttextfield  */,

    /* extends ttfn_ttwindow */

    /* extends ttfn_ttanytext */

  },
 
  {
    order_tttextarea,
    magicmask_tttextarea,
    magic_tttextarea,
    sizeof(s_tttextarea),
    (void *)NULL /* UNDEFINED: New_tttextarea  */,
    (void *)NULL /* inherits:  Build_tttextarea -> Build_ttwidget */,
    (void *)NULL /* inherits:  Break_tttextarea -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_tttextarea  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* inherits:  AddTo_tttextarea -> AddTo_ttwidget */,
    (void *)NULL /* inherits:  Remove_tttextarea -> Remove_ttwidget */,
    (void *)NULL /* UNDEFINED: Invalidate_tttextarea  */,
    (void *)NULL /* inherits:  SetVisible_tttextarea -> SetVisible_ttwidget */,
    (void *)NULL /* UNDEFINED: SetTheme_tttextarea  */,
    (void *)NULL /* UNDEFINED: Draw_tttextarea  */,
    (void *)NULL /* UNDEFINED: BuiltinRepaint_tttextarea  */,

    /* extends ttfn_ttvisible */
    (void *)NULL /* UNDEFINED: SetXY_tttextarea  */,
    (void *)NULL /* UNDEFINED: SetWH_tttextarea  */,
    (void *)NULL /* UNDEFINED: SetXl_tttextarea  */,
    (void *)NULL /* UNDEFINED: SetYl_tttextarea  */,
    (void *)NULL /* UNDEFINED: SetWl_tttextarea  */,
    (void *)NULL /* UNDEFINED: SetHl_tttextarea  */,
    (void *)NULL /* UNDEFINED: SetTooltip_tttextarea  */,

    /* extends ttfn_ttwidget */
    (void *)NULL /* UNDEFINED: SetTitle_tttextarea  */,

    /* extends ttfn_ttwindow */

    /* extends ttfn_ttanytext */

    /* extends ttfn_tttextfield */

  },
 
  {
    order_tttheme,
    magicmask_tttheme,
    magic_tttheme,
    sizeof(s_tttheme),
    (void *)NULL /* UNDEFINED: New_tttheme  */,
    (void *)NULL /* UNDEFINED: Build_tttheme  */,
    (void *)NULL /* inherits:  Break_tttheme -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_tttheme  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */

  },
 
  {
    order_ttapplication,
    magicmask_ttapplication,
    magic_ttapplication,
    sizeof(s_ttapplication),
    (void *)NULL /* UNDEFINED: New_ttapplication  */,
    (void *)NULL /* UNDEFINED: Build_ttapplication  */,
    (void *)NULL /* inherits:  Break_ttapplication -> Break_ttobj */,
    (void *)NULL /* UNDEFINED: Del_ttapplication  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */

  },



  {

    gtk_Sync, 
    gtk_Flush, 
    gtk_TimidFlush, 
    gtk_MainLoopOnce, 
    gtk_DeleteCallback,    
    gtk_Close, 
    gtk_ConnectionFd, 
    gtk_GetErrno, 
    gtk_GetErrnoDetail,    
    gtk_StrError, 
    gtk_StrErrorDetail,   
  },
};



