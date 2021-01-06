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




/* This file was automatically generated with m4 from m4/libtw2.m4, do not edit! */



#define n(a) (tany)a
#define N(a) (tany)(topaque)(TW_CONST void *)a












static uldat _Tw_FindFunction(tw_d TwD, byte a1, TW_CONST char *a2, byte a3, TW_CONST char *a4) {
    return (uldat)_Tw_EncodeCall(ENCODE_FL_NOLOCK|0, order_FindFunction, TwD 
    , n(a1)
    , n((a1) * sizeof(char)), N(a2)
    , n(a3)
    , n((a3) * sizeof(char)), N(a4));
}



static byte _Tw_SyncSocket(tw_d TwD) {
    return (byte)_Tw_EncodeCall(ENCODE_FL_NOLOCK|0, order_SyncSocket, TwD );
}



byte Tw_ServerSizeof(tw_d TwD, byte a1) {
    return (byte)_Tw_EncodeCall(0, order_ServerSizeof, TwD 
    , n(a1));
}



byte Tw_CanCompress(tw_d TwD) {
    return (byte)_Tw_EncodeCall(0, order_CanCompress, TwD );
}


byte Tw_DoCompress(tw_d TwD, byte a1) {
    return (byte)_Tw_EncodeCall(0, order_DoCompress, TwD 
    , n(a1));
}



void Tw_NeedResizeDisplay(tw_d TwD) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_NeedResizeDisplay, TwD );
}



void Tw_AttachHW(tw_d TwD, uldat a1, TW_CONST char *a2, byte a3) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_AttachHW, TwD 
    , n(a1)
    , n((a1) * sizeof(char)), N(a2)
    , n(a3));
}


byte Tw_DetachHW(tw_d TwD, uldat a1, TW_CONST char *a2) {
    return (byte)_Tw_EncodeCall(0, order_DetachHW, TwD 
    , n(a1)
    , n((a1) * sizeof(char)), N(a2));
}



void Tw_SetFontTranslation(tw_d TwD, TW_CONST byte *a1) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_SetFontTranslation, TwD 
    , n((0x80) * sizeof(byte)), N(a1));
}


void Tw_SetTRuneTranslation(tw_d TwD, TW_CONST trune *a1) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_SetTRuneTranslation, TwD 
    , n((0x80) * sizeof(trune)), N(a1));
}



void Tw_DeleteObj(tw_d TwD, tobj a1) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_DeleteObj, TwD 
    , n(a1));
}


void Tw_ChangeFieldObj(tw_d TwD, tobj a1, udat a2, uldat a3, uldat a4) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_ChangeFieldObj, TwD 
    , n(a1)
    , n(a2)
    , n(a3)
    , n(a4));
}



twidget Tw_CreateWidget(tw_d TwD, dat a1, dat a2, uldat a3, uldat a4, dat a5, dat a6, tcell a7) {
    return (tobj)_Tw_EncodeCall(0, order_CreateWidget, TwD 
    , n(a1)
    , n(a2)
    , n(a3)
    , n(a4)
    , n(a5)
    , n(a6)
    , n(a7));
}


void Tw_RecursiveDeleteWidget(tw_d TwD, twidget a1) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_RecursiveDeleteWidget, TwD 
    , n(a1));
}

void Tw_MapWidget(tw_d TwD, twidget a1, twidget a2) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_MapWidget, TwD 
    , n(a1)
    , n(a2));
}

void Tw_UnMapWidget(tw_d TwD, twidget a1) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_UnMapWidget, TwD 
    , n(a1));
}

void Tw_SetXYWidget(tw_d TwD, twidget a1, dat a2, dat a3) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_SetXYWidget, TwD 
    , n(a1)
    , n(a2)
    , n(a3));
}

void Tw_ResizeWidget(tw_d TwD, twidget a1, dat a2, dat a3) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_ResizeWidget, TwD 
    , n(a1)
    , n(a2)
    , n(a3));
}

void Tw_ScrollWidget(tw_d TwD, twidget a1, ldat a2, ldat a3) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_ScrollWidget, TwD 
    , n(a1)
    , n(a2)
    , n(a3));
}

