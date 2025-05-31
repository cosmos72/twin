/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */


/* This file was automatically generated from m4/socket1.m4, do not edit! */

/* clang-format off */










































case order_FindFunction:
    if (a.size() >= 4)
        a[0]_any = (uldat)sockFindFunction((byte)a[1]_any, (const char *)a[2]_vec, (byte)a[3]_any, (const char *)a[4]_vec);
    break;



case order_SyncSocket:
    if (a.size() >= 0)
        a[0]_any = (byte)sockSyncSocket();
    break;



case order_ServerSizeof:
    if (a.size() >= 1)
        a[0]_any = (byte)sockServerSizeof((byte)a[1]_any);
    break;



case order_CanCompress:
    if (a.size() >= 0)
        a[0]_any = (byte)sockCanCompress();
    break;


case order_DoCompress:
    if (a.size() >= 1)
        a[0]_any = (byte)sockDoCompress((byte)a[1]_any);
    break;



case order_NeedResizeDisplay:
    if (a.size() >= 0)
        sockNeedResizeDisplay();
    break;



case order_AttachHW:
    if (a.size() >= 3)
        sockAttachHW((uldat)a[1]_any, (const char *)a[2]_vec, (byte)a[3]_any);
    break;


case order_DetachHW:
    if (a.size() >= 2)
        a[0]_any = (byte)sockDetachHW((uldat)a[1]_any, (const char *)a[2]_vec);
    break;



case order_SetFontTranslation:
    if (a.size() >= 1)
        sockSetFontTranslation((const byte *)a[1]_vec);
    break;


case order_SetTRuneTranslation:
    if (a.size() >= 1)
        sockSetTRuneTranslation((const trune *)a[1]_vec);
    break;



case order_DeleteObj:
    if (a.size() >= 1)
        sockDeleteObj((Tobj)a[1]_obj);
    break;


case order_ChangeFieldObj:
    if (a.size() >= 4)
        ((Tobj)a[1]_obj)->ChangeField((udat)a[2]_any,(uldat)a[3]_any,(uldat)a[4]_any);
    break;



case order_CreateWidget:
    if (a.size() >= 7)
        a[0]_obj = (Tobj)sockCreateWidget((dat)a[1]_any, (dat)a[2]_any, (uldat)a[3]_any, (uldat)a[4]_any, (dat)a[5]_any, (dat)a[6]_any, (tcell)a[7]_any);
    break;


case order_RecursiveDeleteWidget:
    if (a.size() >= 1)
        sockRecursiveDeleteWidget((Twidget)a[1]_obj);
    break;

case order_MapWidget:
    if (a.size() >= 2)
        ((Twidget)a[1]_obj)->Map((Twidget)a[2]_obj);
    break;

case order_UnMapWidget:
    if (a.size() >= 1)
        ((Twidget)a[1]_obj)->UnMap();
    break;

case order_SetXYWidget:
    if (a.size() >= 3)
        sockSetXYWidget((Twidget)a[1]_obj, (dat)a[2]_any, (dat)a[3]_any);
    break;

case order_ResizeWidget:
    if (a.size() >= 3)
        sockResizeWidget((Twidget)a[1]_obj, (dat)a[2]_any, (dat)a[3]_any);
    break;

case order_ScrollWidget:
    if (a.size() >= 3)
        sockScrollWidget((Twidget)a[1]_obj, (ldat)a[2]_any, (ldat)a[3]_any);
    break;

case order_DrawWidget:
    if (a.size() >= 8)
        sockDrawWidget((Twidget)a[1]_obj, (dat)a[2]_any, (dat)a[3]_any, (dat)a[4]_any, (dat)a[5]_any, (const char *)a[6]_vec, (const trune *)a[7]_vec, (const tcell *)a[8]_vec);
    break;


case order_SetVisibleWidget:
    if (a.size() >= 2)
        sockSetVisibleWidget((Twidget)a[1]_obj, (byte)a[2]_any);
    break;


case order_FocusSubWidget:
    if (a.size() >= 1)
        sockFocusSubWidget((Twidget)a[1]_obj);
    break;

case order_FindWidgetAtWidget:
    if (a.size() >= 3)
        a[0]_obj = (Tobj)((Twidget)a[1]_obj)->FindWidgetAt((dat)a[2]_any,(dat)a[3]_any);
    break;


