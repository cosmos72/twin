
/* This file was automatically generated from m4/tree.m4, do not edit! */

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

/*
 *  tree_m4.h  --  internal header for libTT objects hierarchy tree
 */

#ifndef _TT_TREEM4_H
#define _TT_TREEM4_H



/*  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */





















#define TT_TYPELIST(el) \
el(void) el(ttbyte) el(ttshort) el(ttint) el(ttcol) el(ttfont) el(ttattr) el(ttopaque) el(ttany) el(ttfunction_fn) el(va_list) el(ttarg)

#define TT_LIST(el) \
 el(ttobj) el(ttclass) el(ttobject) el(ttvector) el(ttfield) el(ttmethod) el(ttevent) el(tteventbig) el(ttbitmask) el(tteventmask) el(ttcallback) el(ttlistener) el(tttimer) el(ttcomponent) el(ttdata) el(ttvisible) el(ttlayout) el(ttboxlayout) el(ttborderlayout) el(ttnative) el(ttwidget) el(ttlabel) el(tttooltip) el(ttbuttongroup) el(ttanybutton) el(ttbutton) el(ttcheckbutton) el(ttradiobutton) el(ttanyscroll) el(ttscrollbar) el(ttslider) el(ttprogressbar) el(ttscrollpane) el(ttwindow) el(ttframe) el(ttscroller) el(ttmenuitem) el(ttcheckmenuitem) el(ttradiomenuitem) el(ttmenu) el(ttmenubar) el(ttanytext) el(tttextfield) el(tttextarea) el(tttheme) el(ttapplication)

#define TT_NLIST(el) \
 el(ttclass) el(ttobject) el(ttvector) el(ttfield) el(ttmethod) el(ttevent) el(tteventbig) el(ttbitmask) el(tteventmask) el(ttcallback) el(ttlistener) el(tttimer) el(ttcomponent) el(ttdata) el(ttvisible) el(ttlayout) el(ttboxlayout) el(ttborderlayout) el(ttnative) el(ttwidget) el(ttlabel) el(tttooltip) el(ttbuttongroup) el(ttanybutton) el(ttbutton) el(ttcheckbutton) el(ttradiobutton) el(ttanyscroll) el(ttscrollbar) el(ttslider) el(ttprogressbar) el(ttscrollpane) el(ttwindow) el(ttframe) el(ttscroller) el(ttmenuitem) el(ttcheckmenuitem) el(ttradiomenuitem) el(ttmenu) el(ttmenubar) el(ttanytext) el(tttextfield) el(tttextarea) el(tttheme) el(ttapplication)

#define super_(t)	TT_CAT(super_,t)
#define order_(t)	TT_CAT(order_,t)
#define id_ttclass_(t)	TT_CAT(id_ttclass_,t)
#define magicmask_(t)	TT_CAT(magicmask_,t)
#define magic_(t)	TT_CAT(magic_,t)




/* set str_order_xxx defines for objects */


 
#define str_order_ttobj	"\x01" 
#define str_order_ttclass	"\x02" 
#define str_order_ttobject	"\x03" 
#define str_order_ttvector	"\x04" 
#define str_order_ttfield	"\x05" 
#define str_order_ttmethod	"\x06" 
#define str_order_ttevent	"\x07" 
#define str_order_tteventbig	"\x08" 
#define str_order_ttbitmask	"\x09" 
#define str_order_tteventmask	"\x0a" 
#define str_order_ttcallback	"\x0b" 
#define str_order_ttlistener	"\x0c" 
#define str_order_tttimer	"\x0d" 
#define str_order_ttcomponent	"\x0e" 
#define str_order_ttdata	"\x0f" 
#define str_order_ttvisible	"\x10" 
#define str_order_ttlayout	"\x11" 
#define str_order_ttboxlayout	"\x12" 
#define str_order_ttborderlayout	"\x13" 
#define str_order_ttnative	"\x14" 
#define str_order_ttwidget	"\x15" 
#define str_order_ttlabel	"\x16" 
#define str_order_tttooltip	"\x17" 
#define str_order_ttbuttongroup	"\x18" 
#define str_order_ttanybutton	"\x19" 
#define str_order_ttbutton	"\x1a" 
#define str_order_ttcheckbutton	"\x1b" 
#define str_order_ttradiobutton	"\x1c" 
#define str_order_ttanyscroll	"\x1d" 
#define str_order_ttscrollbar	"\x1e" 
#define str_order_ttslider	"\x1f" 
#define str_order_ttprogressbar	"\x20" 
#define str_order_ttscrollpane	"\x21" 
#define str_order_ttwindow	"\x22" 
#define str_order_ttframe	"\x23" 
#define str_order_ttscroller	"\x24" 
#define str_order_ttmenuitem	"\x25" 
#define str_order_ttcheckmenuitem	"\x26" 
#define str_order_ttradiomenuitem	"\x27" 
#define str_order_ttmenu	"\x28" 
#define str_order_ttmenubar	"\x29" 
#define str_order_ttanytext	"\x2a" 
#define str_order_tttextfield	"\x2b" 
#define str_order_tttextarea	"\x2c" 
#define str_order_tttheme	"\x2d" 
#define str_order_ttapplication	"\x2e"

#define str_order_first	str_order_ttobj

#define str_order_last	"\x2f"


/* set str_order_xxx defines for native types */


#define str_order_void	"\x30" 
#define str_order_ttbyte	"\x31" 
#define str_order_ttshort	"\x32" 
#define str_order_ttint	"\x33" 
#define str_order_ttcol	"\x34" 
#define str_order_ttfont	"\x35" 
#define str_order_ttattr	"\x36" 
#define str_order_ttopaque	"\x37" 
#define str_order_ttany	"\x38" 
#define str_order_ttfunction_fn	"\x39" 
#define str_order_va_list	"\x3a" 
#define str_order_ttarg	"\x3b"
    
#define str_order_type_first	str_order_void

