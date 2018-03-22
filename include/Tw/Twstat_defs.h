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

#ifndef _TW_STAT_DEFS_H
#define _TW_STAT_DEFS_H

#define TWS__None			0x0000

#define TWS_obj_Id			0x0001
#define TWS_obj_Prev			0x0002
#define TWS_obj_Next			0x0003
#define TWS_obj_Parent			0x0004
#define TWS_obj_Prev_List		0x0010
#define TWS_obj_Next_List		0x0011
#define TWS_obj_Parent_List		0x0012

#define TWS_widget_Id			TWS_obj_Id
#define TWS_widget_Prev			TWS_obj_Prev
#define TWS_widget_Next			TWS_obj_Next
#define TWS_widget_Parent		TWS_obj_Parent
#define TWS_widget_Prev_List		TWS_Prev_List
#define TWS_widget_Next_List		TWS_Next_List
#define TWS_widget_Parent_List		TWS_Parent_List
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
#define TWS_widget_USE_Fill		0x010E
#define TWS_widget_Fill			TWS_widget_USE_Fill
#define TWS_widget_ChildrenW_List	0x0120
#define TWS_widget_O_Prev_List		0x0121
#define TWS_widget_O_Next_List		0x0122

#define TWS_gadget_Id			TWS_widget_Id
#define TWS_gadget_Prev			TWS_widget_Prev
#define TWS_gadget_Next			TWS_widget_Next
#define TWS_gadget_Parent		TWS_widget_Parent
#define TWS_gadget_Prev_List		TWS_widget_Prev_List
#define TWS_gadget_Next_List		TWS_widget_Next_List
#define TWS_gadget_Parent_List		TWS_widget_Parent_List
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
#define TWS_gadget_ChildrenW_List	TWS_widget_ChildrenW_List
#define TWS_gadget_O_Prev_List		TWS_widget_O_Prev_List
#define TWS_gadget_O_Next_List		TWS_widget_O_Next_List
#define TWS_gadget_ColText		0x0200
#define TWS_gadget_ColNormal		TWS_gadget_ColText
#define TWS_gadget_ColSelect		0x0201
#define TWS_gadget_ColDisabled		0x0202
#define TWS_gadget_ColSelectDisabled	0x0203
#define TWS_gadget_Code			0x0204
#define TWS_gadget_G_Prev		0x0205
#define TWS_gadget_G_Next		0x0206
#define TWS_gadget_Group		0x0207
#define TWS_gadget_USE_T_TextV0		0x0210
#define TWS_gadget_USE_T_TextV1		0x0211
#define TWS_gadget_USE_T_TextV2		0x0212
#define TWS_gadget_USE_T_TextV3		0x0213
#define TWS_gadget_USE_T_TextVN(n)	(TWS_gadget_USE_T_TextV0+(n))
#define TWS_gadget_USE_T_ColorV0	0x0214
#define TWS_gadget_USE_T_ColorV1	0x0215
#define TWS_gadget_USE_T_ColorV2	0x0216
#define TWS_gadget_USE_T_ColorV3	0x0217
#define TWS_gadget_USE_T_ColorVN(n)	(TWS_gadget_USE_T_ColorV0+(n))
#define TWS_gadget_G_Prev_List		0x0220
#define TWS_gadget_G_Next_List		0x0221

#define TWS_window_Id			TWS_widget_Id
#define TWS_window_Prev			TWS_widget_Prev
#define TWS_window_Next			TWS_widget_Next
#define TWS_window_Parent		TWS_widget_Parent
#define TWS_window_Prev_List		TWS_widget_Prev_List
#define TWS_window_Next_List		TWS_widget_Next_List
#define TWS_window_Parent_List		TWS_widget_Parent_List
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
#define TWS_window_ChildrenW_List	TWS_widget_ChildrenW_List
#define TWS_window_O_Prev_List		TWS_widget_O_Prev_List
#define TWS_window_O_Next_List		TWS_widget_O_Next_List
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
#define TWS_window_MenuItem		0x030C
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
#define TWS_window_USE_C_Contents	0x0330
#define TWS_window_USE_C_HSplit		0x0331
#define TWS_window_USE_R_FirstRow	0x0332
#define TWS_window_USE_R_LastRow	0x0333
#define TWS_window_USE_R_ChildrenRow_List	0x0338