case order_RaiseWidget:
    if (a.size() >= 1)
        ((Twidget)a[1]_obj)->Raise();
    break;

case order_LowerWidget:
    if (a.size() >= 1)
        ((Twidget)a[1]_obj)->Lower();
    break;

case order_RestackChildrenWidget:
    if (a.size() >= 3)
        sockRestackChildrenWidget((Twidget)a[1]_obj, (uldat)a[2]_any, (const Twidget *)a[3]_vec);
    break;

case order_CirculateChildrenWidget:
    if (a.size() >= 2)
        sockCirculateChildrenWidget((Twidget)a[1]_obj, (byte)a[2]_any);
    break;



case order_CreateGadget:
    if (a.size() >= 13)
        a[0]_obj = (Tobj)sockCreateGadget((Twidget)a[1]_obj, (dat)a[2]_any, (dat)a[3]_any, (const char *)a[4]_vec, (uldat)a[5]_any, (uldat)a[6]_any, (udat)a[7]_any, (tcolor)a[8]_any, (tcolor)a[9]_any, (tcolor)a[10]_any, (tcolor)a[11]_any, (dat)a[12]_any, (dat)a[13]_any);
    break;


case order_CreateButtonGadget:
    if (a.size() >= 11)
        a[0]_obj = (Tobj)Sgadget::CreateButton((Twidget)a[1]_obj, (dat)a[2]_any, (dat)a[3]_any, (const char *)a[4]_vec, (uldat)a[5]_any, (udat)a[6]_any, (tcolor)a[7]_any, (tcolor)a[8]_any, (tcolor)a[9]_any, (dat)a[10]_any, (dat)a[11]_any);
    break;


case order_WriteTextsGadget:
    if (a.size() >= 7)
        ((Tgadget)a[1]_obj)->WriteTexts((byte)a[2]_any,(dat)a[3]_any,(dat)a[4]_any,(const char *)a[5]_vec,(dat)a[6]_any,(dat)a[7]_any);
    break;

case order_WriteTRunesGadget:
    if (a.size() >= 7)
        ((Tgadget)a[1]_obj)->WriteTRunes((byte)a[2]_any,(dat)a[3]_any,(dat)a[4]_any,(const trune *)a[5]_vec,(dat)a[6]_any,(dat)a[7]_any);
    break;


case order_CreateWindow:
    if (a.size() >= 11)
        a[0]_obj = (Tobj)sockCreateWindow((dat)a[1]_any, (const char *)a[2]_vec, (const tcolor *)a[3]_vec, (Tmenu)a[4]_obj, (tcolor)a[5]_any, (uldat)a[6]_any, (uldat)a[7]_any, (uldat)a[8]_any, (dat)a[9]_any, (dat)a[10]_any, (dat)a[11]_any);
    break;

case order_Create4MenuWindow:
    if (a.size() >= 1)
        a[0]_obj = (Tobj)Swindow::Create4Menu((Tmenu)a[1]_obj);
    break;


case order_WriteCharsetWindow:
    if (a.size() >= 3)
        sockWriteCharsetWindow((Twindow)a[1]_obj, (uldat)a[2]_any, (const char *)a[3]_vec);
    break;

case order_WriteUtf8Window:
    if (a.size() >= 3)
        sockWriteUtf8Window((Twindow)a[1]_obj, (uldat)a[2]_any, (const char *)a[3]_vec);
    break;

case order_WriteTRuneWindow:
    if (a.size() >= 3)
        sockWriteTRuneWindow((Twindow)a[1]_obj, (uldat)a[2]_any, (const trune *)a[3]_vec);
    break;

case order_WriteTCellWindow:
    if (a.size() >= 5)
        sockWriteTCellWindow((Twindow)a[1]_obj, (dat)a[2]_any, (dat)a[3]_any, (uldat)a[4]_any, (const tcell *)a[5]_vec);
    break;


case order_GotoXYWindow:
    if (a.size() >= 3)
        ((Twindow)a[1]_obj)->GotoXY((ldat)a[2]_any,(ldat)a[3]_any);
    break;