void Tw_DrawWidget(tw_d TwD, twidget a1, dat a2, dat a3, dat a4, dat a5, TW_CONST char *a6, TW_CONST trune *a7, TW_CONST tcell *a8) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_DrawWidget, TwD 
    , n(a1)
    , n(a2)
    , n(a3)
    , n(a4)
    , n(a5)
    , n((a2*a3) * sizeof(char)), N(a6)
    , n((a2*a3) * sizeof(trune)), N(a7)
    , n((a2*a3) * sizeof(tcell)), N(a8));
}


void Tw_SetVisibleWidget(tw_d TwD, twidget a1, byte a2) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_SetVisibleWidget, TwD 
    , n(a1)
    , n(a2));
}


void Tw_FocusSubWidget(tw_d TwD, twidget a1) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_FocusSubWidget, TwD 
    , n(a1));
}

twidget Tw_FindWidgetAtWidget(tw_d TwD, twidget a1, dat a2, dat a3) {
    return (tobj)_Tw_EncodeCall(0, order_FindWidgetAtWidget, TwD 
    , n(a1)
    , n(a2)
    , n(a3));
}


void Tw_RaiseWidget(tw_d TwD, twidget a1) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_RaiseWidget, TwD 
    , n(a1));
}

void Tw_LowerWidget(tw_d TwD, twidget a1) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_LowerWidget, TwD 
    , n(a1));
}

void Tw_RestackChildrenWidget(tw_d TwD, twidget a1, uldat a2, TW_CONST twidget *a3) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_RestackChildrenWidget, TwD 
    , n(a1)
    , n(a2)
    , n((a2) * sizeof(tobj)), N(a3));
}

void Tw_CirculateChildrenWidget(tw_d TwD, twidget a1, byte a2) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_CirculateChildrenWidget, TwD 
    , n(a1)
    , n(a2));
}



tgadget Tw_CreateGadget(tw_d TwD, twidget a1, dat a2, dat a3, TW_CONST char *a4, uldat a5, uldat a6, udat a7, tcolor a8, tcolor a9, tcolor a10, tcolor a11, dat a12, dat a13) {
    return (tobj)_Tw_EncodeCall(0, order_CreateGadget, TwD 
    , n(a1)
    , n(a2)
    , n(a3)
    , n((a2*a3) * sizeof(char)), N(a4)
    , n(a5)
    , n(a6)
    , n(a7)
    , n(a8)
    , n(a9)
    , n(a10)
    , n(a11)
    , n(a12)
    , n(a13));
}


tgadget Tw_CreateButtonGadget(tw_d TwD, twidget a1, dat a2, dat a3, TW_CONST char *a4, uldat a5, udat a6, tcolor a7, tcolor a8, tcolor a9, dat a10, dat a11) {
    return (tobj)_Tw_EncodeCall(0, order_CreateButtonGadget, TwD 
    , n(a1)
    , n(a2)
    , n(a3)
    , n((a2*a3) * sizeof(char)), N(a4)
    , n(a5)
    , n(a6)
    , n(a7)
    , n(a8)
    , n(a9)
    , n(a10)
    , n(a11));
}


void Tw_WriteTextsGadget(tw_d TwD, tgadget a1, byte a2, dat a3, dat a4, TW_CONST char *a5, dat a6, dat a7) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_WriteTextsGadget, TwD 
    , n(a1)
    , n(a2)
    , n(a3)
    , n(a4)
    , n((a2*a3) * sizeof(char)), N(a5)
    , n(a6)
    , n(a7));
}

void Tw_WriteTRunesGadget(tw_d TwD, tgadget a1, byte a2, dat a3, dat a4, TW_CONST trune *a5, dat a6, dat a7) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_WriteTRunesGadget, TwD 
    , n(a1)
    , n(a2)
    , n(a3)
    , n(a4)
    , n((a2*a3) * sizeof(trune)), N(a5)
    , n(a6)
    , n(a7));
}


