
/* This file was automatically generated from m4/tree.m4, do not edit! */


/*
 *  tree_m4.h  --  internal header for libTT objects hierarchy tree
 */

#ifndef _TT_TREEM4_H
#define _TT_TREEM4_H

























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










/* set order_xxx enums for objects */
typedef enum e_order_ttobj {


    
    order_first = 0,
    order_ttobj = order_first,
 
    order_ttclass = 1, 
    order_ttobject = 2, 
    order_ttvector = 3, 
    order_ttfield = 4, 
    order_ttmethod = 5, 
    order_ttevent = 6, 
    order_tteventbig = 7, 
    order_ttbitmask = 8, 
    order_tteventmask = 9, 
    order_ttcallback = 10, 
    order_ttlistener = 11, 
    order_tttimer = 12, 
    order_ttcomponent = 13, 
    order_ttdata = 14, 
    order_ttvisible = 15, 
    order_ttlayout = 16, 
    order_ttboxlayout = 17, 
    order_ttborderlayout = 18, 
    order_ttnative = 19, 
    order_ttwidget = 20, 
    order_ttlabel = 21, 
    order_tttooltip = 22, 
    order_ttbuttongroup = 23, 
    order_ttanybutton = 24, 
    order_ttbutton = 25, 
    order_ttcheckbutton = 26, 
    order_ttradiobutton = 27, 
    order_ttanyscroll = 28, 
    order_ttscrollbar = 29, 
    order_ttslider = 30, 
    order_ttprogressbar = 31, 
    order_ttscrollpane = 32, 
    order_ttwindow = 33, 
    order_ttframe = 34, 
    order_ttscroller = 35, 
    order_ttmenuitem = 36, 
    order_ttcheckmenuitem = 37, 
    order_ttradiomenuitem = 38, 
    order_ttmenu = 39, 
    order_ttmenubar = 40, 
    order_ttanytext = 41, 
    order_tttextfield = 42, 
    order_tttextarea = 43, 
    order_tttheme = 44, 
    order_ttapplication = 45,
    
    order_last = 46,

    
/* set order_xxx enums for native types */

    order_void = 47, 
    order_ttbyte = 48, 
    order_ttshort = 49, 
    order_ttint = 50, 
    order_ttcol = 51, 
    order_ttfont = 52, 
    order_ttattr = 53, 
    order_ttopaque = 54, 
    order_ttany = 55, 
    order_ttfunction_fn = 56, 
    order_va_list = 57, 
    order_ttarg = 58,
    


    order_ttsbyte	= order_ttbyte, 
    order_ttushort	= order_ttshort, 
    order_ttuint	= order_ttint,

    order_type_first = order_void,
    order_type_last = order_ttarg + 1
    
} e_order_types;


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








/* enum for existing object fields */
typedef enum e_order_fields {
 
    ttobj_field_first	= 0,        
    ttobj_field_last	= 0,
 
    ttclass_field_first	= 0,            
    ttclass_field_last	= 0,
 
    ttobject_field_first	= 0,    
    ttobject_refcount	=     0,    
    ttobject_oflags	=     1,            
    ttobject_user_data	=     2,    
    ttobject_events_inprogress	=     3,    
    ttobject_events_informed	=     4,
    ttobject_field_last	= 5,
 
    ttvector_field_first	= 5,    
    ttvector_array_size	=     5,    
    ttvector_array_n	=     6,    
    ttvector_array	=     7,
    ttvector_field_last	= 8,
 
    ttfield_field_first	= 8,    
    ttfield_name_len	=     8,    
    ttfield_name	=     9,    
    ttfield_type	=     10,
    ttfield_field_last	= 11,
 
    ttmethod_field_first	= 11,    
    ttmethod_name_len	=     11,    
    ttmethod_name	=     12,    
    ttmethod_address	=     13,    
    ttmethod_return_type	=     14,    
    ttmethod_parameter_type	=     15,
    ttmethod_field_last	= 16,
 
    ttevent_field_first	= 16,    
    ttevent_component	=     16,    
    ttevent_evtype	=     17,    
    ttevent_evcode	=     18,    
    ttevent_evflags	=     19,    
    ttevent_prev	=     20,    
    ttevent_next	=     21,
    ttevent_field_last	= 22,
 
    tteventbig_field_first	= 22,    
    tteventbig_x	=     22,    
    tteventbig_y	=     23,    
    tteventbig_w	=     24,    
    tteventbig_h	=     25,    
    tteventbig_value	=     26,    
    tteventbig_old_value	=     27,    
    tteventbig_data_len	=     28,    
    tteventbig_data	=     29,
    tteventbig_field_last	= 30,
 
    ttbitmask_field_first	= 30,                
    ttbitmask_field_last	= 30,
 
    tteventmask_field_first	= 30,    
    tteventmask_evtype_mask	=     30,    
    tteventmask_evcode_mask	=     31,    
    tteventmask_component_mask	=     32,    
    tteventmask_truth_table	=     33,
    tteventmask_field_last	= 34,
 
    ttcallback_field_first	= 34,    
    ttcallback_lflags	=     34,    
    ttcallback_component	=     35,    
    ttcallback_prev	=     36,    
    ttcallback_next	=     37,
    ttcallback_field_last	= 38,
 
    ttlistener_field_first	= 38,        
    ttlistener_event	=     38,    
    ttlistener_function	=     39,    
    ttlistener_arg_component_n	=     40,    
    ttlistener_arg_event_n	=     41,    
    ttlistener_args	=     42,    
    ttlistener_event_mask	=     43,        
    ttlistener_field_last	= 44,
 
    tttimer_field_first	= 44,    
    tttimer_delay_t	=     44,    
    tttimer_delay_f	=     45,    
    tttimer_timer_prev	=     46,    
    tttimer_timer_next	=     47,
    tttimer_field_last	= 48,
 
    ttcomponent_field_first	= 48,    
    ttcomponent_callbacks	=     48,    
    ttcomponent_listeners	=     49,    
    ttcomponent_timers	=     50,    
    ttcomponent_datas	=     51,
    ttcomponent_field_last	= 52,
 
    ttdata_field_first	= 52,        
    ttdata_component	=     52,    
    ttdata_key_len	=     53,    
    ttdata_key	=     54,    
    ttdata_data	=     55,
    ttdata_field_last	= 56,
 
    ttvisible_field_first	= 56,    
    ttvisible_vflags	=     56,    
    ttvisible_prev	=     57,    
    ttvisible_next	=     58,    
    ttvisible_parent	=     59,    
    ttvisible_child_first	=     60,    
    ttvisible_child_last	=     61,    
    ttvisible_constraint	=     62,    
    ttvisible_theme	=     63,    
    ttvisible_repaint	=     64,
    ttvisible_field_last	= 65,
 
    ttlayout_field_first	= 65,    
    ttlayout_widget	=     65,
    ttlayout_field_last	= 66,
 
    ttboxlayout_field_first	= 66,    
    ttboxlayout_orientation	=     66,
    ttboxlayout_field_last	= 67,
 
    ttborderlayout_field_first	= 67,
    ttborderlayout_field_last	= 67,
 
    ttnative_field_first	= 67,
    ttnative_field_last	= 67,
 
    ttwidget_field_first	= 67,    
    ttwidget_x	=     67,    
    ttwidget_y	=     68,    
    ttwidget_w	=     69,    
    ttwidget_h	=     70,    
    ttwidget_layout	=     71,    
    ttwidget_xl	=     72,    
    ttwidget_yl	=     73,    
    ttwidget_wl	=     74,    
    ttwidget_hl	=     75,    
    ttwidget_col	=     76,    
    ttwidget_tooltip	=     77,
    ttwidget_field_last	= 78,
 
    ttlabel_field_first	= 78,    
    ttlabel_text_len	=     78,    
    ttlabel_text	=     79,
    ttlabel_field_last	= 80,
 
    tttooltip_field_first	= 80,        
    tttooltip_widget	=     80,
    tttooltip_field_last	= 81,
 
    ttbuttongroup_field_first	= 81,    
    ttbuttongroup_group_first	=     81,    
    ttbuttongroup_group_last	=     82,    
    ttbuttongroup_checked	=     83,
    ttbuttongroup_field_last	= 84,
 
    ttanybutton_field_first	= 84,    
    ttanybutton_text_width	=     84,    
    ttanybutton_text_height	=     85,    
    ttanybutton_text	=     86,    
    ttanybutton_field_last	= 87,
 
    ttbutton_field_first	= 87,
    ttbutton_field_last	= 87,
 
    ttcheckbutton_field_first	= 87,
    ttcheckbutton_field_last	= 87,
 
    ttradiobutton_field_first	= 87,    
    ttradiobutton_group	=     87,    
    ttradiobutton_group_prev	=     88,    
    ttradiobutton_group_next	=     89,
    ttradiobutton_field_last	= 90,
 
    ttanyscroll_field_first	= 90,    
    ttanyscroll_orientation	=     90,    
    ttanyscroll_size	=     91,    
    ttanyscroll_real_size	=     92,    
    ttanyscroll_view_size	=     93,        
    ttanyscroll_position	=     94,    
    ttanyscroll_real_position	=     95,    
    ttanyscroll_state	=     96,
    ttanyscroll_field_last	= 97,
 
    ttscrollbar_field_first	= 97,    
    ttscrollbar_field_last	= 97,
 
    ttslider_field_first	= 97,    
    ttslider_slide_min	=     97,    
    ttslider_slide_max	=     98,    
    ttslider_slide_value	=     99,
    ttslider_field_last	= 100,
 
    ttprogressbar_field_first	= 100,        
    ttprogressbar_field_last	= 100,
 
    ttscrollpane_field_first	= 100,    
    ttscrollpane_bar_x	=     100,    
    ttscrollpane_bar_y	=     101,
    ttscrollpane_field_last	= 102,
 
    ttwindow_field_first	= 102,    
    ttwindow_title_len	=     102,    
    ttwindow_title	=     103,
    ttwindow_field_last	= 104,
 
    ttframe_field_first	= 104,    
    ttframe_menubar	=     104,
    ttframe_field_last	= 105,
 
    ttscroller_field_first	= 105,    
    ttscroller_scrollpane	=     105,
    ttscroller_field_last	= 106,
 
    ttmenuitem_field_first	= 106,    
    ttmenuitem_name_len	=     106,    
    ttmenuitem_name	=     107,
    ttmenuitem_field_last	= 108,
 
    ttcheckmenuitem_field_first	= 108,
    ttcheckmenuitem_field_last	= 108,
 
    ttradiomenuitem_field_first	= 108,
    ttradiomenuitem_field_last	= 108,
 
    ttmenu_field_first	= 108,    
    ttmenu_menubar	=     108,
    ttmenu_field_last	= 109,
 
    ttmenubar_field_first	= 109,
    ttmenubar_field_last	= 109,
 
    ttanytext_field_first	= 109,    
    ttanytext_text_len	=     109,    
    ttanytext_text	=     110,
    ttanytext_field_last	= 111,
 
    tttextfield_field_first	= 111,
    tttextfield_field_last	= 111,
 
    tttextarea_field_first	= 111,
    tttextarea_field_last	= 111,
 
    tttheme_field_first	= 111,    
    tttheme_name_len	=     111,    
    tttheme_name	=     112,                
    tttheme_field_last	= 113,
 
    ttapplication_field_first	= 113,    
    ttapplication_name_len	=     113,    
    ttapplication_name	=     114,
    ttapplication_field_last	= 115,

    ttobj_field_max	= 115
} e_order_fields;










    
/* defines for existing object fields */
 

