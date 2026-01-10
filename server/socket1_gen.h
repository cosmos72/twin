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
case order_FindFunction:
    if (a.size() >= 4) {
        a[0]_any = (uldat)sockFindFunction(ctx, (byte)a[1]_any, (const char *)a[2]_vec, (byte)a[3]_any, (const char *)a[4]_vec);
        return ttrue;
    }
    break;

case order_SyncSocket:
    if (a.size() >= 0) {
        a[0]_any = (byte)sockSyncSocket(ctx);
        return ttrue;
    }
    break;

case order_ServerSizeof:
    if (a.size() >= 1) {
        a[0]_any = (byte)sockServerSizeof(ctx, (byte)a[1]_any);
        return ttrue;
    }
    break;

case order_CanCompress:
    if (a.size() >= 0) {
        a[0]_any = (byte)sockCanCompress(ctx);
        return ttrue;
    }
    break;

case order_DoCompress:
    if (a.size() >= 1) {
        a[0]_any = (byte)sockDoCompress(ctx, (byte)a[1]_any);
        return ttrue;
    }
    break;

case order_NeedResizeDisplay:
    if (a.size() >= 0) {
        sockNeedResizeDisplay(ctx);
        return ttrue;
    }
    break;

case order_AttachHW:
    if (a.size() >= 3) {
        sockAttachHW(ctx, (uldat)a[1]_any, (const char *)a[2]_vec, (byte)a[3]_any);
        return ttrue;
    }
    break;

case order_DetachHW:
    if (a.size() >= 2) {
        a[0]_any = (byte)sockDetachHW(ctx, (uldat)a[1]_any, (const char *)a[2]_vec);
        return ttrue;
    }
    break;

case order_SetFontTranslation:
    if (a.size() >= 1) {
        sockSetFontTranslation(ctx, (const byte *)a[1]_vec);
        return ttrue;
    }
    break;

case order_SetTRuneTranslation:
    if (a.size() >= 1) {
        sockSetTRuneTranslation(ctx, (const trune *)a[1]_vec);
        return ttrue;
    }
    break;

case order_DeleteObj:
    if (a.size() >= 1) {
        sockDeleteObj(ctx, (Tobj)a[1]_obj);
        return ttrue;
    }
    break;

case order_ChangeFieldObj:
    if (a.size() >= 4) {
        ((Tobj)a[1]_obj)->ChangeField((udat)a[2]_any, (tany)a[3]_any, (tany)a[4]_any);
        return ttrue;
    }
    break;

case order_CreateWidget:
    if (a.size() >= 7) {
        a[0]_obj = (Tobj)sockCreateWidget(ctx, (dat)a[1]_any, (dat)a[2]_any, (uldat)a[3]_any, (uldat)a[4]_any, (dat)a[5]_any, (dat)a[6]_any, a[7]_tcell);
        return ttrue;
    }
    break;

case order_RecursiveDeleteWidget:
    if (a.size() >= 1) {
        sockRecursiveDeleteWidget(ctx, (Twidget)a[1]_obj);
        return ttrue;
    }
    break;

case order_MapWidget:
    if (a.size() >= 2) {
        ((Twidget)a[1]_obj)->Map((Twidget)a[2]_obj);
        return ttrue;
    }
    break;

case order_UnMapWidget:
    if (a.size() >= 1) {
        ((Twidget)a[1]_obj)->UnMap();
        return ttrue;
    }
    break;

case order_SetXYWidget:
    if (a.size() >= 3) {
        sockSetXYWidget(ctx, (Twidget)a[1]_obj, (dat)a[2]_any, (dat)a[3]_any);
        return ttrue;
    }
    break;

case order_ResizeWidget:
    if (a.size() >= 3) {
        sockResizeWidget(ctx, (Twidget)a[1]_obj, (dat)a[2]_any, (dat)a[3]_any);
        return ttrue;
    }
    break;

