







/* This file was automatically generated with m4 from m4/Tw_common.m4, do not edit! */

/*
 *  common_m4.h  --  common libtw function prototypes
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */



/** check some magic data to ensure client and library have compatible types */
byte Tw_CheckMagic(TW_CONST byte id[]);

/** strip double hyphens from argv long options (i.e. --option -> -option) */
void Tw_MergeHyphensArgv(int argc, char **argv);

/** change Tw_AllocMem(),Tw_ReAllocMem(),Tw_FreeMem() function pointers */
void Tw_ConfigMalloc(void *(*my_malloc)(size_t),
		     void *(*my_realloc)(void *, size_t),
		     void  (*my_free)(void *));

/** pointer to custom malloc() function; use Tw_ConfigMalloc() to change it */
extern void *(*Tw_AllocMem)(size_t);
/** pointer to custom realloc() function; use Tw_ConfigMalloc() to change it */
extern void *(*Tw_ReAllocMem)(void *, size_t);
/** pointer to custom free() function; use Tw_ConfigMalloc() to change it */
extern void  (*Tw_FreeMem)(void *);

/** equivalent to calloc() */
void *Tw_AllocMem0(size_t Size);
/** equivalent to realloc()+memset() */
void *Tw_ReAllocMem0(void * Mem, size_t OldSize, size_t NewSize);

/** custom malloc()+memcpy() function */
extern void *Tw_CloneMem(TW_CONST void *, size_t);
/** custom strdup() function */
extern char *Tw_CloneStr(TW_CONST char *);
/** custom byte-to-unicode conversion + strdup() function */
extern trune *Tw_CloneStr2TRune(TW_CONST char *, size_t);


#define Tw_CopyStr(From,To)		strcpy(To, From)

#define Tw_CopyMem(From, To, Size)	memcpy(To, From, Size)
#define Tw_MoveMem(From, To, Size)	memmove(To, From, Size)



byte Tw_FindFunctionV(tdisplay TwD, va_list vargs);



#define Tw_DropPrivileges() (setegid(getgid()), seteuid(getuid()))
#define Tw_GetPrivileges() seteuid(0)


/** try to enable compression (using zlib); return 1 if success or 0 if failed */
byte Tw_EnableGzip(tdisplay TwD);
/** try to disable compression (using zlib); return 1 if success or 0 if failed */
byte Tw_DisableGzip(tdisplay TwD);

/** return server diagnostic after Tw_AttachHW() */
TW_CONST char * Tw_AttachGetReply(tdisplay TwD, uldat *len);
/** confirm to server it is ok to use newly opened display (used for synchronization) */
void Tw_AttachConfirm(tdisplay TwD);

tmsgport Tw_OwnerWidget(tdisplay TwD, twidget W);
twidget Tw_O_PrevWidget(tdisplay TwD, twidget W);
twidget Tw_O_NextWidget(tdisplay TwD, twidget W);

void Tw_SetFillWidget(tdisplay TwD, twidget W,tcell Fill);
void Tw_Draw2Widget(tdisplay TwD, twidget W,dat XWidth,dat YWidth,dat Left,dat Up,dat Pitch,TW_CONST char *Text,TW_CONST trune *Font,TW_CONST tcell *Attr);

void Tw_DrawTextWidget(tdisplay TwD, twidget W,dat XWidth,dat YWidth,dat Left,dat Up,dat Pitch,TW_CONST char *Text);
void Tw_DrawTRuneWidget(tdisplay TwD, twidget W,dat XWidth,dat YWidth,dat Left,dat Up,dat Pitch,TW_CONST trune *Font);
void Tw_DrawTCellWidget(tdisplay TwD, twidget W,dat XWidth,dat YWidth,dat Left,dat Up,dat Pitch,TW_CONST tcell *Attr);


void Tw_SetPressedGadget(tdisplay TwD, tgadget Gadget,byte on);
byte Tw_IsPressedGadget(tdisplay TwD, tgadget Gadget);
void Tw_SetToggleGadget(tdisplay TwD, tgadget Gadget,byte on);
byte Tw_IsToggleGadget(tdisplay TwD, tgadget Gadget);
void Tw_WriteTextGadget(tdisplay TwD, tgadget Gadget,dat XWidth,dat YWidth,TW_CONST char *Text,dat Left,dat Up);
void Tw_SetTextGadget(tdisplay TwD, tgadget Gadget,dat XWidth,dat YWidth,TW_CONST char *Text,dat Left,dat Up);
void Tw_SetTextsGadget(tdisplay TwD, tgadget Gadget,byte bitmap,dat XWidth,dat YWidth,TW_CONST char *Text,dat Left,dat Up);

tgroup Tw_GroupGadget(tdisplay TwD, tgadget Gadget);
tgadget Tw_G_PrevGadget(tdisplay TwD, tgadget Gadget);
tgadget Tw_G_NextGadget(tdisplay TwD, tgadget Gadget);

void Tw_WriteTRuneGadget(tdisplay TwD, tgadget Gadget,dat XWidth,dat YWidth,TW_CONST trune * TRune,dat Left,dat Up);
void Tw_SetTRuneGadget(tdisplay TwD, tgadget Gadget,dat XWidth,dat YWidth,TW_CONST trune * TRune,dat Left,dat Up);
void Tw_SetTRunesGadget(tdisplay TwD, tgadget Gadget,byte bitmap,dat XWidth,dat YWidth,TW_CONST trune * TRune,dat Left,dat Up);

