



/* This file was automatically generated with m4 from m4/libTw2.m4, do not edit! */



#define c(a) (tlargest)a










static uldat _Tw_FindFunction(tw_d TwD, byte a1, TW_CONST byte *a2, byte a3, TW_CONST byte *a4) {
    return (uldat)EncodeCall(ENCODE_FL_NOLOCK|0, order_FindFunction, TwD 
    , c(a1)
    , c((a1) * sizeof(byte)), (TW_CONST void *)a2
    , c(a3)
    , c((a3) * sizeof(byte)), (TW_CONST void *)a4);
}


static byte _Tw_SyncSocket(tw_d TwD) {
    return (byte)EncodeCall(ENCODE_FL_NOLOCK|0, order_SyncSocket, TwD );
}


byte Tw_ServerSizeof(tw_d TwD, byte a1) {
    return (byte)EncodeCall(0, order_ServerSizeof, TwD 
    , c(a1));
}


byte Tw_CanCompress(tw_d TwD) {
    return (byte)EncodeCall(0, order_CanCompress, TwD );
}

byte Tw_DoCompress(tw_d TwD, byte a1) {
    return (byte)EncodeCall(0, order_DoCompress, TwD 
    , c(a1));
}


void Tw_NeedResizeDisplay(tw_d TwD) {
    EncodeCall(ENCODE_FL_VOID, order_NeedResizeDisplay, TwD );
}


void Tw_AttachHW(tw_d TwD, uldat a1, TW_CONST byte *a2, byte a3) {
    EncodeCall(ENCODE_FL_VOID, order_AttachHW, TwD 
    , c(a1)
    , c((a1) * sizeof(byte)), (TW_CONST void *)a2
    , c(a3));
}

byte Tw_DetachHW(tw_d TwD, uldat a1, TW_CONST byte *a2) {
    return (byte)EncodeCall(0, order_DetachHW, TwD 
    , c(a1)
    , c((a1) * sizeof(byte)), (TW_CONST void *)a2);
}


void Tw_SetFontTranslation(tw_d TwD, TW_CONST byte *a1) {
    EncodeCall(ENCODE_FL_VOID, order_SetFontTranslation, TwD 
    , c((0x80) * sizeof(byte)), (TW_CONST void *)a1);
}

void Tw_SetUniFontTranslation(tw_d TwD, TW_CONST hwfont *a1) {
    EncodeCall(ENCODE_FL_VOID, order_SetUniFontTranslation, TwD 
    , c((0x80) * sizeof(hwfont)), (TW_CONST void *)a1);
}


void Tw_DeleteObj(tw_d TwD, tobj a1) {
    EncodeCall(ENCODE_FL_VOID, order_DeleteObj, TwD 
    , c(a1));
}

void Tw_ChangeFieldObj(tw_d TwD, tobj a1, udat a2, uldat a3, uldat a4) {
    EncodeCall(ENCODE_FL_VOID, order_ChangeFieldObj, TwD 
    , c(a1)
    , c(a2)
    , c(a3)
    , c(a4));
}



twidget Tw_CreateWidget(tw_d TwD, dat a1, dat a2, uldat a3, uldat a4, dat a5, dat a6, hwattr a7) {
    return (tobj)EncodeCall(0, order_CreateWidget, TwD 
    , c(a1)
    , c(a2)
    , c(a3)
    , c(a4)
    , c(a5)
    , c(a6)
    , c(a7));
}

void Tw_RecursiveDeleteWidget(tw_d TwD, twidget a1) {
    EncodeCall(ENCODE_FL_VOID, order_RecursiveDeleteWidget, TwD 
    , c(a1));
}

void Tw_MapWidget(tw_d TwD, twidget a1, twidget a2) {
    EncodeCall(ENCODE_FL_VOID, order_MapWidget, TwD 
    , c(a1)
    , c(a2));
}

void Tw_UnMapWidget(tw_d TwD, twidget a1) {
    EncodeCall(ENCODE_FL_VOID, order_UnMapWidget, TwD 
    , c(a1));
}