case order_ScrollWidget:
    if (a.size() >= 3) {
        sockScrollWidget(ctx, (Twidget)a[1]_obj, (ldat)a[2]_any, (ldat)a[3]_any);
        return ttrue;
    }
    break;

case order_DrawWidget:
    if (a.size() >= 8) {
        sockDrawWidget(ctx, (Twidget)a[1]_obj, (dat)a[2]_any, (dat)a[3]_any, (dat)a[4]_any, (dat)a[5]_any, (const char *)a[6]_vec, (const trune *)a[7]_vec, (const tcell *)a[8]_vec);
        return ttrue;
    }
    break;

case order_SetVisibleWidget:
    if (a.size() >= 2) {
        sockSetVisibleWidget(ctx, (Twidget)a[1]_obj, (byte)a[2]_any);
        return ttrue;
    }
    break;

case order_FocusSubWidget:
    if (a.size() >= 1) {
        sockFocusSubWidget(ctx, (Twidget)a[1]_obj);
        return ttrue;
    }
    break;

case order_FindWidgetAtWidget:
    if (a.size() >= 3) {
        a[0]_obj = (Tobj)((Twidget)a[1]_obj)->FindWidgetAt((dat)a[2]_any, (dat)a[3]_any);
        return ttrue;
    }
    break;

case order_RaiseWidget:
    if (a.size() >= 1) {
        ((Twidget)a[1]_obj)->Raise();
        return ttrue;
    }
    break;

case order_LowerWidget:
    if (a.size() >= 1) {
        ((Twidget)a[1]_obj)->Lower();
        return ttrue;
    }
    break;

case order_RestackChildrenWidget:
    if (a.size() >= 3) {
        sockRestackChildrenWidget(ctx, (Twidget)a[1]_obj, (uldat)a[2]_any, (const Twidget *)a[3]_vec);
        return ttrue;
    }
    break;

case order_CirculateChildrenWidget:
    if (a.size() >= 2) {
        sockCirculateChildrenWidget(ctx, (Twidget)a[1]_obj, (byte)a[2]_any);
        return ttrue;
    }
    break;

case order_CreateGadget:
    if (a.size() >= 13) {
        a[0]_obj = (Tobj)sockCreateGadget(ctx, (Twidget)a[1]_obj, (dat)a[2]_any, (dat)a[3]_any, (const char *)a[4]_vec, (uldat)a[5]_any, (uldat)a[6]_any, (udat)a[7]_any, a[8]_tcolor, a[9]_tcolor, a[10]_tcolor, a[11]_tcolor, (dat)a[12]_any, (dat)a[13]_any);
        return ttrue;
    }
    break;

case order_CreateButtonGadget:
    if (a.size() >= 11) {
        a[0]_obj = (Tobj)Sgadget::CreateButton((Twidget)a[1]_obj, (dat)a[2]_any, (dat)a[3]_any, (const char *)a[4]_vec, (uldat)a[5]_any, (udat)a[6]_any, a[7]_tcolor, a[8]_tcolor, a[9]_tcolor, (dat)a[10]_any, (dat)a[11]_any);
        return ttrue;
    }
    break;

case order_WriteTextsGadget:
    if (a.size() >= 7) {
        ((Tgadget)a[1]_obj)->WriteTexts((byte)a[2]_any, (dat)a[3]_any, (dat)a[4]_any, (const char *)a[5]_vec, (dat)a[6]_any, (dat)a[7]_any);
        return ttrue;
    }
    break;

case order_WriteTRunesGadget:
    if (a.size() >= 7) {
        ((Tgadget)a[1]_obj)->WriteTRunes((byte)a[2]_any, (dat)a[3]_any, (dat)a[4]_any, (const trune *)a[5]_vec, (dat)a[6]_any, (dat)a[7]_any);
        return ttrue;
    }
    break;