tmenuitem Tw_Create4MenuRow(tdisplay TwD, twindow Window,udat Code,byte Flags,ldat Len,TW_CONST char *Text);
tmenuitem Tw_Create4MenuMenuItem(tdisplay TwD, tobj Parent,twindow Window,byte Flags,dat Len,TW_CONST char *Name);

tmsg Tw_CreateMsg(tdisplay TwD, uldat Type,uldat Len);
void Tw_DeleteMsg(tdisplay TwD, tmsg Msg);
byte Tw_SendMsg(tdisplay TwD, tmsgport MsgPort,tmsg Msg);
void Tw_BlindSendMsg(tdisplay TwD, tmsgport MsgPort,tmsg Msg);

int Tw_ConnectionFd(tdisplay TwD);

uldat Tw_LibraryVersion(tdisplay TwD);
uldat Tw_ServerVersion(tdisplay TwD);

byte Tw_InPanic(tdisplay TwD);

/** this returns tfalse only after libtw has paniced */
byte Tw_Flush(tdisplay TwD);
/** this returns tfalse only after libtw has paniced */
byte Tw_Sync(tdisplay TwD);

/**
 * try to write to the underlying socket.
 * if not all data could be written,write as much as possible,* keep the rest queued,then return.
 *
 * returns tfalse only after libtw has paniced,* returns ttrue+ttrue if not all data could be written.
 */
byte Tw_TimidFlush(tdisplay TwD);

/**
 * This is the function you must call to check if there are pending Msgs,* i.e. already received from the socket.
 * Since Msgs can be received even during libtw calls,you cannot rely only
 * on select(Tw_ConnectionFd()) to detect incoming messages,as they could
 * be already queued in the library. So you must call either Tw_PendingMsg()
 * or Tw_PeekMsg() below to check for messages.
 * Returns the first pending Msg,without removing it from the queue,* ot (tmsg)0 if no Msgs are pending.
 */
tmsg Tw_PendingMsg(tdisplay TwD);

/**
 * This is the function you must call to check if there are Msgs available,* i.e. either already received or that can be received non-blocking.
 * Returns the first available Msg,without removing it from the queue,* ot (tmsg)0 if no Msgs are available.
 */
tmsg Tw_PeekMsg(tdisplay TwD);

/**
 * This is the function you must call to get a Msg from the server.
 * If Wait is ttrue  and no Msg is available,it waits until a Msg arrives.
 * If Wait is tfalse and no Msg is available,it tries non-blocking
 *                  to receive more Msgs.
 *
 * In both cases,if there is at least a Msgs available it is returned
 * without waiting.
 */
tmsg Tw_ReadMsg(tdisplay TwD, byte Wait);

/**
 * this is just like Tw_ReadMsg(),but returns a Tw_AllocMem()ed copy
 * of the message,to avoid concurrency problems with other threads.
 * You must Tw_FreeMem() it when done!
 */
tmsg Tw_CloneReadMsg(tdisplay TwD, byte Wait);


tlistener Tw_AddKeyboardListener(tdisplay TwD, twidget W,udat Key,udat ShiftFlags,tfn_listener Listener,void *Arg);
tlistener Tw_AddMouseListener(tdisplay TwD, twidget W,udat Code,udat ShiftFlags,tfn_listener Listener,void *Arg);

tlistener Tw_AddControlListener(tdisplay TwD, twidget W,udat Code,tfn_listener Listener,void *Arg);
tlistener Tw_AddClientMsgListener(tdisplay TwD, twidget W,udat Code,tfn_listener Listener,void *Arg);
tlistener Tw_AddDisplayListener(tdisplay TwD, udat Code,tfn_listener Listener,void *Arg);
tlistener Tw_AddWidgetListener(tdisplay TwD, twidget W,udat Code,tfn_listener Listener,void *Arg);
tlistener Tw_AddGadgetListener(tdisplay TwD, twidget W,udat Code,tfn_listener Listener,void *Arg);

tlistener Tw_AddMenuListener(tdisplay TwD, twidget W,tmenu Menu,udat Code,tfn_listener Listener,void *Arg);

tlistener Tw_AddSelectionListener(tdisplay TwD, twidget W,tfn_listener Listener,void *Arg);
tlistener Tw_AddSelectionNotifyListener(tdisplay TwD, tfn_listener Listener,void *Arg);
tlistener Tw_AddSelectionRequestListener(tdisplay TwD, tfn_listener Listener,void *Arg);

/** these five functions are used by Tw/Tw++.h */
void Tw_InsertListener(tdisplay TwD, tlistener L);
void Tw_RemoveListener(tdisplay TwD, tlistener L);
tlistener Tw_CreateListener(tdisplay TwD, udat Type,tevent_any E,tfn_listener Listener,void *Arg);
void Tw_SetTEListener(tdisplay TwD, tlistener L,udat type,tevent_any E);
void Tw_DeleteListener(tdisplay TwD, tlistener L);

void Tw_SetDefaultListener(tdisplay TwD, tfn_default_listener listener,void *arg);

byte Tw_DispatchMsg(tdisplay TwD, tmsg Msg);
byte Tw_MainLoop(tdisplay TwD);
void Tw_ExitMainLoop(tdisplay TwD);