void Tw_SetXYWidget(tw_d TwD, twidget a1, dat a2, dat a3) {
    EncodeCall(ENCODE_FL_VOID, order_SetXYWidget, TwD 
    , c(a1)
    , c(a2)
    , c(a3));
}

void Tw_ResizeWidget(tw_d TwD, twidget a1, dat a2, dat a3) {
    EncodeCall(ENCODE_FL_VOID, order_ResizeWidget, TwD 
    , c(a1)
    , c(a2)
    , c(a3));
}

void Tw_ScrollWidget(tw_d TwD, twidget a1, ldat a2, ldat a3) {
    EncodeCall(ENCODE_FL_VOID, order_ScrollWidget, TwD 
    , c(a1)
    , c(a2)
    , c(a3));
}

tmsgport Tw_GetOwnerWidget(tw_d TwD, twidget a1) {
    return (tobj)EncodeCall(0, order_GetOwnerWidget, TwD 
    , c(a1));
}

void Tw_DrawWidget(tw_d TwD, twidget a1, dat a2, dat a3, dat a4, dat a5, TW_CONST byte *a6, TW_CONST hwfont *a7, TW_CONST hwattr *a8) {
    EncodeCall(ENCODE_FL_VOID, order_DrawWidget, TwD 
    , c(a1)
    , c(a2)
    , c(a3)
    , c(a4)
    , c(a5)
    , c((a2*a3) * sizeof(byte)), (TW_CONST void *)a6
    , c((a2*a3) * sizeof(hwfont)), (TW_CONST void *)a7
    , c((a2*a3) * sizeof(hwattr)), (TW_CONST void *)a8);
}


void Tw_SetVisibleWidget(tw_d TwD, twidget a1, byte a2) {
    EncodeCall(ENCODE_FL_VOID, order_SetVisibleWidget, TwD 
    , c(a1)
    , c(a2));
}


void Tw_FocusSubWidget(tw_d TwD, twidget a1) {
    EncodeCall(ENCODE_FL_VOID, order_FocusSubWidget, TwD 
    , c(a1));
}

twidget Tw_FindWidgetAtWidget(tw_d TwD, twidget a1, dat a2, dat a3) {
    return (tobj)EncodeCall(0, order_FindWidgetAtWidget, TwD 
    , c(a1)
    , c(a2)
    , c(a3));
}


void Tw_RaiseWidget(tw_d TwD, twidget a1) {
    EncodeCall(ENCODE_FL_VOID, order_RaiseWidget, TwD 
    , c(a1));
}

void Tw_LowerWidget(tw_d TwD, twidget a1) {
    EncodeCall(ENCODE_FL_VOID, order_LowerWidget, TwD 
    , c(a1));
}

void Tw_RestackChildrenWidget(tw_d TwD, twidget a1, uldat a2, TW_CONST twidget *a3) {
    EncodeCall(ENCODE_FL_VOID, order_RestackChildrenWidget, TwD 
    , c(a1)
    , c(a2)
    , c((a2) * sizeof(tobj)), (TW_CONST void *)a3);
}

void Tw_CirculateChildrenWidget(tw_d TwD, twidget a1, byte a2) {
    EncodeCall(ENCODE_FL_VOID, order_CirculateChildrenWidget, TwD 
    , c(a1)
    , c(a2));
}



tgadget Tw_CreateGadget(tw_d TwD, twidget a1, dat a2, dat a3, TW_CONST byte *a4, uldat a5, uldat a6, udat a7, hwcol a8, hwcol a9, hwcol a10, hwcol a11, dat a12, dat a13) {
    return (tobj)EncodeCall(0, order_CreateGadget, TwD 
    , c(a1)
    , c(a2)
    , c(a3)
    , c((a2*a3) * sizeof(byte)), (TW_CONST void *)a4
    , c(a5)
    , c(a6)
    , c(a7)
    , c(a8)
    , c(a9)
    , c(a10)
    , c(a11)
    , c(a12)
    , c(a13));
}