#define str_order_type_last	"\x3c"


#define str_order_ttsbyte	str_order_ttbyte 
#define str_order_ttushort	str_order_ttshort 
#define str_order_ttuint	str_order_ttint










/* set order_xxx defines for objects */


    
#define order_first	0
#define order_ttobj	order_first
 
#define order_ttclass	1 
#define order_ttobject	2 
#define order_ttvector	3 
#define order_ttfield	4 
#define order_ttmethod	5 
#define order_ttevent	6 
#define order_tteventbig	7 
#define order_ttbitmask	8 
#define order_tteventmask	9 
#define order_ttcallback	10 
#define order_ttlistener	11 
#define order_tttimer	12 
#define order_ttcomponent	13 
#define order_ttdata	14 
#define order_ttvisible	15 
#define order_ttlayout	16 
#define order_ttboxlayout	17 
#define order_ttborderlayout	18 
#define order_ttnative	19 
#define order_ttwidget	20 
#define order_ttlabel	21 
#define order_tttooltip	22 
#define order_ttbuttongroup	23 
#define order_ttanybutton	24 
#define order_ttbutton	25 
#define order_ttcheckbutton	26 
#define order_ttradiobutton	27 
#define order_ttanyscroll	28 
#define order_ttscrollbar	29 
#define order_ttslider	30 
#define order_ttprogressbar	31 
#define order_ttscrollpane	32 
#define order_ttwindow	33 
#define order_ttframe	34 
#define order_ttscroller	35 
#define order_ttmenuitem	36 
#define order_ttcheckmenuitem	37 
#define order_ttradiomenuitem	38 
#define order_ttmenu	39 
#define order_ttmenubar	40 
#define order_ttanytext	41 
#define order_tttextfield	42 
#define order_tttextarea	43 
#define order_tttheme	44 
#define order_ttapplication	45
    
#define order_last	46

    
/* set order_xxx enums for native types */

#define order_void	47 
#define order_ttbyte	48 
#define order_ttshort	49 
#define order_ttint	50 
#define order_ttcol	51 
#define order_ttfont	52 
#define order_ttattr	53 
#define order_ttopaque	54 
#define order_ttany	55 
#define order_ttfunction_fn	56 
#define order_va_list	57 
#define order_ttarg	58
    


#define order_ttsbyte	order_ttbyte 
#define order_ttushort	order_ttshort 
#define order_ttuint	order_ttint

#define order_type_first	order_void,
#define order_type_last	(order_ttarg + 1)
    

#define order_ttclass_usedbits	6
#define order_ttclass_bitmask	63

#ifndef TT_SIZEOF_ttopaque
# error internal error: undefined TT_SIZEOF_ttopaque !
#endif

#if 8 * TT_SIZEOF_ttopaque < order_ttclass_usedbits + TT_MAGIC_SHIFT
# error internal error: order_ttclass_usedbits + TT_MAGIC_SHIFT overflows 8 * sizeof(ttopaque) !
#endif



/* set id_ttclass_xxx enums for objects */
typedef enum e_id_ttclass_ttobj {
    
    id_ttclass_first = (order_ttclass<<TT_MAGIC_SHIFT) | order_ttobj,
    id_ttclass_ttobj = id_ttclass_first,
 
    id_ttclass_ttclass, 
    id_ttclass_ttobject, 
    id_ttclass_ttvector, 
    id_ttclass_ttfield, 
    id_ttclass_ttmethod, 
    id_ttclass_ttevent, 
    id_ttclass_tteventbig, 
    id_ttclass_ttbitmask, 
    id_ttclass_tteventmask, 
    id_ttclass_ttcallback, 
    id_ttclass_ttlistener, 
    id_ttclass_tttimer, 
    id_ttclass_ttcomponent, 
    id_ttclass_ttdata, 
    id_ttclass_ttvisible, 
    id_ttclass_ttlayout, 
    id_ttclass_ttboxlayout, 
    id_ttclass_ttborderlayout, 
    id_ttclass_ttnative, 
    id_ttclass_ttwidget, 
    id_ttclass_ttlabel, 
    id_ttclass_tttooltip, 
    id_ttclass_ttbuttongroup, 
    id_ttclass_ttanybutton, 
    id_ttclass_ttbutton, 
    id_ttclass_ttcheckbutton, 
    id_ttclass_ttradiobutton, 
    id_ttclass_ttanyscroll, 
    id_ttclass_ttscrollbar, 
    id_ttclass_ttslider, 
    id_ttclass_ttprogressbar, 
    id_ttclass_ttscrollpane, 
    id_ttclass_ttwindow, 
    id_ttclass_ttframe, 
    id_ttclass_ttscroller, 
    id_ttclass_ttmenuitem, 
    id_ttclass_ttcheckmenuitem, 
    id_ttclass_ttradiomenuitem, 
    id_ttclass_ttmenu, 
    id_ttclass_ttmenubar, 
    id_ttclass_ttanytext, 
    id_ttclass_tttextfield, 
    id_ttclass_tttextarea, 
    id_ttclass_tttheme, 
    id_ttclass_ttapplication,
    id_ttclass_last,
    
/* set order_xxx enums for native types */

    id_ttclass_void, 
    id_ttclass_ttbyte, 
    id_ttclass_ttshort, 
    id_ttclass_ttint, 
    id_ttclass_ttcol, 
    id_ttclass_ttfont, 
    id_ttclass_ttattr, 
    id_ttclass_ttopaque, 
    id_ttclass_ttany, 
    id_ttclass_ttfunction_fn, 
    id_ttclass_va_list, 
    id_ttclass_ttarg,



    id_ttclass_ttsbyte	= id_ttclass_ttbyte, 
    id_ttclass_ttushort	= id_ttclass_ttshort, 
    id_ttclass_ttuint	= id_ttclass_ttint,

    id_ttclass_type_first = id_ttclass_void,
    id_ttclass_type_last = id_ttclass_ttarg + 1,
    
} e_id_ttclass_types;