#define TWS_screen_Id			TWS_widget_Id
#define TWS_screen_Prev			TWS_widget_Prev
#define TWS_screen_Next			TWS_widget_Next
#define TWS_screen_Parent		TWS_widget_Parent
#define TWS_screen_Prev_List		TWS_widget_Prev_List
#define TWS_screen_Next_List		TWS_widget_Next_List
#define TWS_screen_Parent_List		TWS_widget_Parent_List
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
#define TWS_screen_ChildrenW_List	TWS_widget_ChildrenW_List
#define TWS_screen_O_Prev_List		TWS_widget_O_Prev_List
#define TWS_screen_O_Next_List		TWS_widget_O_Next_List
#define TWS_screen_NameLen		0x0400
#define TWS_screen_Name			0x0401
#define TWS_screen_State		0x0402
#define TWS_screen_USE_B_BgWidth	0x0403
#define TWS_screen_USE_B_BgHeight	0x0404
#define TWS_screen_USE_B_Bg		0x0405

#define TWS_group_Id			TWS_obj_Id
#define TWS_group_Prev			TWS_obj_Prev
#define TWS_group_Next			TWS_obj_Next
#define TWS_group_Parent		TWS_obj_Parent
#define TWS_group_Prev_List		TWS_obj_Prev_List
#define TWS_group_Next_List		TWS_obj_Next_List
#define TWS_group_Parent_List		TWS_obj_Parent_List
#define TWS_group_Owner			TWS_obj_Parent
#define TWS_group_MsgPort		TWS_obj_Parent
#define TWS_group_FirstG		0x0500
#define TWS_group_LastG			0x0501
#define TWS_group_SelectG		0x0502
#define TWS_group_ChildrenG_List	0x0510

#define TWS_row_Id			TWS_obj_Id
#define TWS_row_Prev			TWS_obj_Prev
#define TWS_row_Next			TWS_obj_Next
#define TWS_row_Parent			TWS_obj_Parent
#define TWS_row_Prev_List		TWS_obj_Prev_List
#define TWS_row_Next_List		TWS_obj_Next_List
#define TWS_row_Parent_List		TWS_obj_Parent_List
#define TWS_row_Code			0x0600
#define TWS_row_Flags			0x0601
#define TWS_row_Len			0x0602
#define TWS_row_Text			0x0603
#define TWS_row_ColText			0x0604

#define TWS_menuitem_Id			TWS_row_Id
#define TWS_menuitem_Prev		TWS_row_Prev
#define TWS_menuitem_Next		TWS_row_Next
#define TWS_menuitem_Parent		TWS_row_Parent
#define TWS_menuitem_Prev_List		TWS_row_Prev_List
#define TWS_menuitem_Next_List		TWS_row_Next_List
#define TWS_menuitem_Parent_List	TWS_row_Parent_List
#define TWS_menuitem_Code		TWS_row_Code
#define TWS_menuitem_Flags		TWS_row_Flags
#define TWS_menuitem_Len		TWS_row_Len
#define TWS_menuitem_Text		TWS_row_Text
#define TWS_menuitem_ColText		TWS_row_ColText
#define TWS_menuitem_Window		0x0700
#define TWS_menuitem_Left		0x0702
#define TWS_menuitem_ShortCut		0x0704

#define TWS_menu_Id			TWS_obj_Id
#define TWS_menu_Prev			TWS_obj_Prev
#define TWS_menu_Next			TWS_obj_Next
#define TWS_menu_Parent			TWS_obj_Parent
#define TWS_menu_Prev_List		TWS_obj_Prev_List
#define TWS_menu_Next_List		TWS_obj_Next_List
#define TWS_menu_Parent_List		TWS_obj_Parent_List
#define TWS_menu_ColItem		0x0800
#define TWS_menu_ColSelect		0x0801
#define TWS_menu_ColDisabled		0x0802
#define TWS_menu_ColSelectDisabled	0x0803
#define TWS_menu_ColShtCut		0x0804
#define TWS_menu_ColSelShtCut		0x0805
#define TWS_menu_CommonItems		0x0806
#define TWS_menu_FlagDefColInfo		0x0807
#define TWS_menu_FirstI			0x0808
#define TWS_menu_LastI			0x0809
#define TWS_menu_SelectI		0x080A
#define TWS_menu_ChildrenI_List		0x0810