tgadget Tw_CreateButtonGadget(tw_d TwD, twidget a1, dat a2, dat a3, TW_CONST byte *a4, uldat a5, udat a6, hwcol a7, hwcol a8, hwcol a9, dat a10, dat a11) {
    return (tobj)EncodeCall(0, order_CreateButtonGadget, TwD 
    , c(a1)
    , c(a2)
    , c(a3)
    , c((a2*a3) * sizeof(byte)), (TW_CONST void *)a4
    , c(a5)
    , c(a6)
    , c(a7)
    , c(a8)
    , c(a9)
    , c(a10)
    , c(a11));
}


void Tw_WriteTextsGadget(tw_d TwD, tgadget a1, byte a2, dat a3, dat a4, TW_CONST byte *a5, dat a6, dat a7) {
    EncodeCall(ENCODE_FL_VOID, order_WriteTextsGadget, TwD 
    , c(a1)
    , c(a2)
    , c(a3)
    , c(a4)
    , c((a2*a3) * sizeof(byte)), (TW_CONST void *)a5
    , c(a6)
    , c(a7));
}

void Tw_WriteHWFontsGadget(tw_d TwD, tgadget a1, byte a2, dat a3, dat a4, TW_CONST hwfont *a5, dat a6, dat a7) {
    EncodeCall(ENCODE_FL_VOID, order_WriteHWFontsGadget, TwD 
    , c(a1)
    , c(a2)
    , c(a3)
    , c(a4)
    , c((a2*a3) * sizeof(hwfont)), (TW_CONST void *)a5
    , c(a6)
    , c(a7));
}


twindow Tw_CreateWindow(tw_d TwD, dat a1, TW_CONST byte *a2, TW_CONST hwcol *a3, tmenu a4, hwcol a5, uldat a6, uldat a7, uldat a8, dat a9, dat a10, dat a11) {
    return (tobj)EncodeCall(0, order_CreateWindow, TwD 
    , c(a1)
    , c((a1) * sizeof(byte)), (TW_CONST void *)a2
    , c((a1) * sizeof(hwcol)), (TW_CONST void *)a3
    , c(a4)
    , c(a5)
    , c(a6)
    , c(a7)
    , c(a8)
    , c(a9)
    , c(a10)
    , c(a11));
}

twindow Tw_Create4MenuWindow(tw_d TwD, tmenu a1) {
    return (tobj)EncodeCall(0, order_Create4MenuWindow, TwD 
    , c(a1));
}


void Tw_WriteAsciiWindow(tw_d TwD, twindow a1, ldat a2, TW_CONST byte *a3) {
    EncodeCall(ENCODE_FL_VOID, order_WriteAsciiWindow, TwD 
    , c(a1)
    , c(a2)
    , c((a2) * sizeof(byte)), (TW_CONST void *)a3);
}

void Tw_WriteStringWindow(tw_d TwD, twindow a1, ldat a2, TW_CONST byte *a3) {
    EncodeCall(ENCODE_FL_VOID, order_WriteStringWindow, TwD 
    , c(a1)
    , c(a2)
    , c((a2) * sizeof(byte)), (TW_CONST void *)a3);
}

void Tw_WriteHWFontWindow(tw_d TwD, twindow a1, ldat a2, TW_CONST hwfont *a3) {
    EncodeCall(ENCODE_FL_VOID, order_WriteHWFontWindow, TwD 
    , c(a1)
    , c(a2)
    , c((a2) * sizeof(hwfont)), (TW_CONST void *)a3);
}

void Tw_WriteHWAttrWindow(tw_d TwD, twindow a1, dat a2, dat a3, ldat a4, TW_CONST hwattr *a5) {
    EncodeCall(ENCODE_FL_VOID, order_WriteHWAttrWindow, TwD 
    , c(a1)
    , c(a2)
    , c(a3)
    , c(a4)
    , c((a4) * sizeof(hwattr)), (TW_CONST void *)a5);
}


void Tw_GotoXYWindow(tw_d TwD, twindow a1, ldat a2, ldat a3) {
    EncodeCall(ENCODE_FL_VOID, order_GotoXYWindow, TwD 
    , c(a1)
    , c(a2)
    , c(a3));
}

void Tw_SetColTextWindow(tw_d TwD, twindow a1, hwcol a2) {
    EncodeCall(ENCODE_FL_VOID, order_SetColTextWindow, TwD 
    , c(a1)
    , c(a2));
}