/*
 * NOTE:
 *
 * magicmask_* and magic_* are autogenerated using a quite intricated m4 algorythm.
 *
 * the  effect is shown below, on a cutdown hierarchy tree.
 * in the picture, first number is magic_*, second is magicmask_*
 *
 *                         ________ttobj
 *                ________/   ____/(0,0)
 *       ________/       ____/       |
 *      /               /            |
 *ttcallback     ttevent        ttcomponent
 *(0x1,0x3)     (0x2,0x3)     ___(0x3,0x3)___
 *                      _____/    /     \    \_____
 *                _____/         /       \         \_____
 *               /              /         \              \
 *           _ttvisible_   ttbuttongroup   tttheme   ttapplication
 *          /(0x7,0x1F)\\__  (0xB,0x1F)   (0xF,0x1F)  (0x13,0x1F)
 *         /    |       \  \_________
 *        /     |        \           \
 * ttnative   ttwidget   ttmenuitem   ttmenubar
 *(0x27,0xFF)(0x47,0xFF) (0x67,0xFF) (0x87,0xFF)
 *
 * magic_* of a children is the parent's one, plus the progressive number
 * (conveniently shifted) of the children in the parent's children list.
 *
 * as you can see, all children of the same parent share the same magicmask;
 * this magicmask is the smallest bit mask that can hold all children magic_* numbers.
 * in other words, the tree is coded with a Huffman-like strategy
 * (node frequencies do not apply in this case)
 *
 * with this setup, runtime type checking and casting is extremely simple:
 * to check if an object of type A can be cast to type B, just do:
 *
 *  if ((magic_A & magicmask_B) == magic_B) {
 *      success;
 *  } else {
 *      fail;
 *  }
 * 
 */











/* set magicmask_xxx enums */
typedef enum e_magicmask_ttobj {
    magicmask_ttobj = 0, 
    magicmask_ttclass = 0x7, 
    magicmask_ttobject = 0x7, 
    magicmask_ttvector = 0x3f, 
    magicmask_ttfield = 0x7, 
    magicmask_ttmethod = 0x7, 
    magicmask_ttevent = 0x3f, 
    magicmask_tteventbig = 0x7f, 
    magicmask_ttbitmask = 0x3f, 
    magicmask_tteventmask = 0x3f, 
    magicmask_ttcallback = 0x3f, 
    magicmask_ttlistener = 0x7f, 
    magicmask_tttimer = 0xff, 
    magicmask_ttcomponent = 0x3f, 
    magicmask_ttdata = 0x1ff, 
    magicmask_ttvisible = 0x1ff, 
    magicmask_ttlayout = 0x1ff, 
    magicmask_ttboxlayout = 0x7ff, 
    magicmask_ttborderlayout = 0x7ff, 
    magicmask_ttnative = 0xfff, 
    magicmask_ttwidget = 0xfff, 
    magicmask_ttlabel = 0x7fff, 
    magicmask_tttooltip = 0xffff, 
    magicmask_ttbuttongroup = 0x1ff, 
    magicmask_ttanybutton = 0x7fff, 
    magicmask_ttbutton = 0x1ffff, 
    magicmask_ttcheckbutton = 0x3ffff, 
    magicmask_ttradiobutton = 0x7ffff, 
    magicmask_ttanyscroll = 0x1ffff, 
    magicmask_ttscrollbar = 0x7ffff, 
    magicmask_ttslider = 0x7ffff, 
    magicmask_ttprogressbar = 0x7ffff, 
    magicmask_ttscrollpane = 0x7fff, 
    magicmask_ttwindow = 0x7fff, 
    magicmask_ttframe = 0x1ffff, 
    magicmask_ttscroller = 0x1ffff, 
    magicmask_ttmenuitem = 0xfff, 
    magicmask_ttcheckmenuitem = 0x3fff, 
    magicmask_ttradiomenuitem = 0x7fff, 
    magicmask_ttmenu = 0x3fff, 
    magicmask_ttmenubar = 0xfff, 
    magicmask_ttanytext = 0x1ffff, 
    magicmask_tttextfield = 0x3ffff, 
    magicmask_tttextarea = 0x7ffff, 
    magicmask_tttheme = 0x1ff, 
    magicmask_ttapplication = 0x1ff,
    magicmask_last
} e_magicmask_ttobj;


/* set magic_xxx enums */
typedef enum e_magic_ttobj {
    magic_ttobj = 0,

 
    magic_ttclass = 0x1, 
    magic_ttobject = 0x2, 
    magic_ttvector = 0xa, 
    magic_ttfield = 0x3, 
    magic_ttmethod = 0x4, 
    magic_ttevent = 0x12, 
    magic_tteventbig = 0x52, 
    magic_ttbitmask = 0x1a, 
    magic_tteventmask = 0x22, 
    magic_ttcallback = 0x2a, 
    magic_ttlistener = 0x6a, 
    magic_tttimer = 0xea, 
    magic_ttcomponent = 0x32, 
    magic_ttdata = 0x72, 
    magic_ttvisible = 0xb2, 
    magic_ttlayout = 0xf2, 
    magic_ttboxlayout = 0x2f2, 
    magic_ttborderlayout = 0x4f2, 
    magic_ttnative = 0x2b2, 
    magic_ttwidget = 0x4b2, 
    magic_ttlabel = 0x14b2, 
    magic_tttooltip = 0x94b2, 
    magic_ttbuttongroup = 0x132, 
    magic_ttanybutton = 0x24b2, 
    magic_ttbutton = 0xa4b2, 
    magic_ttcheckbutton = 0x2a4b2, 
    magic_ttradiobutton = 0x6a4b2, 
    magic_ttanyscroll = 0x124b2, 
    magic_ttscrollbar = 0x324b2, 
    magic_ttslider = 0x524b2, 
    magic_ttprogressbar = 0x724b2, 
    magic_ttscrollpane = 0x34b2, 
    magic_ttwindow = 0x44b2, 
    magic_ttframe = 0xc4b2, 
    magic_ttscroller = 0x144b2, 
    magic_ttmenuitem = 0x6b2, 
    magic_ttcheckmenuitem = 0x16b2, 
    magic_ttradiomenuitem = 0x56b2, 
    magic_ttmenu = 0x26b2, 
    magic_ttmenubar = 0x8b2, 
    magic_ttanytext = 0x1c4b2, 
    magic_tttextfield = 0x3c4b2, 
    magic_tttextarea = 0x7c4b2, 
    magic_tttheme = 0x172, 
    magic_ttapplication = 0x1b2,
    magic_last
} e_magic_ttobj;








