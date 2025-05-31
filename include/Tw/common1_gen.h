
/* This file was automatically generated with scheme from `twin/include/scheme/Tw/common_inc.scm', do not edit! */

/**
 *  common libtw function prototypes
 *
 *  Copyright (C) 2002-2025 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 */

/** check some magic data to ensure client and library have compatible types */
#define TwCheckMagic Tw_CheckMagic
/** strip double hyphens from argv long options (i.e. --option -> -option) */
#define TwMergeHyphensArgv Tw_MergeHyphensArgv
/** change Tw_AllocMem(), Tw_ReAllocMem(), Tw_FreeMem() function pointers */
#define TwConfigMalloc Tw_ConfigMalloc
/** pointer to custom malloc() function; use Tw_ConfigMalloc() to change it */
#define TwAllocMem Tw_AllocMem
/** pointer to custom realloc() function; use Tw_ConfigMalloc() to change it */
#define TwReAllocMem Tw_ReAllocMem
/** pointer to custom free() function; use Tw_ConfigMalloc() to change it */
#define TwFreeMem Tw_FreeMem
/** equivalent to calloc() */
#define TwAllocMem0 Tw_AllocMem0
/** equivalent to realloc()+memset() */
#define TwReAllocMem0 Tw_ReAllocMem0
/** custom malloc()+memcpy() function */
#define TwCloneMem Tw_CloneMem
/** custom strdup() function */
#define TwCloneStr Tw_CloneStr
/** custom byte-to-unicode conversion + strdup() function */
#define TwCloneStr2TRune Tw_CloneStr2TRune
#define TwCopyStr Tw_CopyStr
#define TwCopyMem Tw_CopyMem
#define TwMoveMem Tw_MoveMem
#define TwFindFunctionV(vargs)	Tw_FindFunctionV(Tw_DefaultD, vargs)
#define TwDropPrivileges Tw_DropPrivileges
#define TwGetPrivileges Tw_GetPrivileges
/** try to enable compression (using zlib); return 1 if success or 0 if failed */
#define TwEnableGzip()	Tw_EnableGzip(Tw_DefaultD)
/** try to disable compression (using zlib); return 1 if success or 0 if failed */
#define TwDisableGzip()	Tw_DisableGzip(Tw_DefaultD)
/** return server diagnostic after Tw_AttachHW() */
#define TwAttachGetReply(len)	Tw_AttachGetReply(Tw_DefaultD, len)
/** confirm to server it is ok to use newly opened display (used for synchronization) */
#define TwAttachConfirm()	Tw_AttachConfirm(Tw_DefaultD)
#define TwOwnerWidget(W)	Tw_OwnerWidget(Tw_DefaultD, W)
#define TwO_PrevWidget(W)	Tw_O_PrevWidget(Tw_DefaultD, W)
#define TwO_NextWidget(W)	Tw_O_NextWidget(Tw_DefaultD, W)
#define TwSetFillWidget(W, Fill)	Tw_SetFillWidget(Tw_DefaultD, W, Fill)
#define TwDraw2Widget(W, XWidth, YWidth, Left, Up, Pitch, Chars, Runes, Cells)	Tw_Draw2Widget(Tw_DefaultD, W, XWidth, YWidth, Left, Up, Pitch, Chars, Runes, Cells)
#define TwDrawTextWidget(W, XWidth, YWidth, Left, Up, Pitch, Chars)	Tw_DrawTextWidget(Tw_DefaultD, W, XWidth, YWidth, Left, Up, Pitch, Chars)
#define TwDrawTRuneWidget(W, XWidth, YWidth, Left, Up, Pitch, Runes)	Tw_DrawTRuneWidget(Tw_DefaultD, W, XWidth, YWidth, Left, Up, Pitch, Runes)
#define TwDrawTCellWidget(W, XWidth, YWidth, Left, Up, Pitch, Cells)	Tw_DrawTCellWidget(Tw_DefaultD, W, XWidth, YWidth, Left, Up, Pitch, Cells)
#define TwSetPressedGadget(Gadget, on)	Tw_SetPressedGadget(Tw_DefaultD, Gadget, on)
#define TwIsPressedGadget(Gadget)	Tw_IsPressedGadget(Tw_DefaultD, Gadget)
#define TwSetToggleGadget(Gadget, on)	Tw_SetToggleGadget(Tw_DefaultD, Gadget, on)
#define TwIsToggleGadget(Gadget)	Tw_IsToggleGadget(Tw_DefaultD, Gadget)
#define TwWriteTextGadget(Gadget, XWidth, YWidth, Chars, Left, Up)	Tw_WriteTextGadget(Tw_DefaultD, Gadget, XWidth, YWidth, Chars, Left, Up)
#define TwSetTextGadget(Gadget, XWidth, YWidth, Chars, Left, Up)	Tw_SetTextGadget(Tw_DefaultD, Gadget, XWidth, YWidth, Chars, Left, Up)
#define TwSetTextsGadget(Gadget, bitmap, XWidth, YWidth, Chars, Left, Up)	Tw_SetTextsGadget(Tw_DefaultD, Gadget, bitmap, XWidth, YWidth, Chars, Left, Up)
#define TwGroupGadget(Gadget)	Tw_GroupGadget(Tw_DefaultD, Gadget)
#define TwG_PrevGadget(Gadget)	Tw_G_PrevGadget(Tw_DefaultD, Gadget)
#define TwG_NextGadget(Gadget)	Tw_G_NextGadget(Tw_DefaultD, Gadget)
#define TwWriteTRuneGadget(Gadget, XWidth, YWidth, Runes, Left, Up)	Tw_WriteTRuneGadget(Tw_DefaultD, Gadget, XWidth, YWidth, Runes, Left, Up)
#define TwSetTRuneGadget(Gadget, XWidth, YWidth, Tunes, Left, Up)	Tw_SetTRuneGadget(Tw_DefaultD, Gadget, XWidth, YWidth, Tunes, Left, Up)
#define TwSetTRunesGadget(Gadget, bitmap, XWidth, YWidth, Tunes, Left, Up)	Tw_SetTRunesGadget(Tw_DefaultD, Gadget, bitmap, XWidth, YWidth, Tunes, Left, Up)
#define TwCreate4MenuRow(Window, Code, Flags, Len, Chars)	Tw_Create4MenuRow(Tw_DefaultD, Window, Code, Flags, Len, Chars)
#define TwCreate4MenuMenuItem(Parent, Window, Flags, Len, Name)	Tw_Create4MenuMenuItem(Tw_DefaultD, Parent, Window, Flags, Len, Name)
#define TwCreateMsg(Type, Len)	Tw_CreateMsg(Tw_DefaultD, Type, Len)
#define TwDeleteMsg(Msg)	Tw_DeleteMsg(Tw_DefaultD, Msg)
#define TwSendMsg(MsgPort, Msg)	Tw_SendMsg(Tw_DefaultD, MsgPort, Msg)
#define TwBlindSendMsg(MsgPort, Msg)	Tw_BlindSendMsg(Tw_DefaultD, MsgPort, Msg)
#define TwConnectionFd()	Tw_ConnectionFd(Tw_DefaultD)
#define TwLibraryVersion()	Tw_LibraryVersion(Tw_DefaultD)
#define TwServerVersion()	Tw_ServerVersion(Tw_DefaultD)
#define TwInPanic()	Tw_InPanic(Tw_DefaultD)
/** this returns tfalse only after libtw has panicked */
#define TwFlush()	Tw_Flush(Tw_DefaultD)
/** this returns tfalse only after libtw has panicked */
#define TwSync()	Tw_Sync(Tw_DefaultD)

