



/* This file was automatically generated from m4/libTw1.m4, do not edit! */


/* To avoid troubles with strlen(), you should not use '\0' or "\0" */
/* Also, using TWS_obj is really a good idea */
#define magic_id_STR(x)	"\xFF"












{ Tw_FindFunction, 0, 0, "Tw_FindFunction", "0""_"TWS_uldat_STR"_"TWS_byte_STR"V"TWS_byte_STR"_"TWS_byte_STR"V"TWS_byte_STR },

{ Tw_SyncSocket, 0, 0, "Tw_SyncSocket", "0""_"TWS_byte_STR },

{ Tw_ServerSizeof, 0, 0, "Tw_ServerSizeof", "0""_"TWS_byte_STR"_"TWS_byte_STR },

{ Tw_CanCompress, 0, 0, "Tw_CanCompress", "0""_"TWS_byte_STR },
{ Tw_DoCompress, 0, 0, "Tw_DoCompress", "0""_"TWS_byte_STR"_"TWS_byte_STR },

{ Tw_NeedResizeDisplay, 0, 0, "Tw_NeedResizeDisplay", "0""v"TWS_void_STR },

{ Tw_AttachHW, 0, 0, "Tw_AttachHW", "0""v"TWS_void_STR"_"TWS_uldat_STR"V"TWS_byte_STR"_"TWS_byte_STR },
{ Tw_DetachHW, 0, 0, "Tw_DetachHW", "0""_"TWS_byte_STR"_"TWS_uldat_STR"V"TWS_byte_STR },

{ Tw_SetFontTranslation, 0, 0, "Tw_SetFontTranslation", "0""v"TWS_void_STR"V"TWS_byte_STR },
{ Tw_SetUniFontTranslation, 0, 0, "Tw_SetUniFontTranslation", "0""v"TWS_void_STR"V"TWS_hwfont_STR },

{ Tw_DeleteObj, 0, 0, "Tw_DeleteObj", "0""v"TWS_void_STR"x"magic_id_STR(obj) },
{ Tw_ChangeFieldObj, 0, 0, "Tw_ChangeFieldObj", "2""v"TWS_void_STR"x"magic_id_STR(obj)"_"TWS_udat_STR"_"TWS_uldat_STR"_"TWS_uldat_STR },