#define ttobj_field_first	ttobj_field_first        
#define ttobj_field_last	ttobj_field_last 

#define ttclass_field_first	ttclass_field_first            
#define ttclass_field_last	ttclass_field_last 

#define ttobject_field_first	ttobject_field_first    
#define ttobject_refcount	ttobject_refcount    
#define ttobject_oflags	ttobject_oflags            
#define ttobject_user_data	ttobject_user_data    
#define ttobject_events_inprogress	ttobject_events_inprogress    
#define ttobject_events_informed	ttobject_events_informed
#define ttobject_field_last	ttobject_field_last 

#define ttvector_field_first	ttvector_field_first    
#define ttvector_array_size	ttvector_array_size    
#define ttvector_array_n	ttvector_array_n    
#define ttvector_array	ttvector_array
#define ttvector_field_last	ttvector_field_last 

#define ttfield_field_first	ttfield_field_first    
#define ttfield_name_len	ttfield_name_len    
#define ttfield_name	ttfield_name    
#define ttfield_type	ttfield_type
#define ttfield_field_last	ttfield_field_last 

#define ttmethod_field_first	ttmethod_field_first    
#define ttmethod_name_len	ttmethod_name_len    
#define ttmethod_name	ttmethod_name    
#define ttmethod_address	ttmethod_address    
#define ttmethod_return_type	ttmethod_return_type    
#define ttmethod_parameter_type	ttmethod_parameter_type
#define ttmethod_field_last	ttmethod_field_last 

#define ttevent_field_first	ttevent_field_first    
#define ttevent_component	ttevent_component    
#define ttevent_evtype	ttevent_evtype    
#define ttevent_evcode	ttevent_evcode    
#define ttevent_evflags	ttevent_evflags    
#define ttevent_prev	ttevent_prev    
#define ttevent_next	ttevent_next
#define ttevent_field_last	ttevent_field_last 

#define tteventbig_field_first	tteventbig_field_first    
#define tteventbig_x	tteventbig_x    
#define tteventbig_y	tteventbig_y    
#define tteventbig_w	tteventbig_w    
#define tteventbig_h	tteventbig_h    
#define tteventbig_value	tteventbig_value    
#define tteventbig_old_value	tteventbig_old_value    
#define tteventbig_data_len	tteventbig_data_len    
#define tteventbig_data	tteventbig_data
#define tteventbig_field_last	tteventbig_field_last 

#define ttbitmask_field_first	ttbitmask_field_first                
#define ttbitmask_field_last	ttbitmask_field_last 

#define tteventmask_field_first	tteventmask_field_first    
#define tteventmask_evtype_mask	tteventmask_evtype_mask    
#define tteventmask_evcode_mask	tteventmask_evcode_mask    
#define tteventmask_component_mask	tteventmask_component_mask    
#define tteventmask_truth_table	tteventmask_truth_table
#define tteventmask_field_last	tteventmask_field_last 

#define ttcallback_field_first	ttcallback_field_first    
#define ttcallback_lflags	ttcallback_lflags    
#define ttcallback_component	ttcallback_component    
#define ttcallback_prev	ttcallback_prev    
#define ttcallback_next	ttcallback_next
#define ttcallback_field_last	ttcallback_field_last 

#define ttlistener_field_first	ttlistener_field_first        
#define ttlistener_event	ttlistener_event    
#define ttlistener_function	ttlistener_function    
#define ttlistener_arg_component_n	ttlistener_arg_component_n    
#define ttlistener_arg_event_n	ttlistener_arg_event_n    
#define ttlistener_args	ttlistener_args    
#define ttlistener_event_mask	ttlistener_event_mask        
#define ttlistener_field_last	ttlistener_field_last 

#define tttimer_field_first	tttimer_field_first    
#define tttimer_delay_t	tttimer_delay_t    
#define tttimer_delay_f	tttimer_delay_f    
#define tttimer_timer_prev	tttimer_timer_prev    
#define tttimer_timer_next	tttimer_timer_next
#define tttimer_field_last	tttimer_field_last 

#define ttcomponent_field_first	ttcomponent_field_first    
#define ttcomponent_callbacks	ttcomponent_callbacks    
#define ttcomponent_listeners	ttcomponent_listeners    
#define ttcomponent_timers	ttcomponent_timers    
#define ttcomponent_datas	ttcomponent_datas
#define ttcomponent_field_last	ttcomponent_field_last 

#define ttdata_field_first	ttdata_field_first        
#define ttdata_component	ttdata_component    
#define ttdata_key_len	ttdata_key_len    
#define ttdata_key	ttdata_key    
#define ttdata_data	ttdata_data
#define ttdata_field_last	ttdata_field_last 

#define ttvisible_field_first	ttvisible_field_first    
#define ttvisible_vflags	ttvisible_vflags    
#define ttvisible_prev	ttvisible_prev    
#define ttvisible_next	ttvisible_next    
#define ttvisible_parent	ttvisible_parent    
#define ttvisible_child_first	ttvisible_child_first    
#define ttvisible_child_last	ttvisible_child_last    
#define ttvisible_constraint	ttvisible_constraint    
#define ttvisible_theme	ttvisible_theme    
#define ttvisible_repaint	ttvisible_repaint
#define ttvisible_field_last	ttvisible_field_last 

#define ttlayout_field_first	ttlayout_field_first    
#define ttlayout_widget	ttlayout_widget
#define ttlayout_field_last	ttlayout_field_last 

#define ttboxlayout_field_first	ttboxlayout_field_first    
#define ttboxlayout_orientation	ttboxlayout_orientation
#define ttboxlayout_field_last	ttboxlayout_field_last 

#define ttborderlayout_field_first	ttborderlayout_field_first
#define ttborderlayout_field_last	ttborderlayout_field_last 

#define ttnative_field_first	ttnative_field_first
#define ttnative_field_last	ttnative_field_last 

#define ttwidget_field_first	ttwidget_field_first    
#define ttwidget_x	ttwidget_x    
#define ttwidget_y	ttwidget_y    
#define ttwidget_w	ttwidget_w    
#define ttwidget_h	ttwidget_h    
#define ttwidget_layout	ttwidget_layout    
#define ttwidget_xl	ttwidget_xl    
#define ttwidget_yl	ttwidget_yl    
#define ttwidget_wl	ttwidget_wl    
#define ttwidget_hl	ttwidget_hl    
#define ttwidget_col	ttwidget_col    
#define ttwidget_tooltip	ttwidget_tooltip
#define ttwidget_field_last	ttwidget_field_last 

#define ttlabel_field_first	ttlabel_field_first    
#define ttlabel_text_len	ttlabel_text_len    
#define ttlabel_text	ttlabel_text
#define ttlabel_field_last	ttlabel_field_last 

#define tttooltip_field_first	tttooltip_field_first        
#define tttooltip_widget	tttooltip_widget
#define tttooltip_field_last	tttooltip_field_last 

#define ttbuttongroup_field_first	ttbuttongroup_field_first    
#define ttbuttongroup_group_first	ttbuttongroup_group_first    
#define ttbuttongroup_group_last	ttbuttongroup_group_last    
#define ttbuttongroup_checked	ttbuttongroup_checked
#define ttbuttongroup_field_last	ttbuttongroup_field_last 

#define ttanybutton_field_first	ttanybutton_field_first    
#define ttanybutton_text_width	ttanybutton_text_width    
#define ttanybutton_text_height	ttanybutton_text_height    
#define ttanybutton_text	ttanybutton_text    
#define ttanybutton_field_last	ttanybutton_field_last 

#define ttbutton_field_first	ttbutton_field_first
#define ttbutton_field_last	ttbutton_field_last 

#define ttcheckbutton_field_first	ttcheckbutton_field_first
#define ttcheckbutton_field_last	ttcheckbutton_field_last 

#define ttradiobutton_field_first	ttradiobutton_field_first    
#define ttradiobutton_group	ttradiobutton_group    
#define ttradiobutton_group_prev	ttradiobutton_group_prev    
#define ttradiobutton_group_next	ttradiobutton_group_next
#define ttradiobutton_field_last	ttradiobutton_field_last 