/* defines for existing object fields */

 
#define ttobj_field_first	0        
#define ttobj_field_last	0
 
#define ttclass_field_first	0            
#define ttclass_field_last	0
 
#define ttobject_field_first	0    
#define ttobject_refcount	0    
#define ttobject_oflags	1            
#define ttobject_user_data	2    
#define ttobject_events_inprogress	3    
#define ttobject_events_informed	4
#define ttobject_field_last	5
 
#define ttvector_field_first	5    
#define ttvector_array_size	5    
#define ttvector_array_n	6    
#define ttvector_array	7
#define ttvector_field_last	8
 
#define ttfield_field_first	8    
#define ttfield_name_len	8    
#define ttfield_name	9    
#define ttfield_type	10
#define ttfield_field_last	11
 
#define ttmethod_field_first	11    
#define ttmethod_name_len	11    
#define ttmethod_name	12    
#define ttmethod_address	13    
#define ttmethod_return_type	14    
#define ttmethod_parameter_type	15
#define ttmethod_field_last	16
 
#define ttevent_field_first	16    
#define ttevent_component	16    
#define ttevent_evtype	17    
#define ttevent_evcode	18    
#define ttevent_evflags	19    
#define ttevent_prev	20    
#define ttevent_next	21
#define ttevent_field_last	22
 
#define tteventbig_field_first	22    
#define tteventbig_x	22    
#define tteventbig_y	23    
#define tteventbig_w	24    
#define tteventbig_h	25    
#define tteventbig_value	26    
#define tteventbig_old_value	27    
#define tteventbig_data_len	28    
#define tteventbig_data	29
#define tteventbig_field_last	30
 
#define ttbitmask_field_first	30                
#define ttbitmask_field_last	30
 
#define tteventmask_field_first	30    
#define tteventmask_evtype_mask	30    
#define tteventmask_evcode_mask	31    
#define tteventmask_component_mask	32    
#define tteventmask_truth_table	33
#define tteventmask_field_last	34
 
#define ttcallback_field_first	34    
#define ttcallback_lflags	34    
#define ttcallback_component	35    
#define ttcallback_prev	36    
#define ttcallback_next	37
#define ttcallback_field_last	38
 
#define ttlistener_field_first	38        
#define ttlistener_event	38    
#define ttlistener_function	39    
#define ttlistener_arg_component_n	40    
#define ttlistener_arg_event_n	41    
#define ttlistener_args	42    
#define ttlistener_event_mask	43        
#define ttlistener_field_last	44
 
#define tttimer_field_first	44    
#define tttimer_delay_t	44    
#define tttimer_delay_f	45    
#define tttimer_timer_prev	46    
#define tttimer_timer_next	47
#define tttimer_field_last	48
 
#define ttcomponent_field_first	48    
#define ttcomponent_callbacks	48    
#define ttcomponent_listeners	49    
#define ttcomponent_timers	50    
#define ttcomponent_datas	51
#define ttcomponent_field_last	52
 
#define ttdata_field_first	52        
#define ttdata_component	52    
#define ttdata_key_len	53    
#define ttdata_key	54    
#define ttdata_data	55
#define ttdata_field_last	56
 
#define ttvisible_field_first	56    
#define ttvisible_vflags	56    
#define ttvisible_prev	57    
#define ttvisible_next	58    
#define ttvisible_parent	59    
#define ttvisible_child_first	60    
#define ttvisible_child_last	61    
#define ttvisible_constraint	62    
#define ttvisible_theme	63    
#define ttvisible_repaint	64
#define ttvisible_field_last	65
 
#define ttlayout_field_first	65    
#define ttlayout_widget	65
#define ttlayout_field_last	66
 
#define ttboxlayout_field_first	66    
#define ttboxlayout_orientation	66
#define ttboxlayout_field_last	67
 
#define ttborderlayout_field_first	67
#define ttborderlayout_field_last	67
 
#define ttnative_field_first	67
#define ttnative_field_last	67
 
#define ttwidget_field_first	67    
#define ttwidget_x	67    
#define ttwidget_y	68    
#define ttwidget_w	69    
#define ttwidget_h	70    
#define ttwidget_layout	71    
#define ttwidget_xl	72    
#define ttwidget_yl	73    
#define ttwidget_wl	74    
#define ttwidget_hl	75    
#define ttwidget_col	76    
#define ttwidget_tooltip	77
#define ttwidget_field_last	78
 
#define ttlabel_field_first	78    
#define ttlabel_text_len	78    
#define ttlabel_text	79
#define ttlabel_field_last	80
 
#define tttooltip_field_first	80        
#define tttooltip_widget	80
#define tttooltip_field_last	81
 
#define ttbuttongroup_field_first	81    
#define ttbuttongroup_group_first	81    
#define ttbuttongroup_group_last	82    
#define ttbuttongroup_checked	83
#define ttbuttongroup_field_last	84
 
#define ttanybutton_field_first	84    
#define ttanybutton_text_width	84    
#define ttanybutton_text_height	85    
#define ttanybutton_text	86    
#define ttanybutton_field_last	87
 
#define ttbutton_field_first	87
#define ttbutton_field_last	87
 
#define ttcheckbutton_field_first	87
#define ttcheckbutton_field_last	87
 
