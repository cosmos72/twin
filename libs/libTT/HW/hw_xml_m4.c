




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
    tt_fn type;
    
    if ((id = OBJ2ID(o))) {
	fprintf(dump_file, "  <TT:%s>\n", name = TTClassNameOf((tt_obj)id));
	sprintf(buf, "%s_field_last", name);
	last = TTGetFieldId(buf);
	for (i = 0; i < last; i++) {
	    if ((name = TTGetFieldName(i)) && (type = TTGetField_ttobj((tt_obj)id, i, &data)) && data != (ttany)0) {
		if (type == TTFN_ttstring)
		    fprintf(dump_file, "    <TT:%s value=\"%s\"/>\n", name, (ttstring)(ttopaque)data);
		else
		    fprintf(dump_file, "    <TT:%s value=\"0x%lX\"/>\n", name, (unsigned long)(ttopaque)data);
	    }
	}
	fprintf(dump_file, "  </TT:%s>\n", TTClassNameOf((tt_obj)id));
    }
}

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
static ttbyte xml_MainLoopOnce(ttbyte wait) {
    return 1;
}
static void xml_DeleteCallback(ttcallback o) {
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
    (void *)NULL /* UNDEFINED: New_ttobj  */,
    (void *)NULL /* UNDEFINED: Build_ttobj  */,
    (void *)NULL /* UNDEFINED: Break_ttobj  */,
    (void *)NULL /* UNDEFINED: Del_ttobj  */,

  },
  
  {
    order_ttevent,
    magicmask_ttevent,
    magic_ttevent,
    sizeof(s_ttevent),
    (void *)NULL /* UNDEFINED: New_ttevent  */,
    (void *)NULL /* UNDEFINED: Build_ttevent  */,
    (void *)NULL /* UNDEFINED: Break_ttevent  */,
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
    (void *)NULL /* UNDEFINED: Break_tteventbig  */,
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
    (void *)NULL /* UNDEFINED: Break_ttbitmask  */,
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
    (void *)NULL /* UNDEFINED: Break_tteventmask  */,
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
    (void *)NULL /* UNDEFINED: Break_ttcallback  */,
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
    (void *)NULL /* UNDEFINED: Break_ttlistener  */,
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
    (void *)NULL /* UNDEFINED: Break_tttimer  */,
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
    (void *)NULL /* UNDEFINED: Break_ttcomponent  */,
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
    (void *)NULL /* UNDEFINED: Break_ttdata  */,
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
    (void *)NULL /* UNDEFINED: Break_ttvisible  */,
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
    (void *)NULL /* UNDEFINED: Break_ttnative  */,
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
    (void *)NULL /* UNDEFINED: Build_ttwidget  */,
    (void *)NULL /* UNDEFINED: Break_ttwidget  */,
    (void *)NULL /* UNDEFINED: Del_ttwidget  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttwidget  */,
    (void *)NULL /* UNDEFINED: Remove_ttwidget  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttwidget  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttwidget  */,
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
    (void *)NULL /* UNDEFINED: Build_ttlabel  */,
    (void *)NULL /* UNDEFINED: Break_ttlabel  */,
    (void *)NULL /* UNDEFINED: Del_ttlabel  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttlabel  */,
    (void *)NULL /* UNDEFINED: Remove_ttlabel  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttlabel  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttlabel  */,
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
    (void *)NULL /* UNDEFINED: Build_tttooltip  */,
    (void *)NULL /* UNDEFINED: Break_tttooltip  */,
    (void *)NULL /* UNDEFINED: Del_tttooltip  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_tttooltip  */,
    (void *)NULL /* UNDEFINED: Remove_tttooltip  */,
    (void *)NULL /* UNDEFINED: Invalidate_tttooltip  */,
    (void *)NULL /* UNDEFINED: SetVisible_tttooltip  */,
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
    (void *)NULL /* UNDEFINED: Break_ttbuttongroup  */,
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
    (void *)NULL /* UNDEFINED: Build_ttanybutton  */,
    (void *)NULL /* UNDEFINED: Break_ttanybutton  */,
    (void *)NULL /* UNDEFINED: Del_ttanybutton  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttanybutton  */,
    (void *)NULL /* UNDEFINED: Remove_ttanybutton  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttanybutton  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttanybutton  */,
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
    (void *)NULL /* UNDEFINED: Build_ttbutton  */,
    (void *)NULL /* UNDEFINED: Break_ttbutton  */,
    (void *)NULL /* UNDEFINED: Del_ttbutton  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttbutton  */,
    (void *)NULL /* UNDEFINED: Remove_ttbutton  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttbutton  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttbutton  */,
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
    (void *)NULL /* UNDEFINED: Build_ttcheckbutton  */,
    (void *)NULL /* UNDEFINED: Break_ttcheckbutton  */,
    (void *)NULL /* UNDEFINED: Del_ttcheckbutton  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttcheckbutton  */,
    (void *)NULL /* UNDEFINED: Remove_ttcheckbutton  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttcheckbutton  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttcheckbutton  */,
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
    (void *)NULL /* UNDEFINED: Build_ttradiobutton  */,
    (void *)NULL /* UNDEFINED: Break_ttradiobutton  */,
    (void *)NULL /* UNDEFINED: Del_ttradiobutton  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttradiobutton  */,
    (void *)NULL /* UNDEFINED: Remove_ttradiobutton  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttradiobutton  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttradiobutton  */,
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
    (void *)NULL /* UNDEFINED: Build_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: Break_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: Del_ttanyscroll  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: Remove_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttanyscroll  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttanyscroll  */,
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
    (void *)NULL /* UNDEFINED: Build_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: Break_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: Del_ttscrollbar  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: Remove_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttscrollbar  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttscrollbar  */,
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
    (void *)NULL /* UNDEFINED: Build_ttslider  */,
    (void *)NULL /* UNDEFINED: Break_ttslider  */,
    (void *)NULL /* UNDEFINED: Del_ttslider  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttslider  */,
    (void *)NULL /* UNDEFINED: Remove_ttslider  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttslider  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttslider  */,
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
    (void *)NULL /* UNDEFINED: Build_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: Break_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: Del_ttprogressbar  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: Remove_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttprogressbar  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttprogressbar  */,
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
    (void *)NULL /* UNDEFINED: Build_ttscrollpane  */,
    (void *)NULL /* UNDEFINED: Break_ttscrollpane  */,
    (void *)NULL /* UNDEFINED: Del_ttscrollpane  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttscrollpane  */,
    (void *)NULL /* UNDEFINED: Remove_ttscrollpane  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttscrollpane  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttscrollpane  */,
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
    (void *)NULL /* UNDEFINED: Build_ttwindow  */,
    (void *)NULL /* UNDEFINED: Break_ttwindow  */,
    (void *)NULL /* UNDEFINED: Del_ttwindow  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttwindow  */,
    (void *)NULL /* UNDEFINED: Remove_ttwindow  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttwindow  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttwindow  */,
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
    (void *)NULL /* UNDEFINED: Build_ttframe  */,
    (void *)NULL /* UNDEFINED: Break_ttframe  */,
    (void *)NULL /* UNDEFINED: Del_ttframe  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttframe  */,
    (void *)NULL /* UNDEFINED: Remove_ttframe  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttframe  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttframe  */,
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
    (void *)NULL /* UNDEFINED: Build_ttscroller  */,
    (void *)NULL /* UNDEFINED: Break_ttscroller  */,
    (void *)NULL /* UNDEFINED: Del_ttscroller  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttscroller  */,
    (void *)NULL /* UNDEFINED: Remove_ttscroller  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttscroller  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttscroller  */,
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
    (void *)NULL /* UNDEFINED: Break_ttmenuitem  */,
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
    (void *)NULL /* UNDEFINED: Break_ttcheckmenuitem  */,
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
    (void *)NULL /* UNDEFINED: Break_ttradiomenuitem  */,
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
    (void *)NULL /* UNDEFINED: Break_ttmenu  */,
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
    (void *)NULL /* UNDEFINED: Break_ttmenubar  */,
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
    (void *)NULL /* UNDEFINED: Build_ttanytext  */,
    (void *)NULL /* UNDEFINED: Break_ttanytext  */,
    (void *)NULL /* UNDEFINED: Del_ttanytext  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_ttanytext  */,
    (void *)NULL /* UNDEFINED: Remove_ttanytext  */,
    (void *)NULL /* UNDEFINED: Invalidate_ttanytext  */,
    (void *)NULL /* UNDEFINED: SetVisible_ttanytext  */,
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
    (void *)NULL /* UNDEFINED: Build_tttextfield  */,
    (void *)NULL /* UNDEFINED: Break_tttextfield  */,
    (void *)NULL /* UNDEFINED: Del_tttextfield  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_tttextfield  */,
    (void *)NULL /* UNDEFINED: Remove_tttextfield  */,
    (void *)NULL /* UNDEFINED: Invalidate_tttextfield  */,
    (void *)NULL /* UNDEFINED: SetVisible_tttextfield  */,
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
    (void *)NULL /* UNDEFINED: Build_tttextarea  */,
    (void *)NULL /* UNDEFINED: Break_tttextarea  */,
    (void *)NULL /* UNDEFINED: Del_tttextarea  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */
    (void *)NULL /* UNDEFINED: AddTo_tttextarea  */,
    (void *)NULL /* UNDEFINED: Remove_tttextarea  */,
    (void *)NULL /* UNDEFINED: Invalidate_tttextarea  */,
    (void *)NULL /* UNDEFINED: SetVisible_tttextarea  */,
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
    (void *)NULL /* UNDEFINED: Break_tttheme  */,
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
    (void *)NULL /* UNDEFINED: Break_ttapplication  */,
    (void *)NULL /* UNDEFINED: Del_ttapplication  */,

    /* extends ttfn_ttobj */

    /* extends ttfn_ttcomponent */

  },



  {

    xml_Sync, 
    xml_Flush, 
    xml_TimidFlush, 
    xml_MainLoopOnce, 
    xml_DeleteCallback,    
    xml_Close, 
    xml_ConnectionFd, 
    xml_GetErrno, 
    xml_GetErrnoDetail,    
    xml_StrError, 
    xml_StrErrorDetail,   
  },
};



