/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */




/* This file was automatically generated from m4/socket2.m4, do not edit! */




































{ 0, 0, "FindFunction",
    "0""_"TWS_uldat_STR"_"TWS_byte_STR"V"TWS_byte_STR"_"TWS_byte_STR"V"TWS_byte_STR },


{ 0, 0, "SyncSocket",
    "0""_"TWS_byte_STR },


{ 0, 0, "ServerSizeof",
    "0""_"TWS_byte_STR"_"TWS_byte_STR },


{ 0, 0, "CanCompress",
    "0""_"TWS_byte_STR },

{ 0, 0, "DoCompress",
    "0""_"TWS_byte_STR"_"TWS_byte_STR },


{ 0, 0, "NeedResizeDisplay",
    "0""v"TWS_void_STR },


{ 0, 0, "AttachHW",
    "0""v"TWS_void_STR"_"TWS_uldat_STR"V"TWS_byte_STR"_"TWS_byte_STR },

{ 0, 0, "DetachHW",
    "0""_"TWS_byte_STR"_"TWS_uldat_STR"V"TWS_byte_STR },


{ 0, 0, "SetFontTranslation",
    "0""v"TWS_void_STR"V"TWS_byte_STR },

{ 0, 0, "SetHWFontTranslation",
    "0""v"TWS_void_STR"V"TWS_hwfont_STR },


{ 0, 0, "DeleteObj",
    "0""v"TWS_void_STR"x"obj_magic_STR },

{ 0, 0, "ChangeFieldObj",
    "2""v"TWS_void_STR"x"obj_magic_STR"_"TWS_udat_STR"_"TWS_uldat_STR"_"TWS_uldat_STR },


{ 0, 0, "CreateWidget",
    "0""x"widget_magic_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_uldat_STR"_"TWS_uldat_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_hwattr_STR },

{ 0, 0, "RecursiveDeleteWidget",
    "0""v"TWS_void_STR"x"widget_magic_STR },
{ 0, 0, "MapWidget",
    "2""v"TWS_void_STR"x"widget_magic_STR"x"widget_magic_STR },
{ 0, 0, "UnMapWidget",
    "2""v"TWS_void_STR"x"widget_magic_STR },
{ 0, 0, "SetXYWidget",
    "0""v"TWS_void_STR"x"widget_magic_STR"_"TWS_dat_STR"_"TWS_dat_STR },
{ 0, 0, "ResizeWidget",
    "0""v"TWS_void_STR"x"widget_magic_STR"_"TWS_dat_STR"_"TWS_dat_STR },
{ 0, 0, "ScrollWidget",
    "0""v"TWS_void_STR"x"widget_magic_STR"_"TWS_ldat_STR"_"TWS_ldat_STR },
{ 0, 0, "DrawWidget",
    "0""v"TWS_void_STR"x"widget_magic_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_dat_STR"W"TWS_byte_STR"W"TWS_hwfont_STR"W"TWS_hwattr_STR },

{ 0, 0, "SetVisibleWidget",
    "0""v"TWS_void_STR"x"widget_magic_STR"_"TWS_byte_STR },

{ 0, 0, "FocusSubWidget",
    "0""v"TWS_void_STR"x"widget_magic_STR },
{ 0, 0, "FindWidgetAtWidget",
    "2""x"widget_magic_STR"x"widget_magic_STR"_"TWS_dat_STR"_"TWS_dat_STR },

{ 0, 0, "RaiseWidget",
    "2""v"TWS_void_STR"x"widget_magic_STR },
{ 0, 0, "LowerWidget",
    "2""v"TWS_void_STR"x"widget_magic_STR },
{ 0, 0, "RestackChildrenWidget",
    "0""v"TWS_void_STR"x"widget_magic_STR"_"TWS_uldat_STR"X"widget_magic_STR },
{ 0, 0, "CirculateChildrenWidget",
    "0""v"TWS_void_STR"x"widget_magic_STR"_"TWS_byte_STR },


{ 0, 0, "CreateGadget",
    "0""x"gadget_magic_STR"x"widget_magic_STR"_"TWS_dat_STR"_"TWS_dat_STR"W"TWS_byte_STR"_"TWS_uldat_STR"_"TWS_uldat_STR"_"TWS_udat_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_dat_STR"_"TWS_dat_STR },

