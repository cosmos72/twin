







/* This file was automatically generated with m4 from m4/Twcommon.m4, do not edit! */

/*
 *  common1_m4.h  --  common libTw function prototypes
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */




#define TwCheckMagic		Tw_CheckMagic


#define TwMergeHyphensArgv		Tw_MergeHyphensArgv


#define TwConfigMalloc		Tw_ConfigMalloc
		     

#define TwAllocMem		Tw_AllocMem

#define TwReAllocMem		Tw_ReAllocMem

#define TwFreeMem		Tw_FreeMem

#define TwCloneMem		Tw_CloneMem

#define TwCloneStr		Tw_CloneStr

#define TwCloneStr2HWFont		Tw_CloneStr2HWFont


#define TwLenStr		Tw_LenStr
#define TwCmpStr		Tw_CmpStr
#define TwCopyStr		Tw_CopyStr

#define TwCopyMem		Tw_CopyMem
#define TwMoveMem		Tw_MoveMem
#define TwWriteMem		Tw_WriteMem
#define TwCmpMem		Tw_CmpMem



#define TwFindFunctionV(a1)	Tw_FindFunctionV(Tw_DefaultD, a1)



#define TwDropPrivileges		Tw_DropPrivileges
#define TwGetPrivileges		Tw_GetPrivileges



#define TwEnableGzip()	Tw_EnableGzip(Tw_DefaultD)

#define TwDisableGzip()	Tw_DisableGzip(Tw_DefaultD)


#define TwAttachGetReply(a1)	Tw_AttachGetReply(Tw_DefaultD, a1)

#define TwAttachConfirm()	Tw_AttachConfirm(Tw_DefaultD)

#define TwOwnerWidget(a1)	Tw_OwnerWidget(Tw_DefaultD, a1)
#define TwO_PrevWidget(a1)	Tw_O_PrevWidget(Tw_DefaultD, a1)
#define TwO_NextWidget(a1)	Tw_O_NextWidget(Tw_DefaultD, a1)

#define TwSetFillWidget(a1, a2)	Tw_SetFillWidget(Tw_DefaultD, a1, a2)
#define TwDraw2Widget(a1, a2, a3, a4, a5, a6, a7, a8, a9)	Tw_Draw2Widget(Tw_DefaultD, a1, a2, a3, a4, a5, a6, a7, a8, a9)

#define TwDrawTextWidget(a1, a2, a3, a4, a5, a6, a7)	Tw_DrawTextWidget(Tw_DefaultD, a1, a2, a3, a4, a5, a6, a7)
#define TwDrawHWFontWidget(a1, a2, a3, a4, a5, a6, a7)	Tw_DrawHWFontWidget(Tw_DefaultD, a1, a2, a3, a4, a5, a6, a7)
#define TwDrawHWAttrWidget(a1, a2, a3, a4, a5, a6, a7)	Tw_DrawHWAttrWidget(Tw_DefaultD, a1, a2, a3, a4, a5, a6, a7)


#define TwSetPressedGadget(a1, a2)	Tw_SetPressedGadget(Tw_DefaultD, a1, a2)
#define TwIsPressedGadget(a1)	Tw_IsPressedGadget(Tw_DefaultD, a1)
#define TwSetToggleGadget(a1, a2)	Tw_SetToggleGadget(Tw_DefaultD, a1, a2)
#define TwIsToggleGadget(a1)	Tw_IsToggleGadget(Tw_DefaultD, a1)
#define TwWriteTextGadget(a1, a2, a3, a4, a5, a6)	Tw_WriteTextGadget(Tw_DefaultD, a1, a2, a3, a4, a5, a6)
#define TwSetTextGadget(a1, a2, a3, a4, a5, a6)	Tw_SetTextGadget(Tw_DefaultD, a1, a2, a3, a4, a5, a6)
#define TwSetTextsGadget(a1, a2, a3, a4, a5, a6, a7)	Tw_SetTextsGadget(Tw_DefaultD, a1, a2, a3, a4, a5, a6, a7)

#define TwGroupGadget(a1)	Tw_GroupGadget(Tw_DefaultD, a1)
#define TwG_PrevGadget(a1)	Tw_G_PrevGadget(Tw_DefaultD, a1)
#define TwG_NextGadget(a1)	Tw_G_NextGadget(Tw_DefaultD, a1)

#define TwWriteHWFontGadget(a1, a2, a3, a4, a5, a6)	Tw_WriteHWFontGadget(Tw_DefaultD, a1, a2, a3, a4, a5, a6)
#define TwSetHWFontGadget(a1, a2, a3, a4, a5, a6)	Tw_SetHWFontGadget(Tw_DefaultD, a1, a2, a3, a4, a5, a6)
#define TwSetHWFontsGadget(a1, a2, a3, a4, a5, a6, a7)	Tw_SetHWFontsGadget(Tw_DefaultD, a1, a2, a3, a4, a5, a6, a7)

