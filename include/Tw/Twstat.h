/*
 *  Twstat.h  --  functions to get informations about libTw objects.
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef _TW_STAT_H
#define _TW_STAT_H

#include <stdarg.h>
#include <Tw/datasizes.h>

#ifdef __cplusplus
  extern "C" {
#endif
#if 0
  }
#endif

#define TWS__CAT(a,b) a##b
#define TWS_CAT(a,b) TWS__CAT(a,b)

#define TWS__STR(a) #a
#define TWS_STR(a) TWS__STR(a)

#define TWS_Id				0x0001
#define TWS_Prev			0x0002
#define TWS_Next			0x0003
#define TWS_Parent			0x0004

#define TWS_obj_Id			TWS_Id
#define TWS_obj_Prev			TWS_Prev
#define TWS_obj_Next			TWS_Next
#define TWS_obj_Parent			TWS_Parent

#define TWS_widget_Id			TWS_obj_Id
#define TWS_widget_Prev			TWS_obj_Prev
#define TWS_widget_Next			TWS_obj_Next
#define TWS_widget_Parent		TWS_obj_Parent
#define TWS_widget_FirstW		0x0100
#define TWS_widget_LastW		0x0101
#define TWS_widget_SelectW		0x0102
#define TWS_widget_FocusW		TWS_widget_SelectW
#define TWS_widget_Left			0x0103
#define TWS_widget_X			TWS_widget_Left
#define TWS_widget_Up			0x0104
#define TWS_widget_Y			TWS_widget_Up
#define TWS_widget_Width		0x0105
#define TWS_widget_XWidth		TWS_widget_Width
#define TWS_widget_Height		0x0106
#define TWS_widget_YWidth		TWS_widget_Height
#define TWS_widget_Attrib		0x0107
#define TWS_widget_Flags		0x0108
#define TWS_widget_XLogic		0x0109
#define TWS_widget_YLogic		0x010A
#define TWS_widget_O_Prev		0x010B
#define TWS_widget_O_Next		0x010C
#define TWS_widget_Owner		0x010D
#define TWS_widget_Msgport		TWS_widget_Owner
#define TWS_widget_USE_Fill		0x0110

#define TWS_gadget_Id			TWS_widget_Id
#define TWS_gadget_Prev			TWS_widget_Prev
#define TWS_gadget_Next			TWS_widget_Next
#define TWS_gadget_Parent		TWS_widget_Parent
#define TWS_gadget_FirstW		TWS_widget_FirstW
#define TWS_gadget_LastW		TWS_widget_LastW
#define TWS_gadget_SelectW		TWS_widget_SelectW
#define TWS_gadget_FocusW		TWS_widget_FocusW
#define TWS_gadget_Left			TWS_widget_Left
#define TWS_gadget_X			TWS_widget_X
#define TWS_gadget_Up			TWS_widget_Up
#define TWS_gadget_Y			TWS_widget_Y
#define TWS_gadget_Width		TWS_widget_Width
#define TWS_gadget_XWidth		TWS_widget_XWidth
#define TWS_gadget_Height		TWS_widget_Height
#define TWS_gadget_YWidth		TWS_widget_YWidth
#define TWS_gadget_Attrib		TWS_widget_Attrib
#define TWS_gadget_Flags		TWS_widget_Flags
#define TWS_gadget_XLogic		TWS_widget_XLogic
#define TWS_gadget_YLogic		TWS_widget_YLogic
#define TWS_gadget_O_Prev		TWS_widget_O_Prev
#define TWS_gadget_O_Next		TWS_widget_O_Next
#define TWS_gadget_Owner		TWS_widget_Owner
#define TWS_gadget_Msgport		TWS_widget_Msgport
#define TWS_gadget_USE_Fill		TWS_widget_USE_Fill

#define TWS_gadget_ColText		0x0200
#define TWS_gadget_ColNormal		TWS_gadget_ColText
#define TWS_gadget_ColSelect		0x0201
#define TWS_gadget_ColDisabled		0x0202
#define TWS_gadget_ColSelectDisabled	0x0203
#define TWS_gadget_Code			0x0204
#define TWS_gadget_G_Prev		0x0205
#define TWS_gadget_G_Next		0x0206
#define TWS_gadget_Group		0x0207
#define TWS_gadget_USE_TextV0		0x0210
#define TWS_gadget_USE_TextV1		0x0211
#define TWS_gadget_USE_TextV2		0x0212
#define TWS_gadget_USE_TextV3		0x0213
#define TWS_gadget_USE_TextVN(n)	(TWS_gadget_USE_TextV0+(n))
#define TWS_gadget_USE_ColorV0		0x0214
#define TWS_gadget_USE_ColorV1		0x0215
#define TWS_gadget_USE_ColorV2		0x0216
#define TWS_gadget_USE_ColorV3		0x0217
#define TWS_gadget_USE_ColorVN(n)	(TWS_gadget_USE_ColorV0+(n))

#define TWS_window_Id			TWS_widget_Id
#define TWS_window_Prev			TWS_widget_Prev
#define TWS_window_Next			TWS_widget_Next
#define TWS_window_Parent		TWS_widget_Parent
#define TWS_window_FirstW		TWS_widget_FirstW
#define TWS_window_LastW		TWS_widget_LastW
#define TWS_window_SelectW		TWS_widget_SelectW
#define TWS_window_FocusW		TWS_widget_FocusW
#define TWS_window_Left			TWS_widget_Left
#define TWS_window_X			TWS_widget_X
#define TWS_window_Up			TWS_widget_Up
#define TWS_window_Y			TWS_widget_Y
#define TWS_window_Width		TWS_widget_Width
#define TWS_window_XWidth		TWS_widget_XWidth
#define TWS_window_Height		TWS_widget_Height
#define TWS_window_YWidth		TWS_widget_YWidth
#define TWS_window_Attrib		TWS_widget_Attrib
#define TWS_window_Flags		TWS_widget_Flags
#define TWS_window_XLogic		TWS_widget_XLogic
#define TWS_window_YLogic		TWS_widget_YLogic
#define TWS_window_O_Prev		TWS_widget_O_Prev
#define TWS_window_O_Next		TWS_widget_O_Next
#define TWS_window_Owner		TWS_widget_Owner
#define TWS_window_Msgport		TWS_widget_Msgport
#define TWS_window_USE_Fill		TWS_widget_USE_Fill

#define TWS_window_Menu			0x0300
#define TWS_window_NameLen		0x0301
#define TWS_window_Name			0x0302
#define TWS_window_ColName		0x0303
#define TWS_window_BorderPatternV0	0x0304
#define TWS_window_BorderPatternV1	0x0305
#define TWS_window_BorderPatternVN(n)	(TWS_window_BorderPatternV0+(n))
#define TWS_window_CurX			0x0306
#define TWS_window_CurY			0x0307
#define TWS_window_XstSel		0x0308
#define TWS_window_YstSel		0x0309
#define TWS_window_XendSel		0x030A
#define TWS_window_YendSel		0x030B
#define TWS_window_ColGadgets		0x0310
#define TWS_window_ColArrows		0x0311
#define TWS_window_ColBars		0x0312
#define TWS_window_ColTabs		0x0313
#define TWS_window_ColBorder		0x0314
#define TWS_window_ColText		0x0315
#define TWS_window_ColSelect		0x0316
#define TWS_window_ColDisabled		0x0317
#define TWS_window_ColSelectDisabled	0x0318
#define TWS_window_State		0x0320
#define TWS_window_CursorType		0x0321
#define TWS_window_MinXWidth		0x0322
#define TWS_window_MinYWidth		0x0323
#define TWS_window_MaxXWidth		0x0324
#define TWS_window_MaxYWidth		0x0325
#define TWS_window_WLogic		0x0326
#define TWS_window_HLogic		0x0327
#define TWS_window_USE_Contents		0x0330
#define TWS_window_USE_HSplit		0x0331

#define TWS_screen_Id			TWS_widget_Id
#define TWS_screen_Prev			TWS_widget_Prev
#define TWS_screen_Next			TWS_widget_Next
#define TWS_screen_Parent		TWS_widget_Parent
#define TWS_screen_FirstW		TWS_widget_FirstW
#define TWS_screen_LastW		TWS_widget_LastW
#define TWS_screen_SelectW		TWS_widget_SelectW
#define TWS_screen_FocusW		TWS_widget_FocusW
#define TWS_screen_Left			TWS_widget_Left
#define TWS_screen_X			TWS_widget_X
#define TWS_screen_Up			TWS_widget_Up
#define TWS_screen_Y			TWS_widget_Y
#define TWS_screen_Width		TWS_widget_Width
#define TWS_screen_XWidth		TWS_widget_XWidth
#define TWS_screen_Height		TWS_widget_Height
#define TWS_screen_YWidth		TWS_widget_YWidth
#define TWS_screen_Attrib		TWS_widget_Attrib
#define TWS_screen_Flags		TWS_widget_Flags
#define TWS_screen_XLogic		TWS_widget_XLogic
#define TWS_screen_YLogic		TWS_widget_YLogic
#define TWS_screen_O_Prev		TWS_widget_O_Prev
#define TWS_screen_O_Next		TWS_widget_O_Next
#define TWS_screen_Owner		TWS_widget_Owner
#define TWS_screen_Msgport		TWS_widget_MsgPort
#define TWS_screen_USE_Fill		TWS_widget_USE_Fill

#define TWS_screen_NameLen		0x0400
#define TWS_screen_Name			0x0401
#define TWS_screen_State		0x0402
#define TWS_screen_USE_BgWidth		0x0403
#define TWS_screen_USE_BgHeight		0x0404
#define TWS_screen_USE_Bg		0x0405

#define TWS_group_Id			TWS_obj_Id
#define TWS_group_Prev			TWS_obj_Prev
#define TWS_group_Next			TWS_obj_Next
#define TWS_group_Parent		TWS_obj_Parent
#define TWS_group_Owner			TWS_obj_Parent
#define TWS_group_MsgPort		TWS_obj_Parent
#define TWS_group_FirstG		0x0500
#define TWS_group_LastG			0x0501
#define TWS_group_SelectG		0x0502

#define TWS_menuitem_Id			TWS_obj_Id
#define TWS_menuitem_Prev		TWS_obj_Prev
#define TWS_menuitem_Next		TWS_obj_Next
#define TWS_menuitem_Parent		TWS_obj_Parent
#define TWS_menuitem_Code		TWS_row_Code
#define TWS_menuitem_Flags		TWS_row_Flags
#define TWS_menuitem_Len		TWS_row_Len
#define TWS_menuitem_Text		TWS_row_Text
#define TWS_menuitem_ColText		TWS_row_ColText
#define TWS_menuitem_Window		0x0600
#define TWS_menuitem_Left		0x0602
#define TWS_menuitem_ShortCut		0x0604

#define TWS_menu_Id			TWS_obj_Id
#define TWS_menu_Prev			TWS_obj_Prev
#define TWS_menu_Next			TWS_obj_Next
#define TWS_menu_Parent			TWS_obj_Parent
#define TWS_menu_ColItem		0x0700
#define TWS_menu_ColSelect		0x0701
#define TWS_menu_ColDisabled		0x0702
#define TWS_menu_ColSelectDisabled	0x0703
#define TWS_menu_ColShtCut		0x0704
#define TWS_menu_ColSelShtCut		0x0705
#define TWS_menu_CommonItems		0x0706
#define TWS_menu_FlagDefColInfo		0x0707
#define TWS_menu_FirstI			0x0708
#define TWS_menu_LastI			0x0709
#define TWS_menu_SelectI		0x070A

#define TWS_msgport_Id			TWS_obj_Id
#define TWS_msgport_Prev		TWS_obj_Prev
#define TWS_msgport_Next		TWS_obj_Next
#define TWS_msgport_Parent		TWS_obj_Parent
#define TWS_msgport_WakeUp		0x0800
#define TWS_msgport_NameLen		0x0801
#define TWS_msgport_Name		0x0802
#define TWS_msgport_FirstMenu		0x0803
#define TWS_msgport_LastMenu		0x0804
#define TWS_msgport_FirstW		0x0805
#define TWS_msgport_LastW		0x0806
#define TWS_msgport_FirstGroup		0x0807
#define TWS_msgport_LastGroup		0x0808
#define TWS_msgport_FirstMutex		0x0809
#define TWS_msgport_LastMutex		0x080A

#define TWS_mutex_Id			TWS_obj_Id
#define TWS_mutex_Prev			TWS_obj_Prev
#define TWS_mutex_Next			TWS_obj_Next
#define TWS_mutex_Parent		TWS_obj_Parent
#define TWS_mutex_O_Prev		0x0900
#define TWS_mutex_O_Next		0x0901
#define TWS_mutex_Owner			0x0902
#define TWS_mutex_Msgport		TWS_mutex_Owner
#define TWS_mutex_Perm			0x0903
#define TWS_mutex_NameLen		0x0904
#define TWS_mutex_Name			0x0905

#define TWS_row_Id			TWS_obj_Id
#define TWS_row_Prev			TWS_obj_Prev
#define TWS_row_Next			TWS_obj_Next
#define TWS_row_Parent			TWS_obj_Parent
#define TWS_row_Code			0x0A00
#define TWS_row_Flags			0x0A01
#define TWS_row_Len			0x0A02
#define TWS_row_Text			0x0A03
#define TWS_row_ColText			0x0A04

/* the following are hardcoded in socket.c, libTw.c and libTw ABI, don't change! */
#define TWS_void   0
#define TWS_byte   1
#define TWS_num    1
#define TWS_dat    2
#define TWS_udat   2
#define TWS_ldat   3
#define TWS_uldat  3
#define TWS_hwcol  4
#define TWS_time_t 5
#define TWS_frac_t 6
#define TWS_hwfont 7
#define TWS_hwattr 8
#define TWS_highest 9
#define TWS_tobj   0xFF
#define TWS_obj    0xFF
#define TWS_last   0xFF
#define TWS_vec    0x100 /* this is used as TWS_vec | TWS_<some other type> */