#define ttanyscroll_field_first	ttanyscroll_field_first    
#define ttanyscroll_orientation	ttanyscroll_orientation    
#define ttanyscroll_size	ttanyscroll_size    
#define ttanyscroll_real_size	ttanyscroll_real_size    
#define ttanyscroll_view_size	ttanyscroll_view_size        
#define ttanyscroll_position	ttanyscroll_position    
#define ttanyscroll_real_position	ttanyscroll_real_position    
#define ttanyscroll_state	ttanyscroll_state
#define ttanyscroll_field_last	ttanyscroll_field_last 

#define ttscrollbar_field_first	ttscrollbar_field_first    
#define ttscrollbar_field_last	ttscrollbar_field_last 

#define ttslider_field_first	ttslider_field_first    
#define ttslider_slide_min	ttslider_slide_min    
#define ttslider_slide_max	ttslider_slide_max    
#define ttslider_slide_value	ttslider_slide_value
#define ttslider_field_last	ttslider_field_last 

#define ttprogressbar_field_first	ttprogressbar_field_first        
#define ttprogressbar_field_last	ttprogressbar_field_last 

#define ttscrollpane_field_first	ttscrollpane_field_first    
#define ttscrollpane_bar_x	ttscrollpane_bar_x    
#define ttscrollpane_bar_y	ttscrollpane_bar_y
#define ttscrollpane_field_last	ttscrollpane_field_last 

#define ttwindow_field_first	ttwindow_field_first    
#define ttwindow_title_len	ttwindow_title_len    
#define ttwindow_title	ttwindow_title
#define ttwindow_field_last	ttwindow_field_last 

#define ttframe_field_first	ttframe_field_first    
#define ttframe_menubar	ttframe_menubar
#define ttframe_field_last	ttframe_field_last 

#define ttscroller_field_first	ttscroller_field_first    
#define ttscroller_scrollpane	ttscroller_scrollpane
#define ttscroller_field_last	ttscroller_field_last 

#define ttmenuitem_field_first	ttmenuitem_field_first    
#define ttmenuitem_name_len	ttmenuitem_name_len    
#define ttmenuitem_name	ttmenuitem_name
#define ttmenuitem_field_last	ttmenuitem_field_last 

#define ttcheckmenuitem_field_first	ttcheckmenuitem_field_first
#define ttcheckmenuitem_field_last	ttcheckmenuitem_field_last 

#define ttradiomenuitem_field_first	ttradiomenuitem_field_first
#define ttradiomenuitem_field_last	ttradiomenuitem_field_last 

#define ttmenu_field_first	ttmenu_field_first    
#define ttmenu_menubar	ttmenu_menubar
#define ttmenu_field_last	ttmenu_field_last 

#define ttmenubar_field_first	ttmenubar_field_first
#define ttmenubar_field_last	ttmenubar_field_last 

#define ttanytext_field_first	ttanytext_field_first    
#define ttanytext_text_len	ttanytext_text_len    
#define ttanytext_text	ttanytext_text
#define ttanytext_field_last	ttanytext_field_last 

#define tttextfield_field_first	tttextfield_field_first
#define tttextfield_field_last	tttextfield_field_last 

#define tttextarea_field_first	tttextarea_field_first
#define tttextarea_field_last	tttextarea_field_last 

#define tttheme_field_first	tttheme_field_first    
#define tttheme_name_len	tttheme_name_len    
#define tttheme_name	tttheme_name                
#define tttheme_field_last	tttheme_field_last 

#define ttapplication_field_first	ttapplication_field_first    
#define ttapplication_name_len	ttapplication_name_len    
#define ttapplication_name	ttapplication_name
#define ttapplication_field_last	ttapplication_field_last


#define ttobj_field_max	ttobj_field_max










/* enum of existing object methods */