case order_CreateWindow:
    if (a.size() >= 11) {
        a[0]_obj = (Tobj)sockCreateWindow(ctx, (dat)a[1]_any, (const char *)a[2]_vec, (const tcolor *)a[3]_vec, (Tmenu)a[4]_obj, a[5]_tcolor, (uldat)a[6]_any, (uldat)a[7]_any, (uldat)a[8]_any, (dat)a[9]_any, (dat)a[10]_any, (dat)a[11]_any);
        return ttrue;
    }
    break;

case order_Create4MenuWindow:
    if (a.size() >= 1) {
        a[0]_obj = (Tobj)Swindow::Create4Menu((Tmenu)a[1]_obj);
        return ttrue;
    }
    break;

case order_WriteCharsetWindow:
    if (a.size() >= 3) {
        sockWriteCharsetWindow(ctx, (Twindow)a[1]_obj, (uldat)a[2]_any, (const char *)a[3]_vec);
        return ttrue;
    }
    break;

case order_WriteUtf8Window:
    if (a.size() >= 3) {
        sockWriteUtf8Window(ctx, (Twindow)a[1]_obj, (uldat)a[2]_any, (const char *)a[3]_vec);
        return ttrue;
    }
    break;

case order_WriteTRuneWindow:
    if (a.size() >= 3) {
        sockWriteTRuneWindow(ctx, (Twindow)a[1]_obj, (uldat)a[2]_any, (const trune *)a[3]_vec);
        return ttrue;
    }
    break;

case order_WriteTCellWindow:
    if (a.size() >= 5) {
        sockWriteTCellWindow(ctx, (Twindow)a[1]_obj, (dat)a[2]_any, (dat)a[3]_any, (uldat)a[4]_any, (const tcell *)a[5]_vec);
        return ttrue;
    }
    break;

case order_GotoXYWindow:
    if (a.size() >= 3) {
        ((Twindow)a[1]_obj)->GotoXY((ldat)a[2]_any, (ldat)a[3]_any);
        return ttrue;
    }
    break;

case order_SetTitleWindow:
    if (a.size() >= 3) {
        sockSetTitleWindow(ctx, (Twindow)a[1]_obj, (dat)a[2]_any, (const char *)a[3]_vec);
        return ttrue;
    }
    break;

case order_SetColTextWindow:
    if (a.size() >= 2) {
        ((Twindow)a[1]_obj)->SetColText(a[2]_tcolor);
        return ttrue;
    }
    break;

case order_SetColorsWindow:
    if (a.size() >= 11) {
        ((Twindow)a[1]_obj)->SetColors((udat)a[2]_any, a[3]_tcolor, a[4]_tcolor, a[5]_tcolor, a[6]_tcolor, a[7]_tcolor, a[8]_tcolor, a[9]_tcolor, a[10]_tcolor, a[11]_tcolor);
        return ttrue;
    }
    break;

case order_ConfigureWindow:
    if (a.size() >= 8) {
        ((Twindow)a[1]_obj)->Configure((byte)a[2]_any, (dat)a[3]_any, (dat)a[4]_any, (dat)a[5]_any, (dat)a[6]_any, (dat)a[7]_any, (dat)a[8]_any);
        return ttrue;
    }
    break;

case order_FindRowByCodeWindow:
    if (a.size() >= 2) {
        a[0]_obj = (Tobj)sockFindRowByCodeWindow(ctx, (Twindow)a[1]_obj, (dat)a[2]_any);
        return ttrue;
    }
    break;

case order_CreateGroup:
    if (a.size() >= 0) {
        a[0]_obj = (Tobj)sockCreateGroup(ctx);
        return ttrue;
    }
    break;

case order_InsertGadgetGroup:
    if (a.size() >= 2) {
        ((Tgroup)a[1]_obj)->InsertGadget((Tgadget)a[2]_obj);
        return ttrue;
    }
    break;

case order_RemoveGadgetGroup:
    if (a.size() >= 2) {
        ((Tgroup)a[1]_obj)->RemoveGadget((Tgadget)a[2]_obj);
        return ttrue;
    }
    break;

