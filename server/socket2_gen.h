/* clang-format off */

/* This file was automatically generated with scheme from `twin/include/scheme/sockproto_inc.scm', do not edit! */

/**
 *  Copyright (C) 2001-2025 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 */

{ 0, 0, "FindFunction",
  "0" "_" TWS_uldat_STR "_" TWS_byte_STR "V" TWS_char_STR "_" TWS_byte_STR "V" TWS_char_STR },

{ 0, 0, "SyncSocket",
  "0" "_" TWS_byte_STR },

{ 0, 0, "ServerSizeof",
  "0" "_" TWS_byte_STR "_" TWS_byte_STR },

{ 0, 0, "CanCompress",
  "0" "_" TWS_byte_STR },

{ 0, 0, "DoCompress",
  "0" "_" TWS_byte_STR "_" TWS_byte_STR },

{ 0, 0, "NeedResizeDisplay",
  "0" "v" TWS_void_STR },

{ 0, 0, "AttachHW",
  "0" "v" TWS_void_STR "_" TWS_uldat_STR "V" TWS_char_STR "_" TWS_byte_STR },

{ 0, 0, "DetachHW",
  "0" "_" TWS_byte_STR "_" TWS_uldat_STR "V" TWS_char_STR },

{ 0, 0, "SetFontTranslation",
  "0" "v" TWS_void_STR "V" TWS_byte_STR },

{ 0, 0, "SetTRuneTranslation",
  "0" "v" TWS_void_STR "V" TWS_trune_STR },

{ 0, 0, "DeleteObj",
  "0" "v" TWS_void_STR "x" Tobj_magic_STR },

{ 0, 0, "ChangeFieldObj",
  "2" "v" TWS_void_STR "x" Tobj_magic_STR "_" TWS_udat_STR "_" TWS_uldat_STR "_" TWS_uldat_STR },

{ 0, 0, "CreateWidget",
  "0" "x" Twidget_magic_STR "_" TWS_dat_STR "_" TWS_dat_STR "_" TWS_uldat_STR "_" TWS_uldat_STR "_" TWS_dat_STR "_" TWS_dat_STR "_" TWS_tcell_STR },

{ 0, 0, "RecursiveDeleteWidget",
  "0" "v" TWS_void_STR "x" Twidget_magic_STR },

{ 0, 0, "MapWidget",
  "2" "v" TWS_void_STR "x" Twidget_magic_STR "x" Twidget_magic_STR },

{ 0, 0, "UnMapWidget",
  "2" "v" TWS_void_STR "x" Twidget_magic_STR },

{ 0, 0, "SetXYWidget",
  "0" "v" TWS_void_STR "x" Twidget_magic_STR "_" TWS_dat_STR "_" TWS_dat_STR },

{ 0, 0, "ResizeWidget",
  "0" "v" TWS_void_STR "x" Twidget_magic_STR "_" TWS_dat_STR "_" TWS_dat_STR },

{ 0, 0, "ScrollWidget",
  "0" "v" TWS_void_STR "x" Twidget_magic_STR "_" TWS_ldat_STR "_" TWS_ldat_STR },

{ 0, 0, "DrawWidget",
  "0" "v" TWS_void_STR "x" Twidget_magic_STR "_" TWS_dat_STR "_" TWS_dat_STR "_" TWS_dat_STR "_" TWS_dat_STR "W" TWS_char_STR "W" TWS_trune_STR "W" TWS_tcell_STR },

{ 0, 0, "SetVisibleWidget",
  "0" "v" TWS_void_STR "x" Twidget_magic_STR "_" TWS_byte_STR },

{ 0, 0, "FocusSubWidget",
  "0" "v" TWS_void_STR "x" Twidget_magic_STR },

{ 0, 0, "FindWidgetAtWidget",
  "2" "x" Twidget_magic_STR "x" Twidget_magic_STR "_" TWS_dat_STR "_" TWS_dat_STR },

{ 0, 0, "RaiseWidget",
  "2" "v" TWS_void_STR "x" Twidget_magic_STR },

{ 0, 0, "LowerWidget",
  "2" "v" TWS_void_STR "x" Twidget_magic_STR },

{ 0, 0, "RestackChildrenWidget",
  "0" "v" TWS_void_STR "x" Twidget_magic_STR "_" TWS_uldat_STR "X" Twidget_magic_STR },

{ 0, 0, "CirculateChildrenWidget",
  "0" "v" TWS_void_STR "x" Twidget_magic_STR "_" TWS_byte_STR },