typedef enum e_order_methods {

    /* generic functions */





    ttmethod_LibraryVersion	= 0,
    ttmethod_ExitMainLoop	= 1,
    ttmethod_New	= 2,
    ttmethod_Del	= 3,
    ttmethod_ClassOf	= 4,
    ttmethod_SuperClassOf	= 5,
    ttmethod_InstanceOf	= 6,
    ttmethod_ClassNameOf	= 7,






 
    /* ttobj methods */                    
    ttmethod_GetField_ttobj	= 8,
    ttmethod_SetField_ttobj	= 9,
    ttmethod_ChangeField_ttobj	= 10, 
    /* ttclass methods */
    ttmethod_GetName_ttclass	= 11,
    ttmethod_GetSize_ttclass	= 12,
    ttmethod_GetSuper_ttclass	= 13,
    ttmethod_IsInstance_ttclass	= 14,
    ttmethod_FromType_ttclass	= 15,
    ttmethod_IsArrayType_ttclass	= 16,
    ttmethod_IsPrimitiveType_ttclass	= 17, 
    /* ttobject methods */                
    ttmethod_Ref_ttobject	= 18,
    ttmethod_Unref_ttobject	= 19,
    ttmethod_SetUserData_ttobject	= 20, 
    /* ttvector methods */    
    ttmethod_GetAt_ttvector	= 21,    
    ttmethod_SetAt_ttvector	= 22,
    ttmethod_Create1_ttvector	= 23,
    ttmethod_CreateA_ttvector	= 24,
    ttmethod_CreateR_ttvector	= 25,
    ttmethod_CreateV_ttvector	= 26,
    ttmethod_CreateY_ttvector	= 27,
    ttmethod_Append_ttvector	= 28,
    ttmethod_Add_ttvector	= 29,
    ttmethod_AddA_ttvector	= 30,
    ttmethod_AddR_ttvector	= 31,
    ttmethod_AddV_ttvector	= 32,    
    ttmethod_AddY_ttvector	= 33,    
    ttmethod_ContainsValue_ttvector	= 34,
    ttmethod_RemoveAt_ttvector	= 35,    
    ttmethod_RemoveRange_ttvector	= 36, 
    /* ttfield methods */
    ttmethod_Get_ttfield	= 37,
    ttmethod_GetValue_ttfield	= 38,
    ttmethod_SetValue_ttfield	= 39,
    ttmethod_ChangeValue_ttfield	= 40,
    ttmethod_ToEvcode_ttfield	= 41,
    ttmethod_FromEvcode_ttfield	= 42,
    ttmethod_GetEvcode_ttfield	= 43, 
    /* ttmethod methods */
    ttmethod_Get_ttmethod	= 44,
    ttmethod_GetByAddress_ttmethod	= 45,
    ttmethod_GetArraySizeA_ttmethod	= 46,
    ttmethod_CheckArraySizesA_ttmethod	= 47,
    ttmethod_CallA_ttmethod	= 48,
    ttmethod_CallR_ttmethod	= 49,
    ttmethod_CallV_ttmethod	= 50,
    ttmethod_CallY_ttmethod	= 51, 
    /* ttevent methods */
    ttmethod_Create_ttevent	= 52,
    ttmethod_Fire_ttevent	= 53, 
    /* tteventbig methods */
    ttmethod_Create8_tteventbig	= 54,
    ttmethod_Create4_tteventbig	= 55,
    ttmethod_Create4s4_tteventbig	= 56, 
    /* ttbitmask methods */
    ttmethod_Create_ttbitmask	= 57,
    ttmethod_CreateB2_ttbitmask	= 58,
    ttmethod_CreateR_ttbitmask	= 59,
    ttmethod_CreateR2_ttbitmask	= 60,
    ttmethod_Clear_ttbitmask	= 61,
    ttmethod_Fill_ttbitmask	= 62,
    ttmethod_Negate_ttbitmask	= 63,
    ttmethod_Change_ttbitmask	= 64,
    ttmethod_Contains_ttbitmask	= 65,
    ttmethod_Add_ttbitmask	= 66,
    ttmethod_AddB_ttbitmask	= 67,
    ttmethod_AddR_ttbitmask	= 68,
    ttmethod_Remove_ttbitmask	= 69,
    ttmethod_RemoveB_ttbitmask	= 70,
    ttmethod_RemoveR_ttbitmask	= 71,
    ttmethod_Combine_ttbitmask	= 72,
    ttmethod_CombineR_ttbitmask	= 73,
    ttmethod_CombineB_ttbitmask	= 74, 
    /* tteventmask methods */
    ttmethod_CreateB_tteventmask	= 75,
    ttmethod_CreateB2_tteventmask	= 76,
    ttmethod_SetEvtypeMask_tteventmask	= 77,
    ttmethod_SetEvcodeMask_tteventmask	= 78,
    ttmethod_SetComponentMask_tteventmask	= 79,
    ttmethod_SetTruthTable_tteventmask	= 80, 
    /* ttcallback methods */     
    /* ttlistener methods */
    ttmethod_Create_ttlistener	= 81,
    ttmethod_CreateE_ttlistener	= 82,
    ttmethod_CreateR_ttlistener	= 83,
    ttmethod_CreateDel_ttlistener	= 84,
    ttmethod_CreateAskclose_ttlistener	= 85,
    ttmethod_CreateKey_ttlistener	= 86,
    ttmethod_CreateMouse_ttlistener	= 87,
    ttmethod_CreateActivate_ttlistener	= 88,
    ttmethod_CreateChange_ttlistener	= 89,
    ttmethod_CreateKeyData_ttlistener	= 90,
    ttmethod_CreateExpose_ttlistener	= 91,    
    ttmethod_AddTo_ttlistener	= 92,    
    ttmethod_Remove_ttlistener	= 93,
    ttmethod_SetArgsR_ttlistener	= 94,
    ttmethod_SetEventMask_ttlistener	= 95, 
    /* tttimer methods */
    ttmethod_CreateR_tttimer	= 96,
    ttmethod_Create_tttimer	= 97,
    ttmethod_SetDelay_tttimer	= 98,    
    ttmethod_SetEnabled_tttimer	= 99,    
    ttmethod_IsEnabled_tttimer	= 100, 
    /* ttcomponent methods */
    ttmethod_AddListener_ttcomponent	= 101,
    ttmethod_RemoveListener_ttcomponent	= 102,
    ttmethod_SetKeyData_ttcomponent	= 103,
    ttmethod_GetKeyData_ttcomponent	= 104,
    ttmethod_UnsetKeyData_ttcomponent	= 105,
    ttmethod_GetData_ttcomponent	= 106, 
    /* ttdata methods */        
    ttmethod_SetData_ttdata	= 107, 
    /* ttvisible methods */    
    ttmethod_AddTo_ttvisible	= 108,    
    ttmethod_Validate_ttvisible	= 109,    
    ttmethod_Remove_ttvisible	= 110,    
    ttmethod_Invalidate_ttvisible	= 111,    
    ttmethod_SetVisible_ttvisible	= 112,    
    ttmethod_SetTheme_ttvisible	= 113,    
    ttmethod_Draw_ttvisible	= 114,    
    ttmethod_BuiltinRepaint_ttvisible	= 115,

    ttmethod_SetRepaint_ttvisible	= 116,
    ttmethod_Expose_ttvisible	= 117,
    ttmethod_Add_ttvisible	= 118, 
    /* ttlayout methods */         
    /* ttboxlayout methods */
    ttmethod_Create_ttboxlayout	= 119, 
    /* ttborderlayout methods */ 
    /* ttnative methods */    
    ttmethod_GetRoot_ttnative	= 120,    
    ttmethod_GetW_ttnative	= 121,    
    ttmethod_GetH_ttnative	= 122,
    ttmethod_Create_ttnative	= 123, 
    /* ttwidget methods */    
    ttmethod_SetLayout_ttwidget	= 124,    
    ttmethod_SetXY_ttwidget	= 125,    
    ttmethod_SetWH_ttwidget	= 126,    
    ttmethod_SetXl_ttwidget	= 127,    
    ttmethod_SetYl_ttwidget	= 128,    
    ttmethod_SetWl_ttwidget	= 129,    
    ttmethod_SetHl_ttwidget	= 130,    
    ttmethod_SetTooltip_ttwidget	= 131,
    ttmethod_SetXlYl_ttwidget	= 132,
    ttmethod_SetWlHl_ttwidget	= 133,
    ttmethod_SetXYWH_ttwidget	= 134,
    ttmethod_SetX_ttwidget	= 135,
    ttmethod_SetY_ttwidget	= 136,
    ttmethod_SetW_ttwidget	= 137,
    ttmethod_SetH_ttwidget	= 138, 
    /* ttlabel methods */    
    ttmethod_SetFontD_ttlabel	= 139,
    ttmethod_SetFont_ttlabel	= 140,
    ttmethod_SetText_ttlabel	= 141,
    ttmethod_Create_ttlabel	= 142, 
    /* tttooltip methods */    
    ttmethod_SetWidget_tttooltip	= 143, 
    /* ttbuttongroup methods */    
    ttmethod_Add_ttbuttongroup	= 144,    
    ttmethod_Remove_ttbuttongroup	= 145,    
    ttmethod_SetChecked_ttbuttongroup	= 146, 
    /* ttanybutton methods */        
    ttmethod_SetAttrD_ttanybutton	= 147,
    ttmethod_SetAttr_ttanybutton	= 148,
    ttmethod_SetFont_ttanybutton	= 149,
    ttmethod_SetText_ttanybutton	= 150,
    ttmethod_Create_ttanybutton	= 151, 
    /* ttbutton methods */    
    ttmethod_SetPressed_ttbutton	= 152,
    ttmethod_IsPressed_ttbutton	= 153,
    ttmethod_Create_ttbutton	= 154, 
    /* ttcheckbutton methods */    
    ttmethod_SetChecked_ttcheckbutton	= 155,
    ttmethod_IsChecked_ttcheckbutton	= 156,
    ttmethod_Create_ttcheckbutton	= 157, 
    /* ttradiobutton methods */
    ttmethod_AddToGroup_ttradiobutton	= 158,
    ttmethod_RemoveFromGroup_ttradiobutton	= 159,
    ttmethod_Create_ttradiobutton	= 160, 
    /* ttanyscroll methods */            
    ttmethod_SetOrientation_ttanyscroll	= 161,    
    ttmethod_SetSize_ttanyscroll	= 162,    
    ttmethod_SetRealSize_ttanyscroll	= 163,    
    ttmethod_SetViewSize_ttanyscroll	= 164,    
    ttmethod_SetPosition_ttanyscroll	= 165, 
    /* ttscrollbar methods */
    ttmethod_Create1_ttscrollbar	= 166,
    ttmethod_Create4_ttscrollbar	= 167, 
    /* ttslider methods */    
    ttmethod_SetSlideMin_ttslider	= 168,    
    ttmethod_SetSlideMax_ttslider	= 169,    
    ttmethod_SetSlideValue_ttslider	= 170,
    ttmethod_Create_ttslider	= 171, 
    /* ttprogressbar methods */
    ttmethod_Create_ttprogressbar	= 172, 
    /* ttscrollpane methods */
    ttmethod_Create_ttscrollpane	= 173,    
    ttmethod_SetBarX_ttscrollpane	= 174,    
    ttmethod_SetBarY_ttscrollpane	= 175, 
    /* ttwindow methods */    
    ttmethod_SetTitle_ttwindow	= 176, 
    /* ttframe methods */    
    ttmethod_SetMenubar_ttframe	= 177,
    ttmethod_Create_ttframe	= 178, 
    /* ttscroller methods */
    ttmethod_Create_ttscroller	= 179, 
    /* ttmenuitem methods */ 
    /* ttcheckmenuitem methods */ 
    /* ttradiomenuitem methods */ 
    /* ttmenu methods */ 
    /* ttmenubar methods */
    ttmethod_GetDefault_ttmenubar	= 180, 
    /* ttanytext methods */ 
    /* tttextfield methods */ 
    /* tttextarea methods */ 
    /* tttheme methods */
    ttmethod_GetDefault_tttheme	= 181, 
    /* ttapplication methods */
    ttmethod_Create_ttapplication	= 182,
    ttmethod_Get_ttapplication	= 183,








    /* TTSet*_* methods omitted, they are listed above! */
    
#if 0
    ttmethod_FN_set_first	= 184,

    ttmethod_SetField_ttobj,
    ttmethod_ChangeField_ttobj,

    ttmethod_FN_set_last	= 184,
#endif

    /* TTGet*_* methods */

    ttmethod_FN_get_first	= 184,





 
    /* TTGet*_ttobj methods */         
    /* TTGet*_ttclass methods */             
    /* TTGet*_ttobject methods */    
    ttmethod_GetRefcount_ttobject	= 184,    
    ttmethod_GetOflags_ttobject	= 185,            
    ttmethod_GetUserData_ttobject	= 186,    
    ttmethod_GetEventsInprogress_ttobject	= 187,    
    ttmethod_GetEventsInformed_ttobject	= 188, 
    /* TTGet*_ttvector methods */    
    ttmethod_GetArraySize_ttvector	= 189,    
    ttmethod_GetArrayN_ttvector	= 190,    
    ttmethod_GetArray_ttvector	= 191, 
    /* TTGet*_ttfield methods */    
    ttmethod_GetNameLen_ttfield	= 192,    
    ttmethod_GetName_ttfield	= 193,    
    ttmethod_GetType_ttfield	= 194, 
    /* TTGet*_ttmethod methods */    
    ttmethod_GetNameLen_ttmethod	= 195,    
    ttmethod_GetName_ttmethod	= 196,    
    ttmethod_GetAddress_ttmethod	= 197,    
    ttmethod_GetReturnType_ttmethod	= 198,    
    ttmethod_GetParameterType_ttmethod	= 199, 
    /* TTGet*_ttevent methods */    
    ttmethod_GetComponent_ttevent	= 200,    
    ttmethod_GetEvtype_ttevent	= 201,    
    ttmethod_GetEvcode_ttevent	= 202,    
    ttmethod_GetEvflags_ttevent	= 203,    
    ttmethod_GetPrev_ttevent	= 204,    
    ttmethod_GetNext_ttevent	= 205, 
    /* TTGet*_tteventbig methods */    
    ttmethod_GetX_tteventbig	= 206,    
    ttmethod_GetY_tteventbig	= 207,    
    ttmethod_GetW_tteventbig	= 208,    
    ttmethod_GetH_tteventbig	= 209,    
    ttmethod_GetValue_tteventbig	= 210,    
    ttmethod_GetOldValue_tteventbig	= 211,    
    ttmethod_GetDataLen_tteventbig	= 212,    
    ttmethod_GetData_tteventbig	= 213, 
    /* TTGet*_ttbitmask methods */                 
    /* TTGet*_tteventmask methods */    
    ttmethod_GetEvtypeMask_tteventmask	= 214,    
    ttmethod_GetEvcodeMask_tteventmask	= 215,    
    ttmethod_GetComponentMask_tteventmask	= 216,    
    ttmethod_GetTruthTable_tteventmask	= 217, 
    /* TTGet*_ttcallback methods */    
    ttmethod_GetLflags_ttcallback	= 218,    
    ttmethod_GetComponent_ttcallback	= 219,    
    ttmethod_GetPrev_ttcallback	= 220,    
    ttmethod_GetNext_ttcallback	= 221, 
    /* TTGet*_ttlistener methods */        
    ttmethod_GetEvent_ttlistener	= 222,    
    ttmethod_GetFunction_ttlistener	= 223,    
    ttmethod_GetArgComponentN_ttlistener	= 224,    
    ttmethod_GetArgEventN_ttlistener	= 225,    
    ttmethod_GetArgs_ttlistener	= 226,    
    ttmethod_GetEventMask_ttlistener	= 227,         
    /* TTGet*_tttimer methods */    
    ttmethod_GetDelayT_tttimer	= 228,    
    ttmethod_GetDelayF_tttimer	= 229,    
    ttmethod_GetTimerPrev_tttimer	= 230,    
    ttmethod_GetTimerNext_tttimer	= 231, 
    /* TTGet*_ttcomponent methods */    
    ttmethod_GetCallbacks_ttcomponent	= 232,    
    ttmethod_GetListeners_ttcomponent	= 233,    
    ttmethod_GetTimers_ttcomponent	= 234,    
    ttmethod_GetDatas_ttcomponent	= 235, 
    /* TTGet*_ttdata methods */        
    ttmethod_GetComponent_ttdata	= 236,    
    ttmethod_GetKeyLen_ttdata	= 237,    
    ttmethod_GetKey_ttdata	= 238,    
    ttmethod_GetData_ttdata	= 239, 
    /* TTGet*_ttvisible methods */    
    ttmethod_GetVflags_ttvisible	= 240,    
    ttmethod_GetPrev_ttvisible	= 241,    
    ttmethod_GetNext_ttvisible	= 242,    
    ttmethod_GetParent_ttvisible	= 243,    
    ttmethod_GetChildFirst_ttvisible	= 244,    
    ttmethod_GetChildLast_ttvisible	= 245,    
    ttmethod_GetConstraint_ttvisible	= 246,    
    ttmethod_GetTheme_ttvisible	= 247,    
    ttmethod_GetRepaint_ttvisible	= 248, 
    /* TTGet*_ttlayout methods */    
    ttmethod_GetWidget_ttlayout	= 249, 
    /* TTGet*_ttboxlayout methods */    
    ttmethod_GetOrientation_ttboxlayout	= 250, 
    /* TTGet*_ttborderlayout methods */ 
    /* TTGet*_ttnative methods */ 
    /* TTGet*_ttwidget methods */    
    ttmethod_GetX_ttwidget	= 251,    
    ttmethod_GetY_ttwidget	= 252,    
    ttmethod_GetW_ttwidget	= 253,    
    ttmethod_GetH_ttwidget	= 254,    
    ttmethod_GetLayout_ttwidget	= 255,    
    ttmethod_GetXl_ttwidget	= 256,    
    ttmethod_GetYl_ttwidget	= 257,    
    ttmethod_GetWl_ttwidget	= 258,    
    ttmethod_GetHl_ttwidget	= 259,    
    ttmethod_GetCol_ttwidget	= 260,    
    ttmethod_GetTooltip_ttwidget	= 261, 
    /* TTGet*_ttlabel methods */    
    ttmethod_GetTextLen_ttlabel	= 262,    
    ttmethod_GetText_ttlabel	= 263, 
    /* TTGet*_tttooltip methods */        
    ttmethod_GetWidget_tttooltip	= 264, 
    /* TTGet*_ttbuttongroup methods */    
    ttmethod_GetGroupFirst_ttbuttongroup	= 265,    
    ttmethod_GetGroupLast_ttbuttongroup	= 266,    
    ttmethod_GetChecked_ttbuttongroup	= 267, 
    /* TTGet*_ttanybutton methods */    
    ttmethod_GetTextWidth_ttanybutton	= 268,    
    ttmethod_GetTextHeight_ttanybutton	= 269,    
    ttmethod_GetText_ttanybutton	= 270,     
    /* TTGet*_ttbutton methods */ 
    /* TTGet*_ttcheckbutton methods */ 
    /* TTGet*_ttradiobutton methods */    
    ttmethod_GetGroup_ttradiobutton	= 271,    
    ttmethod_GetGroupPrev_ttradiobutton	= 272,    
    ttmethod_GetGroupNext_ttradiobutton	= 273, 
    /* TTGet*_ttanyscroll methods */    
    ttmethod_GetOrientation_ttanyscroll	= 274,    
    ttmethod_GetSize_ttanyscroll	= 275,    
    ttmethod_GetRealSize_ttanyscroll	= 276,    
    ttmethod_GetViewSize_ttanyscroll	= 277,        
    ttmethod_GetPosition_ttanyscroll	= 278,    
    ttmethod_GetRealPosition_ttanyscroll	= 279,    
    ttmethod_GetState_ttanyscroll	= 280, 
    /* TTGet*_ttscrollbar methods */     
    /* TTGet*_ttslider methods */    
    ttmethod_GetSlideMin_ttslider	= 281,    
    ttmethod_GetSlideMax_ttslider	= 282,    
    ttmethod_GetSlideValue_ttslider	= 283, 
    /* TTGet*_ttprogressbar methods */         
    /* TTGet*_ttscrollpane methods */    
    ttmethod_GetBarX_ttscrollpane	= 284,    
    ttmethod_GetBarY_ttscrollpane	= 285, 
    /* TTGet*_ttwindow methods */    
    ttmethod_GetTitleLen_ttwindow	= 286,    
    ttmethod_GetTitle_ttwindow	= 287, 
    /* TTGet*_ttframe methods */    
    ttmethod_GetMenubar_ttframe	= 288, 
    /* TTGet*_ttscroller methods */    
    ttmethod_GetScrollpane_ttscroller	= 289, 
    /* TTGet*_ttmenuitem methods */    
    ttmethod_GetNameLen_ttmenuitem	= 290,    
    ttmethod_GetName_ttmenuitem	= 291, 
    /* TTGet*_ttcheckmenuitem methods */ 
    /* TTGet*_ttradiomenuitem methods */ 
    /* TTGet*_ttmenu methods */    
    ttmethod_GetMenubar_ttmenu	= 292, 
    /* TTGet*_ttmenubar methods */ 
    /* TTGet*_ttanytext methods */    
    ttmethod_GetTextLen_ttanytext	= 293,    
    ttmethod_GetText_ttanytext	= 294, 
    /* TTGet*_tttextfield methods */ 
    /* TTGet*_tttextarea methods */ 
    /* TTGet*_tttheme methods */    
    ttmethod_GetNameLen_tttheme	= 295,    
    ttmethod_GetName_tttheme	= 296,                 
    /* TTGet*_ttapplication methods */    
    ttmethod_GetNameLen_ttapplication	= 297,    
    ttmethod_GetName_ttapplication	= 298,





    ttmethod_FN_get_last	= 299,
    ttmethod_FN_last = 299
} e_order_methods;


