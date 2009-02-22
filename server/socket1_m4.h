/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */




/* This file was automatically generated from m4/socket1.m4, do not edit! */










































case order_FindFunction:
    if (N >= 4)
	a[0]_any = (tany)sockFindFunction((byte)a[1]_any, (CONST byte *)a[2]_vec, (byte)a[3]_any, (CONST byte *)a[4]_vec);
    break;



case order_SyncSocket:
    if (N >= 0)
	a[0]_any = (tany)sockSyncSocket();
    break;



case order_ServerSizeof:
    if (N >= 1)
	a[0]_any = (tany)sockServerSizeof((byte)a[1]_any);
    break;



case order_CanCompress:
    if (N >= 0)
	a[0]_any = (tany)sockCanCompress();
    break;


case order_DoCompress:
    if (N >= 1)
	a[0]_any = (tany)sockDoCompress((byte)a[1]_any);
    break;



case order_NeedResizeDisplay:
    if (N >= 0)
	sockNeedResizeDisplay();
    break;



case order_AttachHW:
    if (N >= 3)
	sockAttachHW((uldat)a[1]_any, (CONST byte *)a[2]_vec, (byte)a[3]_any);
    break;


case order_DetachHW:
    if (N >= 2)
	a[0]_any = (tany)sockDetachHW((uldat)a[1]_any, (CONST byte *)a[2]_vec);
    break;



case order_SetFontTranslation:
    if (N >= 1)
	sockSetFontTranslation((CONST byte *)a[1]_vec);
    break;


case order_SetHWFontTranslation:
    if (N >= 1)
	sockSetHWFontTranslation((CONST hwfont *)a[1]_vec);
    break;



case order_DeleteObj:
    if (N >= 1)
	sockDeleteObj((obj)a[1]_obj);
    break;


case order_ChangeFieldObj:
    if (N >= 4)
	Act(ChangeField,(obj)a[1]_obj)((obj)a[1]_obj, (udat)a[2]_any, (uldat)a[3]_any, (uldat)a[4]_any);
    break;



case order_CreateWidget:
    if (N >= 7)
	a[0]_obj = (obj)sockCreateWidget((dat)a[1]_any, (dat)a[2]_any, (uldat)a[3]_any, (uldat)a[4]_any, (dat)a[5]_any, (dat)a[6]_any, (hwattr)a[7]_any);
    break;


case order_RecursiveDeleteWidget:
    if (N >= 1)
	sockRecursiveDeleteWidget((widget)a[1]_obj);
    break;

case order_MapWidget:
    if (N >= 2)
	Act(Map,(widget)a[1]_obj)((widget)a[1]_obj, (widget)a[2]_obj);
    break;

case order_UnMapWidget:
    if (N >= 1)
	Act(UnMap,(widget)a[1]_obj)((widget)a[1]_obj);
    break;

case order_SetXYWidget:
    if (N >= 3)
	sockSetXYWidget((widget)a[1]_obj, (dat)a[2]_any, (dat)a[3]_any);
    break;

case order_ResizeWidget:
    if (N >= 3)
	sockResizeWidget((widget)a[1]_obj, (dat)a[2]_any, (dat)a[3]_any);
    break;

case order_ScrollWidget:
    if (N >= 3)
	sockScrollWidget((widget)a[1]_obj, (ldat)a[2]_any, (ldat)a[3]_any);
    break;

case order_DrawWidget:
    if (N >= 8)
	sockDrawWidget((widget)a[1]_obj, (dat)a[2]_any, (dat)a[3]_any, (dat)a[4]_any, (dat)a[5]_any, (CONST byte *)a[6]_vec, (CONST hwfont *)a[7]_vec, (CONST hwattr *)a[8]_vec);
    break;


case order_SetVisibleWidget:
    if (N >= 2)
	sockSetVisibleWidget((widget)a[1]_obj, (byte)a[2]_any);
    break;


case order_FocusSubWidget:
    if (N >= 1)
	sockFocusSubWidget((widget)a[1]_obj);
    break;

case order_FindWidgetAtWidget:
    if (N >= 3)
	a[0]_obj = (obj)Act(FindWidgetAt,(widget)a[1]_obj)((widget)a[1]_obj, (dat)a[2]_any, (dat)a[3]_any);
    break;