{ 0, 0, "CreateGadget",
  "0" "x" Tgadget_magic_STR "x" Twidget_magic_STR "_" TWS_dat_STR "_" TWS_dat_STR "W" TWS_char_STR "_" TWS_uldat_STR "_" TWS_uldat_STR "_" TWS_udat_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_dat_STR "_" TWS_dat_STR },

{ 0, 0, "CreateButtonGadget",
  "1" "x" Tgadget_magic_STR "x" Twidget_magic_STR "_" TWS_dat_STR "_" TWS_dat_STR "V" TWS_char_STR "_" TWS_uldat_STR "_" TWS_udat_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_dat_STR "_" TWS_dat_STR },

{ 0, 0, "WriteTextsGadget",
  "2" "v" TWS_void_STR "x" Tgadget_magic_STR "_" TWS_byte_STR "_" TWS_dat_STR "_" TWS_dat_STR "W" TWS_char_STR "_" TWS_dat_STR "_" TWS_dat_STR },

{ 0, 0, "WriteTRunesGadget",
  "2" "v" TWS_void_STR "x" Tgadget_magic_STR "_" TWS_byte_STR "_" TWS_dat_STR "_" TWS_dat_STR "W" TWS_trune_STR "_" TWS_dat_STR "_" TWS_dat_STR },

{ 0, 0, "CreateWindow",
  "0" "x" Twindow_magic_STR "_" TWS_dat_STR "V" TWS_char_STR "W" TWS_tcolor_STR "x" Tmenu_magic_STR "_" TWS_tcolor_STR "_" TWS_uldat_STR "_" TWS_uldat_STR "_" TWS_uldat_STR "_" TWS_dat_STR "_" TWS_dat_STR "_" TWS_dat_STR },

{ 0, 0, "Create4MenuWindow",
  "1" "x" Twindow_magic_STR "x" Tmenu_magic_STR },

{ 0, 0, "WriteCharsetWindow",
  "0" "v" TWS_void_STR "x" Twindow_magic_STR "_" TWS_uldat_STR "V" TWS_char_STR },

{ 0, 0, "WriteUtf8Window",
  "0" "v" TWS_void_STR "x" Twindow_magic_STR "_" TWS_uldat_STR "V" TWS_char_STR },

{ 0, 0, "WriteTRuneWindow",
  "0" "v" TWS_void_STR "x" Twindow_magic_STR "_" TWS_uldat_STR "V" TWS_trune_STR },

{ 0, 0, "WriteTCellWindow",
  "0" "v" TWS_void_STR "x" Twindow_magic_STR "_" TWS_dat_STR "_" TWS_dat_STR "_" TWS_uldat_STR "V" TWS_tcell_STR },

{ 0, 0, "GotoXYWindow",
  "2" "v" TWS_void_STR "x" Twindow_magic_STR "_" TWS_ldat_STR "_" TWS_ldat_STR },

{ 0, 0, "SetTitleWindow",
  "0" "v" TWS_void_STR "x" Twindow_magic_STR "_" TWS_dat_STR "V" TWS_char_STR },

{ 0, 0, "SetColTextWindow",
  "2" "v" TWS_void_STR "x" Twindow_magic_STR "_" TWS_tcolor_STR },

{ 0, 0, "SetColorsWindow",
  "2" "v" TWS_void_STR "x" Twindow_magic_STR "_" TWS_udat_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR },

{ 0, 0, "ConfigureWindow",
  "2" "v" TWS_void_STR "x" Twindow_magic_STR "_" TWS_byte_STR "_" TWS_dat_STR "_" TWS_dat_STR "_" TWS_dat_STR "_" TWS_dat_STR "_" TWS_dat_STR "_" TWS_dat_STR },

{ 0, 0, "FindRowByCodeWindow",
  "0" "x" Trow_magic_STR "x" Twindow_magic_STR "_" TWS_dat_STR },

{ 0, 0, "CreateGroup",
  "0" "x" Tgroup_magic_STR },

{ 0, 0, "InsertGadgetGroup",
  "2" "v" TWS_void_STR "x" Tgroup_magic_STR "x" Tgadget_magic_STR },

{ 0, 0, "RemoveGadgetGroup",
  "2" "v" TWS_void_STR "x" Tgroup_magic_STR "x" Tgadget_magic_STR },

{ 0, 0, "GetSelectedGadgetGroup",
  "2" "x" Tgadget_magic_STR "x" Tgroup_magic_STR },

{ 0, 0, "SetSelectedGadgetGroup",
  "2" "v" TWS_void_STR "x" Tgroup_magic_STR "x" Tgadget_magic_STR },

{ 0, 0, "RaiseRow",
  "2" "v" TWS_void_STR "x" Trow_magic_STR },

{ 0, 0, "LowerRow",
  "2" "v" TWS_void_STR "x" Trow_magic_STR },