#define ttradiobutton_field_first	87    
#define ttradiobutton_group	87    
#define ttradiobutton_group_prev	88    
#define ttradiobutton_group_next	89
#define ttradiobutton_field_last	90
 
#define ttanyscroll_field_first	90    
#define ttanyscroll_orientation	90    
#define ttanyscroll_size	91    
#define ttanyscroll_real_size	92    
#define ttanyscroll_view_size	93        
#define ttanyscroll_position	94    
#define ttanyscroll_real_position	95    
#define ttanyscroll_state	96
#define ttanyscroll_field_last	97
 
#define ttscrollbar_field_first	97    
#define ttscrollbar_field_last	97
 
#define ttslider_field_first	97    
#define ttslider_slide_min	97    
#define ttslider_slide_max	98    
#define ttslider_slide_value	99
#define ttslider_field_last	100
 
#define ttprogressbar_field_first	100        
#define ttprogressbar_field_last	100
 
#define ttscrollpane_field_first	100    
#define ttscrollpane_bar_x	100    
#define ttscrollpane_bar_y	101
#define ttscrollpane_field_last	102
 
#define ttwindow_field_first	102    
#define ttwindow_title_len	102    
#define ttwindow_title	103
#define ttwindow_field_last	104
 
#define ttframe_field_first	104    
#define ttframe_menubar	104
#define ttframe_field_last	105
 
#define ttscroller_field_first	105    
#define ttscroller_scrollpane	105
#define ttscroller_field_last	106
 
#define ttmenuitem_field_first	106    
#define ttmenuitem_name_len	106    
#define ttmenuitem_name	107
#define ttmenuitem_field_last	108
 
#define ttcheckmenuitem_field_first	108
#define ttcheckmenuitem_field_last	108
 
#define ttradiomenuitem_field_first	108
#define ttradiomenuitem_field_last	108
 
#define ttmenu_field_first	108    
#define ttmenu_menubar	108
#define ttmenu_field_last	109
 
#define ttmenubar_field_first	109
#define ttmenubar_field_last	109
 
#define ttanytext_field_first	109    
#define ttanytext_text_len	109    
#define ttanytext_text	110
#define ttanytext_field_last	111
 
#define tttextfield_field_first	111
#define tttextfield_field_last	111
 
#define tttextarea_field_first	111
#define tttextarea_field_last	111
 
#define tttheme_field_first	111    
#define tttheme_name_len	111    
#define tttheme_name	112                
#define tttheme_field_last	113
 
#define ttapplication_field_first	113    
#define ttapplication_name_len	113    
#define ttapplication_name	114
#define ttapplication_field_last	115

#define ttobj_field_max	115











    


/* defines of existing object methods */


    /* generic functions */





#define ttmethod_LibraryVersion	0
#define ttmethod_ExitMainLoop	1
#define ttmethod_New	2
#define ttmethod_Del	3
#define ttmethod_ClassOf	4
#define ttmethod_SuperClassOf	5
#define ttmethod_InstanceOf	6
#define ttmethod_ClassNameOf	7






 
    /* ttobj methods */                    
#define ttmethod_GetField_ttobj	8
#define ttmethod_SetField_ttobj	9
#define ttmethod_ChangeField_ttobj	10 
    /* ttclass methods */
#define ttmethod_GetName_ttclass	11
#define ttmethod_GetSize_ttclass	12
#define ttmethod_GetSuper_ttclass	13
#define ttmethod_IsInstance_ttclass	14
#define ttmethod_FromType_ttclass	15
#define ttmethod_IsArrayType_ttclass	16
#define ttmethod_IsPrimitiveType_ttclass	17 
    /* ttobject methods */                
#define ttmethod_Ref_ttobject	18
#define ttmethod_Unref_ttobject	19
#define ttmethod_SetUserData_ttobject	20 
    /* ttvector methods */    
#define ttmethod_GetAt_ttvector	21    
#define ttmethod_SetAt_ttvector	22
#define ttmethod_Create1_ttvector	23
#define ttmethod_CreateA_ttvector	24
#define ttmethod_CreateR_ttvector	25
#define ttmethod_CreateV_ttvector	26
#define ttmethod_CreateY_ttvector	27
#define ttmethod_Append_ttvector	28
#define ttmethod_Add_ttvector	29
#define ttmethod_AddA_ttvector	30
#define ttmethod_AddR_ttvector	31
#define ttmethod_AddV_ttvector	32    
#define ttmethod_AddY_ttvector	33    
#define ttmethod_ContainsValue_ttvector	34
#define ttmethod_RemoveAt_ttvector	35    
#define ttmethod_RemoveRange_ttvector	36 
    /* ttfield methods */
#define ttmethod_Get_ttfield	37
#define ttmethod_GetValue_ttfield	38
#define ttmethod_SetValue_ttfield	39
#define ttmethod_ChangeValue_ttfield	40
#define ttmethod_ToEvcode_ttfield	41
#define ttmethod_FromEvcode_ttfield	42
#define ttmethod_GetEvcode_ttfield	43 
    /* ttmethod methods */
#define ttmethod_Get_ttmethod	44
#define ttmethod_GetByAddress_ttmethod	45
#define ttmethod_GetArraySizeA_ttmethod	46
#define ttmethod_CheckArraySizesA_ttmethod	47
#define ttmethod_CallA_ttmethod	48
#define ttmethod_CallR_ttmethod	49
#define ttmethod_CallV_ttmethod	50
#define ttmethod_CallY_ttmethod	51 
    /* ttevent methods */
#define ttmethod_Create_ttevent	52
#define ttmethod_Fire_ttevent	53 
    /* tteventbig methods */
#define ttmethod_Create8_tteventbig	54
#define ttmethod_Create4_tteventbig	55
#define ttmethod_Create4s4_tteventbig	56 
    /* ttbitmask methods */