case order_GetSelectedGadgetGroup:
    if (a.size() >= 1) {
        a[0]_obj = (Tobj)((Tgroup)a[1]_obj)->GetSelectedGadget();
        return ttrue;
    }
    break;

case order_SetSelectedGadgetGroup:
    if (a.size() >= 2) {
        ((Tgroup)a[1]_obj)->SetSelectedGadget((Tgadget)a[2]_obj);
        return ttrue;
    }
    break;

case order_RaiseRow:
    if (a.size() >= 1) {
        ((Trow)a[1]_obj)->Raise();
        return ttrue;
    }
    break;

case order_LowerRow:
    if (a.size() >= 1) {
        ((Trow)a[1]_obj)->Lower();
        return ttrue;
    }
    break;

case order_RestackChildrenRow:
    if (a.size() >= 3) {
        sockRestackChildrenRow(ctx, (Tobj)a[1]_obj, (uldat)a[2]_any, (const Trow *)a[3]_vec);
        return ttrue;
    }
    break;

case order_CirculateChildrenRow:
    if (a.size() >= 2) {
        sockCirculateChildrenRow(ctx, (Tobj)a[1]_obj, (byte)a[2]_any);
        return ttrue;
    }
    break;

case order_Create4MenuAny:
    if (a.size() >= 6) {
        a[0]_obj = (Tobj)sockCreate4MenuAny(ctx, (Tobj)a[1]_obj, (Twindow)a[2]_obj, (udat)a[3]_any, (byte)a[4]_any, (ldat)a[5]_any, (const char *)a[6]_vec);
        return ttrue;
    }
    break;

case order_Create4MenuCommonMenuItem:
    if (a.size() >= 1) {
        a[0]_any = (uldat)Smenuitem::Create4MenuCommon((Tmenu)a[1]_obj);
        return ttrue;
    }
    break;

case order_CreateMenu:
    if (a.size() >= 7) {
        a[0]_obj = (Tobj)sockCreateMenu(ctx, a[1]_tcolor, a[2]_tcolor, a[3]_tcolor, a[4]_tcolor, a[5]_tcolor, a[6]_tcolor, (byte)a[7]_any);
        return ttrue;
    }
    break;

case order_SetInfoMenu:
    if (a.size() >= 5) {
        ((Tmenu)a[1]_obj)->SetInfo((byte)a[2]_any, (ldat)a[3]_any, (const char *)a[4]_vec, (const tcolor *)a[5]_vec);
        return ttrue;
    }
    break;

case order_CreateMsgPort:
    if (a.size() >= 2) {
        a[0]_obj = (Tobj)sockCreateMsgPort(ctx, (byte)a[1]_any, (const char *)a[2]_vec);
        return ttrue;
    }
    break;

case order_FindMsgPort:
    if (a.size() >= 3) {
        a[0]_obj = (Tobj)sockFindMsgPort(ctx, (Tmsgport)a[1]_obj, (byte)a[2]_any, (const char *)a[3]_vec);
        return ttrue;
    }
    break;

case order_BgImageScreen:
    if (a.size() >= 4) {
        ((Tscreen)a[1]_obj)->BgImage((dat)a[2]_any, (dat)a[3]_any, (const tcell *)a[4]_vec);
        return ttrue;
    }
    break;

case order_PrevObj:
    if (a.size() >= 1) {
        a[0]_obj = (Tobj)sockPrevObj(ctx, (Tobj)a[1]_obj);
        return ttrue;
    }
    break;

case order_NextObj:
    if (a.size() >= 1) {
        a[0]_obj = (Tobj)sockNextObj(ctx, (Tobj)a[1]_obj);
        return ttrue;
    }
    break;

case order_ParentObj:
    if (a.size() >= 1) {
        a[0]_obj = (Tobj)sockParentObj(ctx, (Tobj)a[1]_obj);
        return ttrue;
    }
    break;

case order_FirstScreen:
    if (a.size() >= 0) {
        a[0]_obj = (Tobj)sockFirstScreen(ctx);
        return ttrue;
    }
    break;

