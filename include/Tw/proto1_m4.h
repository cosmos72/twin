



/* This file was automatically generated with m4 from m4/proto.m4, do not edit! */

/*
 *  proto1_m4.h  -- single-head compatibility defines for libTw functions prototypes
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */













#define TwFindFunction(a1, a2, a3, a4)		Tw_FindFunction(Tw_DefaultD, a1, a2, a3, a4)

#define TwSync()		Tw_Sync(Tw_DefaultD)

#define TwServerSizeof(a1)		Tw_ServerSizeof(Tw_DefaultD, a1)

#define TwCanCompress()		Tw_CanCompress(Tw_DefaultD)
#define TwDoCompress(a1)		Tw_DoCompress(Tw_DefaultD, a1)

#define TwNeedResizeDisplay()		Tw_NeedResizeDisplay(Tw_DefaultD)

#define TwAttachHW(a1, a2, a3)		Tw_AttachHW(Tw_DefaultD, a1, a2, a3)
#define TwDetachHW(a1, a2)		Tw_DetachHW(Tw_DefaultD, a1, a2)

#define TwSetFontTranslation(a1)		Tw_SetFontTranslation(Tw_DefaultD, a1)
#define TwSetHWFontTranslation(a1)		Tw_SetHWFontTranslation(Tw_DefaultD, a1)

#define TwDeleteObj(a1)		Tw_DeleteObj(Tw_DefaultD, a1)
#define TwChangeField(a1, a2, a3, a4)		Tw_ChangeField(Tw_DefaultD, a1, a2, a3, a4)


#define TwCreateWidget(a1, a2, a3, a4, a5, a6, a7)		Tw_CreateWidget(Tw_DefaultD, a1, a2, a3, a4, a5, a6, a7)
#define TwRecursiveDeleteWidget(a1)		Tw_RecursiveDeleteWidget(Tw_DefaultD, a1)
#define TwMapWidget(a1, a2)		Tw_MapWidget(Tw_DefaultD, a1, a2)
#define TwUnMapWidget(a1)		Tw_UnMapWidget(Tw_DefaultD, a1)
#define TwSetXYWidget(a1, a2, a3)		Tw_SetXYWidget(Tw_DefaultD, a1, a2, a3)
#define TwResizeWidget(a1, a2, a3)		Tw_ResizeWidget(Tw_DefaultD, a1, a2, a3)
#define TwScrollWidget(a1, a2, a3)		Tw_ScrollWidget(Tw_DefaultD, a1, a2, a3)
#define TwDrawWidget(a1, a2, a3, a4, a5, a6, a7, a8)		Tw_DrawWidget(Tw_DefaultD, a1, a2, a3, a4, a5, a6, a7, a8)

#define TwSetVisibleWidget(a1, a2)		Tw_SetVisibleWidget(Tw_DefaultD, a1, a2)

#define TwFocusSubWidget(a1)		Tw_FocusSubWidget(Tw_DefaultD, a1)
#define TwFindWidgetAtWidget(a1, a2, a3)		Tw_FindWidgetAtWidget(Tw_DefaultD, a1, a2, a3)

#define TwRaiseWidget(a1)		Tw_RaiseWidget(Tw_DefaultD, a1)
#define TwLowerWidget(a1)		Tw_LowerWidget(Tw_DefaultD, a1)
#define TwRestackChildrenWidget(a1, a2, a3)		Tw_RestackChildrenWidget(Tw_DefaultD, a1, a2, a3)
#define TwCirculateChildrenWidget(a1, a2)		Tw_CirculateChildrenWidget(Tw_DefaultD, a1, a2)


#define TwCreateGadget(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13)		Tw_CreateGadget(Tw_DefaultD, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13)

#define TwCreateButtonGadget(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)		Tw_CreateButtonGadget(Tw_DefaultD, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)

#define TwWriteTextsGadget(a1, a2, a3, a4, a5, a6, a7)		Tw_WriteTextsGadget(Tw_DefaultD, a1, a2, a3, a4, a5, a6, a7)
#define TwWriteHWFontsGadget(a1, a2, a3, a4, a5, a6, a7)		Tw_WriteHWFontsGadget(Tw_DefaultD, a1, a2, a3, a4, a5, a6, a7)

#define TwCreateWindow(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)		Tw_CreateWindow(Tw_DefaultD, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)
#define TwCreate4MenuWindow(a1)		Tw_Create4MenuWindow(Tw_DefaultD, a1)

#define TwWriteAsciiWindow(a1, a2, a3)		Tw_WriteAsciiWindow(Tw_DefaultD, a1, a2, a3)
#define TwWriteStringWindow(a1, a2, a3)		Tw_WriteStringWindow(Tw_DefaultD, a1, a2, a3)
#define TwWriteHWFontWindow(a1, a2, a3)		Tw_WriteHWFontWindow(Tw_DefaultD, a1, a2, a3)
#define TwWriteHWAttrWindow(a1, a2, a3, a4, a5)		Tw_WriteHWAttrWindow(Tw_DefaultD, a1, a2, a3, a4, a5)