case order_SetTitleWindow:
    if (a.size() >= 3)
        sockSetTitleWindow((Twindow)a[1]_obj, (dat)a[2]_any, (const char *)a[3]_vec);
    break;

case order_SetColTextWindow:
    if (a.size() >= 2)
        ((Twindow)a[1]_obj)->SetColText((tcolor)a[2]_any);
    break;

case order_SetColorsWindow:
    if (a.size() >= 11)
        ((Twindow)a[1]_obj)->SetColors((udat)a[2]_any,(tcolor)a[3]_any,(tcolor)a[4]_any,(tcolor)a[5]_any,(tcolor)a[6]_any,(tcolor)a[7]_any,(tcolor)a[8]_any,(tcolor)a[9]_any,(tcolor)a[10]_any,(tcolor)a[11]_any);
    break;

case order_ConfigureWindow:
    if (a.size() >= 8)
        ((Twindow)a[1]_obj)->Configure((byte)a[2]_any,(dat)a[3]_any,(dat)a[4]_any,(dat)a[5]_any,(dat)a[6]_any,(dat)a[7]_any,(dat)a[8]_any);
    break;

case order_FindRowByCodeWindow:
    if (a.size() >= 2)
        a[0]_obj = (Tobj)sockFindRowByCodeWindow((Twindow)a[1]_obj, (dat)a[2]_any);
    break;


case order_CreateGroup:
    if (a.size() >= 0)
        a[0]_obj = (Tobj)sockCreateGroup();
    break;

case order_InsertGadgetGroup:
    if (a.size() >= 2)
        ((Tgroup)a[1]_obj)->InsertGadget((Tgadget)a[2]_obj);
    break;

case order_RemoveGadgetGroup:
    if (a.size() >= 2)
        ((Tgroup)a[1]_obj)->RemoveGadget((Tgadget)a[2]_obj);
    break;


case order_GetSelectedGadgetGroup:
    if (a.size() >= 1)
        a[0]_obj = (Tobj)((Tgroup)a[1]_obj)->GetSelectedGadget();
    break;

case order_SetSelectedGadgetGroup:
    if (a.size() >= 2)
        ((Tgroup)a[1]_obj)->SetSelectedGadget((Tgadget)a[2]_obj);
    break;


case order_RaiseRow:
    if (a.size() >= 1)
        ((Trow)a[1]_obj)->Raise();
    break;

case order_LowerRow:
    if (a.size() >= 1)
        ((Trow)a[1]_obj)->Lower();
    break;

case order_RestackChildrenRow:
    if (a.size() >= 3)
        sockRestackChildrenRow((Tobj)a[1]_obj, (uldat)a[2]_any, (const Trow *)a[3]_vec);
    break;

case order_CirculateChildrenRow:
    if (a.size() >= 2)
        sockCirculateChildrenRow((Tobj)a[1]_obj, (byte)a[2]_any);
    break;


case order_Create4MenuAny:
    if (a.size() >= 6)
        a[0]_obj = (Tobj)sockCreate4MenuAny((Tobj)a[1]_obj, (Twindow)a[2]_obj, (udat)a[3]_any, (byte)a[4]_any, (ldat)a[5]_any, (const char *)a[6]_vec);
    break;


case order_Create4MenuCommonMenuItem:
    if (a.size() >= 1)
        a[0]_any = (uldat)Smenuitem::Create4MenuCommon((Tmenu)a[1]_obj);
    break;


case order_CreateMenu:
    if (a.size() >= 7)
        a[0]_obj = (Tobj)sockCreateMenu((tcolor)a[1]_any, (tcolor)a[2]_any, (tcolor)a[3]_any, (tcolor)a[4]_any, (tcolor)a[5]_any, (tcolor)a[6]_any, (byte)a[7]_any);
    break;

case order_SetInfoMenu:
    if (a.size() >= 5)
        ((Tmenu)a[1]_obj)->SetInfo((byte)a[2]_any,(ldat)a[3]_any,(const char *)a[4]_vec,(const tcolor *)a[5]_vec);
    break;


case order_CreateMsgPort:
    if (a.size() >= 2)
        a[0]_obj = (Tobj)sockCreateMsgPort((byte)a[1]_any, (const char *)a[2]_vec);
    break;