case order_RaiseWidget:
    if (N >= 1)
	Act(Raise,(widget)a[1]_obj)((widget)a[1]_obj);
    break;

case order_LowerWidget:
    if (N >= 1)
	Act(Lower,(widget)a[1]_obj)((widget)a[1]_obj);
    break;

case order_RestackChildrenWidget:
    if (N >= 3)
	sockRestackChildrenWidget((widget)a[1]_obj, (uldat)a[2]_any, (CONST widget *)a[3]_vec);
    break;

case order_CirculateChildrenWidget:
    if (N >= 2)
	sockCirculateChildrenWidget((widget)a[1]_obj, (byte)a[2]_any);
    break;



case order_CreateGadget:
    if (N >= 13)
	a[0]_obj = (obj)sockCreateGadget((widget)a[1]_obj, (dat)a[2]_any, (dat)a[3]_any, (CONST byte *)a[4]_vec, (uldat)a[5]_any, (uldat)a[6]_any, (udat)a[7]_any, (hwcol)a[8]_any, (hwcol)a[9]_any, (hwcol)a[10]_any, (hwcol)a[11]_any, (dat)a[12]_any, (dat)a[13]_any);
    break;


case order_CreateButtonGadget:
    if (N >= 11)
	a[0]_obj = (obj)Do(CreateButton,Gadget)(FnGadget, (widget)a[1]_obj, (dat)a[2]_any, (dat)a[3]_any, (CONST byte *)a[4]_vec, (uldat)a[5]_any, (udat)a[6]_any, (hwcol)a[7]_any, (hwcol)a[8]_any, (hwcol)a[9]_any, (dat)a[10]_any, (dat)a[11]_any);
    break;


case order_WriteTextsGadget:
    if (N >= 7)
	Act(WriteTexts,(gadget)a[1]_obj)((gadget)a[1]_obj, (byte)a[2]_any, (dat)a[3]_any, (dat)a[4]_any, (CONST byte *)a[5]_vec, (dat)a[6]_any, (dat)a[7]_any);
    break;

case order_WriteHWFontsGadget:
    if (N >= 7)
	Act(WriteHWFonts,(gadget)a[1]_obj)((gadget)a[1]_obj, (byte)a[2]_any, (dat)a[3]_any, (dat)a[4]_any, (CONST hwfont *)a[5]_vec, (dat)a[6]_any, (dat)a[7]_any);
    break;


case order_CreateWindow:
    if (N >= 11)
	a[0]_obj = (obj)sockCreateWindow((dat)a[1]_any, (CONST byte *)a[2]_vec, (CONST hwcol *)a[3]_vec, (menu)a[4]_obj, (hwcol)a[5]_any, (uldat)a[6]_any, (uldat)a[7]_any, (uldat)a[8]_any, (dat)a[9]_any, (dat)a[10]_any, (dat)a[11]_any);
    break;

case order_Create4MenuWindow:
    if (N >= 1)
	a[0]_obj = (obj)Do(Create4Menu,Window)(FnWindow, (menu)a[1]_obj);
    break;


case order_WriteAsciiWindow:
    if (N >= 3)
	sockWriteAsciiWindow((window)a[1]_obj, (ldat)a[2]_any, (CONST byte *)a[3]_vec);
    break;

case order_WriteStringWindow:
    if (N >= 3)
	sockWriteStringWindow((window)a[1]_obj, (ldat)a[2]_any, (CONST byte *)a[3]_vec);
    break;

case order_WriteHWFontWindow:
    if (N >= 3)
	sockWriteHWFontWindow((window)a[1]_obj, (ldat)a[2]_any, (CONST hwfont *)a[3]_vec);
    break;

case order_WriteHWAttrWindow:
    if (N >= 5)
	sockWriteHWAttrWindow((window)a[1]_obj, (dat)a[2]_any, (dat)a[3]_any, (ldat)a[4]_any, (CONST hwattr *)a[5]_vec);
    break;


case order_GotoXYWindow:
    if (N >= 3)
	Act(GotoXY,(window)a[1]_obj)((window)a[1]_obj, (ldat)a[2]_any, (ldat)a[3]_any);
    break;

case order_SetTitleWindow:
    if (N >= 3)
	sockSetTitleWindow((window)a[1]_obj, (dat)a[2]_any, (CONST byte *)a[3]_vec);
    break;