/**
 * try to write to the underlying socket.
 * if not all data could be written, write as much as possible,
 * keep the rest queued, then return.
 *
 * returns tfalse only after libtw has panicked,
 * returns ttrue+ttrue if not all data could be written.
 */
#define TwTimidFlush()	Tw_TimidFlush(Tw_DefaultD)

/**
 * This is the function you must call to check if there are pending Msgs,
 * i.e. already received from the socket.
 * Since Msgs can be received even during libtw calls, you cannot rely only
 * on select(Tw_ConnectionFd()) to detect incoming messages, as they could
 * be already queued in the library. So you must call either Tw_PendingMsg()
 * or Tw_PeekMsg() below to check for messages.
 * Returns the first pending Msg, without removing it from the queue,
 * ot (tmsg)0 if no Msgs are pending.
 */
#define TwPendingMsg()	Tw_PendingMsg(Tw_DefaultD)

/**
 * This is the function you must call to check if there are Msgs available,
 * i.e. either already received or that can be received non-blocking.
 * Returns the first available Msg, without removing it from the queue,
 * ot (tmsg)0 if no Msgs are available.
 */
#define TwPeekMsg()	Tw_PeekMsg(Tw_DefaultD)

/**
 * This is the function you must call to get a Msg from the server.
 * If Wait is ttrue  and no Msg is available, it waits until a Msg arrives.
 * If Wait is tfalse and no Msg is available, it tries non-blocking
 *                  to receive more Msgs.
 *
 * In both cases, if there is at least a Msgs available it is returned
 * without waiting.
 */
