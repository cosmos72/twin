



/* This file was automatically generated from m4/libTw1.m4, do not edit! */

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

/* To avoid troubles with strlen(), you should not use '\0' or "\0" */
/* Also, using TWS_obj is really a good idea */
#define magic_id_STR(x)	"\xFF"
















{ Tw_FindFunction, 12,
10, (byte *)"Tw_FindFunction", (byte *)"0""_"TWS_uldat_STR"_"TWS_byte_STR"V"TWS_byte_STR"_"TWS_byte_STR"V"TWS_byte_STR },


{ Tw_SyncSocket, 10,
2, (byte *)"Tw_SyncSocket", (byte *)"0""_"TWS_byte_STR },


{ Tw_ServerSizeof, 12,
4, (byte *)"Tw_ServerSizeof", (byte *)"0""_"TWS_byte_STR"_"TWS_byte_STR },


{ Tw_CanCompress, 11,
2, (byte *)"Tw_CanCompress", (byte *)"0""_"TWS_byte_STR },

{ Tw_DoCompress, 10,
4, (byte *)"Tw_DoCompress", (byte *)"0""_"TWS_byte_STR"_"TWS_byte_STR },


{ Tw_NeedResizeDisplay, 17,
2, (byte *)"Tw_NeedResizeDisplay", (byte *)"0""v"TWS_void_STR },


{ Tw_AttachHW, 8,
8, (byte *)"Tw_AttachHW", (byte *)"0""v"TWS_void_STR"_"TWS_uldat_STR"V"TWS_byte_STR"_"TWS_byte_STR },

{ Tw_DetachHW, 8,
6, (byte *)"Tw_DetachHW", (byte *)"0""_"TWS_byte_STR"_"TWS_uldat_STR"V"TWS_byte_STR },


{ Tw_SetFontTranslation, 18,
4, (byte *)"Tw_SetFontTranslation", (byte *)"0""v"TWS_void_STR"V"TWS_byte_STR },

{ Tw_SetHWFontTranslation, 20,
4, (byte *)"Tw_SetHWFontTranslation", (byte *)"0""v"TWS_void_STR"V"TWS_hwfont_STR },


{ Tw_DeleteObj, 9,
4, (byte *)"Tw_DeleteObj", (byte *)"0""v"TWS_void_STR"x"magic_id_STR(obj) },

{ Tw_ChangeFieldObj, 14,
10, (byte *)"Tw_ChangeFieldObj", (byte *)"2""v"TWS_void_STR"x"magic_id_STR(obj)"_"TWS_udat_STR"_"TWS_uldat_STR"_"TWS_uldat_STR },