{ Tw_CreateWidget, 0, 0, "Tw_CreateWidget", "0""x"magic_id_STR(widget)"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_uldat_STR"_"TWS_uldat_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_hwattr_STR },
{ Tw_RecursiveDeleteWidget, 0, 0, "Tw_RecursiveDeleteWidget", "0""v"TWS_void_STR"x"magic_id_STR(widget) },
{ Tw_MapWidget, 0, 0, "Tw_MapWidget", "2""v"TWS_void_STR"x"magic_id_STR(widget)"x"magic_id_STR(widget) },
{ Tw_UnMapWidget, 0, 0, "Tw_UnMapWidget", "2""v"TWS_void_STR"x"magic_id_STR(widget) },
{ Tw_SetXYWidget, 0, 0, "Tw_SetXYWidget", "0""v"TWS_void_STR"x"magic_id_STR(widget)"_"TWS_dat_STR"_"TWS_dat_STR },
{ Tw_ResizeWidget, 0, 0, "Tw_ResizeWidget", "0""v"TWS_void_STR"x"magic_id_STR(widget)"_"TWS_dat_STR"_"TWS_dat_STR },
{ Tw_GetOwnerWidget, 0, 0, "Tw_GetOwnerWidget", "0""x"magic_id_STR(msgport)"x"magic_id_STR(widget) },
{ Tw_ExposeWidget, 0, 0, "Tw_ExposeWidget", "0""v"TWS_void_STR"x"magic_id_STR(widget)"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_dat_STR"W"TWS_byte_STR"W"TWS_hwfont_STR"W"TWS_hwattr_STR },

{ Tw_FocusSubWidget, 0, 0, "Tw_FocusSubWidget", "0""v"TWS_void_STR"x"magic_id_STR(widget) },
{ Tw_FindWidgetAtWidget, 0, 0, "Tw_FindWidgetAtWidget", "2""x"magic_id_STR(widget)"x"magic_id_STR(widget)"_"TWS_dat_STR"_"TWS_dat_STR },

{ Tw_RaiseWidget, 0, 0, "Tw_RaiseWidget", "2""v"TWS_void_STR"x"magic_id_STR(widget) },
{ Tw_LowerWidget, 0, 0, "Tw_LowerWidget", "2""v"TWS_void_STR"x"magic_id_STR(widget) },
{ Tw_RestackChildrenWidget, 0, 0, "Tw_RestackChildrenWidget", "0""v"TWS_void_STR"x"magic_id_STR(widget)"_"TWS_uldat_STR"X"magic_id_STR(widget) },
{ Tw_CirculateChildrenWidget, 0, 0, "Tw_CirculateChildrenWidget", "0""v"TWS_void_STR"x"magic_id_STR(widget)"_"TWS_byte_STR },


{ Tw_CreateGadget, 0, 0, "Tw_CreateGadget", "0""x"magic_id_STR(gadget)"x"magic_id_STR(widget)"_"TWS_dat_STR"_"TWS_dat_STR"W"TWS_byte_STR"_"TWS_uldat_STR"_"TWS_uldat_STR"_"TWS_udat_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_dat_STR"_"TWS_dat_STR },

{ Tw_CreateButtonGadget, 0, 0, "Tw_CreateButtonGadget", "1""x"magic_id_STR(gadget)"x"magic_id_STR(widget)"_"TWS_dat_STR"_"TWS_dat_STR"V"TWS_byte_STR"_"TWS_uldat_STR"_"TWS_udat_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_dat_STR"_"TWS_dat_STR },

{ Tw_WriteTextsGadget, 0, 0, "Tw_WriteTextsGadget", "2""v"TWS_void_STR"x"magic_id_STR(gadget)"_"TWS_byte_STR"_"TWS_dat_STR"_"TWS_dat_STR"W"TWS_byte_STR"_"TWS_dat_STR"_"TWS_dat_STR },
{ Tw_WriteHWFontsGadget, 0, 0, "Tw_WriteHWFontsGadget", "2""v"TWS_void_STR"x"magic_id_STR(gadget)"_"TWS_byte_STR"_"TWS_dat_STR"_"TWS_dat_STR"W"TWS_hwfont_STR"_"TWS_dat_STR"_"TWS_dat_STR },

{ Tw_CreateWindow, 0, 0, "Tw_CreateWindow", "1""x"magic_id_STR(window)"_"TWS_dat_STR"V"TWS_byte_STR"W"TWS_hwcol_STR"x"magic_id_STR(menu)"_"TWS_hwcol_STR"_"TWS_uldat_STR"_"TWS_uldat_STR"_"TWS_uldat_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_dat_STR },
{ Tw_Create4MenuWindow, 0, 0, "Tw_Create4MenuWindow", "1""x"magic_id_STR(window)"x"magic_id_STR(menu) },

{ Tw_WriteAsciiWindow, 0, 0, "Tw_WriteAsciiWindow", "0""v"TWS_void_STR"x"magic_id_STR(window)"_"TWS_ldat_STR"V"TWS_byte_STR },
{ Tw_WriteStringWindow, 0, 0, "Tw_WriteStringWindow", "0""v"TWS_void_STR"x"magic_id_STR(window)"_"TWS_ldat_STR"V"TWS_byte_STR },
{ Tw_WriteHWFontWindow, 0, 0, "Tw_WriteHWFontWindow", "0""v"TWS_void_STR"x"magic_id_STR(window)"_"TWS_ldat_STR"V"TWS_hwfont_STR },
{ Tw_WriteHWAttrWindow, 0, 0, "Tw_WriteHWAttrWindow", "0""v"TWS_void_STR"x"magic_id_STR(window)"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_ldat_STR"V"TWS_hwattr_STR },

{ Tw_GotoXYWindow, 0, 0, "Tw_GotoXYWindow", "2""v"TWS_void_STR"x"magic_id_STR(window)"_"TWS_ldat_STR"_"TWS_ldat_STR },
{ Tw_SetColTextWindow, 0, 0, "Tw_SetColTextWindow", "2""v"TWS_void_STR"x"magic_id_STR(window)"_"TWS_hwcol_STR },
{ Tw_SetColorsWindow, 0, 0, "Tw_SetColorsWindow", "2""v"TWS_void_STR"x"magic_id_STR(window)"_"TWS_udat_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR },
{ Tw_ConfigureWindow, 0, 0, "Tw_ConfigureWindow", "2""v"TWS_void_STR"x"magic_id_STR(window)"_"TWS_byte_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_dat_STR },
{ Tw_FindRowByCodeWindow, 0, 0, "Tw_FindRowByCodeWindow", "0""x"magic_id_STR(row)"x"magic_id_STR(window)"_"TWS_dat_STR },

{ Tw_CreateGroup, 0, 0, "Tw_CreateGroup", "0""x"magic_id_STR(group) },
{ Tw_InsertGadgetGroup, 0, 0, "Tw_InsertGadgetGroup", "2""v"TWS_void_STR"x"magic_id_STR(group)"x"magic_id_STR(gadget) },
{ Tw_RemoveGadgetGroup, 0, 0, "Tw_RemoveGadgetGroup", "2""v"TWS_void_STR"x"magic_id_STR(group)"x"magic_id_STR(gadget) },

{ Tw_GetSelectedGadgetGroup, 0, 0, "Tw_GetSelectedGadgetGroup", "2""x"magic_id_STR(gadget)"x"magic_id_STR(group) },
{ Tw_SetSelectedGadgetGroup, 0, 0, "Tw_SetSelectedGadgetGroup", "2""v"TWS_void_STR"x"magic_id_STR(group)"x"magic_id_STR(gadget) },

{ Tw_Create4MenuRow, 0, 0, "Tw_Create4MenuRow", "0""v"TWS_void_STR"x"magic_id_STR(window)"_"TWS_udat_STR"_"TWS_byte_STR"_"TWS_uldat_STR"V"TWS_byte_STR },

{ Tw_RaiseRow, 0, 0, "Tw_RaiseRow", "2""v"TWS_void_STR"x"magic_id_STR(row) },
{ Tw_LowerRow, 0, 0, "Tw_LowerRow", "2""v"TWS_void_STR"x"magic_id_STR(row) },
{ Tw_RestackChildrenRow, 0, 0, "Tw_RestackChildrenRow", "0""v"TWS_void_STR"x"magic_id_STR(obj)"_"TWS_uldat_STR"X"magic_id_STR(row) }, 
{ Tw_CirculateChildrenRow, 0, 0, "Tw_CirculateChildrenRow", "0""v"TWS_void_STR"x"magic_id_STR(obj)"_"TWS_byte_STR },

{ Tw_Create4MenuMenuItem, 0, 0, "Tw_Create4MenuMenuItem", "0""x"magic_id_STR(menuitem)"x"magic_id_STR(obj)"x"magic_id_STR(window)"_"TWS_byte_STR"_"TWS_dat_STR"V"TWS_byte_STR },
{ Tw_Create4MenuCommonMenuItem, 0, 0, "Tw_Create4MenuCommonMenuItem", "1""_"TWS_uldat_STR"x"magic_id_STR(menu) },

{ Tw_CreateMenu, 0, 0, "Tw_CreateMenu", "0""x"magic_id_STR(menu)"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_byte_STR },
{ Tw_SetInfoMenu, 0, 0, "Tw_SetInfoMenu", "2""v"TWS_void_STR"x"magic_id_STR(menu)"_"TWS_byte_STR"_"TWS_ldat_STR"V"TWS_byte_STR"W"TWS_hwcol_STR },

{ Tw_CreateMsgPort, 0, 0, "Tw_CreateMsgPort", "0""x"magic_id_STR(msgport)"_"TWS_byte_STR"V"TWS_byte_STR"_"TWS_time_t_STR"_"TWS_frac_t_STR"_"TWS_byte_STR },
{ Tw_FindMsgPort, 0, 0, "Tw_FindMsgPort", "0""x"magic_id_STR(msgport)"x"magic_id_STR(msgport)"_"TWS_byte_STR"V"TWS_byte_STR },

{ Tw_BgImageScreen, 0, 0, "Tw_BgImageScreen", "2""v"TWS_void_STR"x"magic_id_STR(screen)"_"TWS_dat_STR"_"TWS_dat_STR"V"TWS_hwattr_STR },

{ Tw_PrevObj, 0, 0, "Tw_PrevObj", "0""x"magic_id_STR(obj)"x"magic_id_STR(obj) },
{ Tw_NextObj, 0, 0, "Tw_NextObj", "0""x"magic_id_STR(obj)"x"magic_id_STR(obj) },
{ Tw_ParentObj, 0, 0, "Tw_ParentObj", "0""x"magic_id_STR(obj)"x"magic_id_STR(obj) },

{ Tw_G_PrevGadget, 0, 0, "Tw_G_PrevGadget", "0""x"magic_id_STR(gadget)"x"magic_id_STR(gadget) },
{ Tw_G_NextGadget, 0, 0, "Tw_G_NextGadget", "0""x"magic_id_STR(gadget)"x"magic_id_STR(gadget) },
{ Tw_GroupGadget, 0, 0, "Tw_GroupGadget", "0""x"magic_id_STR(group)"x"magic_id_STR(gadget) },

{ Tw_O_PrevWidget, 0, 0, "Tw_O_PrevWidget", "0""x"magic_id_STR(widget)"x"magic_id_STR(widget) },
{ Tw_O_NextWidget, 0, 0, "Tw_O_NextWidget", "0""x"magic_id_STR(widget)"x"magic_id_STR(widget) },
{ Tw_OwnerWidget, 0, 0, "Tw_OwnerWidget", "0""x"magic_id_STR(msgport)"x"magic_id_STR(widget) },

{ Tw_FirstScreen, 0, 0, "Tw_FirstScreen", "0""x"magic_id_STR(screen) },
{ Tw_FirstWidget, 0, 0, "Tw_FirstWidget", "0""x"magic_id_STR(widget)"x"magic_id_STR(widget) },
{ Tw_FirstMsgPort, 0, 0, "Tw_FirstMsgPort", "0""x"magic_id_STR(msgport) },
{ Tw_FirstMenu, 0, 0, "Tw_FirstMenu", "0""x"magic_id_STR(menu)"x"magic_id_STR(msgport) },
{ Tw_FirstW, 0, 0, "Tw_FirstW", "0""x"magic_id_STR(widget)"x"magic_id_STR(msgport) },
{ Tw_FirstGroup, 0, 0, "Tw_FirstGroup", "0""x"magic_id_STR(group)"x"magic_id_STR(msgport) },
{ Tw_FirstMutex, 0, 0, "Tw_FirstMutex", "0""x"magic_id_STR(mutex)"x"magic_id_STR(msgport) },
{ Tw_FirstMenuItem, 0, 0, "Tw_FirstMenuItem", "0""x"magic_id_STR(menuitem)"x"magic_id_STR(menu) },
{ Tw_FirstGadget, 0, 0, "Tw_FirstGadget", "0""x"magic_id_STR(gadget)"x"magic_id_STR(group) },

{ Tw_GetDisplayWidth, 0, 0, "Tw_GetDisplayWidth", "0""_"TWS_dat_STR },
{ Tw_GetDisplayHeight, 0, 0, "Tw_GetDisplayHeight", "0""_"TWS_dat_STR },
{ Tw_GetAll, 0, 0, "Tw_GetAll", "0""x"magic_id_STR(all) },

{ Tw_SendToMsgPort, 0, 0, "Tw_SendToMsgPort", "0""_"TWS_byte_STR"x"magic_id_STR(msgport)"_"TWS_udat_STR"V"TWS_byte_STR },
{ Tw_BlindSendToMsgPort, 0, 0, "Tw_BlindSendToMsgPort", "0""v"TWS_void_STR"x"magic_id_STR(msgport)"_"TWS_udat_STR"V"TWS_byte_STR },

{ Tw_GetOwnerSelection, 0, 0, "Tw_GetOwnerSelection", "0""x"magic_id_STR(obj) },
{ Tw_SetOwnerSelection, 0, 0, "Tw_SetOwnerSelection", "0""v"TWS_void_STR"_"TWS_time_t_STR"_"TWS_frac_t_STR },
{ Tw_RequestSelection, 0, 0, "Tw_RequestSelection", "0""v"TWS_void_STR"x"magic_id_STR(obj)"_"TWS_uldat_STR },
{ Tw_NotifySelection, 0, 0, "Tw_NotifySelection", "0""v"TWS_void_STR"x"magic_id_STR(obj)"_"TWS_uldat_STR"_"TWS_uldat_STR"V"TWS_byte_STR"_"TWS_uldat_STR"V"TWS_byte_STR },

{ Tw_SetServerUid, 0, 0, "Tw_SetServerUid", "0""_"TWS_byte_STR"_"TWS_uldat_STR"_"TWS_byte_STR },