void Tw_SetColorsWindow(tw_d TwD, twindow a1, udat a2, hwcol a3, hwcol a4, hwcol a5, hwcol a6, hwcol a7, hwcol a8, hwcol a9, hwcol a10, hwcol a11) {
    EncodeCall(ENCODE_FL_VOID, order_SetColorsWindow, TwD 
    , c(a1)
    , c(a2)
    , c(a3)
    , c(a4)
    , c(a5)
    , c(a6)
    , c(a7)
    , c(a8)
    , c(a9)
    , c(a10)
    , c(a11));
}

void Tw_ConfigureWindow(tw_d TwD, twindow a1, byte a2, dat a3, dat a4, dat a5, dat a6, dat a7, dat a8) {
    EncodeCall(ENCODE_FL_VOID, order_ConfigureWindow, TwD 
    , c(a1)
    , c(a2)
    , c(a3)
    , c(a4)
    , c(a5)
    , c(a6)
    , c(a7)
    , c(a8));
}

trow Tw_FindRowByCodeWindow(tw_d TwD, twindow a1, dat a2) {
    return (tobj)EncodeCall(0, order_FindRowByCodeWindow, TwD 
    , c(a1)
    , c(a2));
}


tgroup Tw_CreateGroup(tw_d TwD) {
    return (tobj)EncodeCall(0, order_CreateGroup, TwD );
}

void Tw_InsertGadgetGroup(tw_d TwD, tgroup a1, tgadget a2) {
    EncodeCall(ENCODE_FL_VOID, order_InsertGadgetGroup, TwD 
    , c(a1)
    , c(a2));
}

void Tw_RemoveGadgetGroup(tw_d TwD, tgroup a1, tgadget a2) {
    EncodeCall(ENCODE_FL_VOID, order_RemoveGadgetGroup, TwD 
    , c(a1)
    , c(a2));
}


tgadget Tw_GetSelectedGadgetGroup(tw_d TwD, tgroup a1) {
    return (tobj)EncodeCall(0, order_GetSelectedGadgetGroup, TwD 
    , c(a1));
}

void Tw_SetSelectedGadgetGroup(tw_d TwD, tgroup a1, tgadget a2) {
    EncodeCall(ENCODE_FL_VOID, order_SetSelectedGadgetGroup, TwD 
    , c(a1)
    , c(a2));
}


void Tw_Create4MenuRow(tw_d TwD, twindow a1, udat a2, byte a3, ldat a4, TW_CONST byte *a5) {
    EncodeCall(ENCODE_FL_VOID, order_Create4MenuRow, TwD 
    , c(a1)
    , c(a2)
    , c(a3)
    , c(a4)
    , c((a4) * sizeof(byte)), (TW_CONST void *)a5);
}


void Tw_RaiseRow(tw_d TwD, trow a1) {
    EncodeCall(ENCODE_FL_VOID, order_RaiseRow, TwD 
    , c(a1));
}

void Tw_LowerRow(tw_d TwD, trow a1) {
    EncodeCall(ENCODE_FL_VOID, order_LowerRow, TwD 
    , c(a1));
}

void Tw_RestackChildrenRow(tw_d TwD, tobj a1, uldat a2, TW_CONST trow *a3) {
    EncodeCall(ENCODE_FL_VOID, order_RestackChildrenRow, TwD 
    , c(a1)
    , c(a2)
    , c((a2) * sizeof(tobj)), (TW_CONST void *)a3);
}
 
void Tw_CirculateChildrenRow(tw_d TwD, tobj a1, byte a2) {
    EncodeCall(ENCODE_FL_VOID, order_CirculateChildrenRow, TwD 
    , c(a1)
    , c(a2));
}


tmenuitem Tw_Create4MenuMenuItem(tw_d TwD, tobj a1, twindow a2, byte a3, dat a4, TW_CONST byte *a5) {
    return (tobj)EncodeCall(0, order_Create4MenuMenuItem, TwD 
    , c(a1)
    , c(a2)
    , c(a3)
    , c(a4)
    , c((a4) * sizeof(byte)), (TW_CONST void *)a5);
}