case order_FirstWidget:
    if (a.size() >= 1) {
        a[0]_obj = (Tobj)sockFirstWidget(ctx, (Twidget)a[1]_obj);
        return ttrue;
    }
    break;

case order_FirstMsgPort:
    if (a.size() >= 0) {
        a[0]_obj = (Tobj)sockFirstMsgPort(ctx);
        return ttrue;
    }
    break;

case order_FirstMenu:
    if (a.size() >= 1) {
        a[0]_obj = (Tobj)sockFirstMenu(ctx, (Tmsgport)a[1]_obj);
        return ttrue;
    }
    break;

case order_FirstW:
    if (a.size() >= 1) {
        a[0]_obj = (Tobj)sockFirstW(ctx, (Tmsgport)a[1]_obj);
        return ttrue;
    }
    break;

case order_FirstGroup:
    if (a.size() >= 1) {
        a[0]_obj = (Tobj)sockFirstGroup(ctx, (Tmsgport)a[1]_obj);
        return ttrue;
    }
    break;

case order_FirstMutex:
    if (a.size() >= 1) {
        a[0]_obj = (Tobj)sockFirstMutex(ctx, (Tmsgport)a[1]_obj);
        return ttrue;
    }
    break;

case order_FirstMenuItem:
    if (a.size() >= 1) {
        a[0]_obj = (Tobj)sockFirstMenuItem(ctx, (Tmenu)a[1]_obj);
        return ttrue;
    }
    break;

case order_FirstGadget:
    if (a.size() >= 1) {
        a[0]_obj = (Tobj)sockFirstGadget(ctx, (Tgroup)a[1]_obj);
        return ttrue;
    }
    break;

case order_GetDisplayWidth:
    if (a.size() >= 0) {
        a[0]_any = (dat)sockGetDisplayWidth(ctx);
        return ttrue;
    }
    break;

case order_GetDisplayHeight:
    if (a.size() >= 0) {
        a[0]_any = (dat)sockGetDisplayHeight(ctx);
        return ttrue;
    }
    break;

case order_GetAll:
    if (a.size() >= 0) {
        a[0]_obj = (Tobj)sockGetAll(ctx);
        return ttrue;
    }
    break;

case order_SendToMsgPort:
    if (a.size() >= 3) {
        a[0]_any = (byte)sockSendToMsgPort(ctx, (Tmsgport)a[1]_obj, (udat)a[2]_any, (const byte *)a[3]_vec);
        return ttrue;
    }
    break;

case order_BlindSendToMsgPort:
    if (a.size() >= 3) {
        sockBlindSendToMsgPort(ctx, (Tmsgport)a[1]_obj, (udat)a[2]_any, (const byte *)a[3]_vec);
        return ttrue;
    }
    break;

case order_GetOwnerSelection:
    if (a.size() >= 0) {
        a[0]_obj = (Tobj)sockGetOwnerSelection(ctx);
        return ttrue;
    }
    break;

case order_SetOwnerSelection:
    if (a.size() >= 2) {
        sockSetOwnerSelection(ctx, (tany)a[1]_any, (tany)a[2]_any);
        return ttrue;
    }
    break;

case order_RequestSelection:
    if (a.size() >= 2) {
        sockRequestSelection(ctx, (Tobj)a[1]_obj, (uldat)a[2]_any);
        return ttrue;
    }
    break;

case order_NotifySelection:
    if (a.size() >= 6) {
        sockNotifySelection(ctx, (Tobj)a[1]_obj, (uldat)a[2]_any, (uldat)a[3]_any, (const char *)a[4]_vec, (uldat)a[5]_any, (const char *)a[6]_vec);
        return ttrue;
    }
    break;

case order_SetServerUid:
    if (a.size() >= 2) {
        a[0]_any = (byte)sockSetServerUid(ctx, (uldat)a[1]_any, (byte)a[2]_any);
        return ttrue;
    }
    break;