{ 0, 0, "CreateButtonGadget",
    "1""x"gadget_magic_STR"x"widget_magic_STR"_"TWS_dat_STR"_"TWS_dat_STR"V"TWS_byte_STR"_"TWS_uldat_STR"_"TWS_udat_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_dat_STR"_"TWS_dat_STR },

{ 0, 0, "WriteTextsGadget",
    "2""v"TWS_void_STR"x"gadget_magic_STR"_"TWS_byte_STR"_"TWS_dat_STR"_"TWS_dat_STR"W"TWS_byte_STR"_"TWS_dat_STR"_"TWS_dat_STR },
{ 0, 0, "WriteHWFontsGadget",
    "2""v"TWS_void_STR"x"gadget_magic_STR"_"TWS_byte_STR"_"TWS_dat_STR"_"TWS_dat_STR"W"TWS_hwfont_STR"_"TWS_dat_STR"_"TWS_dat_STR },

{ 0, 0, "CreateWindow",
    "0""x"window_magic_STR"_"TWS_dat_STR"V"TWS_byte_STR"W"TWS_hwcol_STR"x"menu_magic_STR"_"TWS_hwcol_STR"_"TWS_uldat_STR"_"TWS_uldat_STR"_"TWS_uldat_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_dat_STR },
{ 0, 0, "Create4MenuWindow",
    "1""x"window_magic_STR"x"menu_magic_STR },

{ 0, 0, "WriteAsciiWindow",
    "0""v"TWS_void_STR"x"window_magic_STR"_"TWS_ldat_STR"V"TWS_byte_STR },
{ 0, 0, "WriteStringWindow",
    "0""v"TWS_void_STR"x"window_magic_STR"_"TWS_ldat_STR"V"TWS_byte_STR },
{ 0, 0, "WriteHWFontWindow",
    "0""v"TWS_void_STR"x"window_magic_STR"_"TWS_ldat_STR"V"TWS_hwfont_STR },
{ 0, 0, "WriteHWAttrWindow",
    "0""v"TWS_void_STR"x"window_magic_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_ldat_STR"V"TWS_hwattr_STR },

{ 0, 0, "GotoXYWindow",
    "2""v"TWS_void_STR"x"window_magic_STR"_"TWS_ldat_STR"_"TWS_ldat_STR },
{ 0, 0, "SetTitleWindow",
    "0""v"TWS_void_STR"x"window_magic_STR"_"TWS_dat_STR"V"TWS_byte_STR },
{ 0, 0, "SetColTextWindow",
    "2""v"TWS_void_STR"x"window_magic_STR"_"TWS_hwcol_STR },
{ 0, 0, "SetColorsWindow",
    "2""v"TWS_void_STR"x"window_magic_STR"_"TWS_udat_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR },
{ 0, 0, "ConfigureWindow",
    "2""v"TWS_void_STR"x"window_magic_STR"_"TWS_byte_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_dat_STR"_"TWS_dat_STR },
{ 0, 0, "FindRowByCodeWindow",
    "0""x"row_magic_STR"x"window_magic_STR"_"TWS_dat_STR },

{ 0, 0, "CreateGroup",
    "0""x"group_magic_STR },
{ 0, 0, "InsertGadgetGroup",
    "2""v"TWS_void_STR"x"group_magic_STR"x"gadget_magic_STR },
{ 0, 0, "RemoveGadgetGroup",
    "2""v"TWS_void_STR"x"group_magic_STR"x"gadget_magic_STR },

{ 0, 0, "GetSelectedGadgetGroup",
    "2""x"gadget_magic_STR"x"group_magic_STR },
{ 0, 0, "SetSelectedGadgetGroup",
    "2""v"TWS_void_STR"x"group_magic_STR"x"gadget_magic_STR },

{ 0, 0, "RaiseRow",
    "2""v"TWS_void_STR"x"row_magic_STR },
{ 0, 0, "LowerRow",
    "2""v"TWS_void_STR"x"row_magic_STR },
{ 0, 0, "RestackChildrenRow",
    "0""v"TWS_void_STR"x"obj_magic_STR"_"TWS_uldat_STR"X"row_magic_STR }, 
{ 0, 0, "CirculateChildrenRow",
    "0""v"TWS_void_STR"x"obj_magic_STR"_"TWS_byte_STR },