/* the following are hardcoded in libTw ABI, don't change! */
#define TWS_base_CHR   '\x00'
#define TWS_void_STR   "\xFE" /* "\xFF" would conflict with TWS_obj */
#define TWS_byte_STR   "\x01"
#define TWS_num_STR    "\x01"
#define TWS_dat_STR    "\x02"
#define TWS_udat_STR   "\x02"
#define TWS_ldat_STR   "\x03"
#define TWS_uldat_STR  "\x03"
#define TWS_hwcol_STR  "\x04"
#define TWS_time_t_STR "\x05"
#define TWS_frac_t_STR "\x06"
#define TWS_hwfont_STR "\x07"
#define TWS_hwattr_STR "\x08"


#define TWS_field_byte   val._
#define TWS_field_num    val._
#define TWS_field_dat    val._
#define TWS_field_udat   val._
#define TWS_field_ldat   val._
#define TWS_field_uldat  val._
#define TWS_field_hwcol  val._
#define TWS_field_hwfont val._
#define TWS_field_hwattr val._
#define TWS_field_time_t val._
#define TWS_field_frac_t val._
#define TWS_field_tobj   val._

#define TWS_field_scalar val._

#define TWS_field_obj    val.o
#define TWS_field_vecV   val.vec.V
#define TWS_field_vecL   val.vec.L

