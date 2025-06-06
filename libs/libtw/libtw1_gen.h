
/* This file was automatically generated with scheme from `twin/include/scheme/sockproto_inc.scm', do not edit! */

/**
 *  Copyright (C) 2001-2025 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 */
{ Tw_FindFunction, 12,
10, "Tw_FindFunction", "0" "_" TWS_uldat_STR "_" TWS_byte_STR "V" TWS_char_STR "_" TWS_byte_STR "V" TWS_char_STR },
{ Tw_SyncSocket, 10,
2, "Tw_SyncSocket", "0" "_" TWS_byte_STR },
{ Tw_ServerSizeof, 12,
4, "Tw_ServerSizeof", "0" "_" TWS_byte_STR "_" TWS_byte_STR },
{ Tw_CanCompress, 11,
2, "Tw_CanCompress", "0" "_" TWS_byte_STR },
{ Tw_DoCompress, 10,
4, "Tw_DoCompress", "0" "_" TWS_byte_STR "_" TWS_byte_STR },
{ Tw_NeedResizeDisplay, 17,
2, "Tw_NeedResizeDisplay", "0" "v" TWS_void_STR },
{ Tw_AttachHW, 8,
8, "Tw_AttachHW", "0" "v" TWS_void_STR "_" TWS_uldat_STR "V" TWS_char_STR "_" TWS_byte_STR },
{ Tw_DetachHW, 8,
6, "Tw_DetachHW", "0" "_" TWS_byte_STR "_" TWS_uldat_STR "V" TWS_char_STR },
{ Tw_SetFontTranslation, 18,
4, "Tw_SetFontTranslation", "0" "v" TWS_void_STR "V" TWS_byte_STR },
{ Tw_SetTRuneTranslation, 19,
4, "Tw_SetTRuneTranslation", "0" "v" TWS_void_STR "V" TWS_trune_STR },
{ Tw_DeleteObj, 9,
4, "Tw_DeleteObj", "0" "v" TWS_void_STR "x" TWS_tobj_STR },
{ Tw_ChangeFieldObj, 14,
10, "Tw_ChangeFieldObj", "2" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_udat_STR "_" TWS_uldat_STR "_" TWS_uldat_STR },
{ Tw_CreateWidget, 12,
16, "Tw_CreateWidget", "0" "x" TWS_tobj_STR "_" TWS_dat_STR "_" TWS_dat_STR "_" TWS_uldat_STR "_" TWS_uldat_STR "_" TWS_dat_STR "_" TWS_dat_STR "_" TWS_tcell_STR },
{ Tw_RecursiveDeleteWidget, 21,
4, "Tw_RecursiveDeleteWidget", "0" "v" TWS_void_STR "x" TWS_tobj_STR },
{ Tw_MapWidget, 9,
6, "Tw_MapWidget", "2" "v" TWS_void_STR "x" TWS_tobj_STR "x" TWS_tobj_STR },
{ Tw_UnMapWidget, 11,
4, "Tw_UnMapWidget", "2" "v" TWS_void_STR "x" TWS_tobj_STR },
{ Tw_SetXYWidget, 11,
8, "Tw_SetXYWidget", "0" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_dat_STR "_" TWS_dat_STR },
{ Tw_ResizeWidget, 12,
8, "Tw_ResizeWidget", "0" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_dat_STR "_" TWS_dat_STR },
{ Tw_ScrollWidget, 12,
8, "Tw_ScrollWidget", "0" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_ldat_STR "_" TWS_ldat_STR },
{ Tw_DrawWidget, 10,
18, "Tw_DrawWidget", "0" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_dat_STR "_" TWS_dat_STR "_" TWS_dat_STR "_" TWS_dat_STR "W" TWS_char_STR "W" TWS_trune_STR "W" TWS_tcell_STR },
{ Tw_SetVisibleWidget, 16,
6, "Tw_SetVisibleWidget", "0" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_byte_STR },
{ Tw_FocusSubWidget, 14,
4, "Tw_FocusSubWidget", "0" "v" TWS_void_STR "x" TWS_tobj_STR },
{ Tw_FindWidgetAtWidget, 18,
8, "Tw_FindWidgetAtWidget", "2" "x" TWS_tobj_STR "x" TWS_tobj_STR "_" TWS_dat_STR "_" TWS_dat_STR },
{ Tw_RaiseWidget, 11,
4, "Tw_RaiseWidget", "2" "v" TWS_void_STR "x" TWS_tobj_STR },
{ Tw_LowerWidget, 11,
4, "Tw_LowerWidget", "2" "v" TWS_void_STR "x" TWS_tobj_STR },
{ Tw_RestackChildrenWidget, 21,
8, "Tw_RestackChildrenWidget", "0" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_uldat_STR "X" TWS_tobj_STR },
{ Tw_CirculateChildrenWidget, 23,
6, "Tw_CirculateChildrenWidget", "0" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_byte_STR },
{ Tw_CreateGadget, 12,
28, "Tw_CreateGadget", "0" "x" TWS_tobj_STR "x" TWS_tobj_STR "_" TWS_dat_STR "_" TWS_dat_STR "W" TWS_char_STR "_" TWS_uldat_STR "_" TWS_uldat_STR "_" TWS_udat_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_dat_STR "_" TWS_dat_STR },
{ Tw_CreateButtonGadget, 18,
24, "Tw_CreateButtonGadget", "1" "x" TWS_tobj_STR "x" TWS_tobj_STR "_" TWS_dat_STR "_" TWS_dat_STR "V" TWS_char_STR "_" TWS_uldat_STR "_" TWS_udat_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_dat_STR "_" TWS_dat_STR },
{ Tw_WriteTextsGadget, 16,
16, "Tw_WriteTextsGadget", "2" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_byte_STR "_" TWS_dat_STR "_" TWS_dat_STR "W" TWS_char_STR "_" TWS_dat_STR "_" TWS_dat_STR },
{ Tw_WriteTRunesGadget, 17,
16, "Tw_WriteTRunesGadget", "2" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_byte_STR "_" TWS_dat_STR "_" TWS_dat_STR "W" TWS_trune_STR "_" TWS_dat_STR "_" TWS_dat_STR },
{ Tw_CreateWindow, 12,
24, "Tw_CreateWindow", "0" "x" TWS_tobj_STR "_" TWS_dat_STR "V" TWS_char_STR "W" TWS_tcolor_STR "x" TWS_tobj_STR "_" TWS_tcolor_STR "_" TWS_uldat_STR "_" TWS_uldat_STR "_" TWS_uldat_STR "_" TWS_dat_STR "_" TWS_dat_STR "_" TWS_dat_STR },
{ Tw_Create4MenuWindow, 17,
4, "Tw_Create4MenuWindow", "1" "x" TWS_tobj_STR "x" TWS_tobj_STR },
{ Tw_WriteCharsetWindow, 18,
8, "Tw_WriteCharsetWindow", "0" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_uldat_STR "V" TWS_char_STR },
{ Tw_WriteUtf8Window, 15,
8, "Tw_WriteUtf8Window", "0" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_uldat_STR "V" TWS_char_STR },
{ Tw_WriteTRuneWindow, 16,
8, "Tw_WriteTRuneWindow", "0" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_uldat_STR "V" TWS_trune_STR },
{ Tw_WriteTCellWindow, 16,
12, "Tw_WriteTCellWindow", "0" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_dat_STR "_" TWS_dat_STR "_" TWS_uldat_STR "V" TWS_tcell_STR },
{ Tw_GotoXYWindow, 12,
8, "Tw_GotoXYWindow", "2" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_ldat_STR "_" TWS_ldat_STR },
{ Tw_SetTitleWindow, 14,
8, "Tw_SetTitleWindow", "0" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_dat_STR "V" TWS_char_STR },
{ Tw_SetColTextWindow, 16,
6, "Tw_SetColTextWindow", "2" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_tcolor_STR },
{ Tw_SetColorsWindow, 15,
24, "Tw_SetColorsWindow", "2" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_udat_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR },
{ Tw_ConfigureWindow, 15,
18, "Tw_ConfigureWindow", "2" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_byte_STR "_" TWS_dat_STR "_" TWS_dat_STR "_" TWS_dat_STR "_" TWS_dat_STR "_" TWS_dat_STR "_" TWS_dat_STR },
{ Tw_FindRowByCodeWindow, 19,
6, "Tw_FindRowByCodeWindow", "0" "x" TWS_tobj_STR "x" TWS_tobj_STR "_" TWS_dat_STR },
{ Tw_CreateGroup, 11,
2, "Tw_CreateGroup", "0" "x" TWS_tobj_STR },
{ Tw_InsertGadgetGroup, 17,
6, "Tw_InsertGadgetGroup", "2" "v" TWS_void_STR "x" TWS_tobj_STR "x" TWS_tobj_STR },
{ Tw_RemoveGadgetGroup, 17,
6, "Tw_RemoveGadgetGroup", "2" "v" TWS_void_STR "x" TWS_tobj_STR "x" TWS_tobj_STR },
{ Tw_GetSelectedGadgetGroup, 22,
4, "Tw_GetSelectedGadgetGroup", "2" "x" TWS_tobj_STR "x" TWS_tobj_STR },
{ Tw_SetSelectedGadgetGroup, 22,
6, "Tw_SetSelectedGadgetGroup", "2" "v" TWS_void_STR "x" TWS_tobj_STR "x" TWS_tobj_STR },
{ Tw_RaiseRow, 8,
4, "Tw_RaiseRow", "2" "v" TWS_void_STR "x" TWS_tobj_STR },
{ Tw_LowerRow, 8,
4, "Tw_LowerRow", "2" "v" TWS_void_STR "x" TWS_tobj_STR },
{ Tw_RestackChildrenRow, 18,
8, "Tw_RestackChildrenRow", "0" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_uldat_STR "X" TWS_tobj_STR },
{ Tw_CirculateChildrenRow, 20,
6, "Tw_CirculateChildrenRow", "0" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_byte_STR },
{ Tw_Create4MenuAny, 14,
14, "Tw_Create4MenuAny", "0" "x" TWS_tobj_STR "x" TWS_tobj_STR "x" TWS_tobj_STR "_" TWS_udat_STR "_" TWS_byte_STR "_" TWS_ldat_STR "V" TWS_char_STR },
{ Tw_Create4MenuCommonMenuItem, 25,
4, "Tw_Create4MenuCommonMenuItem", "1" "_" TWS_uldat_STR "x" TWS_tobj_STR },
{ Tw_CreateMenu, 10,
16, "Tw_CreateMenu", "0" "x" TWS_tobj_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_byte_STR },
{ Tw_SetInfoMenu, 11,
12, "Tw_SetInfoMenu", "2" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_byte_STR "_" TWS_ldat_STR "V" TWS_char_STR "W" TWS_tcolor_STR },
{ Tw_CreateMsgPort, 13,
6, "Tw_CreateMsgPort", "0" "x" TWS_tobj_STR "_" TWS_byte_STR "V" TWS_char_STR },
{ Tw_FindMsgPort, 11,
8, "Tw_FindMsgPort", "0" "x" TWS_tobj_STR "x" TWS_tobj_STR "_" TWS_byte_STR "V" TWS_char_STR },
{ Tw_BgImageScreen, 13,
10, "Tw_BgImageScreen", "2" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_dat_STR "_" TWS_dat_STR "V" TWS_tcell_STR },
{ Tw_PrevObj, 7,
4, "Tw_PrevObj", "0" "x" TWS_tobj_STR "x" TWS_tobj_STR },
{ Tw_NextObj, 7,
4, "Tw_NextObj", "0" "x" TWS_tobj_STR "x" TWS_tobj_STR },
{ Tw_ParentObj, 9,
4, "Tw_ParentObj", "0" "x" TWS_tobj_STR "x" TWS_tobj_STR },
{ Tw_FirstScreen, 11,
2, "Tw_FirstScreen", "0" "x" TWS_tobj_STR },
{ Tw_FirstWidget, 11,
4, "Tw_FirstWidget", "0" "x" TWS_tobj_STR "x" TWS_tobj_STR },
{ Tw_FirstMsgPort, 12,
2, "Tw_FirstMsgPort", "0" "x" TWS_tobj_STR },
{ Tw_FirstMenu, 9,
4, "Tw_FirstMenu", "0" "x" TWS_tobj_STR "x" TWS_tobj_STR },
{ Tw_FirstW, 6,
4, "Tw_FirstW", "0" "x" TWS_tobj_STR "x" TWS_tobj_STR },
{ Tw_FirstGroup, 10,
4, "Tw_FirstGroup", "0" "x" TWS_tobj_STR "x" TWS_tobj_STR },
{ Tw_FirstMutex, 10,
4, "Tw_FirstMutex", "0" "x" TWS_tobj_STR "x" TWS_tobj_STR },
{ Tw_FirstMenuItem, 13,
4, "Tw_FirstMenuItem", "0" "x" TWS_tobj_STR "x" TWS_tobj_STR },
{ Tw_FirstGadget, 11,
4, "Tw_FirstGadget", "0" "x" TWS_tobj_STR "x" TWS_tobj_STR },
{ Tw_GetDisplayWidth, 15,
2, "Tw_GetDisplayWidth", "0" "_" TWS_dat_STR },
{ Tw_GetDisplayHeight, 16,
2, "Tw_GetDisplayHeight", "0" "_" TWS_dat_STR },
{ Tw_GetAll, 6,
2, "Tw_GetAll", "0" "x" TWS_tobj_STR },
{ Tw_SendToMsgPort, 13,
8, "Tw_SendToMsgPort", "0" "_" TWS_byte_STR "x" TWS_tobj_STR "_" TWS_udat_STR "V" TWS_byte_STR },
{ Tw_BlindSendToMsgPort, 18,
8, "Tw_BlindSendToMsgPort", "0" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_udat_STR "V" TWS_byte_STR },
{ Tw_GetOwnerSelection, 17,
2, "Tw_GetOwnerSelection", "0" "x" TWS_tobj_STR },
{ Tw_SetOwnerSelection, 17,
6, "Tw_SetOwnerSelection", "0" "v" TWS_void_STR "_" TWS_tany_STR "_" TWS_tany_STR },
{ Tw_RequestSelection, 16,
6, "Tw_RequestSelection", "0" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_uldat_STR },
{ Tw_NotifySelection, 15,
14, "Tw_NotifySelection", "0" "v" TWS_void_STR "x" TWS_tobj_STR "_" TWS_uldat_STR "_" TWS_uldat_STR "V" TWS_char_STR "_" TWS_uldat_STR "V" TWS_char_STR },
{ Tw_SetServerUid, 12,
6, "Tw_SetServerUid", "0" "_" TWS_byte_STR "_" TWS_uldat_STR "_" TWS_byte_STR },