#define ttmethod_Create_ttbitmask	57
#define ttmethod_CreateB2_ttbitmask	58
#define ttmethod_CreateR_ttbitmask	59
#define ttmethod_CreateR2_ttbitmask	60
#define ttmethod_Clear_ttbitmask	61
#define ttmethod_Fill_ttbitmask	62
#define ttmethod_Negate_ttbitmask	63
#define ttmethod_Change_ttbitmask	64
#define ttmethod_Contains_ttbitmask	65
#define ttmethod_Add_ttbitmask	66
#define ttmethod_AddB_ttbitmask	67
#define ttmethod_AddR_ttbitmask	68
#define ttmethod_Remove_ttbitmask	69
#define ttmethod_RemoveB_ttbitmask	70
#define ttmethod_RemoveR_ttbitmask	71
#define ttmethod_Combine_ttbitmask	72
#define ttmethod_CombineR_ttbitmask	73
#define ttmethod_CombineB_ttbitmask	74 
    /* tteventmask methods */
#define ttmethod_CreateB_tteventmask	75
#define ttmethod_CreateB2_tteventmask	76
#define ttmethod_SetEvtypeMask_tteventmask	77
#define ttmethod_SetEvcodeMask_tteventmask	78
#define ttmethod_SetComponentMask_tteventmask	79
#define ttmethod_SetTruthTable_tteventmask	80 
    /* ttcallback methods */     
    /* ttlistener methods */
#define ttmethod_Create_ttlistener	81
#define ttmethod_CreateE_ttlistener	82
#define ttmethod_CreateR_ttlistener	83
#define ttmethod_CreateDel_ttlistener	84
#define ttmethod_CreateAskclose_ttlistener	85
#define ttmethod_CreateKey_ttlistener	86
#define ttmethod_CreateMouse_ttlistener	87
#define ttmethod_CreateActivate_ttlistener	88
#define ttmethod_CreateChange_ttlistener	89
#define ttmethod_CreateKeyData_ttlistener	90
#define ttmethod_CreateExpose_ttlistener	91    
#define ttmethod_AddTo_ttlistener	92    
#define ttmethod_Remove_ttlistener	93
#define ttmethod_SetArgsR_ttlistener	94
#define ttmethod_SetEventMask_ttlistener	95 
    /* tttimer methods */
#define ttmethod_CreateR_tttimer	96
#define ttmethod_Create_tttimer	97
#define ttmethod_SetDelay_tttimer	98    
#define ttmethod_SetEnabled_tttimer	99    
#define ttmethod_IsEnabled_tttimer	100 
    /* ttcomponent methods */
#define ttmethod_AddListener_ttcomponent	101
#define ttmethod_RemoveListener_ttcomponent	102
#define ttmethod_SetKeyData_ttcomponent	103
#define ttmethod_GetKeyData_ttcomponent	104
#define ttmethod_UnsetKeyData_ttcomponent	105
#define ttmethod_GetData_ttcomponent	106 
    /* ttdata methods */        
#define ttmethod_SetData_ttdata	107 
    /* ttvisible methods */    
#define ttmethod_AddTo_ttvisible	108    
#define ttmethod_Validate_ttvisible	109    
#define ttmethod_Remove_ttvisible	110    
#define ttmethod_Invalidate_ttvisible	111    
#define ttmethod_SetVisible_ttvisible	112    
#define ttmethod_SetTheme_ttvisible	113    
#define ttmethod_Draw_ttvisible	114    
#define ttmethod_BuiltinRepaint_ttvisible	115

#define ttmethod_SetRepaint_ttvisible	116
#define ttmethod_Expose_ttvisible	117
#define ttmethod_Add_ttvisible	118 
    /* ttlayout methods */         
    /* ttboxlayout methods */
#define ttmethod_Create_ttboxlayout	119 
    /* ttborderlayout methods */ 
    /* ttnative methods */    
#define ttmethod_GetRoot_ttnative	120    
#define ttmethod_GetW_ttnative	121    
#define ttmethod_GetH_ttnative	122
#define ttmethod_Create_ttnative	123 
    /* ttwidget methods */    
#define ttmethod_SetLayout_ttwidget	124    
#define ttmethod_SetXY_ttwidget	125    
#define ttmethod_SetWH_ttwidget	126    
#define ttmethod_SetXl_ttwidget	127    
#define ttmethod_SetYl_ttwidget	128    
#define ttmethod_SetWl_ttwidget	129    
#define ttmethod_SetHl_ttwidget	130    
#define ttmethod_SetTooltip_ttwidget	131
#define ttmethod_SetXlYl_ttwidget	132
#define ttmethod_SetWlHl_ttwidget	133
#define ttmethod_SetXYWH_ttwidget	134
#define ttmethod_SetX_ttwidget	135
#define ttmethod_SetY_ttwidget	136
#define ttmethod_SetW_ttwidget	137
#define ttmethod_SetH_ttwidget	138 
    /* ttlabel methods */    
#define ttmethod_SetFontD_ttlabel	139
#define ttmethod_SetFont_ttlabel	140
#define ttmethod_SetText_ttlabel	141
#define ttmethod_Create_ttlabel	142 
    /* tttooltip methods */    
#define ttmethod_SetWidget_tttooltip	143 
    /* ttbuttongroup methods */    
#define ttmethod_Add_ttbuttongroup	144    
#define ttmethod_Remove_ttbuttongroup	145    
#define ttmethod_SetChecked_ttbuttongroup	146 
    /* ttanybutton methods */        
#define ttmethod_SetAttrD_ttanybutton	147
#define ttmethod_SetAttr_ttanybutton	148
#define ttmethod_SetFont_ttanybutton	149
#define ttmethod_SetText_ttanybutton	150
#define ttmethod_Create_ttanybutton	151 
    /* ttbutton methods */    
#define ttmethod_SetPressed_ttbutton	152
#define ttmethod_IsPressed_ttbutton	153
#define ttmethod_Create_ttbutton	154 
    /* ttcheckbutton methods */    
