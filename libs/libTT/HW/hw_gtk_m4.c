




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
static ttwidget gtk_New_ttwidget(ttfn_ttwidget FN, ttwidget o) {
    if ((o->native = (opaque)gtk_fixed_new()))
	return o;
    return NULL;
}
static ttwidget gtk_Build_ttwidget(ttwidget o) {
    gtk_widget_realize((GtkWidget *)o->native);
    return o;
}
static void gtk_AddTo_ttwidget(ttwidget o, ttvisible parent) {
    if (parent && !o->parent) {
	/* prepend to list, cannot use o->FN->FN_ttvisible->AddTo() */
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
	o->FN->FN_ttvisible->Remove((super_(ttwidget))o);
	
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
static ttbyte gtk_MainLoop(void) {
    gtk_main();
    return 1;
}
static void gtk_ExitMainLoop(void) {
    gtk_main_quit();
}
static void gtk_DeleteListener(ttlistener o) {
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
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
  },
  
  {
    order_ttevent,
    magicmask_ttevent,
    magic_ttevent,
    sizeof(s_ttevent),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,
    
  },
 
  {
    order_tteventbig,
    magicmask_tteventbig,
    magic_tteventbig,
    sizeof(s_tteventbig),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,
    
    TFN_ttevent,

    
  },
 
  {
    order_ttlistener,
    magicmask_ttlistener,
    magic_ttlistener,
    sizeof(s_ttlistener),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    (void *)NULL /* WARNING: undefined Create */,

  },
 
  {
    order_ttcallback,
    magicmask_ttcallback,
    magic_ttcallback,
    sizeof(s_ttcallback),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    (void *)NULL /* WARNING: undefined Create */,

    TFN_ttlistener,
            
  },
 
  {
    order_ttcomponent,
    magicmask_ttcomponent,
    magic_ttcomponent,
    sizeof(s_ttcomponent),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

  },
 
  {
    order_ttvisible,
    magicmask_ttvisible,
    magic_ttvisible,
    sizeof(s_ttvisible),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)NULL /* WARNING: undefined AddTo */,
    (void *)NULL /* WARNING: undefined Remove */,
    (void *)NULL /* WARNING: undefined SetVisible */,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
  },
 
  {
    order_ttnative,
    magicmask_ttnative,
    magic_ttnative,
    sizeof(s_ttnative),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)NULL /* WARNING: undefined AddTo */,
    (void *)NULL /* WARNING: undefined Remove */,
    (void *)NULL /* WARNING: undefined SetVisible */,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,
    (void *)NULL /* WARNING: undefined GetRoot */,
    
  },
 
  {
    order_ttwidget,
    magicmask_ttwidget,
    magic_ttwidget,
    sizeof(s_ttwidget),
    gtk_New_ttwidget,
    gtk_Build_ttwidget,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    gtk_AddTo_ttwidget,
    gtk_Remove_ttwidget,
    gtk_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,
    (void *)NULL /* WARNING: undefined SetXYWH */,
    (void *)NULL /* WARNING: undefined SetXlYl */,
                
  },
 
  {
    order_ttlabel,
    magicmask_ttlabel,
    magic_ttlabel,
    sizeof(s_ttlabel),
    (void *)gtk_New_ttwidget,
    (void *)gtk_Build_ttwidget,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)gtk_AddTo_ttwidget,
    (void *)gtk_Remove_ttwidget,
    (void *)gtk_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,
    (void *)NULL /* WARNING: undefined SetXYWH */,
    (void *)NULL /* WARNING: undefined SetXlYl */,
                
    TFN_ttwidget,
    (void *)NULL /* WARNING: undefined SetText */,
    
  },
 
  {
    order_ttanybutton,
    magicmask_ttanybutton,
    magic_ttanybutton,
    sizeof(s_ttanybutton),
    (void *)gtk_New_ttwidget,
    (void *)gtk_Build_ttwidget,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)gtk_AddTo_ttwidget,
    (void *)gtk_Remove_ttwidget,
    (void *)gtk_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,
    (void *)NULL /* WARNING: undefined SetXYWH */,
    (void *)NULL /* WARNING: undefined SetXlYl */,
                
    TFN_ttwidget,
    (void *)NULL /* WARNING: undefined SetText */,
    
  },
 
  {
    order_ttbutton,
    magicmask_ttbutton,
    magic_ttbutton,
    sizeof(s_ttbutton),
    (void *)gtk_New_ttwidget,
    (void *)gtk_Build_ttwidget,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)gtk_AddTo_ttwidget,
    (void *)gtk_Remove_ttwidget,
    (void *)gtk_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,
    (void *)NULL /* WARNING: undefined SetXYWH */,
    (void *)NULL /* WARNING: undefined SetXlYl */,
                
    TFN_ttwidget,
    (void *)NULL /* WARNING: undefined SetText */,
    
    TFN_ttanybutton,
    (void *)NULL /* WARNING: undefined SetPressed */,
        
  },
 
  {
    order_ttcheckbutton,
    magicmask_ttcheckbutton,
    magic_ttcheckbutton,
    sizeof(s_ttcheckbutton),
    (void *)gtk_New_ttwidget,
    (void *)gtk_Build_ttwidget,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)gtk_AddTo_ttwidget,
    (void *)gtk_Remove_ttwidget,
    (void *)gtk_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,
    (void *)NULL /* WARNING: undefined SetXYWH */,
    (void *)NULL /* WARNING: undefined SetXlYl */,
                
    TFN_ttwidget,
    (void *)NULL /* WARNING: undefined SetText */,
    
    TFN_ttanybutton,
    (void *)NULL /* WARNING: undefined SetPressed */,
        
    TFN_ttbutton,

  },
 
  {
    order_ttradiobutton,
    magicmask_ttradiobutton,
    magic_ttradiobutton,
    sizeof(s_ttradiobutton),
    (void *)gtk_New_ttwidget,
    (void *)gtk_Build_ttwidget,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)gtk_AddTo_ttwidget,
    (void *)gtk_Remove_ttwidget,
    (void *)gtk_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,
    (void *)NULL /* WARNING: undefined SetXYWH */,
    (void *)NULL /* WARNING: undefined SetXlYl */,
                
    TFN_ttwidget,
    (void *)NULL /* WARNING: undefined SetText */,
    
    TFN_ttanybutton,
    (void *)NULL /* WARNING: undefined SetPressed */,
        
    TFN_ttbutton,

    TFN_ttcheckbutton,

  },
 
  {
    order_ttscrollbar,
    magicmask_ttscrollbar,
    magic_ttscrollbar,
    sizeof(s_ttscrollbar),
    (void *)gtk_New_ttwidget,
    (void *)gtk_Build_ttwidget,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)gtk_AddTo_ttwidget,
    (void *)gtk_Remove_ttwidget,
    (void *)gtk_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,
    (void *)NULL /* WARNING: undefined SetXYWH */,
    (void *)NULL /* WARNING: undefined SetXlYl */,
                
    TFN_ttwidget,
    (void *)NULL /* WARNING: undefined SetText */,
    
    TFN_ttanybutton,

  },
 
  {
    order_ttbuttongroup,
    magicmask_ttbuttongroup,
    magic_ttbuttongroup,
    sizeof(s_ttbuttongroup),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,

  },
 
  {
    order_ttwindow,
    magicmask_ttwindow,
    magic_ttwindow,
    sizeof(s_ttwindow),
    (void *)gtk_New_ttwidget,
    (void *)gtk_Build_ttwidget,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)gtk_AddTo_ttwidget,
    (void *)gtk_Remove_ttwidget,
    (void *)gtk_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,
    (void *)NULL /* WARNING: undefined SetXYWH */,
    (void *)NULL /* WARNING: undefined SetXlYl */,
                
    TFN_ttwidget,

  },
 
  {
    order_ttframe,
    magicmask_ttframe,
    magic_ttframe,
    sizeof(s_ttframe),
    (void *)gtk_New_ttwidget,
    (void *)gtk_Build_ttwidget,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)gtk_AddTo_ttwidget,
    (void *)gtk_Remove_ttwidget,
    (void *)gtk_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,
    (void *)NULL /* WARNING: undefined SetXYWH */,
    (void *)NULL /* WARNING: undefined SetXlYl */,
                
    TFN_ttwidget,

    TFN_ttwindow,

  },
 
  {
    order_ttscroller,
    magicmask_ttscroller,
    magic_ttscroller,
    sizeof(s_ttscroller),
    (void *)gtk_New_ttwidget,
    (void *)gtk_Build_ttwidget,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)gtk_AddTo_ttwidget,
    (void *)gtk_Remove_ttwidget,
    (void *)gtk_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,
    (void *)NULL /* WARNING: undefined SetXYWH */,
    (void *)NULL /* WARNING: undefined SetXlYl */,
                
    TFN_ttwidget,

    TFN_ttwindow,

  },
 
  {
    order_ttmenuitem,
    magicmask_ttmenuitem,
    magic_ttmenuitem,
    sizeof(s_ttmenuitem),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)NULL /* WARNING: undefined AddTo */,
    (void *)NULL /* WARNING: undefined Remove */,
    (void *)NULL /* WARNING: undefined SetVisible */,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,

  },
 
  {
    order_ttcheckmenuitem,
    magicmask_ttcheckmenuitem,
    magic_ttcheckmenuitem,
    sizeof(s_ttcheckmenuitem),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)NULL /* WARNING: undefined AddTo */,
    (void *)NULL /* WARNING: undefined Remove */,
    (void *)NULL /* WARNING: undefined SetVisible */,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,

    TFN_ttmenuitem,

  },
 
  {
    order_ttradiomenuitem,
    magicmask_ttradiomenuitem,
    magic_ttradiomenuitem,
    sizeof(s_ttradiomenuitem),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)NULL /* WARNING: undefined AddTo */,
    (void *)NULL /* WARNING: undefined Remove */,
    (void *)NULL /* WARNING: undefined SetVisible */,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,

    TFN_ttmenuitem,

    TFN_ttcheckmenuitem,

  },
 
  {
    order_ttmenu,
    magicmask_ttmenu,
    magic_ttmenu,
    sizeof(s_ttmenu),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)NULL /* WARNING: undefined AddTo */,
    (void *)NULL /* WARNING: undefined Remove */,
    (void *)NULL /* WARNING: undefined SetVisible */,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,

    TFN_ttmenuitem,

  },
 
  {
    order_ttmenubar,
    magicmask_ttmenubar,
    magic_ttmenubar,
    sizeof(s_ttmenubar),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)NULL /* WARNING: undefined AddTo */,
    (void *)NULL /* WARNING: undefined Remove */,
    (void *)NULL /* WARNING: undefined SetVisible */,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,
    
  },
 
  {
    order_ttanytext,
    magicmask_ttanytext,
    magic_ttanytext,
    sizeof(s_ttanytext),
    (void *)gtk_New_ttwidget,
    (void *)gtk_Build_ttwidget,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)gtk_AddTo_ttwidget,
    (void *)gtk_Remove_ttwidget,
    (void *)gtk_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,
    (void *)NULL /* WARNING: undefined SetXYWH */,
    (void *)NULL /* WARNING: undefined SetXlYl */,
                
    TFN_ttwidget,

    TFN_ttwindow,

  },
 
  {
    order_tttextfield,
    magicmask_tttextfield,
    magic_tttextfield,
    sizeof(s_tttextfield),
    (void *)gtk_New_ttwidget,
    (void *)gtk_Build_ttwidget,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)gtk_AddTo_ttwidget,
    (void *)gtk_Remove_ttwidget,
    (void *)gtk_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,
    (void *)NULL /* WARNING: undefined SetXYWH */,
    (void *)NULL /* WARNING: undefined SetXlYl */,
                
    TFN_ttwidget,

    TFN_ttwindow,

    TFN_ttanytext,

  },
 
  {
    order_tttextarea,
    magicmask_tttextarea,
    magic_tttextarea,
    sizeof(s_tttextarea),
    (void *)gtk_New_ttwidget,
    (void *)gtk_Build_ttwidget,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)gtk_AddTo_ttwidget,
    (void *)gtk_Remove_ttwidget,
    (void *)gtk_SetVisible_ttwidget,
    (void *)NULL /* WARNING: undefined SetTheme */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,
    (void *)NULL /* WARNING: undefined SetXYWH */,
    (void *)NULL /* WARNING: undefined SetXlYl */,
                
    TFN_ttwidget,

    TFN_ttwindow,

    TFN_ttanytext,

    TFN_tttextfield,

  },
 
  {
    order_tttheme,
    magicmask_tttheme,
    magic_tttheme,
    sizeof(s_tttheme),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    (void *)NULL /* WARNING: undefined ComputeShapeButton */,
    
  },
 
  {
    order_ttapplication,
    magicmask_ttapplication,
    magic_ttapplication,
    sizeof(s_ttapplication),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)gtk_Break_ttobj,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    
  },



  {

    gtk_Sync, 
    gtk_Flush, 
    gtk_TimidFlush, 
    gtk_MainLoop, 
    gtk_ExitMainLoop, 
    gtk_DeleteListener,    
    gtk_Close, 
    gtk_ConnectionFd, 
    gtk_GetErrno, 
    gtk_GetErrnoDetail, 
    gtk_StrError, 
    gtk_StrErrorDetail,   
  },
};