trow Tw_Create4MenuAny(tw_d TwD, tobj a1, twindow a2, udat a3, byte a4, ldat a5, TW_CONST byte *a6) {
    return (tobj)EncodeCall(0, order_Create4MenuAny, TwD 
    , c(a1)
    , c(a2)
    , c(a3)
    , c(a4)
    , c(a5)
    , c((a5) * sizeof(byte)), (TW_CONST void *)a6);
}


uldat Tw_Create4MenuCommonMenuItem(tw_d TwD, tmenu a1) {
    return (uldat)EncodeCall(0, order_Create4MenuCommonMenuItem, TwD 
    , c(a1));
}


tmenu Tw_CreateMenu(tw_d TwD, hwcol a1, hwcol a2, hwcol a3, hwcol a4, hwcol a5, hwcol a6, byte a7) {
    return (tobj)EncodeCall(0, order_CreateMenu, TwD 
    , c(a1)
    , c(a2)
    , c(a3)
    , c(a4)
    , c(a5)
    , c(a6)
    , c(a7));
}

void Tw_SetInfoMenu(tw_d TwD, tmenu a1, byte a2, ldat a3, TW_CONST byte *a4, TW_CONST hwcol *a5) {
    EncodeCall(ENCODE_FL_VOID, order_SetInfoMenu, TwD 
    , c(a1)
    , c(a2)
    , c(a3)
    , c((a3) * sizeof(byte)), (TW_CONST void *)a4
    , c((a3) * sizeof(hwcol)), (TW_CONST void *)a5);
}


tmsgport Tw_CreateMsgPort(tw_d TwD, byte a1, TW_CONST byte *a2, time_t a3, frac_t a4, byte a5) {
    return (tobj)EncodeCall(0, order_CreateMsgPort, TwD 
    , c(a1)
    , c((a1) * sizeof(byte)), (TW_CONST void *)a2
    , c(a3)
    , c(a4)
    , c(a5));
}

tmsgport Tw_FindMsgPort(tw_d TwD, tmsgport a1, byte a2, TW_CONST byte *a3) {
    return (tobj)EncodeCall(0, order_FindMsgPort, TwD 
    , c(a1)
    , c(a2)
    , c((a2) * sizeof(byte)), (TW_CONST void *)a3);
}


void Tw_BgImageScreen(tw_d TwD, tscreen a1, dat a2, dat a3, TW_CONST hwattr *a4) {
    EncodeCall(ENCODE_FL_VOID, order_BgImageScreen, TwD 
    , c(a1)
    , c(a2)
    , c(a3)
    , c((a2*a3) * sizeof(hwattr)), (TW_CONST void *)a4);
}


tobj Tw_PrevObj(tw_d TwD, tobj a1) {
    return (tobj)EncodeCall(0, order_PrevObj, TwD 
    , c(a1));
}

tobj Tw_NextObj(tw_d TwD, tobj a1) {
    return (tobj)EncodeCall(0, order_NextObj, TwD 
    , c(a1));
}

tobj Tw_ParentObj(tw_d TwD, tobj a1) {
    return (tobj)EncodeCall(0, order_ParentObj, TwD 
    , c(a1));
}


tgadget   Tw_G_PrevGadget(tw_d TwD, tgadget  a1) {
    return (tobj)EncodeCall(0, order_G_PrevGadget, TwD 
    , c(a1));
}

tgadget   Tw_G_NextGadget(tw_d TwD, tgadget  a1) {
    return (tobj)EncodeCall(0, order_G_NextGadget, TwD 
    , c(a1));
}

tgroup    Tw_GroupGadget(tw_d TwD, tgadget  a1) {
    return (tobj)EncodeCall(0, order_GroupGadget, TwD 
    , c(a1));
}


twidget   Tw_O_PrevWidget(tw_d TwD, twidget  a1) {
    return (tobj)EncodeCall(0, order_O_PrevWidget, TwD 
    , c(a1));
}

twidget   Tw_O_NextWidget(tw_d TwD, twidget  a1) {
    return (tobj)EncodeCall(0, order_O_NextWidget, TwD 
    , c(a1));
}

tmsgport  Tw_OwnerWidget(tw_d TwD, twidget  a1) {
    return (tobj)EncodeCall(0, order_OwnerWidget, TwD 
    , c(a1));
}