#define ttmethod_SetChecked_ttcheckbutton	155
#define ttmethod_IsChecked_ttcheckbutton	156
#define ttmethod_Create_ttcheckbutton	157 
    /* ttradiobutton methods */
#define ttmethod_AddToGroup_ttradiobutton	158
#define ttmethod_RemoveFromGroup_ttradiobutton	159
#define ttmethod_Create_ttradiobutton	160 
    /* ttanyscroll methods */            
#define ttmethod_SetOrientation_ttanyscroll	161    
#define ttmethod_SetSize_ttanyscroll	162    
#define ttmethod_SetRealSize_ttanyscroll	163    
#define ttmethod_SetViewSize_ttanyscroll	164    
#define ttmethod_SetPosition_ttanyscroll	165 
    /* ttscrollbar methods */
#define ttmethod_Create1_ttscrollbar	166
#define ttmethod_Create4_ttscrollbar	167 
    /* ttslider methods */    
#define ttmethod_SetSlideMin_ttslider	168    
#define ttmethod_SetSlideMax_ttslider	169    
#define ttmethod_SetSlideValue_ttslider	170
#define ttmethod_Create_ttslider	171 
    /* ttprogressbar methods */
#define ttmethod_Create_ttprogressbar	172 
    /* ttscrollpane methods */
#define ttmethod_Create_ttscrollpane	173    
#define ttmethod_SetBarX_ttscrollpane	174    
#define ttmethod_SetBarY_ttscrollpane	175 
    /* ttwindow methods */    
#define ttmethod_SetTitle_ttwindow	176 
    /* ttframe methods */    
#define ttmethod_SetMenubar_ttframe	177
#define ttmethod_Create_ttframe	178 
    /* ttscroller methods */
#define ttmethod_Create_ttscroller	179 
    /* ttmenuitem methods */ 
    /* ttcheckmenuitem methods */ 
    /* ttradiomenuitem methods */ 
    /* ttmenu methods */ 
    /* ttmenubar methods */
#define ttmethod_GetDefault_ttmenubar	180 
    /* ttanytext methods */ 
    /* tttextfield methods */ 
    /* tttextarea methods */ 
    /* tttheme methods */
#define ttmethod_GetDefault_tttheme	181 
    /* ttapplication methods */
#define ttmethod_Create_ttapplication	182
#define ttmethod_Get_ttapplication	183








    /* TTSet*_* methods omitted, they are listed above! */
    
#if 0
#define ttmethod_FN_set_first		184

#define ttmethod_SetField_ttobj	185
#define ttmethod_ChangeField_ttobj	186

#define ttmethod_FN_set_last		184
#endif

    /* TTGet*_* methods */

#define ttmethod_FN_get_first	184





 
    /* TTGet*_ttobj methods */         
    /* TTGet*_ttclass methods */             
    /* TTGet*_ttobject methods */    
#define ttmethod_GetRefcount_ttobject	184    
#define ttmethod_GetOflags_ttobject	185            
#define ttmethod_GetUserData_ttobject	186    
#define ttmethod_GetEventsInprogress_ttobject	187    
#define ttmethod_GetEventsInformed_ttobject	188 
    /* TTGet*_ttvector methods */    
#define ttmethod_GetArraySize_ttvector	189    
#define ttmethod_GetArrayN_ttvector	190    
#define ttmethod_GetArray_ttvector	191 
    /* TTGet*_ttfield methods */    
#define ttmethod_GetNameLen_ttfield	192    
#define ttmethod_GetName_ttfield	193    
#define ttmethod_GetType_ttfield	194 
    /* TTGet*_ttmethod methods */    
#define ttmethod_GetNameLen_ttmethod	195    
#define ttmethod_GetName_ttmethod	196    
#define ttmethod_GetAddress_ttmethod	197    
#define ttmethod_GetReturnType_ttmethod	198    
#define ttmethod_GetParameterType_ttmethod	199 
    /* TTGet*_ttevent methods */    
#define ttmethod_GetComponent_ttevent	200    
#define ttmethod_GetEvtype_ttevent	201    
#define ttmethod_GetEvcode_ttevent	202    
#define ttmethod_GetEvflags_ttevent	203    
#define ttmethod_GetPrev_ttevent	204    
#define ttmethod_GetNext_ttevent	205 
    /* TTGet*_tteventbig methods */    
#define ttmethod_GetX_tteventbig	206    
#define ttmethod_GetY_tteventbig	207    
#define ttmethod_GetW_tteventbig	208    
#define ttmethod_GetH_tteventbig	209    
#define ttmethod_GetValue_tteventbig	210    
#define ttmethod_GetOldValue_tteventbig	211    
#define ttmethod_GetDataLen_tteventbig	212    
#define ttmethod_GetData_tteventbig	213 
    /* TTGet*_ttbitmask methods */                 
    /* TTGet*_tteventmask methods */    
#define ttmethod_GetEvtypeMask_tteventmask	214    
#define ttmethod_GetEvcodeMask_tteventmask	215    
#define ttmethod_GetComponentMask_tteventmask	216    
#define ttmethod_GetTruthTable_tteventmask	217 
    /* TTGet*_ttcallback methods */    
#define ttmethod_GetLflags_ttcallback	218    
#define ttmethod_GetComponent_ttcallback	219    
#define ttmethod_GetPrev_ttcallback	220    
#define ttmethod_GetNext_ttcallback	221 
    /* TTGet*_ttlistener methods */        
#define ttmethod_GetEvent_ttlistener	222    
#define ttmethod_GetFunction_ttlistener	223    
#define ttmethod_GetArgComponentN_ttlistener	224    
#define ttmethod_GetArgEventN_ttlistener	225    
#define ttmethod_GetArgs_ttlistener	226    
#define ttmethod_GetEventMask_ttlistener	227         
    /* TTGet*_tttimer methods */    