twindow Tw_CreateWindow(tw_d TwD, dat a1, TW_CONST char *a2, TW_CONST tcolor *a3, tmenu a4, tcolor a5, uldat a6, uldat a7, uldat a8, dat a9, dat a10, dat a11) {
    return (tobj)_Tw_EncodeCall(0, order_CreateWindow, TwD 
    , n(a1)
    , n((a1) * sizeof(char)), N(a2)
    , n((a1) * sizeof(tcolor)), N(a3)
    , n(a4)
    , n(a5)
    , n(a6)
    , n(a7)
    , n(a8)
    , n(a9)
    , n(a10)
    , n(a11));
}

twindow Tw_Create4MenuWindow(tw_d TwD, tmenu a1) {
    return (tobj)_Tw_EncodeCall(0, order_Create4MenuWindow, TwD 
    , n(a1));
}


void Tw_WriteCharsetWindow(tw_d TwD, twindow a1, uldat a2, TW_CONST char *a3) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_WriteCharsetWindow, TwD 
    , n(a1)
    , n(a2)
    , n((a2) * sizeof(char)), N(a3));
}

void Tw_WriteUtf8Window(tw_d TwD, twindow a1, uldat a2, TW_CONST char *a3) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_WriteUtf8Window, TwD 
    , n(a1)
    , n(a2)
    , n((a2) * sizeof(char)), N(a3));
}

void Tw_WriteTRuneWindow(tw_d TwD, twindow a1, uldat a2, TW_CONST trune *a3) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_WriteTRuneWindow, TwD 
    , n(a1)
    , n(a2)
    , n((a2) * sizeof(trune)), N(a3));
}

void Tw_WriteTCellWindow(tw_d TwD, twindow a1, dat a2, dat a3, uldat a4, TW_CONST tcell *a5) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_WriteTCellWindow, TwD 
    , n(a1)
    , n(a2)
    , n(a3)
    , n(a4)
    , n((a4) * sizeof(tcell)), N(a5));
}


void Tw_GotoXYWindow(tw_d TwD, twindow a1, ldat a2, ldat a3) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_GotoXYWindow, TwD 
    , n(a1)
    , n(a2)
    , n(a3));
}

void Tw_SetTitleWindow(tw_d TwD, twindow a1, dat a2, TW_CONST char *a3) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_SetTitleWindow, TwD 
    , n(a1)
    , n(a2)
    , n((a2) * sizeof(char)), N(a3));
}

void Tw_SetColTextWindow(tw_d TwD, twindow a1, tcolor a2) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_SetColTextWindow, TwD 
    , n(a1)
    , n(a2));
}

void Tw_SetColorsWindow(tw_d TwD, twindow a1, udat a2, tcolor a3, tcolor a4, tcolor a5, tcolor a6, tcolor a7, tcolor a8, tcolor a9, tcolor a10, tcolor a11) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_SetColorsWindow, TwD 
    , n(a1)
    , n(a2)
    , n(a3)
    , n(a4)
    , n(a5)
    , n(a6)
    , n(a7)
    , n(a8)
    , n(a9)
    , n(a10)
    , n(a11));
}

void Tw_ConfigureWindow(tw_d TwD, twindow a1, byte a2, dat a3, dat a4, dat a5, dat a6, dat a7, dat a8) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_ConfigureWindow, TwD 
    , n(a1)
    , n(a2)
    , n(a3)
    , n(a4)
    , n(a5)
    , n(a6)
    , n(a7)
    , n(a8));
}

trow Tw_FindRowByCodeWindow(tw_d TwD, twindow a1, dat a2) {
    return (tobj)_Tw_EncodeCall(0, order_FindRowByCodeWindow, TwD 
    , n(a1)
    , n(a2));
}


tgroup Tw_CreateGroup(tw_d TwD) {
    return (tobj)_Tw_EncodeCall(0, order_CreateGroup, TwD );
}

void Tw_InsertGadgetGroup(tw_d TwD, tgroup a1, tgadget a2) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_InsertGadgetGroup, TwD 
    , n(a1)
    , n(a2));
}