/* defines for existing object methods */





#define ttmethod_LibraryVersion	ttmethod_LibraryVersion
#define ttmethod_ExitMainLoop	ttmethod_ExitMainLoop
#define ttmethod_New	ttmethod_New
#define ttmethod_Del	ttmethod_Del
#define ttmethod_ClassOf	ttmethod_ClassOf
#define ttmethod_SuperClassOf	ttmethod_SuperClassOf
#define ttmethod_InstanceOf	ttmethod_InstanceOf
#define ttmethod_ClassNameOf	ttmethod_ClassNameOf






 
    /* ttobj methods */                    
#define ttmethod_GetField_ttobj	ttmethod_GetField_ttobj
#define ttmethod_SetField_ttobj	ttmethod_SetField_ttobj
#define ttmethod_ChangeField_ttobj	ttmethod_ChangeField_ttobj 
    /* ttclass methods */
#define ttmethod_GetName_ttclass	ttmethod_GetName_ttclass
#define ttmethod_GetSize_ttclass	ttmethod_GetSize_ttclass
#define ttmethod_GetSuper_ttclass	ttmethod_GetSuper_ttclass
#define ttmethod_IsInstance_ttclass	ttmethod_IsInstance_ttclass
#define ttmethod_FromType_ttclass	ttmethod_FromType_ttclass
#define ttmethod_IsArrayType_ttclass	ttmethod_IsArrayType_ttclass
#define ttmethod_IsPrimitiveType_ttclass	ttmethod_IsPrimitiveType_ttclass 
    /* ttobject methods */                
#define ttmethod_Ref_ttobject	ttmethod_Ref_ttobject
#define ttmethod_Unref_ttobject	ttmethod_Unref_ttobject
#define ttmethod_SetUserData_ttobject	ttmethod_SetUserData_ttobject 
    /* ttvector methods */    
#define ttmethod_GetAt_ttvector	ttmethod_GetAt_ttvector    
#define ttmethod_SetAt_ttvector	ttmethod_SetAt_ttvector
#define ttmethod_Create1_ttvector	ttmethod_Create1_ttvector
#define ttmethod_CreateA_ttvector	ttmethod_CreateA_ttvector
#define ttmethod_CreateR_ttvector	ttmethod_CreateR_ttvector
#define ttmethod_CreateV_ttvector	ttmethod_CreateV_ttvector
#define ttmethod_CreateY_ttvector	ttmethod_CreateY_ttvector
#define ttmethod_Append_ttvector	ttmethod_Append_ttvector
#define ttmethod_Add_ttvector	ttmethod_Add_ttvector
#define ttmethod_AddA_ttvector	ttmethod_AddA_ttvector
#define ttmethod_AddR_ttvector	ttmethod_AddR_ttvector
#define ttmethod_AddV_ttvector	ttmethod_AddV_ttvector    
#define ttmethod_AddY_ttvector	ttmethod_AddY_ttvector    
#define ttmethod_ContainsValue_ttvector	ttmethod_ContainsValue_ttvector
#define ttmethod_RemoveAt_ttvector	ttmethod_RemoveAt_ttvector    
#define ttmethod_RemoveRange_ttvector	ttmethod_RemoveRange_ttvector 
    /* ttfield methods */