{ 0, 0, "RestackChildrenRow",
  "0" "v" TWS_void_STR "x" Tobj_magic_STR "_" TWS_uldat_STR "X" Trow_magic_STR },

{ 0, 0, "CirculateChildrenRow",
  "0" "v" TWS_void_STR "x" Tobj_magic_STR "_" TWS_byte_STR },

{ 0, 0, "Create4MenuAny",
  "0" "x" Trow_magic_STR "x" Tobj_magic_STR "x" Twindow_magic_STR "_" TWS_udat_STR "_" TWS_byte_STR "_" TWS_ldat_STR "V" TWS_char_STR },

{ 0, 0, "Create4MenuCommonMenuItem",
  "1" "_" TWS_uldat_STR "x" Tmenu_magic_STR },

{ 0, 0, "CreateMenu",
  "0" "x" Tmenu_magic_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_tcolor_STR "_" TWS_byte_STR },

{ 0, 0, "SetInfoMenu",
  "2" "v" TWS_void_STR "x" Tmenu_magic_STR "_" TWS_byte_STR "_" TWS_ldat_STR "V" TWS_char_STR "W" TWS_tcolor_STR },

{ 0, 0, "CreateMsgPort",
  "0" "x" Tmsgport_magic_STR "_" TWS_byte_STR "V" TWS_char_STR },

{ 0, 0, "FindMsgPort",
  "0" "x" Tmsgport_magic_STR "x" Tmsgport_magic_STR "_" TWS_byte_STR "V" TWS_char_STR },

{ 0, 0, "BgImageScreen",
  "2" "v" TWS_void_STR "x" Tscreen_magic_STR "_" TWS_dat_STR "_" TWS_dat_STR "V" TWS_tcell_STR },

{ 0, 0, "PrevObj",
  "0" "x" Tobj_magic_STR "x" Tobj_magic_STR },

{ 0, 0, "NextObj",
  "0" "x" Tobj_magic_STR "x" Tobj_magic_STR },

{ 0, 0, "ParentObj",
  "0" "x" Tobj_magic_STR "x" Tobj_magic_STR },

{ 0, 0, "FirstScreen",
  "0" "x" Tscreen_magic_STR },

{ 0, 0, "FirstWidget",
  "0" "x" Twidget_magic_STR "x" Twidget_magic_STR },

{ 0, 0, "FirstMsgPort",
  "0" "x" Tmsgport_magic_STR },

{ 0, 0, "FirstMenu",
  "0" "x" Tmenu_magic_STR "x" Tmsgport_magic_STR },

{ 0, 0, "FirstW",
  "0" "x" Twidget_magic_STR "x" Tmsgport_magic_STR },

{ 0, 0, "FirstGroup",
  "0" "x" Tgroup_magic_STR "x" Tmsgport_magic_STR },

{ 0, 0, "FirstMutex",
  "0" "x" Tmutex_magic_STR "x" Tmsgport_magic_STR },

{ 0, 0, "FirstMenuItem",
  "0" "x" Tmenuitem_magic_STR "x" Tmenu_magic_STR },

{ 0, 0, "FirstGadget",
  "0" "x" Tgadget_magic_STR "x" Tgroup_magic_STR },

{ 0, 0, "GetDisplayWidth",
  "0" "_" TWS_dat_STR },

{ 0, 0, "GetDisplayHeight",
  "0" "_" TWS_dat_STR },

{ 0, 0, "GetAll",
  "0" "x" Tall_magic_STR },

{ 0, 0, "SendToMsgPort",
  "0" "_" TWS_byte_STR "x" Tmsgport_magic_STR "_" TWS_udat_STR "V" TWS_byte_STR },

{ 0, 0, "BlindSendToMsgPort",
  "0" "v" TWS_void_STR "x" Tmsgport_magic_STR "_" TWS_udat_STR "V" TWS_byte_STR },

{ 0, 0, "GetOwnerSelection",
  "0" "x" Tobj_magic_STR },

{ 0, 0, "SetOwnerSelection",
  "0" "v" TWS_void_STR "_" TWS_tany_STR "_" TWS_tany_STR },

{ 0, 0, "RequestSelection",
  "0" "v" TWS_void_STR "x" Tobj_magic_STR "_" TWS_uldat_STR },

{ 0, 0, "NotifySelection",
  "0" "v" TWS_void_STR "x" Tobj_magic_STR "_" TWS_uldat_STR "_" TWS_uldat_STR "V" TWS_char_STR "_" TWS_uldat_STR "V" TWS_char_STR },

{ 0, 0, "SetServerUid",
  "0" "_" TWS_byte_STR "_" TWS_uldat_STR "_" TWS_byte_STR },