case order_SetColTextWindow:
    if (N >= 2)
	Act(SetColText,(window)a[1]_obj)((window)a[1]_obj, (hwcol)a[2]_any);
    break;

case order_SetColorsWindow:
    if (N >= 11)
	Act(SetColors,(window)a[1]_obj)((window)a[1]_obj, (udat)a[2]_any, (hwcol)a[3]_any, (hwcol)a[4]_any, (hwcol)a[5]_any, (hwcol)a[6]_any, (hwcol)a[7]_any, (hwcol)a[8]_any, (hwcol)a[9]_any, (hwcol)a[10]_any, (hwcol)a[11]_any);
    break;

case order_ConfigureWindow:
    if (N >= 8)
	Act(Configure,(window)a[1]_obj)((window)a[1]_obj, (byte)a[2]_any, (dat)a[3]_any, (dat)a[4]_any, (dat)a[5]_any, (dat)a[6]_any, (dat)a[7]_any, (dat)a[8]_any);
    break;

case order_FindRowByCodeWindow:
    if (N >= 2)
	a[0]_obj = (obj)sockFindRowByCodeWindow((window)a[1]_obj, (dat)a[2]_any);
    break;


case order_CreateGroup:
    if (N >= 0)
	a[0]_obj = (obj)sockCreateGroup();
    break;

case order_InsertGadgetGroup:
    if (N >= 2)
	Act(InsertGadget,(group)a[1]_obj)((group)a[1]_obj, (gadget)a[2]_obj);
    break;

case order_RemoveGadgetGroup:
    if (N >= 2)
	Act(RemoveGadget,(group)a[1]_obj)((group)a[1]_obj, (gadget)a[2]_obj);
    break;


case order_GetSelectedGadgetGroup:
    if (N >= 1)
	a[0]_obj = (obj)Act(GetSelectedGadget,(group)a[1]_obj)((group)a[1]_obj);
    break;

case order_SetSelectedGadgetGroup:
    if (N >= 2)
	Act(SetSelectedGadget,(group)a[1]_obj)((group)a[1]_obj, (gadget)a[2]_obj);
    break;


case order_RaiseRow:
    if (N >= 1)
	Act(Raise,(row)a[1]_obj)((row)a[1]_obj);
    break;

case order_LowerRow:
    if (N >= 1)
	Act(Lower,(row)a[1]_obj)((row)a[1]_obj);
    break;

case order_RestackChildrenRow:
    if (N >= 3)
	sockRestackChildrenRow((obj)a[1]_obj, (uldat)a[2]_any, (CONST row *)a[3]_vec);
    break;
 
case order_CirculateChildrenRow:
    if (N >= 2)
	sockCirculateChildrenRow((obj)a[1]_obj, (byte)a[2]_any);
    break;


case order_Create4MenuAny:
    if (N >= 6)
	a[0]_obj = (obj)sockCreate4MenuAny((obj)a[1]_obj, (window)a[2]_obj, (udat)a[3]_any, (byte)a[4]_any, (ldat)a[5]_any, (CONST byte *)a[6]_vec);
    break;


case order_Create4MenuCommonMenuItem:
    if (N >= 1)
	a[0]_any = (tany)Do(Create4MenuCommon,MenuItem)(FnMenuItem, (menu)a[1]_obj);
    break;


case order_CreateMenu:
    if (N >= 7)
	a[0]_obj = (obj)sockCreateMenu((hwcol)a[1]_any, (hwcol)a[2]_any, (hwcol)a[3]_any, (hwcol)a[4]_any, (hwcol)a[5]_any, (hwcol)a[6]_any, (byte)a[7]_any);
    break;

case order_SetInfoMenu:
    if (N >= 5)
	Act(SetInfo,(menu)a[1]_obj)((menu)a[1]_obj, (byte)a[2]_any, (ldat)a[3]_any, (CONST byte *)a[4]_vec, (CONST hwcol *)a[5]_vec);
    break;


case order_CreateMsgPort:
    if (N >= 2)
	a[0]_obj = (obj)sockCreateMsgPort((byte)a[1]_any, (CONST byte *)a[2]_vec);
    break;

case order_FindMsgPort:
    if (N >= 3)
	a[0]_obj = (obj)sockFindMsgPort((msgport)a[1]_obj, (byte)a[2]_any, (CONST byte *)a[3]_vec);
    break;