{ Tw_CreateWidget, 12,
16, (byte *)"Tw_CreateWidget", (byte *)"0""x"magic_id_STR(widget)"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_uldat_STR"_"TWS_uldat_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_hwattr_STR },

{ Tw_RecursiveDeleteWidget, 21,
4, (byte *)"Tw_RecursiveDeleteWidget", (byte *)"0""v"TWS_void_STR"x"magic_id_STR(widget) },
{ Tw_MapWidget, 9,
6, (byte *)"Tw_MapWidget", (byte *)"2""v"TWS_void_STR"x"magic_id_STR(widget)"x"magic_id_STR(widget) },
{ Tw_UnMapWidget, 11,
4, (byte *)"Tw_UnMapWidget", (byte *)"2""v"TWS_void_STR"x"magic_id_STR(widget) },
{ Tw_SetXYWidget, 11,
8, (byte *)"Tw_SetXYWidget", (byte *)"0""v"TWS_void_STR"x"magic_id_STR(widget)"_"TWS_dat_STR"_"TWS_dat_STR },
{ Tw_ResizeWidget, 12,
8, (byte *)"Tw_ResizeWidget", (byte *)"0""v"TWS_void_STR"x"magic_id_STR(widget)"_"TWS_dat_STR"_"TWS_dat_STR },
{ Tw_ScrollWidget, 12,
8, (byte *)"Tw_ScrollWidget", (byte *)"0""v"TWS_void_STR"x"magic_id_STR(widget)"_"TWS_ldat_STR"_"TWS_ldat_STR },
{ Tw_DrawWidget, 10,
18, (byte *)"Tw_DrawWidget", (byte *)"0""v"TWS_void_STR"x"magic_id_STR(widget)"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_dat_STR"W"TWS_byte_STR"W"TWS_hwfont_STR"W"TWS_hwattr_STR },

{ Tw_SetVisibleWidget, 16,
6, (byte *)"Tw_SetVisibleWidget", (byte *)"0""v"TWS_void_STR"x"magic_id_STR(widget)"_"TWS_byte_STR },

{ Tw_FocusSubWidget, 14,
4, (byte *)"Tw_FocusSubWidget", (byte *)"0""v"TWS_void_STR"x"magic_id_STR(widget) },
{ Tw_FindWidgetAtWidget, 18,
8, (byte *)"Tw_FindWidgetAtWidget", (byte *)"2""x"magic_id_STR(widget)"x"magic_id_STR(widget)"_"TWS_dat_STR"_"TWS_dat_STR },

{ Tw_RaiseWidget, 11,
4, (byte *)"Tw_RaiseWidget", (byte *)"2""v"TWS_void_STR"x"magic_id_STR(widget) },
{ Tw_LowerWidget, 11,
4, (byte *)"Tw_LowerWidget", (byte *)"2""v"TWS_void_STR"x"magic_id_STR(widget) },
{ Tw_RestackChildrenWidget, 21,
8, (byte *)"Tw_RestackChildrenWidget", (byte *)"0""v"TWS_void_STR"x"magic_id_STR(widget)"_"TWS_uldat_STR"X"magic_id_STR(widget) },
{ Tw_CirculateChildrenWidget, 23,
6, (byte *)"Tw_CirculateChildrenWidget", (byte *)"0""v"TWS_void_STR"x"magic_id_STR(widget)"_"TWS_byte_STR },


{ Tw_CreateGadget, 12,
28, (byte *)"Tw_CreateGadget", (byte *)"0""x"magic_id_STR(gadget)"x"magic_id_STR(widget)"_"TWS_dat_STR"_"TWS_dat_STR"W"TWS_byte_STR"_"TWS_uldat_STR"_"TWS_uldat_STR"_"TWS_udat_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_dat_STR"_"TWS_dat_STR },

{ Tw_CreateButtonGadget, 18,
24, (byte *)"Tw_CreateButtonGadget", (byte *)"1""x"magic_id_STR(gadget)"x"magic_id_STR(widget)"_"TWS_dat_STR"_"TWS_dat_STR"V"TWS_byte_STR"_"TWS_uldat_STR"_"TWS_udat_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_dat_STR"_"TWS_dat_STR },

{ Tw_WriteTextsGadget, 16,
16, (byte *)"Tw_WriteTextsGadget", (byte *)"2""v"TWS_void_STR"x"magic_id_STR(gadget)"_"TWS_byte_STR"_"TWS_dat_STR"_"TWS_dat_STR"W"TWS_byte_STR"_"TWS_dat_STR"_"TWS_dat_STR },
{ Tw_WriteHWFontsGadget, 18,
16, (byte *)"Tw_WriteHWFontsGadget", (byte *)"2""v"TWS_void_STR"x"magic_id_STR(gadget)"_"TWS_byte_STR"_"TWS_dat_STR"_"TWS_dat_STR"W"TWS_hwfont_STR"_"TWS_dat_STR"_"TWS_dat_STR },

{ Tw_CreateWindow, 12,
24, (byte *)"Tw_CreateWindow", (byte *)"0""x"magic_id_STR(window)"_"TWS_dat_STR"V"TWS_byte_STR"W"TWS_hwcol_STR"x"magic_id_STR(menu)"_"TWS_hwcol_STR"_"TWS_uldat_STR"_"TWS_uldat_STR"_"TWS_uldat_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_dat_STR },
{ Tw_Create4MenuWindow, 17,
4, (byte *)"Tw_Create4MenuWindow", (byte *)"1""x"magic_id_STR(window)"x"magic_id_STR(menu) },

{ Tw_WriteAsciiWindow, 16,
8, (byte *)"Tw_WriteAsciiWindow", (byte *)"0""v"TWS_void_STR"x"magic_id_STR(window)"_"TWS_ldat_STR"V"TWS_byte_STR },
{ Tw_WriteStringWindow, 17,
8, (byte *)"Tw_WriteStringWindow", (byte *)"0""v"TWS_void_STR"x"magic_id_STR(window)"_"TWS_ldat_STR"V"TWS_byte_STR },
{ Tw_WriteHWFontWindow, 17,
8, (byte *)"Tw_WriteHWFontWindow", (byte *)"0""v"TWS_void_STR"x"magic_id_STR(window)"_"TWS_ldat_STR"V"TWS_hwfont_STR },
{ Tw_WriteHWAttrWindow, 17,
12, (byte *)"Tw_WriteHWAttrWindow", (byte *)"0""v"TWS_void_STR"x"magic_id_STR(window)"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_ldat_STR"V"TWS_hwattr_STR },

{ Tw_GotoXYWindow, 12,
8, (byte *)"Tw_GotoXYWindow", (byte *)"2""v"TWS_void_STR"x"magic_id_STR(window)"_"TWS_ldat_STR"_"TWS_ldat_STR },
{ Tw_SetTitleWindow, 14,
8, (byte *)"Tw_SetTitleWindow", (byte *)"0""v"TWS_void_STR"x"magic_id_STR(window)"_"TWS_dat_STR"V"TWS_byte_STR },
{ Tw_SetColTextWindow, 16,
6, (byte *)"Tw_SetColTextWindow", (byte *)"2""v"TWS_void_STR"x"magic_id_STR(window)"_"TWS_hwcol_STR },
{ Tw_SetColorsWindow, 15,
24, (byte *)"Tw_SetColorsWindow", (byte *)"2""v"TWS_void_STR"x"magic_id_STR(window)"_"TWS_udat_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR },
{ Tw_ConfigureWindow, 15,
18, (byte *)"Tw_ConfigureWindow", (byte *)"2""v"TWS_void_STR"x"magic_id_STR(window)"_"TWS_byte_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_dat_STR },
{ Tw_FindRowByCodeWindow, 19,
6, (byte *)"Tw_FindRowByCodeWindow", (byte *)"0""x"magic_id_STR(row)"x"magic_id_STR(window)"_"TWS_dat_STR },

{ Tw_CreateGroup, 11,
2, (byte *)"Tw_CreateGroup", (byte *)"0""x"magic_id_STR(group) },
{ Tw_InsertGadgetGroup, 17,
6, (byte *)"Tw_InsertGadgetGroup", (byte *)"2""v"TWS_void_STR"x"magic_id_STR(group)"x"magic_id_STR(gadget) },
{ Tw_RemoveGadgetGroup, 17,
6, (byte *)"Tw_RemoveGadgetGroup", (byte *)"2""v"TWS_void_STR"x"magic_id_STR(group)"x"magic_id_STR(gadget) },

{ Tw_GetSelectedGadgetGroup, 22,
4, (byte *)"Tw_GetSelectedGadgetGroup", (byte *)"2""x"magic_id_STR(gadget)"x"magic_id_STR(group) },
{ Tw_SetSelectedGadgetGroup, 22,
6, (byte *)"Tw_SetSelectedGadgetGroup", (byte *)"2""v"TWS_void_STR"x"magic_id_STR(group)"x"magic_id_STR(gadget) },

{ Tw_RaiseRow, 8,
4, (byte *)"Tw_RaiseRow", (byte *)"2""v"TWS_void_STR"x"magic_id_STR(row) },
{ Tw_LowerRow, 8,
4, (byte *)"Tw_LowerRow", (byte *)"2""v"TWS_void_STR"x"magic_id_STR(row) },
{ Tw_RestackChildrenRow, 18,
8, (byte *)"Tw_RestackChildrenRow", (byte *)"0""v"TWS_void_STR"x"magic_id_STR(obj)"_"TWS_uldat_STR"X"magic_id_STR(row) }, 
{ Tw_CirculateChildrenRow, 20,
6, (byte *)"Tw_CirculateChildrenRow", (byte *)"0""v"TWS_void_STR"x"magic_id_STR(obj)"_"TWS_byte_STR },

{ Tw_Create4MenuAny, 14,
14, (byte *)"Tw_Create4MenuAny", (byte *)"0""x"magic_id_STR(row)"x"magic_id_STR(obj)"x"magic_id_STR(window)"_"TWS_udat_STR"_"TWS_byte_STR"_"TWS_ldat_STR"V"TWS_byte_STR },

{ Tw_Create4MenuCommonMenuItem, 25,
4, (byte *)"Tw_Create4MenuCommonMenuItem", (byte *)"1""_"TWS_uldat_STR"x"magic_id_STR(menu) },

{ Tw_CreateMenu, 10,
16, (byte *)"Tw_CreateMenu", (byte *)"0""x"magic_id_STR(menu)"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_byte_STR },
{ Tw_SetInfoMenu, 11,
12, (byte *)"Tw_SetInfoMenu", (byte *)"2""v"TWS_void_STR"x"magic_id_STR(menu)"_"TWS_byte_STR"_"TWS_ldat_STR"V"TWS_byte_STR"W"TWS_hwcol_STR },

{ Tw_CreateMsgPort, 13,
6, (byte *)"Tw_CreateMsgPort", (byte *)"0""x"magic_id_STR(msgport)"_"TWS_byte_STR"V"TWS_byte_STR },
{ Tw_FindMsgPort, 11,
8, (byte *)"Tw_FindMsgPort", (byte *)"0""x"magic_id_STR(msgport)"x"magic_id_STR(msgport)"_"TWS_byte_STR"V"TWS_byte_STR },

{ Tw_BgImageScreen, 13,
10, (byte *)"Tw_BgImageScreen", (byte *)"2""v"TWS_void_STR"x"magic_id_STR(screen)"_"TWS_dat_STR"_"TWS_dat_STR"V"TWS_hwattr_STR },

{ Tw_PrevObj, 7,
4, (byte *)"Tw_PrevObj", (byte *)"0""x"magic_id_STR(obj)"x"magic_id_STR(obj) },
{ Tw_NextObj, 7,
4, (byte *)"Tw_NextObj", (byte *)"0""x"magic_id_STR(obj)"x"magic_id_STR(obj) },
{ Tw_ParentObj, 9,
4, (byte *)"Tw_ParentObj", (byte *)"0""x"magic_id_STR(obj)"x"magic_id_STR(obj) },

{ Tw_FirstScreen, 11,
2, (byte *)"Tw_FirstScreen", (byte *)"0""x"magic_id_STR(screen) },
{ Tw_FirstWidget, 11,
4, (byte *)"Tw_FirstWidget", (byte *)"0""x"magic_id_STR(widget)"x"magic_id_STR(widget) },
{ Tw_FirstMsgPort, 12,
2, (byte *)"Tw_FirstMsgPort", (byte *)"0""x"magic_id_STR(msgport) },
{ Tw_FirstMenu, 9,
4, (byte *)"Tw_FirstMenu", (byte *)"0""x"magic_id_STR(menu)"x"magic_id_STR(msgport) },
{ Tw_FirstW, 6,
4, (byte *)"Tw_FirstW", (byte *)"0""x"magic_id_STR(widget)"x"magic_id_STR(msgport) },
{ Tw_FirstGroup, 10,
4, (byte *)"Tw_FirstGroup", (byte *)"0""x"magic_id_STR(group)"x"magic_id_STR(msgport) },
{ Tw_FirstMutex, 10,
4, (byte *)"Tw_FirstMutex", (byte *)"0""x"magic_id_STR(mutex)"x"magic_id_STR(msgport) },
{ Tw_FirstMenuItem, 13,
4, (byte *)"Tw_FirstMenuItem", (byte *)"0""x"magic_id_STR(menuitem)"x"magic_id_STR(menu) },
{ Tw_FirstGadget, 11,
4, (byte *)"Tw_FirstGadget", (byte *)"0""x"magic_id_STR(gadget)"x"magic_id_STR(group) },

{ Tw_GetDisplayWidth, 15,
2, (byte *)"Tw_GetDisplayWidth", (byte *)"0""_"TWS_dat_STR },
{ Tw_GetDisplayHeight, 16,
2, (byte *)"Tw_GetDisplayHeight", (byte *)"0""_"TWS_dat_STR },
{ Tw_GetAll, 6,
2, (byte *)"Tw_GetAll", (byte *)"0""x"magic_id_STR(all) },

{ Tw_SendToMsgPort, 13,
8, (byte *)"Tw_SendToMsgPort", (byte *)"0""_"TWS_byte_STR"x"magic_id_STR(msgport)"_"TWS_udat_STR"V"TWS_byte_STR },
{ Tw_BlindSendToMsgPort, 18,
8, (byte *)"Tw_BlindSendToMsgPort", (byte *)"0""v"TWS_void_STR"x"magic_id_STR(msgport)"_"TWS_udat_STR"V"TWS_byte_STR },

{ Tw_GetOwnerSelection, 17,
2, (byte *)"Tw_GetOwnerSelection", (byte *)"0""x"magic_id_STR(obj) },
{ Tw_SetOwnerSelection, 17,
6, (byte *)"Tw_SetOwnerSelection", (byte *)"0""v"TWS_void_STR"_"TWS_tany_STR"_"TWS_tany_STR },
{ Tw_RequestSelection, 16,
6, (byte *)"Tw_RequestSelection", (byte *)"0""v"TWS_void_STR"x"magic_id_STR(obj)"_"TWS_uldat_STR },
{ Tw_NotifySelection, 15,
14, (byte *)"Tw_NotifySelection", (byte *)"0""v"TWS_void_STR"x"magic_id_STR(obj)"_"TWS_uldat_STR"_"TWS_uldat_STR"V"TWS_byte_STR"_"TWS_uldat_STR"V"TWS_byte_STR },

{ Tw_SetServerUid, 12,
6, (byte *)"Tw_SetServerUid", (byte *)"0""_"TWS_byte_STR"_"TWS_uldat_STR"_"TWS_byte_STR },

{ Tw_OpenExtension, 13,
6, (byte *)"Tw_OpenExtension", (byte *)"0""x"magic_id_STR(extension)"_"TWS_byte_STR"V"TWS_byte_STR },
{ Tw_CallBExtension, 14,
10, (byte *)"Tw_CallBExtension", (byte *)"0""O"TWS_tany_STR"x"magic_id_STR(extension)"_"TWS_topaque_STR"V"TWS_byte_STR"V"TWS_byte_STR },
{ Tw_CloseExtension, 14,
4, (byte *)"Tw_CloseExtension", (byte *)"0""v"TWS_void_STR"x"magic_id_STR(extension) },