{ 0, 0, "Create4MenuAny",
    "0""x"row_magic_STR"x"obj_magic_STR"x"window_magic_STR"_"TWS_udat_STR"_"TWS_byte_STR"_"TWS_ldat_STR"V"TWS_byte_STR },

{ 0, 0, "Create4MenuCommonMenuItem",
    "1""_"TWS_uldat_STR"x"menu_magic_STR },

{ 0, 0, "CreateMenu",
    "0""x"menu_magic_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_hwcol_STR"_"TWS_byte_STR },
{ 0, 0, "SetInfoMenu",
    "2""v"TWS_void_STR"x"menu_magic_STR"_"TWS_byte_STR"_"TWS_ldat_STR"V"TWS_byte_STR"W"TWS_hwcol_STR },

{ 0, 0, "CreateMsgPort",
    "0""x"msgport_magic_STR"_"TWS_byte_STR"V"TWS_byte_STR },
{ 0, 0, "FindMsgPort",
    "0""x"msgport_magic_STR"x"msgport_magic_STR"_"TWS_byte_STR"V"TWS_byte_STR },

{ 0, 0, "BgImageScreen",
    "2""v"TWS_void_STR"x"screen_magic_STR"_"TWS_dat_STR"_"TWS_dat_STR"V"TWS_hwattr_STR },

{ 0, 0, "PrevObj",
    "0""x"obj_magic_STR"x"obj_magic_STR },
{ 0, 0, "NextObj",
    "0""x"obj_magic_STR"x"obj_magic_STR },
{ 0, 0, "ParentObj",
    "0""x"obj_magic_STR"x"obj_magic_STR },

{ 0, 0, "FirstScreen",
    "0""x"screen_magic_STR },
{ 0, 0, "FirstWidget",
    "0""x"widget_magic_STR"x"widget_magic_STR },
{ 0, 0, "FirstMsgPort",
    "0""x"msgport_magic_STR },
{ 0, 0, "FirstMenu",
    "0""x"menu_magic_STR"x"msgport_magic_STR },
{ 0, 0, "FirstW",
    "0""x"widget_magic_STR"x"msgport_magic_STR },
{ 0, 0, "FirstGroup",
    "0""x"group_magic_STR"x"msgport_magic_STR },
{ 0, 0, "FirstMutex",
    "0""x"mutex_magic_STR"x"msgport_magic_STR },
{ 0, 0, "FirstMenuItem",
    "0""x"menuitem_magic_STR"x"menu_magic_STR },
{ 0, 0, "FirstGadget",
    "0""x"gadget_magic_STR"x"group_magic_STR },

{ 0, 0, "GetDisplayWidth",
    "0""_"TWS_dat_STR },
{ 0, 0, "GetDisplayHeight",
    "0""_"TWS_dat_STR },
{ 0, 0, "GetAll",
    "0""x"all_magic_STR },

{ 0, 0, "SendToMsgPort",
    "0""_"TWS_byte_STR"x"msgport_magic_STR"_"TWS_udat_STR"V"TWS_byte_STR },
{ 0, 0, "BlindSendToMsgPort",
    "0""v"TWS_void_STR"x"msgport_magic_STR"_"TWS_udat_STR"V"TWS_byte_STR },

{ 0, 0, "GetOwnerSelection",
    "0""x"obj_magic_STR },
{ 0, 0, "SetOwnerSelection",
    "0""v"TWS_void_STR"_"TWS_tany_STR"_"TWS_tany_STR },
{ 0, 0, "RequestSelection",
    "0""v"TWS_void_STR"x"obj_magic_STR"_"TWS_uldat_STR },
{ 0, 0, "NotifySelection",
    "0""v"TWS_void_STR"x"obj_magic_STR"_"TWS_uldat_STR"_"TWS_uldat_STR"V"TWS_byte_STR"_"TWS_uldat_STR"V"TWS_byte_STR },

{ 0, 0, "SetServerUid",
    "0""_"TWS_byte_STR"_"TWS_uldat_STR"_"TWS_byte_STR },

{ 0, 0, "OpenExtension",
    "0""x"extension_magic_STR"_"TWS_byte_STR"V"TWS_byte_STR },
{ 0, 0, "CallBExtension",
    "0""O"TWS_tany_STR"x"extension_magic_STR"_"TWS_topaque_STR"V"TWS_byte_STR"V"TWS_byte_STR },
{ 0, 0, "CloseExtension",
    "0""v"TWS_void_STR"x"extension_magic_STR },