#define TWS_msgport_Id			TWS_obj_Id
#define TWS_msgport_Prev		TWS_obj_Prev
#define TWS_msgport_Next		TWS_obj_Next
#define TWS_msgport_Parent		TWS_obj_Parent
#define TWS_msgport_Prev_List		TWS_obj_Prev_List
#define TWS_msgport_Next_List		TWS_obj_Next_List
#define TWS_msgport_Parent_List		TWS_obj_Parent_List
#define TWS_msgport_WakeUp		0x0900
#define TWS_msgport_NameLen		0x0901
#define TWS_msgport_Name		0x0902
#define TWS_msgport_FirstMenu		0x0903
#define TWS_msgport_LastMenu		0x0904
#define TWS_msgport_FirstW		0x0905
#define TWS_msgport_LastW		0x0906
#define TWS_msgport_FirstGroup		0x0907
#define TWS_msgport_LastGroup		0x0908
#define TWS_msgport_FirstMutex		0x0909
#define TWS_msgport_LastMutex		0x090A
#define TWS_msgport_ChildrenMenu_List	0x0910
#define TWS_msgport_ChildrenW_List	0x0911
#define TWS_msgport_ChildrenGroup_List	0x0912
#define TWS_msgport_ChildrenMutex_List	0x0913

#define TWS_mutex_Id			TWS_obj_Id
#define TWS_mutex_Prev			TWS_obj_Prev
#define TWS_mutex_Next			TWS_obj_Next
#define TWS_mutex_Parent		TWS_obj_Parent
#define TWS_mutex_Prev_List		TWS_obj_Prev_List
#define TWS_mutex_Next_List		TWS_obj_Next_List
#define TWS_mutex_Parent_List		TWS_obj_Parent_List
#define TWS_mutex_O_Prev		0x0B00
#define TWS_mutex_O_Next		0x0B01
#define TWS_mutex_Owner			0x0B02
#define TWS_mutex_Msgport		TWS_mutex_Owner
#define TWS_mutex_Perm			0x0B03
#define TWS_mutex_NameLen		0x0B04
#define TWS_mutex_Name			0x0B05
#define TWS_mutex_O_Prev_List		0x0B10
#define TWS_mutex_O_Next_List		0x0B11

#define TWS_all_Id			TWS_obj_Id
#define TWS_all_Prev			TWS_obj_Prev
#define TWS_all_Next			TWS_obj_Next
#define TWS_all_Parent			TWS_obj_Parent
#define TWS_all_Prev_List		TWS_obj_Prev_List
#define TWS_all_Next_List		TWS_obj_Next_List
#define TWS_all_Parent_List		TWS_obj_Parent_List
#define TWS_all_FirstScreen		0xF000
#define TWS_all_LastScreen		0xF001
#define TWS_all_FirstMsgPort		0xF002
#define TWS_all_LastMsgPort		0xF003
#define TWS_all_FirstMutex		0xF004
#define TWS_all_LastMutex		0xF005
#define TWS_all_ChildrenScreen_List	0xF010
#define TWS_all_ChildrenMsgPort_List	0xF011
#define TWS_all_ChildrenMutex_List	0xF012
#define TWS_all_DisplayWidth		0xF020
#define TWS_all_DisplayHeight		0xF021
#define TWS_all_BuiltinMenu		0xF022
#define TWS_all_CommonMenu		0xF023

/* TWS_field_list_EL() does not include TWS_*_List fields. */