void Tw_RemoveGadgetGroup(tw_d TwD, tgroup a1, tgadget a2) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_RemoveGadgetGroup, TwD 
    , n(a1)
    , n(a2));
}


tgadget Tw_GetSelectedGadgetGroup(tw_d TwD, tgroup a1) {
    return (tobj)_Tw_EncodeCall(0, order_GetSelectedGadgetGroup, TwD 
    , n(a1));
}

void Tw_SetSelectedGadgetGroup(tw_d TwD, tgroup a1, tgadget a2) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_SetSelectedGadgetGroup, TwD 
    , n(a1)
    , n(a2));
}


void Tw_RaiseRow(tw_d TwD, trow a1) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_RaiseRow, TwD 
    , n(a1));
}

void Tw_LowerRow(tw_d TwD, trow a1) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_LowerRow, TwD 
    , n(a1));
}

void Tw_RestackChildrenRow(tw_d TwD, tobj a1, uldat a2, TW_CONST trow *a3) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_RestackChildrenRow, TwD 
    , n(a1)
    , n(a2)
    , n((a2) * sizeof(tobj)), N(a3));
}

void Tw_CirculateChildrenRow(tw_d TwD, tobj a1, byte a2) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_CirculateChildrenRow, TwD 
    , n(a1)
    , n(a2));
}


trow Tw_Create4MenuAny(tw_d TwD, tobj a1, twindow a2, udat a3, byte a4, ldat a5, TW_CONST char *a6) {
    return (tobj)_Tw_EncodeCall(0, order_Create4MenuAny, TwD 
    , n(a1)
    , n(a2)
    , n(a3)
    , n(a4)
    , n(a5)
    , n((a5) * sizeof(char)), N(a6));
}


uldat Tw_Create4MenuCommonMenuItem(tw_d TwD, tmenu a1) {
    return (uldat)_Tw_EncodeCall(0, order_Create4MenuCommonMenuItem, TwD 
    , n(a1));
}


tmenu Tw_CreateMenu(tw_d TwD, tcolor a1, tcolor a2, tcolor a3, tcolor a4, tcolor a5, tcolor a6, byte a7) {
    return (tobj)_Tw_EncodeCall(0, order_CreateMenu, TwD 
    , n(a1)
    , n(a2)
    , n(a3)
    , n(a4)
    , n(a5)
    , n(a6)
    , n(a7));
}

void Tw_SetInfoMenu(tw_d TwD, tmenu a1, byte a2, ldat a3, TW_CONST char *a4, TW_CONST tcolor *a5) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_SetInfoMenu, TwD 
    , n(a1)
    , n(a2)
    , n(a3)
    , n((a3) * sizeof(char)), N(a4)
    , n((a3) * sizeof(tcolor)), N(a5));
}


tmsgport Tw_CreateMsgPort(tw_d TwD, byte a1, TW_CONST char *a2) {
    return (tobj)_Tw_EncodeCall(0, order_CreateMsgPort, TwD 
    , n(a1)
    , n((a1) * sizeof(char)), N(a2));
}

tmsgport Tw_FindMsgPort(tw_d TwD, tmsgport a1, byte a2, TW_CONST char *a3) {
    return (tobj)_Tw_EncodeCall(0, order_FindMsgPort, TwD 
    , n(a1)
    , n(a2)
    , n((a2) * sizeof(char)), N(a3));
}


void Tw_BgImageScreen(tw_d TwD, tscreen a1, dat a2, dat a3, TW_CONST tcell *a4) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_BgImageScreen, TwD 
    , n(a1)
    , n(a2)
    , n(a3)
    , n((a2*a3) * sizeof(tcell)), N(a4));
}


tobj Tw_PrevObj(tw_d TwD, tobj a1) {
    return (tobj)_Tw_EncodeCall(0, order_PrevObj, TwD 
    , n(a1));
}

tobj Tw_NextObj(tw_d TwD, tobj a1) {
    return (tobj)_Tw_EncodeCall(0, order_NextObj, TwD 
    , n(a1));
}

tobj Tw_ParentObj(tw_d TwD, tobj a1) {
    return (tobj)_Tw_EncodeCall(0, order_ParentObj, TwD 
    , n(a1));
}