typedef struct s_tsfield {
    udat hash;
    udat type;
    union {
	tlargest _;
#ifdef obj
	obj    o;
#endif
	struct {
	    void *V;
	    uldat L;
	} vec;
    } val;
} *tsfield;

typedef struct s_tlist {
    udat N;
    udat flags;
    struct s_tsfield TSF[1];
} *tslist;

tlargest Tw_Stat(      tdisplay TwD, tobj Id, udat h);
tslist  Tw_LStat(     tdisplay TwD, tobj Id, udat hN, ...);
tslist  Tw_AStat(     tdisplay TwD, tobj Id, udat hN, TW_CONST udat *h);
tslist  Tw_VStat(     tdisplay TwD, tobj Id, udat hN, va_list h);
tslist  Tw_CloneStat( tdisplay TwD, tobj Id, udat hN, ...);
tslist  Tw_CloneAStat(tdisplay TwD, tobj Id, udat hN, TW_CONST udat *h);
tslist  Tw_CloneVStat(tdisplay TwD, tobj Id, udat hN, va_list h);
void    Tw_DeleteStat(tdisplay TwD, tslist TSL);
tsfield Tw_FindStat(  tdisplay TwD, tslist TSL, udat hash);
void    Tw_ChangeField(tdisplay TwD, tobj Obj, udat field, uldat CLEARMask, uldat XORMask);



#define TwStat(Id, h)                   Tw_Stat(      Tw_DefaultD, Id, h)
tslist  TwLStat(tobj Id, uldat hN, ...);
#define TwAStat(     Id, hN, h)		Tw_AStat(     Tw_DefaultD, Id, hN, h)
#define TwVStat(     Id, hN, h)		Tw_VStat(     Tw_DefaultD, Id, hN, h)
tslist  TwCloneLStat(tobj Id, uldat hN, ...);
#define TwCloneAStat(Id, hN, h)		Tw_CloneAStat(Tw_DefaultD, Id, hN, h)
#define TwCloneVStat(Id, hN, h)		Tw_CloneVStat(Tw_DefaultD, Id, hN, h)
#define TwDeleteStat(TSL)		Tw_DeleteStat(Tw_DefaultD, TSL)
#define TwFindStat(TSL, hash)		Tw_IndexStat( Tw_DefaultD, TSL, hash)
#define TwChangeField(Obj, field, CLEARMask, XORMask) \
	Tw_ChangeField(Tw_DefaultD, Obj, field, CLEARMask, XORMask)


#ifdef __cplusplus
}
#endif

#endif /* _TW_STAT_H */