#define TwGotoXYWindow(a1, a2, a3)		Tw_GotoXYWindow(Tw_DefaultD, a1, a2, a3)
#define TwSetTitleWindow(a1, a2, a3)		Tw_SetTitleWindow(Tw_DefaultD, a1, a2, a3)
#define TwSetColTextWindow(a1, a2)		Tw_SetColTextWindow(Tw_DefaultD, a1, a2)
#define TwSetColorsWindow(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)		Tw_SetColorsWindow(Tw_DefaultD, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)
#define TwConfigureWindow(a1, a2, a3, a4, a5, a6, a7, a8)		Tw_ConfigureWindow(Tw_DefaultD, a1, a2, a3, a4, a5, a6, a7, a8)
#define TwFindRowByCodeWindow(a1, a2)		Tw_FindRowByCodeWindow(Tw_DefaultD, a1, a2)

#define TwCreateGroup()		Tw_CreateGroup(Tw_DefaultD)
#define TwInsertGadgetGroup(a1, a2)		Tw_InsertGadgetGroup(Tw_DefaultD, a1, a2)
#define TwRemoveGadgetGroup(a1, a2)		Tw_RemoveGadgetGroup(Tw_DefaultD, a1, a2)

#define TwGetSelectedGadgetGroup(a1)		Tw_GetSelectedGadgetGroup(Tw_DefaultD, a1)
#define TwSetSelectedGadgetGroup(a1, a2)		Tw_SetSelectedGadgetGroup(Tw_DefaultD, a1, a2)

#define TwRaiseRow(a1)		Tw_RaiseRow(Tw_DefaultD, a1)
#define TwLowerRow(a1)		Tw_LowerRow(Tw_DefaultD, a1)
#define TwRestackChildrenRow(a1, a2, a3)		Tw_RestackChildrenRow(Tw_DefaultD, a1, a2, a3) 
#define TwCirculateChildrenRow(a1, a2)		Tw_CirculateChildrenRow(Tw_DefaultD, a1, a2)

#define TwCreate4MenuAny(a1, a2, a3, a4, a5, a6)		Tw_Create4MenuAny(Tw_DefaultD, a1, a2, a3, a4, a5, a6)

#define TwCreate4MenuCommonMenuItem(a1)		Tw_Create4MenuCommonMenuItem(Tw_DefaultD, a1)

#define TwCreateMenu(a1, a2, a3, a4, a5, a6, a7)		Tw_CreateMenu(Tw_DefaultD, a1, a2, a3, a4, a5, a6, a7)
#define TwSetInfoMenu(a1, a2, a3, a4, a5)		Tw_SetInfoMenu(Tw_DefaultD, a1, a2, a3, a4, a5)

#define TwCreateMsgPort(a1, a2)		Tw_CreateMsgPort(Tw_DefaultD, a1, a2)
#define TwFindMsgPort(a1, a2, a3)		Tw_FindMsgPort(Tw_DefaultD, a1, a2, a3)

#define TwBgImageScreen(a1, a2, a3, a4)		Tw_BgImageScreen(Tw_DefaultD, a1, a2, a3, a4)

#define TwPrevObj(a1)		Tw_PrevObj(Tw_DefaultD, a1)
#define TwNextObj(a1)		Tw_NextObj(Tw_DefaultD, a1)
#define TwParentObj(a1)		Tw_ParentObj(Tw_DefaultD, a1)

#define TwFirstScreen()		Tw_FirstScreen(Tw_DefaultD)
#define TwFirstWidget(a1)		Tw_FirstWidget(Tw_DefaultD, a1)
#define TwFirstMsgPort()		Tw_FirstMsgPort(Tw_DefaultD)
#define TwFirstMenu(a1)		Tw_FirstMenu(Tw_DefaultD, a1)
#define TwFirstW(a1)		Tw_FirstW(Tw_DefaultD, a1)
#define TwFirstGroup(a1)		Tw_FirstGroup(Tw_DefaultD, a1)
#define TwFirstMutex(a1)		Tw_FirstMutex(Tw_DefaultD, a1)
#define TwFirstMenuItem(a1)		Tw_FirstMenuItem(Tw_DefaultD, a1)
#define TwFirstGadget(a1)		Tw_FirstGadget(Tw_DefaultD, a1)

#define TwGetDisplayWidth()		Tw_GetDisplayWidth(Tw_DefaultD)
#define TwGetDisplayHeight()		Tw_GetDisplayHeight(Tw_DefaultD)
#define TwGetAll()		Tw_GetAll(Tw_DefaultD)

#define TwSendToMsgPort(a1, a2, a3)		Tw_SendToMsgPort(Tw_DefaultD, a1, a2, a3)
#define TwBlindSendToMsgPort(a1, a2, a3)		Tw_BlindSendToMsgPort(Tw_DefaultD, a1, a2, a3)

#define TwGetOwnerSelection()		Tw_GetOwnerSelection(Tw_DefaultD)
#define TwSetOwnerSelection(a1, a2)		Tw_SetOwnerSelection(Tw_DefaultD, a1, a2)
#define TwRequestSelection(a1, a2)		Tw_RequestSelection(Tw_DefaultD, a1, a2)
#define TwNotifySelection(a1, a2, a3, a4, a5, a6)		Tw_NotifySelection(Tw_DefaultD, a1, a2, a3, a4, a5, a6)

#define TwSetServerUid(a1, a2)		Tw_SetServerUid(Tw_DefaultD, a1, a2)

#define TwOpenExtension(a1, a2)		Tw_OpenExtension(Tw_DefaultD, a1, a2)
#define TwCallBExtension(a1, a2, a3, a4)		Tw_CallBExtension(Tw_DefaultD, a1, a2, a3, a4)
#define TwCloseExtension(a1)		Tw_CloseExtension(Tw_DefaultD, a1)