#define TWS_field_list_EL(EL) \
	EL(obj_Id) \
	EL(obj_Prev) \
	EL(obj_Next) \
	EL(obj_Parent) \
	\
	EL(widget_FirstW) \
	EL(widget_LastW) \
	EL(widget_SelectW) \
	EL(widget_Left) \
	EL(widget_Up) \
	EL(widget_Width) \
	EL(widget_Height) \
	EL(widget_Attrib) \
	EL(widget_Flags) \
	EL(widget_XLogic) \
	EL(widget_YLogic) \
	EL(widget_O_Prev) \
	EL(widget_O_Next) \
	EL(widget_Owner) \
	EL(widget_USE_Fill) \
	\
	EL(gadget_ColText) \
	EL(gadget_ColSelect) \
	EL(gadget_ColDisabled) \
	EL(gadget_ColSelectDisabled) \
	EL(gadget_Code) \
	EL(gadget_G_Prev) \
	EL(gadget_G_Next) \
	EL(gadget_Group) \
	EL(gadget_USE_T_TextV0) \
	EL(gadget_USE_T_TextV1) \
	EL(gadget_USE_T_TextV2) \
	EL(gadget_USE_T_TextV3) \
	EL(gadget_USE_T_ColorV0) \
	EL(gadget_USE_T_ColorV1) \
	EL(gadget_USE_T_ColorV2) \
	EL(gadget_USE_T_ColorV3) \
	\
	EL(window_Menu) \
	EL(window_NameLen) \
	EL(window_Name) \
	EL(window_ColName) \
	EL(window_BorderPatternV0) \
	EL(window_BorderPatternV1) \
	EL(window_CurX) \
	EL(window_CurY) \
	EL(window_XstSel) \
	EL(window_YstSel) \
	EL(window_XendSel) \
	EL(window_YendSel) \
	EL(window_MenuItem) \
	EL(window_ColGadgets) \
	EL(window_ColArrows) \
	EL(window_ColBars) \
	EL(window_ColTabs) \
	EL(window_ColBorder) \
	EL(window_ColText) \
	EL(window_ColSelect) \
	EL(window_ColDisabled) \
	EL(window_ColSelectDisabled) \
	EL(window_State) \
	EL(window_CursorType) \
	EL(window_MinXWidth) \
	EL(window_MinYWidth) \
	EL(window_MaxXWidth) \
	EL(window_MaxYWidth) \
	EL(window_WLogic) \
	EL(window_HLogic) \
	EL(window_USE_C_Contents) \
	EL(window_USE_C_HSplit) \
	EL(window_USE_R_FirstRow) \
	EL(window_USE_R_LastRow) \
	\
	EL(screen_NameLen) \
	EL(screen_Name) \
	EL(screen_State) \
	EL(screen_USE_B_BgWidth) \
	EL(screen_USE_B_BgHeight) \
	EL(screen_USE_B_Bg) \
	\
	EL(group_FirstG) \
	EL(group_LastG) \
	EL(group_SelectG) \
	\
	EL(row_Code) \
	EL(row_Flags) \
	EL(row_Len) \
	EL(row_Text) \
	EL(row_ColText)	\
	\
	EL(menuitem_Window) \
	EL(menuitem_Left) \
	EL(menuitem_ShortCut) \
	\
	EL(menu_ColItem) \
	EL(menu_ColSelect) \
	EL(menu_ColDisabled) \
	EL(menu_ColSelectDisabled) \
	EL(menu_ColShtCut) \
	EL(menu_ColSelShtCut) \
	EL(menu_CommonItems) \
	EL(menu_FlagDefColInfo) \
	EL(menu_FirstI) \
	EL(menu_LastI) \
	EL(menu_SelectI) \
	\
	EL(msgport_WakeUp) \
	EL(msgport_NameLen) \
	EL(msgport_Name) \
	EL(msgport_FirstMenu) \
	EL(msgport_LastMenu) \
	EL(msgport_FirstW) \
	EL(msgport_LastW) \
	EL(msgport_FirstGroup) \
	EL(msgport_LastGroup) \
	EL(msgport_FirstMutex) \
	EL(msgport_LastMutex) \
	\
	EL(mutex_O_Prev) \
	EL(mutex_O_Next) \
	EL(mutex_Owner) \
	EL(mutex_Perm) \
	EL(mutex_NameLen) \
	EL(mutex_Name) \
	\
	EL(all_FirstScreen) \
	EL(all_LastScreen) \
	EL(all_FirstMsgPort) \
	EL(all_LastMsgPort) \
	EL(all_FirstMutex) \
	EL(all_LastMutex) \
	EL(all_DisplayWidth) \
	EL(all_DisplayHeight) \
	EL(all_BuiltinMenu) \
	EL(all_CommonMenu)


/* TWS_*_List fields are kept separate */

#define TWS_field_list_List_EL(EL) \
	EL(obj_Prev_List) \
	EL(obj_Next_List) \
	EL(obj_Parent_List) \
	EL(widget_ChildrenW_List) \
	EL(widget_O_Prev_List) \
	EL(widget_O_Next_List) \
	EL(gadget_G_Prev_List) \
	EL(gadget_G_Next_List) \
	EL(window_USE_R_ChildrenRow_List) \
	EL(group_ChildrenG_List) \
	EL(menu_ChildrenI_List) \
	EL(msgport_ChildrenMenu_List) \
	EL(msgport_ChildrenW_List) \
	EL(msgport_ChildrenGroup_List) \
	EL(msgport_ChildrenMutex_List) \
	EL(all_ChildrenScreen_List) \
	EL(all_ChildrenMsgPort_List) \
	EL(all_ChildrenMutex_List)

#endif /* _TW_STAT_DEFS_H */