case order_BgImageScreen:
    if (N >= 4)
	Act(BgImage,(screen)a[1]_obj)((screen)a[1]_obj, (dat)a[2]_any, (dat)a[3]_any, (CONST hwattr *)a[4]_vec);
    break;


case order_PrevObj:
    if (N >= 1)
	a[0]_obj = (obj)sockPrevObj((obj)a[1]_obj);
    break;

case order_NextObj:
    if (N >= 1)
	a[0]_obj = (obj)sockNextObj((obj)a[1]_obj);
    break;

case order_ParentObj:
    if (N >= 1)
	a[0]_obj = (obj)sockParentObj((obj)a[1]_obj);
    break;


case order_FirstScreen:
    if (N >= 0)
	a[0]_obj = (obj)sockFirstScreen();
    break;

case order_FirstWidget:
    if (N >= 1)
	a[0]_obj = (obj)sockFirstWidget((widget )a[1]_obj);
    break;

case order_FirstMsgPort:
    if (N >= 0)
	a[0]_obj = (obj)sockFirstMsgPort();
    break;

case order_FirstMenu:
    if (N >= 1)
	a[0]_obj = (obj)sockFirstMenu((msgport)a[1]_obj);
    break;

case order_FirstW:
    if (N >= 1)
	a[0]_obj = (obj)sockFirstW((msgport)a[1]_obj);
    break;

case order_FirstGroup:
    if (N >= 1)
	a[0]_obj = (obj)sockFirstGroup((msgport)a[1]_obj);
    break;

case order_FirstMutex:
    if (N >= 1)
	a[0]_obj = (obj)sockFirstMutex((msgport)a[1]_obj);
    break;

case order_FirstMenuItem:
    if (N >= 1)
	a[0]_obj = (obj)sockFirstMenuItem((menu   )a[1]_obj);
    break;

case order_FirstGadget:
    if (N >= 1)
	a[0]_obj = (obj)sockFirstGadget((group  )a[1]_obj);
    break;


case order_GetDisplayWidth:
    if (N >= 0)
	a[0]_any = (tany)sockGetDisplayWidth();
    break;

case order_GetDisplayHeight:
    if (N >= 0)
	a[0]_any = (tany)sockGetDisplayHeight();
    break;

case order_GetAll:
    if (N >= 0)
	a[0]_obj = (obj)sockGetAll();
    break;


case order_SendToMsgPort:
    if (N >= 3)
	a[0]_any = (tany)sockSendToMsgPort((msgport)a[1]_obj, (udat)a[2]_any, (CONST byte *)a[3]_vec);
    break;

case order_BlindSendToMsgPort:
    if (N >= 3)
	sockBlindSendToMsgPort((msgport)a[1]_obj, (udat)a[2]_any, (CONST byte *)a[3]_vec);
    break;


case order_GetOwnerSelection:
    if (N >= 0)
	a[0]_obj = (obj)sockGetOwnerSelection();
    break;

case order_SetOwnerSelection:
    if (N >= 2)
	sockSetOwnerSelection((tany)a[1]_any, (tany)a[2]_any);
    break;

case order_RequestSelection:
    if (N >= 2)
	sockRequestSelection((obj)a[1]_obj, (uldat)a[2]_any);
    break;

case order_NotifySelection:
    if (N >= 6)
	sockNotifySelection((obj)a[1]_obj, (uldat)a[2]_any, (uldat)a[3]_any, (CONST byte *)a[4]_vec, (uldat)a[5]_any, (CONST byte *)a[6]_vec);
    break;


case order_SetServerUid:
    if (N >= 2)
	a[0]_any = (tany)sockSetServerUid((uldat)a[1]_any, (byte)a[2]_any);
    break;


case order_OpenExtension:
    if (N >= 2)
	a[0]_obj = (obj)sockOpenExtension((byte)a[1]_any, (CONST byte *)a[2]_vec);
    break;

case order_CallBExtension:
    if (N >= 4)
	a[0]_any = (tany)sockCallBExtension((extension)a[1]_obj, (topaque)a[2]_any, (CONST byte *)a[3]_vec, (CONST byte *)a[4]_vec);
    break;

case order_CloseExtension:
    if (N >= 1)
	sockCloseExtension((extension)a[1]_obj);
    break;