case order_FindMsgPort:
    if (a.size() >= 3)
        a[0]_obj = (Tobj)sockFindMsgPort((Tmsgport)a[1]_obj, (byte)a[2]_any, (const char *)a[3]_vec);
    break;


case order_BgImageScreen:
    if (a.size() >= 4)
        ((Tscreen)a[1]_obj)->BgImage((dat)a[2]_any,(dat)a[3]_any,(const tcell *)a[4]_vec);
    break;


case order_PrevObj:
    if (a.size() >= 1)
        a[0]_obj = (Tobj)sockPrevObj((Tobj)a[1]_obj);
    break;

case order_NextObj:
    if (a.size() >= 1)
        a[0]_obj = (Tobj)sockNextObj((Tobj)a[1]_obj);
    break;

case order_ParentObj:
    if (a.size() >= 1)
        a[0]_obj = (Tobj)sockParentObj((Tobj)a[1]_obj);
    break;


case order_FirstScreen:
    if (a.size() >= 0)
        a[0]_obj = (Tobj)sockFirstScreen();
    break;

case order_FirstWidget:
    if (a.size() >= 1)
        a[0]_obj = (Tobj)sockFirstWidget((Twidget )a[1]_obj);
    break;

case order_FirstMsgPort:
    if (a.size() >= 0)
        a[0]_obj = (Tobj)sockFirstMsgPort();
    break;

case order_FirstMenu:
    if (a.size() >= 1)
        a[0]_obj = (Tobj)sockFirstMenu((Tmsgport)a[1]_obj);
    break;

case order_FirstW:
    if (a.size() >= 1)
        a[0]_obj = (Tobj)sockFirstW((Tmsgport)a[1]_obj);
    break;

case order_FirstGroup:
    if (a.size() >= 1)
        a[0]_obj = (Tobj)sockFirstGroup((Tmsgport)a[1]_obj);
    break;

case order_FirstMutex:
    if (a.size() >= 1)
        a[0]_obj = (Tobj)sockFirstMutex((Tmsgport)a[1]_obj);
    break;

case order_FirstMenuItem:
    if (a.size() >= 1)
        a[0]_obj = (Tobj)sockFirstMenuItem((Tmenu   )a[1]_obj);
    break;

case order_FirstGadget:
    if (a.size() >= 1)
        a[0]_obj = (Tobj)sockFirstGadget((Tgroup  )a[1]_obj);
    break;


case order_GetDisplayWidth:
    if (a.size() >= 0)
        a[0]_any = (dat)sockGetDisplayWidth();
    break;

case order_GetDisplayHeight:
    if (a.size() >= 0)
        a[0]_any = (dat)sockGetDisplayHeight();
    break;

case order_GetAll:
    if (a.size() >= 0)
        a[0]_obj = (Tobj)sockGetAll();
    break;


case order_SendToMsgPort:
    if (a.size() >= 3)
        a[0]_any = (byte)sockSendToMsgPort((Tmsgport)a[1]_obj, (udat)a[2]_any, (const byte *)a[3]_vec);
    break;

case order_BlindSendToMsgPort:
    if (a.size() >= 3)
        sockBlindSendToMsgPort((Tmsgport)a[1]_obj, (udat)a[2]_any, (const byte *)a[3]_vec);
    break;


case order_GetOwnerSelection:
    if (a.size() >= 0)
        a[0]_obj = (Tobj)sockGetOwnerSelection();
    break;

case order_SetOwnerSelection:
    if (a.size() >= 2)
        sockSetOwnerSelection((tany)a[1]_any, (tany)a[2]_any);
    break;

case order_RequestSelection:
    if (a.size() >= 2)
        sockRequestSelection((Tobj)a[1]_obj, (uldat)a[2]_any);
    break;

case order_NotifySelection:
    if (a.size() >= 6)
        sockNotifySelection((Tobj)a[1]_obj, (uldat)a[2]_any, (uldat)a[3]_any, (const char *)a[4]_vec, (uldat)a[5]_any, (const char *)a[6]_vec);
    break;


case order_SetServerUid:
    if (a.size() >= 2)
        a[0]_any = (byte)sockSetServerUid((uldat)a[1]_any, (byte)a[2]_any);
    break;