tscreen   Tw_FirstScreen(tw_d TwD) {
    return (tobj)EncodeCall(0, order_FirstScreen, TwD );
}

twidget   Tw_FirstWidget(tw_d TwD, twidget  a1) {
    return (tobj)EncodeCall(0, order_FirstWidget, TwD 
    , c(a1));
}

tmsgport  Tw_FirstMsgPort(tw_d TwD) {
    return (tobj)EncodeCall(0, order_FirstMsgPort, TwD );
}

tmenu     Tw_FirstMenu(tw_d TwD, tmsgport a1) {
    return (tobj)EncodeCall(0, order_FirstMenu, TwD 
    , c(a1));
}

twidget   Tw_FirstW(tw_d TwD, tmsgport a1) {
    return (tobj)EncodeCall(0, order_FirstW, TwD 
    , c(a1));
}

tgroup    Tw_FirstGroup(tw_d TwD, tmsgport a1) {
    return (tobj)EncodeCall(0, order_FirstGroup, TwD 
    , c(a1));
}

tmutex    Tw_FirstMutex(tw_d TwD, tmsgport a1) {
    return (tobj)EncodeCall(0, order_FirstMutex, TwD 
    , c(a1));
}

tmenuitem Tw_FirstMenuItem(tw_d TwD, tmenu    a1) {
    return (tobj)EncodeCall(0, order_FirstMenuItem, TwD 
    , c(a1));
}

tgadget   Tw_FirstGadget(tw_d TwD, tgroup   a1) {
    return (tobj)EncodeCall(0, order_FirstGadget, TwD 
    , c(a1));
}


dat Tw_GetDisplayWidth(tw_d TwD) {
    return (dat)EncodeCall(0, order_GetDisplayWidth, TwD );
}

dat Tw_GetDisplayHeight(tw_d TwD) {
    return (dat)EncodeCall(0, order_GetDisplayHeight, TwD );
}

tall Tw_GetAll(tw_d TwD) {
    return (tobj)EncodeCall(0, order_GetAll, TwD );
}


byte Tw_SendToMsgPort(tw_d TwD, tmsgport a1, udat a2, TW_CONST byte *a3) {
    return (byte)EncodeCall(0, order_SendToMsgPort, TwD 
    , c(a1)
    , c(a2)
    , c((a2) * sizeof(byte)), (TW_CONST void *)a3);
}

void Tw_BlindSendToMsgPort(tw_d TwD, tmsgport a1, udat a2, TW_CONST byte *a3) {
    EncodeCall(ENCODE_FL_VOID, order_BlindSendToMsgPort, TwD 
    , c(a1)
    , c(a2)
    , c((a2) * sizeof(byte)), (TW_CONST void *)a3);
}


tobj Tw_GetOwnerSelection(tw_d TwD) {
    return (tobj)EncodeCall(0, order_GetOwnerSelection, TwD );
}

void Tw_SetOwnerSelection(tw_d TwD, time_t a1, frac_t a2) {
    EncodeCall(ENCODE_FL_VOID, order_SetOwnerSelection, TwD 
    , c(a1)
    , c(a2));
}

void Tw_RequestSelection(tw_d TwD, tobj a1, uldat a2) {
    EncodeCall(ENCODE_FL_VOID, order_RequestSelection, TwD 
    , c(a1)
    , c(a2));
}

void Tw_NotifySelection(tw_d TwD, tobj a1, uldat a2, uldat a3, TW_CONST byte *a4, uldat a5, TW_CONST byte *a6) {
    EncodeCall(ENCODE_FL_VOID, order_NotifySelection, TwD 
    , c(a1)
    , c(a2)
    , c(a3)
    , c((TW_MAX_MIMELEN) * sizeof(byte)), (TW_CONST void *)a4
    , c(a5)
    , c((a5) * sizeof(byte)), (TW_CONST void *)a6);
}


byte Tw_SetServerUid(tw_d TwD, uldat a1, byte a2) {
    return (byte)EncodeCall(0, order_SetServerUid, TwD 
    , c(a1)
    , c(a2));
}




#undef c