#define ttmethod_Get_ttfield	ttmethod_Get_ttfield
#define ttmethod_GetValue_ttfield	ttmethod_GetValue_ttfield
#define ttmethod_SetValue_ttfield	ttmethod_SetValue_ttfield
#define ttmethod_ChangeValue_ttfield	ttmethod_ChangeValue_ttfield
#define ttmethod_ToEvcode_ttfield	ttmethod_ToEvcode_ttfield
#define ttmethod_FromEvcode_ttfield	ttmethod_FromEvcode_ttfield
#define ttmethod_GetEvcode_ttfield	ttmethod_GetEvcode_ttfield 
    /* ttmethod methods */
#define ttmethod_Get_ttmethod	ttmethod_Get_ttmethod
#define ttmethod_GetByAddress_ttmethod	ttmethod_GetByAddress_ttmethod
#define ttmethod_GetArraySizeA_ttmethod	ttmethod_GetArraySizeA_ttmethod
#define ttmethod_CheckArraySizesA_ttmethod	ttmethod_CheckArraySizesA_ttmethod
#define ttmethod_CallA_ttmethod	ttmethod_CallA_ttmethod
#define ttmethod_CallR_ttmethod	ttmethod_CallR_ttmethod
#define ttmethod_CallV_ttmethod	ttmethod_CallV_ttmethod
#define ttmethod_CallY_ttmethod	ttmethod_CallY_ttmethod 
    /* ttevent methods */
#define ttmethod_Create_ttevent	ttmethod_Create_ttevent
#define ttmethod_Fire_ttevent	ttmethod_Fire_ttevent 
    /* tteventbig methods */
#define ttmethod_Create8_tteventbig	ttmethod_Create8_tteventbig
#define ttmethod_Create4_tteventbig	ttmethod_Create4_tteventbig
#define ttmethod_Create4s4_tteventbig	ttmethod_Create4s4_tteventbig 
    /* ttbitmask methods */
#define ttmethod_Create_ttbitmask	ttmethod_Create_ttbitmask
#define ttmethod_CreateB2_ttbitmask	ttmethod_CreateB2_ttbitmask
#define ttmethod_CreateR_ttbitmask	ttmethod_CreateR_ttbitmask
#define ttmethod_CreateR2_ttbitmask	ttmethod_CreateR2_ttbitmask
#define ttmethod_Clear_ttbitmask	ttmethod_Clear_ttbitmask
#define ttmethod_Fill_ttbitmask	ttmethod_Fill_ttbitmask
#define ttmethod_Negate_ttbitmask	ttmethod_Negate_ttbitmask
#define ttmethod_Change_ttbitmask	ttmethod_Change_ttbitmask
#define ttmethod_Contains_ttbitmask	ttmethod_Contains_ttbitmask
#define ttmethod_Add_ttbitmask	ttmethod_Add_ttbitmask
#define ttmethod_AddB_ttbitmask	ttmethod_AddB_ttbitmask
#define ttmethod_AddR_ttbitmask	ttmethod_AddR_ttbitmask
#define ttmethod_Remove_ttbitmask	ttmethod_Remove_ttbitmask
#define ttmethod_RemoveB_ttbitmask	ttmethod_RemoveB_ttbitmask
#define ttmethod_RemoveR_ttbitmask	ttmethod_RemoveR_ttbitmask
#define ttmethod_Combine_ttbitmask	ttmethod_Combine_ttbitmask
#define ttmethod_CombineR_ttbitmask	ttmethod_CombineR_ttbitmask
#define ttmethod_CombineB_ttbitmask	ttmethod_CombineB_ttbitmask 
    /* tteventmask methods */
#define ttmethod_CreateB_tteventmask	ttmethod_CreateB_tteventmask
#define ttmethod_CreateB2_tteventmask	ttmethod_CreateB2_tteventmask
#define ttmethod_SetEvtypeMask_tteventmask	ttmethod_SetEvtypeMask_tteventmask
#define ttmethod_SetEvcodeMask_tteventmask	ttmethod_SetEvcodeMask_tteventmask
#define ttmethod_SetComponentMask_tteventmask	ttmethod_SetComponentMask_tteventmask
#define ttmethod_SetTruthTable_tteventmask	ttmethod_SetTruthTable_tteventmask 
    /* ttcallback methods */     
    /* ttlistener methods */
#define ttmethod_Create_ttlistener	ttmethod_Create_ttlistener
#define ttmethod_CreateE_ttlistener	ttmethod_CreateE_ttlistener
#define ttmethod_CreateR_ttlistener	ttmethod_CreateR_ttlistener
#define ttmethod_CreateDel_ttlistener	ttmethod_CreateDel_ttlistener
#define ttmethod_CreateAskclose_ttlistener	ttmethod_CreateAskclose_ttlistener
#define ttmethod_CreateKey_ttlistener	ttmethod_CreateKey_ttlistener
#define ttmethod_CreateMouse_ttlistener	ttmethod_CreateMouse_ttlistener
#define ttmethod_CreateActivate_ttlistener	ttmethod_CreateActivate_ttlistener
#define ttmethod_CreateChange_ttlistener	ttmethod_CreateChange_ttlistener
#define ttmethod_CreateKeyData_ttlistener	ttmethod_CreateKeyData_ttlistener
#define ttmethod_CreateExpose_ttlistener	ttmethod_CreateExpose_ttlistener    
#define ttmethod_AddTo_ttlistener	ttmethod_AddTo_ttlistener    
#define ttmethod_Remove_ttlistener	ttmethod_Remove_ttlistener
#define ttmethod_SetArgsR_ttlistener	ttmethod_SetArgsR_ttlistener
#define ttmethod_SetEventMask_ttlistener	ttmethod_SetEventMask_ttlistener 
    /* tttimer methods */
#define ttmethod_CreateR_tttimer	ttmethod_CreateR_tttimer
#define ttmethod_Create_tttimer	ttmethod_Create_tttimer
#define ttmethod_SetDelay_tttimer	ttmethod_SetDelay_tttimer    
#define ttmethod_SetEnabled_tttimer	ttmethod_SetEnabled_tttimer    
#define ttmethod_IsEnabled_tttimer	ttmethod_IsEnabled_tttimer 
    /* ttcomponent methods */
#define ttmethod_AddListener_ttcomponent	ttmethod_AddListener_ttcomponent
#define ttmethod_RemoveListener_ttcomponent	ttmethod_RemoveListener_ttcomponent
#define ttmethod_SetKeyData_ttcomponent	ttmethod_SetKeyData_ttcomponent
#define ttmethod_GetKeyData_ttcomponent	ttmethod_GetKeyData_ttcomponent
#define ttmethod_UnsetKeyData_ttcomponent	ttmethod_UnsetKeyData_ttcomponent
#define ttmethod_GetData_ttcomponent	ttmethod_GetData_ttcomponent 
    /* ttdata methods */        
#define ttmethod_SetData_ttdata	ttmethod_SetData_ttdata 
    /* ttvisible methods */    
#define ttmethod_AddTo_ttvisible	ttmethod_AddTo_ttvisible    
#define ttmethod_Validate_ttvisible	ttmethod_Validate_ttvisible    
#define ttmethod_Remove_ttvisible	ttmethod_Remove_ttvisible    
#define ttmethod_Invalidate_ttvisible	ttmethod_Invalidate_ttvisible    
#define ttmethod_SetVisible_ttvisible	ttmethod_SetVisible_ttvisible    
#define ttmethod_SetTheme_ttvisible	ttmethod_SetTheme_ttvisible    
#define ttmethod_Draw_ttvisible	ttmethod_Draw_ttvisible    
#define ttmethod_BuiltinRepaint_ttvisible	ttmethod_BuiltinRepaint_ttvisible

#define ttmethod_SetRepaint_ttvisible	ttmethod_SetRepaint_ttvisible
#define ttmethod_Expose_ttvisible	ttmethod_Expose_ttvisible
#define ttmethod_Add_ttvisible	ttmethod_Add_ttvisible 
    /* ttlayout methods */         
    /* ttboxlayout methods */
#define ttmethod_Create_ttboxlayout	ttmethod_Create_ttboxlayout 
    /* ttborderlayout methods */ 
    /* ttnative methods */    
#define ttmethod_GetRoot_ttnative	ttmethod_GetRoot_ttnative    
#define ttmethod_GetW_ttnative	ttmethod_GetW_ttnative    
#define ttmethod_GetH_ttnative	ttmethod_GetH_ttnative
#define ttmethod_Create_ttnative	ttmethod_Create_ttnative 
    /* ttwidget methods */    
#define ttmethod_SetLayout_ttwidget	ttmethod_SetLayout_ttwidget    
#define ttmethod_SetXY_ttwidget	ttmethod_SetXY_ttwidget    
#define ttmethod_SetWH_ttwidget	ttmethod_SetWH_ttwidget    
#define ttmethod_SetXl_ttwidget	ttmethod_SetXl_ttwidget    
#define ttmethod_SetYl_ttwidget	ttmethod_SetYl_ttwidget    
#define ttmethod_SetWl_ttwidget	ttmethod_SetWl_ttwidget    
#define ttmethod_SetHl_ttwidget	ttmethod_SetHl_ttwidget    
#define ttmethod_SetTooltip_ttwidget	ttmethod_SetTooltip_ttwidget
#define ttmethod_SetXlYl_ttwidget	ttmethod_SetXlYl_ttwidget
#define ttmethod_SetWlHl_ttwidget	ttmethod_SetWlHl_ttwidget
#define ttmethod_SetXYWH_ttwidget	ttmethod_SetXYWH_ttwidget
#define ttmethod_SetX_ttwidget	ttmethod_SetX_ttwidget
#define ttmethod_SetY_ttwidget	ttmethod_SetY_ttwidget
#define ttmethod_SetW_ttwidget	ttmethod_SetW_ttwidget
#define ttmethod_SetH_ttwidget	ttmethod_SetH_ttwidget 
    /* ttlabel methods */    