#define TwCreate4MenuRow(a1, a2, a3, a4, a5)	Tw_Create4MenuRow(Tw_DefaultD, a1, a2, a3, a4, a5)
#define TwCreate4MenuMenuItem(a1, a2, a3, a4, a5)	Tw_Create4MenuMenuItem(Tw_DefaultD, a1, a2, a3, a4, a5)

#define TwCreateMsg(a1, a2)	Tw_CreateMsg(Tw_DefaultD, a1, a2)
#define TwDeleteMsg(a1)	Tw_DeleteMsg(Tw_DefaultD, a1)
#define TwSendMsg(a1, a2)	Tw_SendMsg(Tw_DefaultD, a1, a2)
#define TwBlindSendMsg(a1, a2)	Tw_BlindSendMsg(Tw_DefaultD, a1, a2)

#define TwConnectionFd()	Tw_ConnectionFd(Tw_DefaultD)

#define TwLibraryVersion()	Tw_LibraryVersion(Tw_DefaultD)
#define TwServerVersion()	Tw_ServerVersion(Tw_DefaultD)

#define TwInPanic()	Tw_InPanic(Tw_DefaultD)


#define TwFlush()	Tw_Flush(Tw_DefaultD)

#define TwSync()	Tw_Sync(Tw_DefaultD)


#define TwTimidFlush()	Tw_TimidFlush(Tw_DefaultD)


#define TwPendingMsg()	Tw_PendingMsg(Tw_DefaultD)


#define TwPeekMsg()	Tw_PeekMsg(Tw_DefaultD)


#define TwReadMsg(a1)	Tw_ReadMsg(Tw_DefaultD, a1)


#define TwCloneReadMsg(a1)	Tw_CloneReadMsg(Tw_DefaultD, a1)


#define TwAddKeyboardListener(a1, a2, a3, a4, a5)	Tw_AddKeyboardListener(Tw_DefaultD, a1, a2, a3, a4, a5)
#define TwAddMouseListener(a1, a2, a3, a4, a5)	Tw_AddMouseListener(Tw_DefaultD, a1, a2, a3, a4, a5)

#define TwAddControlListener(a1, a2, a3, a4)	Tw_AddControlListener(Tw_DefaultD, a1, a2, a3, a4)
#define TwAddClientMsgListener(a1, a2, a3, a4)	Tw_AddClientMsgListener(Tw_DefaultD, a1, a2, a3, a4)
#define TwAddDisplayListener(a1, a2, a3)	Tw_AddDisplayListener(Tw_DefaultD, a1, a2, a3)
#define TwAddWidgetListener(a1, a2, a3, a4)	Tw_AddWidgetListener(Tw_DefaultD, a1, a2, a3, a4)
#define TwAddGadgetListener(a1, a2, a3, a4)	Tw_AddGadgetListener(Tw_DefaultD, a1, a2, a3, a4)

#define TwAddMenuListener(a1, a2, a3, a4, a5)	Tw_AddMenuListener(Tw_DefaultD, a1, a2, a3, a4, a5)

#define TwAddSelectionListener(a1, a2, a3)	Tw_AddSelectionListener(Tw_DefaultD, a1, a2, a3)
#define TwAddSelectionNotifyListener(a1, a2)	Tw_AddSelectionNotifyListener(Tw_DefaultD, a1, a2)
#define TwAddSelectionRequestListener(a1, a2)	Tw_AddSelectionRequestListener(Tw_DefaultD, a1, a2)


#define TwInsertListener(a1)	Tw_InsertListener(Tw_DefaultD, a1)
#define TwRemoveListener(a1)	Tw_RemoveListener(Tw_DefaultD, a1)
#define TwCreateListener(a1, a2, a3, a4)	Tw_CreateListener(Tw_DefaultD, a1, a2, a3, a4)
#define TwSetTEListener(a1, a2, a3)	Tw_SetTEListener(Tw_DefaultD, a1, a2, a3)
#define TwDeleteListener(a1)	Tw_DeleteListener(Tw_DefaultD, a1)

#define TwSetDefaultListener(a1, a2)	Tw_SetDefaultListener(Tw_DefaultD, a1, a2)

#define TwDispatchMsg(a1)	Tw_DispatchMsg(Tw_DefaultD, a1)
#define TwMainLoop()	Tw_MainLoop(Tw_DefaultD)
#define TwExitMainLoop()	Tw_ExitMainLoop(Tw_DefaultD)


#define TwCallAExtension(a1, a2, a3, a4)	Tw_CallAExtension(Tw_DefaultD, a1, a2, a3, a4)

#define TwCallVExtension(a1, a2, a3, a4)	Tw_CallVExtension(Tw_DefaultD, a1, a2, a3, a4)



