




/* This file was automatically generated from hw_xml_c by m4/hw.m4h, do not edit! */


/*
 *  hw_xml_m4.c  --  implementation of libTT methods on "xml" target display
 *
 *  Copyright (C) 2001,2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */
























#include <stdio.h>



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

static s_ttfns xml_TTFNs;



static TT_CONST ttbyte * dump_file_name;

static FILE * dump_file;

static ttbyte open_dump_file(void) {
    if ((dump_file = fopen(dump_file_name, "w"))) {
	fprintf(dump_file, "%s",
		"<?xml version=\"1.0\"?>\n"
		"<TT:dump xmlns:TT=\"http://twin.sourceforge.net/libTT/xml/\">\n"
		);
	return TRUE;
    }
    return FALSE;
}

static void close_dump_file(void) {
    fprintf(dump_file, "%s", "</TT:dump>\n");
    fclose(dump_file);
}

TT_INLINE void dump_obj(ttobj o) {
    ttuint i, last;
    ttopaque id;
    TT_CONST ttbyte *name;
    ttbyte buf[64];
    ttany data;
    ttfn type;
    
    if ((id = OBJ2ID(o))) {
	fprintf(dump_file, "  <TT:%s>\n", name = TTClassNameOf((tt_obj)id));
	sprintf(buf, "%s_field_last", name);
	last = TTGetValueId(buf);
	for (i = 0; i < last; i++) {
	    if ((name = TTGetValueName(i)) && (type = o->FN->GetValue(o, i, &data))) {
		if (type == TTFN_ttstring)
		    fprintf(dump_file, "    <TT:%s value=\"%s\"/>\n", name, (ttstring)(ttopaque)data);
		else
		    fprintf(dump_file, "    <TT:%s value=\"0x%lx\"/>\n", name, (unsigned long)(ttopaque)data);
	    }
	}
	fprintf(dump_file, "  </TT:%s>\n", TTClassNameOf((tt_obj)id));
    }
}

#define IdList	 TTD.UTIL.IdList
#define IdSize	 TTD.UTIL.IdSize
#define IdTop	 TTD.UTIL.IdTop
#define IdBottom TTD.UTIL.IdBottom

static void dump_all(void) {
    ttbyte n;
    ttopaque i;
    ttobj o;

    if (open_dump_file()) {
	for (n = order_ttobj; n < order_n; n++) {
	    for (i = 0; i < IdTop[n]; i++) {
		if ((o = IdList[n][i]))
		    dump_obj(o);
	    }
	}
	close_dump_file();
    }
}

/*
 * We need to write down only overloaded methods (actually none).
 */

static ttbyte xml_Sync(void) {
    return 1;
}
static ttbyte xml_Flush(void) {
    return 1;
}
static ttbyte xml_TimidFlush(void) {
    return 1;
}
static ttbyte xml_MainLoop(void) {
    return 1;
}
static ttbyte xml_MainLoopOnce(ttbyte wait) {
    return 1;
}
static void xml_ExitMainLoop(void) {
}
static void xml_DeleteListener(ttlistener o) {
}
static void xml_Close(void) {
    dump_all();
}
static int xml_ConnectionFd(void) {
  return -1;
}
static ttuint xml_GetErrno(void) {
  return 0;
}
static ttuint xml_GetErrnoDetail(void) {
  return 0;
}
static TT_CONST ttbyte *xml_StrError(ttuint E) {
    return "";
}
static TT_CONST ttbyte *xml_StrErrorDetail(ttuint E, ttuint S) {
    return "";
}


#ifdef THIS_MODULE
ttfns InitModule(tthw *HW)
#else
ttfns _TT_xml_InitHW(tthw *HW)
#endif
{
    ttbyte *arg = TTD.HWOptions;
    
    /*
     * (*HW) must be set even if initialization fails,
     * to report error messages.
     */
    *HW = &xml_TTFNs.HW;

    dump_file_name = "libTT.xml";
    if (arg && (arg = strstr("file=", arg))) {
	dump_file_name = arg + 5;
	if ((arg = strchr(dump_file_name, ',')))
	    *arg = '\0';
    }

    return &xml_TTFNs;
}




                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             


/*
 * use default values for methods not implemented in hw_xml_c
 *
 * null display target MUST IMPLEMENT ALL METHODS, even if as stubs.
 */











static s_ttfns xml_TTFNs = {


  {
    order_ttobj,
    magicmask_ttobj,
    magic_ttobj,
    sizeof(s_ttobj),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Draw */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
  },
 
  {
    order_ttnative,
    magicmask_ttnative,
    magic_ttnative,
    sizeof(s_ttnative),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Draw */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,
    (void *)NULL /* WARNING: undefined GetRoot */,
    
  },
 
  {
    order_ttwidget,
    magicmask_ttwidget,
    magic_ttwidget,
    sizeof(s_ttwidget),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Draw */,
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
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Draw */,
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
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Draw */,
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
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Draw */,
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
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Draw */,
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
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Draw */,
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
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Draw */,
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
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Draw */,
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
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Draw */,
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
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Draw */,
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
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Draw */,
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
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Draw */,
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
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Draw */,
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
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Draw */,
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
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Draw */,
    (void *)NULL /* WARNING: undefined BuiltinRepaint */,
            
    TFN_ttvisible,
    
  },
 
  {
    order_ttanytext,
    magicmask_ttanytext,
    magic_ttanytext,
    sizeof(s_ttanytext),
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Draw */,
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
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Draw */,
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
    (void *)NULL /* WARNING: undefined New */,
    (void *)NULL /* WARNING: undefined Build */,
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Draw */,
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
    (void *)NULL /* WARNING: undefined Break */,
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
    (void *)NULL /* WARNING: undefined Break */,
    (void *)NULL /* WARNING: undefined Del */,
    (void *)NULL /* WARNING: undefined GetValue */,
    (void *)NULL /* WARNING: undefined SetValue */,
    (void *)NULL /* WARNING: undefined ChangeValue */,
        
    TFN_ttobj,

    TFN_ttcomponent,
    
  },



  {

    xml_Sync, 
    xml_Flush, 
    xml_TimidFlush, 
    xml_MainLoop, 
    xml_MainLoopOnce, 
    xml_ExitMainLoop,    
    xml_DeleteListener, 
    xml_Close, 
    xml_ConnectionFd, 
    xml_GetErrno, 
    xml_GetErrnoDetail,    
    xml_StrError, 
    xml_StrErrorDetail,   
  },
};