#define ttmethod_SetFontD_ttlabel	ttmethod_SetFontD_ttlabel
#define ttmethod_SetFont_ttlabel	ttmethod_SetFont_ttlabel
#define ttmethod_SetText_ttlabel	ttmethod_SetText_ttlabel
#define ttmethod_Create_ttlabel	ttmethod_Create_ttlabel 
    /* tttooltip methods */    
#define ttmethod_SetWidget_tttooltip	ttmethod_SetWidget_tttooltip 
    /* ttbuttongroup methods */    
#define ttmethod_Add_ttbuttongroup	ttmethod_Add_ttbuttongroup    
#define ttmethod_Remove_ttbuttongroup	ttmethod_Remove_ttbuttongroup    
#define ttmethod_SetChecked_ttbuttongroup	ttmethod_SetChecked_ttbuttongroup 
    /* ttanybutton methods */        
#define ttmethod_SetAttrD_ttanybutton	ttmethod_SetAttrD_ttanybutton
#define ttmethod_SetAttr_ttanybutton	ttmethod_SetAttr_ttanybutton
#define ttmethod_SetFont_ttanybutton	ttmethod_SetFont_ttanybutton
#define ttmethod_SetText_ttanybutton	ttmethod_SetText_ttanybutton
#define ttmethod_Create_ttanybutton	ttmethod_Create_ttanybutton 
    /* ttbutton methods */    
#define ttmethod_SetPressed_ttbutton	ttmethod_SetPressed_ttbutton
#define ttmethod_IsPressed_ttbutton	ttmethod_IsPressed_ttbutton
#define ttmethod_Create_ttbutton	ttmethod_Create_ttbutton 
    /* ttcheckbutton methods */    
#define ttmethod_SetChecked_ttcheckbutton	ttmethod_SetChecked_ttcheckbutton
#define ttmethod_IsChecked_ttcheckbutton	ttmethod_IsChecked_ttcheckbutton
#define ttmethod_Create_ttcheckbutton	ttmethod_Create_ttcheckbutton 
    /* ttradiobutton methods */
#define ttmethod_AddToGroup_ttradiobutton	ttmethod_AddToGroup_ttradiobutton
#define ttmethod_RemoveFromGroup_ttradiobutton	ttmethod_RemoveFromGroup_ttradiobutton
#define ttmethod_Create_ttradiobutton	ttmethod_Create_ttradiobutton 
    /* ttanyscroll methods */            
#define ttmethod_SetOrientation_ttanyscroll	ttmethod_SetOrientation_ttanyscroll    
#define ttmethod_SetSize_ttanyscroll	ttmethod_SetSize_ttanyscroll    
#define ttmethod_SetRealSize_ttanyscroll	ttmethod_SetRealSize_ttanyscroll    
#define ttmethod_SetViewSize_ttanyscroll	ttmethod_SetViewSize_ttanyscroll    
#define ttmethod_SetPosition_ttanyscroll	ttmethod_SetPosition_ttanyscroll 
    /* ttscrollbar methods */
#define ttmethod_Create1_ttscrollbar	ttmethod_Create1_ttscrollbar
#define ttmethod_Create4_ttscrollbar	ttmethod_Create4_ttscrollbar 
    /* ttslider methods */    
#define ttmethod_SetSlideMin_ttslider	ttmethod_SetSlideMin_ttslider    
#define ttmethod_SetSlideMax_ttslider	ttmethod_SetSlideMax_ttslider    
#define ttmethod_SetSlideValue_ttslider	ttmethod_SetSlideValue_ttslider
#define ttmethod_Create_ttslider	ttmethod_Create_ttslider 
    /* ttprogressbar methods */
#define ttmethod_Create_ttprogressbar	ttmethod_Create_ttprogressbar 
    /* ttscrollpane methods */
#define ttmethod_Create_ttscrollpane	ttmethod_Create_ttscrollpane    
#define ttmethod_SetBarX_ttscrollpane	ttmethod_SetBarX_ttscrollpane    
#define ttmethod_SetBarY_ttscrollpane	ttmethod_SetBarY_ttscrollpane 
    /* ttwindow methods */    
#define ttmethod_SetTitle_ttwindow	ttmethod_SetTitle_ttwindow 
    /* ttframe methods */    
#define ttmethod_SetMenubar_ttframe	ttmethod_SetMenubar_ttframe
#define ttmethod_Create_ttframe	ttmethod_Create_ttframe 
    /* ttscroller methods */
#define ttmethod_Create_ttscroller	ttmethod_Create_ttscroller 
    /* ttmenuitem methods */ 
    /* ttcheckmenuitem methods */ 
    /* ttradiomenuitem methods */ 
    /* ttmenu methods */ 
    /* ttmenubar methods */
#define ttmethod_GetDefault_ttmenubar	ttmethod_GetDefault_ttmenubar 
    /* ttanytext methods */ 
    /* tttextfield methods */ 
    /* tttextarea methods */ 
    /* tttheme methods */
#define ttmethod_GetDefault_tttheme	ttmethod_GetDefault_tttheme 
    /* ttapplication methods */
#define ttmethod_Create_ttapplication	ttmethod_Create_ttapplication
#define ttmethod_Get_ttapplication	ttmethod_Get_ttapplication







    /* TTSet*_* methods omitted, they are listed above! */
    
#if 0
# define ttmethod_FN_set_first	ttmethod_FN_set_first

# define ttmethod_SetField_ttobj	ttmethod_SetField_ttobj
# define ttmethod_ChangeField_ttobj	ttmethod_ChangeField_ttobj

# define ttmethod_FN_set_last	ttmethod_FN_set_last
#endif

    /* TTGet*_* methods */

#define ttmethod_FN_get_first	ttmethod_FN_get_first





 
    /* TTGet*_ttobj methods */         
    /* TTGet*_ttclass methods */             
    /* TTGet*_ttobject methods */    
#define ttmethod_GetRefcount_ttobject	ttmethod_GetRefcount_ttobject    
#define ttmethod_GetOflags_ttobject	ttmethod_GetOflags_ttobject            
#define ttmethod_GetUserData_ttobject	ttmethod_GetUserData_ttobject    
#define ttmethod_GetEventsInprogress_ttobject	ttmethod_GetEventsInprogress_ttobject    
#define ttmethod_GetEventsInformed_ttobject	ttmethod_GetEventsInformed_ttobject 
    /* TTGet*_ttvector methods */    
#define ttmethod_GetArraySize_ttvector	ttmethod_GetArraySize_ttvector    
#define ttmethod_GetArrayN_ttvector	ttmethod_GetArrayN_ttvector    
#define ttmethod_GetArray_ttvector	ttmethod_GetArray_ttvector 
    /* TTGet*_ttfield methods */    
#define ttmethod_GetNameLen_ttfield	ttmethod_GetNameLen_ttfield    
#define ttmethod_GetName_ttfield	ttmethod_GetName_ttfield    
#define ttmethod_GetType_ttfield	ttmethod_GetType_ttfield 
    /* TTGet*_ttmethod methods */    
#define ttmethod_GetNameLen_ttmethod	ttmethod_GetNameLen_ttmethod    
#define ttmethod_GetName_ttmethod	ttmethod_GetName_ttmethod    
#define ttmethod_GetAddress_ttmethod	ttmethod_GetAddress_ttmethod    
#define ttmethod_GetReturnType_ttmethod	ttmethod_GetReturnType_ttmethod    
#define ttmethod_GetParameterType_ttmethod	ttmethod_GetParameterType_ttmethod 
    /* TTGet*_ttevent methods */    
#define ttmethod_GetComponent_ttevent	ttmethod_GetComponent_ttevent    
#define ttmethod_GetEvtype_ttevent	ttmethod_GetEvtype_ttevent    
#define ttmethod_GetEvcode_ttevent	ttmethod_GetEvcode_ttevent    
#define ttmethod_GetEvflags_ttevent	ttmethod_GetEvflags_ttevent    
#define ttmethod_GetPrev_ttevent	ttmethod_GetPrev_ttevent    
#define ttmethod_GetNext_ttevent	ttmethod_GetNext_ttevent 
    /* TTGet*_tteventbig methods */    
#define ttmethod_GetX_tteventbig	ttmethod_GetX_tteventbig    
#define ttmethod_GetY_tteventbig	ttmethod_GetY_tteventbig    
#define ttmethod_GetW_tteventbig	ttmethod_GetW_tteventbig    
#define ttmethod_GetH_tteventbig	ttmethod_GetH_tteventbig    
#define ttmethod_GetValue_tteventbig	ttmethod_GetValue_tteventbig    
#define ttmethod_GetOldValue_tteventbig	ttmethod_GetOldValue_tteventbig    
#define ttmethod_GetDataLen_tteventbig	ttmethod_GetDataLen_tteventbig    
#define ttmethod_GetData_tteventbig	ttmethod_GetData_tteventbig 
    /* TTGet*_ttbitmask methods */                 
    /* TTGet*_tteventmask methods */    
#define ttmethod_GetEvtypeMask_tteventmask	ttmethod_GetEvtypeMask_tteventmask    
#define ttmethod_GetEvcodeMask_tteventmask	ttmethod_GetEvcodeMask_tteventmask    
#define ttmethod_GetComponentMask_tteventmask	ttmethod_GetComponentMask_tteventmask    
#define ttmethod_GetTruthTable_tteventmask	ttmethod_GetTruthTable_tteventmask 
    /* TTGet*_ttcallback methods */    
#define ttmethod_GetLflags_ttcallback	ttmethod_GetLflags_ttcallback    
#define ttmethod_GetComponent_ttcallback	ttmethod_GetComponent_ttcallback    
#define ttmethod_GetPrev_ttcallback	ttmethod_GetPrev_ttcallback    
#define ttmethod_GetNext_ttcallback	ttmethod_GetNext_ttcallback 
    /* TTGet*_ttlistener methods */        