#define ttmethod_GetDelayT_tttimer	228    
#define ttmethod_GetDelayF_tttimer	229    
#define ttmethod_GetTimerPrev_tttimer	230    
#define ttmethod_GetTimerNext_tttimer	231 
    /* TTGet*_ttcomponent methods */    
#define ttmethod_GetCallbacks_ttcomponent	232    
#define ttmethod_GetListeners_ttcomponent	233    
#define ttmethod_GetTimers_ttcomponent	234    
#define ttmethod_GetDatas_ttcomponent	235 
    /* TTGet*_ttdata methods */        
#define ttmethod_GetComponent_ttdata	236    
#define ttmethod_GetKeyLen_ttdata	237    
#define ttmethod_GetKey_ttdata	238    
#define ttmethod_GetData_ttdata	239 
    /* TTGet*_ttvisible methods */    
#define ttmethod_GetVflags_ttvisible	240    
#define ttmethod_GetPrev_ttvisible	241    
#define ttmethod_GetNext_ttvisible	242    
#define ttmethod_GetParent_ttvisible	243    
#define ttmethod_GetChildFirst_ttvisible	244    
#define ttmethod_GetChildLast_ttvisible	245    
#define ttmethod_GetConstraint_ttvisible	246    
#define ttmethod_GetTheme_ttvisible	247    
#define ttmethod_GetRepaint_ttvisible	248 
    /* TTGet*_ttlayout methods */    
#define ttmethod_GetWidget_ttlayout	249 
    /* TTGet*_ttboxlayout methods */    
#define ttmethod_GetOrientation_ttboxlayout	250 
    /* TTGet*_ttborderlayout methods */ 
    /* TTGet*_ttnative methods */ 
    /* TTGet*_ttwidget methods */    
#define ttmethod_GetX_ttwidget	251    
#define ttmethod_GetY_ttwidget	252    
#define ttmethod_GetW_ttwidget	253    
#define ttmethod_GetH_ttwidget	254    
#define ttmethod_GetLayout_ttwidget	255    
#define ttmethod_GetXl_ttwidget	256    
#define ttmethod_GetYl_ttwidget	257    
#define ttmethod_GetWl_ttwidget	258    
#define ttmethod_GetHl_ttwidget	259    
#define ttmethod_GetCol_ttwidget	260    
#define ttmethod_GetTooltip_ttwidget	261 
    /* TTGet*_ttlabel methods */    
#define ttmethod_GetTextLen_ttlabel	262    
#define ttmethod_GetText_ttlabel	263 
    /* TTGet*_tttooltip methods */        
#define ttmethod_GetWidget_tttooltip	264 
    /* TTGet*_ttbuttongroup methods */    
#define ttmethod_GetGroupFirst_ttbuttongroup	265    
#define ttmethod_GetGroupLast_ttbuttongroup	266    
#define ttmethod_GetChecked_ttbuttongroup	267 
    /* TTGet*_ttanybutton methods */    
#define ttmethod_GetTextWidth_ttanybutton	268    
#define ttmethod_GetTextHeight_ttanybutton	269    
#define ttmethod_GetText_ttanybutton	270     
    /* TTGet*_ttbutton methods */ 
    /* TTGet*_ttcheckbutton methods */ 
    /* TTGet*_ttradiobutton methods */    
#define ttmethod_GetGroup_ttradiobutton	271    
#define ttmethod_GetGroupPrev_ttradiobutton	272    
#define ttmethod_GetGroupNext_ttradiobutton	273 
    /* TTGet*_ttanyscroll methods */    
#define ttmethod_GetOrientation_ttanyscroll	274    
#define ttmethod_GetSize_ttanyscroll	275    
#define ttmethod_GetRealSize_ttanyscroll	276    
#define ttmethod_GetViewSize_ttanyscroll	277        
#define ttmethod_GetPosition_ttanyscroll	278    
#define ttmethod_GetRealPosition_ttanyscroll	279    
#define ttmethod_GetState_ttanyscroll	280 
    /* TTGet*_ttscrollbar methods */     
    /* TTGet*_ttslider methods */    
#define ttmethod_GetSlideMin_ttslider	281    
#define ttmethod_GetSlideMax_ttslider	282    
#define ttmethod_GetSlideValue_ttslider	283 
    /* TTGet*_ttprogressbar methods */         
    /* TTGet*_ttscrollpane methods */    
#define ttmethod_GetBarX_ttscrollpane	284    
#define ttmethod_GetBarY_ttscrollpane	285 
    /* TTGet*_ttwindow methods */    
#define ttmethod_GetTitleLen_ttwindow	286    
#define ttmethod_GetTitle_ttwindow	287 
    /* TTGet*_ttframe methods */    
#define ttmethod_GetMenubar_ttframe	288 
    /* TTGet*_ttscroller methods */    
#define ttmethod_GetScrollpane_ttscroller	289 
    /* TTGet*_ttmenuitem methods */    
#define ttmethod_GetNameLen_ttmenuitem	290    
#define ttmethod_GetName_ttmenuitem	291 
    /* TTGet*_ttcheckmenuitem methods */ 
    /* TTGet*_ttradiomenuitem methods */ 
    /* TTGet*_ttmenu methods */    
#define ttmethod_GetMenubar_ttmenu	292 
    /* TTGet*_ttmenubar methods */ 
    /* TTGet*_ttanytext methods */    
#define ttmethod_GetTextLen_ttanytext	293    
#define ttmethod_GetText_ttanytext	294 
    /* TTGet*_tttextfield methods */ 
    /* TTGet*_tttextarea methods */ 
    /* TTGet*_tttheme methods */    
#define ttmethod_GetNameLen_tttheme	295    
#define ttmethod_GetName_tttheme	296                 
    /* TTGet*_ttapplication methods */    
#define ttmethod_GetNameLen_ttapplication	297    
#define ttmethod_GetName_ttapplication	298





#define ttmethod_FN_get_last	299
#define ttmethod_FN_last 299



#endif /* _TT_TREEM4_H */