tscreen   Tw_FirstScreen(tw_d TwD) {
    return (tobj)_Tw_EncodeCall(0, order_FirstScreen, TwD );
}

twidget   Tw_FirstWidget(tw_d TwD, twidget  a1) {
    return (tobj)_Tw_EncodeCall(0, order_FirstWidget, TwD 
    , n(a1));
}

tmsgport  Tw_FirstMsgPort(tw_d TwD) {
    return (tobj)_Tw_EncodeCall(0, order_FirstMsgPort, TwD );
}

tmenu     Tw_FirstMenu(tw_d TwD, tmsgport a1) {
    return (tobj)_Tw_EncodeCall(0, order_FirstMenu, TwD 
    , n(a1));
}

twidget   Tw_FirstW(tw_d TwD, tmsgport a1) {
    return (tobj)_Tw_EncodeCall(0, order_FirstW, TwD 
    , n(a1));
}

tgroup    Tw_FirstGroup(tw_d TwD, tmsgport a1) {
    return (tobj)_Tw_EncodeCall(0, order_FirstGroup, TwD 
    , n(a1));
}

tmutex    Tw_FirstMutex(tw_d TwD, tmsgport a1) {
    return (tobj)_Tw_EncodeCall(0, order_FirstMutex, TwD 
    , n(a1));
}

tmenuitem Tw_FirstMenuItem(tw_d TwD, tmenu    a1) {
    return (tobj)_Tw_EncodeCall(0, order_FirstMenuItem, TwD 
    , n(a1));
}

tgadget   Tw_FirstGadget(tw_d TwD, tgroup   a1) {
    return (tobj)_Tw_EncodeCall(0, order_FirstGadget, TwD 
    , n(a1));
}


dat Tw_GetDisplayWidth(tw_d TwD) {
    return (dat)_Tw_EncodeCall(0, order_GetDisplayWidth, TwD );
}

dat Tw_GetDisplayHeight(tw_d TwD) {
    return (dat)_Tw_EncodeCall(0, order_GetDisplayHeight, TwD );
}

tall Tw_GetAll(tw_d TwD) {
    return (tobj)_Tw_EncodeCall(0, order_GetAll, TwD );
}


byte Tw_SendToMsgPort(tw_d TwD, tmsgport a1, udat a2, TW_CONST byte *a3) {
    return (byte)_Tw_EncodeCall(0, order_SendToMsgPort, TwD 
    , n(a1)
    , n(a2)
    , n((a2) * sizeof(byte)), N(a3));
}

void Tw_BlindSendToMsgPort(tw_d TwD, tmsgport a1, udat a2, TW_CONST byte *a3) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_BlindSendToMsgPort, TwD 
    , n(a1)
    , n(a2)
    , n((a2) * sizeof(byte)), N(a3));
}


tobj Tw_GetOwnerSelection(tw_d TwD) {
    return (tobj)_Tw_EncodeCall(0, order_GetOwnerSelection, TwD );
}

void Tw_SetOwnerSelection(tw_d TwD, tany a1, tany a2) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_SetOwnerSelection, TwD 
    , n(a1)
    , n(a2));
}

void Tw_RequestSelection(tw_d TwD, tobj a1, uldat a2) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_RequestSelection, TwD 
    , n(a1)
    , n(a2));
}

void Tw_NotifySelection(tw_d TwD, tobj a1, uldat a2, uldat a3, TW_CONST char *a4, uldat a5, TW_CONST char *a6) {
    _Tw_EncodeCall(ENCODE_FL_VOID, order_NotifySelection, TwD 
    , n(a1)
    , n(a2)
    , n(a3)
    , n((TW_MAX_MIMELEN) * sizeof(char)), N(a4)
    , n(a5)
    , n((a5) * sizeof(char)), N(a6));
}


byte Tw_SetServerUid(tw_d TwD, uldat a1, byte a2) {
    return (byte)_Tw_EncodeCall(0, order_SetServerUid, TwD 
    , n(a1)
    , n(a2));
}



#undef n
#undef N