#define ttmethod_GetEvent_ttlistener	ttmethod_GetEvent_ttlistener    
#define ttmethod_GetFunction_ttlistener	ttmethod_GetFunction_ttlistener    
#define ttmethod_GetArgComponentN_ttlistener	ttmethod_GetArgComponentN_ttlistener    
#define ttmethod_GetArgEventN_ttlistener	ttmethod_GetArgEventN_ttlistener    
#define ttmethod_GetArgs_ttlistener	ttmethod_GetArgs_ttlistener    
#define ttmethod_GetEventMask_ttlistener	ttmethod_GetEventMask_ttlistener         
    /* TTGet*_tttimer methods */    
#define ttmethod_GetDelayT_tttimer	ttmethod_GetDelayT_tttimer    
#define ttmethod_GetDelayF_tttimer	ttmethod_GetDelayF_tttimer    
#define ttmethod_GetTimerPrev_tttimer	ttmethod_GetTimerPrev_tttimer    
#define ttmethod_GetTimerNext_tttimer	ttmethod_GetTimerNext_tttimer 
    /* TTGet*_ttcomponent methods */    
#define ttmethod_GetCallbacks_ttcomponent	ttmethod_GetCallbacks_ttcomponent    
#define ttmethod_GetListeners_ttcomponent	ttmethod_GetListeners_ttcomponent    
#define ttmethod_GetTimers_ttcomponent	ttmethod_GetTimers_ttcomponent    
#define ttmethod_GetDatas_ttcomponent	ttmethod_GetDatas_ttcomponent 
    /* TTGet*_ttdata methods */        
#define ttmethod_GetComponent_ttdata	ttmethod_GetComponent_ttdata    
#define ttmethod_GetKeyLen_ttdata	ttmethod_GetKeyLen_ttdata    
#define ttmethod_GetKey_ttdata	ttmethod_GetKey_ttdata    
#define ttmethod_GetData_ttdata	ttmethod_GetData_ttdata 
    /* TTGet*_ttvisible methods */    
#define ttmethod_GetVflags_ttvisible	ttmethod_GetVflags_ttvisible    
#define ttmethod_GetPrev_ttvisible	ttmethod_GetPrev_ttvisible    
#define ttmethod_GetNext_ttvisible	ttmethod_GetNext_ttvisible    
#define ttmethod_GetParent_ttvisible	ttmethod_GetParent_ttvisible    
#define ttmethod_GetChildFirst_ttvisible	ttmethod_GetChildFirst_ttvisible    
#define ttmethod_GetChildLast_ttvisible	ttmethod_GetChildLast_ttvisible    
#define ttmethod_GetConstraint_ttvisible	ttmethod_GetConstraint_ttvisible    
#define ttmethod_GetTheme_ttvisible	ttmethod_GetTheme_ttvisible    
#define ttmethod_GetRepaint_ttvisible	ttmethod_GetRepaint_ttvisible 
    /* TTGet*_ttlayout methods */    
#define ttmethod_GetWidget_ttlayout	ttmethod_GetWidget_ttlayout 
    /* TTGet*_ttboxlayout methods */    
#define ttmethod_GetOrientation_ttboxlayout	ttmethod_GetOrientation_ttboxlayout 
    /* TTGet*_ttborderlayout methods */ 
    /* TTGet*_ttnative methods */ 
    /* TTGet*_ttwidget methods */    
#define ttmethod_GetX_ttwidget	ttmethod_GetX_ttwidget    
#define ttmethod_GetY_ttwidget	ttmethod_GetY_ttwidget    
#define ttmethod_GetW_ttwidget	ttmethod_GetW_ttwidget    
#define ttmethod_GetH_ttwidget	ttmethod_GetH_ttwidget    
#define ttmethod_GetLayout_ttwidget	ttmethod_GetLayout_ttwidget    
#define ttmethod_GetXl_ttwidget	ttmethod_GetXl_ttwidget    
#define ttmethod_GetYl_ttwidget	ttmethod_GetYl_ttwidget    
#define ttmethod_GetWl_ttwidget	ttmethod_GetWl_ttwidget    
#define ttmethod_GetHl_ttwidget	ttmethod_GetHl_ttwidget    
#define ttmethod_GetCol_ttwidget	ttmethod_GetCol_ttwidget    
#define ttmethod_GetTooltip_ttwidget	ttmethod_GetTooltip_ttwidget 
    /* TTGet*_ttlabel methods */    
#define ttmethod_GetTextLen_ttlabel	ttmethod_GetTextLen_ttlabel    
#define ttmethod_GetText_ttlabel	ttmethod_GetText_ttlabel 
    /* TTGet*_tttooltip methods */        
#define ttmethod_GetWidget_tttooltip	ttmethod_GetWidget_tttooltip 
    /* TTGet*_ttbuttongroup methods */    
#define ttmethod_GetGroupFirst_ttbuttongroup	ttmethod_GetGroupFirst_ttbuttongroup    
#define ttmethod_GetGroupLast_ttbuttongroup	ttmethod_GetGroupLast_ttbuttongroup    
#define ttmethod_GetChecked_ttbuttongroup	ttmethod_GetChecked_ttbuttongroup 
    /* TTGet*_ttanybutton methods */    
#define ttmethod_GetTextWidth_ttanybutton	ttmethod_GetTextWidth_ttanybutton    
#define ttmethod_GetTextHeight_ttanybutton	ttmethod_GetTextHeight_ttanybutton    
#define ttmethod_GetText_ttanybutton	ttmethod_GetText_ttanybutton     
    /* TTGet*_ttbutton methods */ 
    /* TTGet*_ttcheckbutton methods */ 
    /* TTGet*_ttradiobutton methods */    
#define ttmethod_GetGroup_ttradiobutton	ttmethod_GetGroup_ttradiobutton    
#define ttmethod_GetGroupPrev_ttradiobutton	ttmethod_GetGroupPrev_ttradiobutton    
#define ttmethod_GetGroupNext_ttradiobutton	ttmethod_GetGroupNext_ttradiobutton 
    /* TTGet*_ttanyscroll methods */    
#define ttmethod_GetOrientation_ttanyscroll	ttmethod_GetOrientation_ttanyscroll    
#define ttmethod_GetSize_ttanyscroll	ttmethod_GetSize_ttanyscroll    
#define ttmethod_GetRealSize_ttanyscroll	ttmethod_GetRealSize_ttanyscroll    
#define ttmethod_GetViewSize_ttanyscroll	ttmethod_GetViewSize_ttanyscroll        
#define ttmethod_GetPosition_ttanyscroll	ttmethod_GetPosition_ttanyscroll    
#define ttmethod_GetRealPosition_ttanyscroll	ttmethod_GetRealPosition_ttanyscroll    
#define ttmethod_GetState_ttanyscroll	ttmethod_GetState_ttanyscroll 
    /* TTGet*_ttscrollbar methods */     
    /* TTGet*_ttslider methods */    
#define ttmethod_GetSlideMin_ttslider	ttmethod_GetSlideMin_ttslider    
#define ttmethod_GetSlideMax_ttslider	ttmethod_GetSlideMax_ttslider    
#define ttmethod_GetSlideValue_ttslider	ttmethod_GetSlideValue_ttslider 
    /* TTGet*_ttprogressbar methods */         
    /* TTGet*_ttscrollpane methods */    
#define ttmethod_GetBarX_ttscrollpane	ttmethod_GetBarX_ttscrollpane    
#define ttmethod_GetBarY_ttscrollpane	ttmethod_GetBarY_ttscrollpane 
    /* TTGet*_ttwindow methods */    
#define ttmethod_GetTitleLen_ttwindow	ttmethod_GetTitleLen_ttwindow    
#define ttmethod_GetTitle_ttwindow	ttmethod_GetTitle_ttwindow 
    /* TTGet*_ttframe methods */    
#define ttmethod_GetMenubar_ttframe	ttmethod_GetMenubar_ttframe 
    /* TTGet*_ttscroller methods */    
#define ttmethod_GetScrollpane_ttscroller	ttmethod_GetScrollpane_ttscroller 
    /* TTGet*_ttmenuitem methods */    
#define ttmethod_GetNameLen_ttmenuitem	ttmethod_GetNameLen_ttmenuitem    
#define ttmethod_GetName_ttmenuitem	ttmethod_GetName_ttmenuitem 
    /* TTGet*_ttcheckmenuitem methods */ 
    /* TTGet*_ttradiomenuitem methods */ 
    /* TTGet*_ttmenu methods */    
#define ttmethod_GetMenubar_ttmenu	ttmethod_GetMenubar_ttmenu 
    /* TTGet*_ttmenubar methods */ 
    /* TTGet*_ttanytext methods */    
#define ttmethod_GetTextLen_ttanytext	ttmethod_GetTextLen_ttanytext    
#define ttmethod_GetText_ttanytext	ttmethod_GetText_ttanytext 
    /* TTGet*_tttextfield methods */ 
    /* TTGet*_tttextarea methods */ 
    /* TTGet*_tttheme methods */    
#define ttmethod_GetNameLen_tttheme	ttmethod_GetNameLen_tttheme    
#define ttmethod_GetName_tttheme	ttmethod_GetName_tttheme                 
    /* TTGet*_ttapplication methods */    
#define ttmethod_GetNameLen_ttapplication	ttmethod_GetNameLen_ttapplication    
#define ttmethod_GetName_ttapplication	ttmethod_GetName_ttapplication





#define ttmethod_FN_get_last	ttmethod_FN_get_last
#define ttmethod_FN_last	ttmethod_FN_last


#endif /* _TT_TREEM4_H */