#define TwReadMsg(Wait)	Tw_ReadMsg(Tw_DefaultD, Wait)

/**
 * this is just like Tw_ReadMsg(), but returns a Tw_AllocMem()ed copy
 * of the message, to avoid concurrency problems with other threads.
 * You must Tw_FreeMem() it when done!
 */
#define TwCloneReadMsg(Wait)	Tw_CloneReadMsg(Tw_DefaultD, Wait)
#define TwAddKeyboardListener(W, Key, ShiftFlags, Listener, Arg)	Tw_AddKeyboardListener(Tw_DefaultD, W, Key, ShiftFlags, Listener, Arg)
#define TwAddMouseListener(W, Code, ShiftFlags, Listener, Arg)	Tw_AddMouseListener(Tw_DefaultD, W, Code, ShiftFlags, Listener, Arg)
#define TwAddControlListener(W, Code, Listener, Arg)	Tw_AddControlListener(Tw_DefaultD, W, Code, Listener, Arg)
#define TwAddClientMsgListener(W, Code, Listener, Arg)	Tw_AddClientMsgListener(Tw_DefaultD, W, Code, Listener, Arg)
#define TwAddDisplayListener(Code, Listener, Arg)	Tw_AddDisplayListener(Tw_DefaultD, Code, Listener, Arg)
#define TwAddWidgetListener(W, Code, Listener, Arg)	Tw_AddWidgetListener(Tw_DefaultD, W, Code, Listener, Arg)
#define TwAddGadgetListener(W, Code, Listener, Arg)	Tw_AddGadgetListener(Tw_DefaultD, W, Code, Listener, Arg)
#define TwAddMenuListener(W, Menu, Code, Listener, Arg)	Tw_AddMenuListener(Tw_DefaultD, W, Menu, Code, Listener, Arg)
#define TwAddSelectionListener(W, Listener, Arg)	Tw_AddSelectionListener(Tw_DefaultD, W, Listener, Arg)
#define TwAddSelectionNotifyListener(Listener, Arg)	Tw_AddSelectionNotifyListener(Tw_DefaultD, Listener, Arg)
#define TwAddSelectionRequestListener(Listener, Arg)	Tw_AddSelectionRequestListener(Tw_DefaultD, Listener, Arg)
/** these five functions are used by Tw/Tw++.h */
#define TwInsertListener(L)	Tw_InsertListener(Tw_DefaultD, L)
#define TwRemoveListener(L)	Tw_RemoveListener(Tw_DefaultD, L)
#define TwCreateListener(Type, E, Listener, Arg)	Tw_CreateListener(Tw_DefaultD, Type, E, Listener, Arg)
#define TwSetTEListener(L, type, E)	Tw_SetTEListener(Tw_DefaultD, L, type, E)
#define TwDeleteListener(L)	Tw_DeleteListener(Tw_DefaultD, L)
#define TwSetDefaultListener(listener, Arg)	Tw_SetDefaultListener(Tw_DefaultD, listener, Arg)
#define TwDispatchMsg(Msg)	Tw_DispatchMsg(Tw_DefaultD, Msg)
#define TwMainLoop()	Tw_MainLoop(Tw_DefaultD)
#define TwExitMainLoop()	Tw_ExitMainLoop(Tw_DefaultD)
