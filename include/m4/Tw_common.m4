




dnl This file is `m4/Twcommon.m4' : m4 macros to autogenerate common`'TARGET`'_m4.h
dnl Tell the user about this.
/* This file was automatically generated with `m4' from `m4/Twcommon.m4', do not edit! */

/*
 *  common`'TARGET`'_m4.h  --  common libTw function prototypes
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

divert(-1)

define(`NSHIFT', `ifelse($1, 0, `shift($@)', `NSHIFT(decr($1), shift(shift($@)))')')

define(`COMMA', `ifelse($1, 2, `', `, ')')

define(`ARG', `a$1')
define(`_ARGS', `ifelse($1, $2, `', `, ARG($1)`'_ARGS(incr($1), $2)')')
define(`ARGS', `ifelse($1, 2, `', $1, 3, `ARG(1)', `ARG(1)`'_ARGS(2, decr($1))')')


ifelse(TARGET, `1', `
  define(`c_doxygen')
  define(`EASY', ``#'define Tw`'$1		Tw_`'$1')
  define(`DECL', ``#'define Tw`'$2`'(ARGS($#))	Tw_`'$2`'(Tw_DefaultD`'COMMA($#)`'ARGS($#))')
', `
  define(`EASY', `$2')`'dnl
  define(`DECL', `$1 Tw_`'$2`'(tdisplay TwD`'COMMA($#)`'NSHIFT(2, $@));')
  define(`c_doxygen', `$@')
')

divert

c_doxygen(/** check some magic data to ensure client and library have compatible types */)
EASY(CheckMagic, byte Tw_CheckMagic(TW_CONST byte id[]);)

c_doxygen(/** strip double hyphens from argv long options (i.e. --option -> -option) */)
EASY(MergeHyphensArgv, void Tw_MergeHyphensArgv(int argc, char **argv);)

c_doxygen(/** change Tw_AllocMem(), Tw_ReAllocMem(), Tw_FreeMem() function pointers */)
EASY(ConfigMalloc, void Tw_ConfigMalloc(void *(*my_malloc)(size_t),
		     void *(*my_realloc)(void *, size_t),
		     void  (*my_free)(void *));)
		     
c_doxygen(/** pointer to custom malloc() function; use Tw_ConfigMalloc() to change it */)
EASY(AllocMem, extern void *(*Tw_AllocMem)(size_t);)
c_doxygen(/** pointer to custom realloc() function; use Tw_ConfigMalloc() to change it */)
EASY(ReAllocMem, extern void *(*Tw_ReAllocMem)(void *, size_t);)
c_doxygen(/** pointer to custom free() function; use Tw_ConfigMalloc() to change it */)
EASY(FreeMem, extern void  (*Tw_FreeMem)(void *);)
c_doxygen(/** custom malloc()+memcpy() function */)
EASY(CloneMem, extern void *Tw_CloneMem(TW_CONST void *, size_t);)
c_doxygen(/** custom strdup() function */)
EASY(CloneStr, extern byte *Tw_CloneStr(TW_CONST byte *);)
c_doxygen(/** custom byte-to-unicode conversion + strdup() function */)
EASY(CloneStr2HWFont, extern hwfont *Tw_CloneStr2HWFont(TW_CONST byte *, size_t);)


EASY(LenStr, `#define Tw_LenStr		strlen')
EASY(CmpStr, `#define Tw_CmpStr		strcmp')
EASY(CopyStr, `#define Tw_CopyStr(From,To)		strcpy(To, From)')

EASY(CopyMem, `#define Tw_CopyMem(From, To, Size)	memcpy(To, From, Size)')
EASY(MoveMem, `#define Tw_MoveMem(From, To, Size)	memmove(To, From, Size)')
EASY(WriteMem, `#define Tw_WriteMem		memset')
EASY(CmpMem, `#define Tw_CmpMem		memcmp')



DECL(byte,FindFunctionV, va_list vargs)



EASY(DropPrivileges, `#define Tw_DropPrivileges() (setegid(getgid()), seteuid(getuid()))')
EASY(GetPrivileges, `#define Tw_GetPrivileges() seteuid(0)')


c_doxygen(/** try to enable compression (using zlib); return 1 if success or 0 if failed */)
DECL(byte,EnableGzip)
c_doxygen(/** try to disable compression (using zlib); return 1 if success or 0 if failed */)
DECL(byte,DisableGzip)

c_doxygen(/** return server diagnostic after Tw_AttachHW() */)
DECL(TW_CONST byte *,AttachGetReply,uldat *len)
c_doxygen(/** confirm to server it is ok to use newly opened display (used for synchronization) */)
DECL(void,AttachConfirm)

DECL(tmsgport,OwnerWidget,   twidget W)
DECL(twidget, O_PrevWidget,  twidget W)
DECL(twidget, O_NextWidget,  twidget W)

DECL(void,SetFillWidget,twidget W, hwattr Fill)
DECL(void,Draw2Widget,  twidget W, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch, TW_CONST byte *Text, TW_CONST hwfont *Font, TW_CONST hwattr *Attr)

DECL(void,DrawTextWidget,  twidget W, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch, TW_CONST byte *Text)
DECL(void,DrawHWFontWidget,twidget W, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch, TW_CONST hwfont *Font)
DECL(void,DrawHWAttrWidget,twidget W, dat XWidth, dat YWidth, dat Left, dat Up, dat Pitch, TW_CONST hwattr *Attr)


DECL(void,SetPressedGadget, tgadget Gadget, byte on)
DECL(byte,IsPressedGadget,  tgadget Gadget)
DECL(void,SetToggleGadget,  tgadget Gadget, byte on)
DECL(byte,IsToggleGadget,   tgadget Gadget)
DECL(void,WriteTextGadget,  tgadget Gadget,              dat XWidth, dat YWidth, TW_CONST byte *Text, dat Left, dat Up)
DECL(void,SetTextGadget,    tgadget Gadget,              dat XWidth, dat YWidth, TW_CONST byte *Text, dat Left, dat Up)
DECL(void,SetTextsGadget,   tgadget Gadget, byte bitmap, dat XWidth, dat YWidth, TW_CONST byte *Text, dat Left, dat Up)

DECL(tgroup, GroupGadget,    tgadget Gadget)
DECL(tgadget,G_PrevGadget,   tgadget Gadget)
DECL(tgadget,G_NextGadget,   tgadget Gadget)

DECL(void,WriteHWFontGadget, tgadget Gadget,              dat XWidth, dat YWidth, TW_CONST hwfont * HWFont, dat Left, dat Up)
DECL(void,SetHWFontGadget,   tgadget Gadget,              dat XWidth, dat YWidth, TW_CONST hwfont * HWFont, dat Left, dat Up)
DECL(void,SetHWFontsGadget,  tgadget Gadget, byte bitmap, dat XWidth, dat YWidth, TW_CONST hwfont * HWFont, dat Left, dat Up)

DECL(tmenuitem,Create4MenuRow, twindow Window, udat Code, byte Flags, ldat Len, TW_CONST byte *Text)
DECL(tmenuitem,Create4MenuMenuItem, tobj Parent, twindow Window, byte Flags, dat Len, TW_CONST byte *Name)

DECL(tmsg,CreateMsg, uldat Type, uldat Len)
DECL(void,DeleteMsg,      tmsg Msg)
DECL(byte,SendMsg,        tmsgport MsgPort, tmsg Msg)
DECL(void,BlindSendMsg,   tmsgport MsgPort, tmsg Msg)

DECL(int,ConnectionFd)

DECL(uldat,LibraryVersion)
DECL(uldat,ServerVersion)

DECL(byte,InPanic)

c_doxygen(/** this returns FALSE only after libTw has paniced */)
DECL(byte,Flush)
c_doxygen(/** this returns FALSE only after libTw has paniced */)
DECL(byte,Sync)

c_doxygen(
/**
 * try to write to the underlying socket.
 * if not all data could be written, write as much as possible,
 * keep the rest queued, then return.
 * 
 * returns FALSE only after libTw has paniced,
 * returns TRUE+TRUE if not all data could be written.
 */)
DECL(byte,TimidFlush)

c_doxygen(
/**
 * This is the function you must call to check if there are pending Msgs,
 * i.e. already received from the socket.
 * Since Msgs can be received even during libTw calls, you cannot rely only
 * on select(Tw_ConnectionFd()) to detect incoming messages, as they could
 * be already queued in the library. So you must call either Tw_PendingMsg()
 * or Tw_PeekMsg() below to check for messages.
 * Returns the first pending Msg, without removing it from the queue,
 * ot (tmsg)0 if no Msgs are pending.
 */)
DECL(tmsg,PendingMsg)

c_doxygen(
/**
 * This is the function you must call to check if there are Msgs available,
 * i.e. either already received or that can be received non-blocking.
 * Returns the first available Msg, without removing it from the queue,
 * ot (tmsg)0 if no Msgs are available.
 */)
DECL(tmsg,PeekMsg)

c_doxygen(
/**
 * This is the function you must call to get a Msg from the server.
 * If Wait is TRUE  and no Msg is available, it waits until a Msg arrives.
 * If Wait is FALSE and no Msg is available, it tries non-blocking
 *                  to receive more Msgs.
 * 
 * In both cases, if there is at least a Msgs available it is returned
 * without waiting.
 */)
DECL(tmsg,ReadMsg, byte Wait)

c_doxygen(
/**
 * this is just like Tw_ReadMsg(), but returns a Tw_AllocMem()ed copy
 * of the message, to avoid concurrency problems with other threads.
 * You must Tw_FreeMem() it when done!
 */)
DECL(tmsg,CloneReadMsg, byte Wait)


DECL(tlistener,AddKeyboardListener,  twidget W, udat Key,  udat ShiftFlags, tfn_listener Listener, void *Arg)
DECL(tlistener,AddMouseListener,     twidget W, udat Code, udat ShiftFlags, tfn_listener Listener, void *Arg)

DECL(tlistener,AddControlListener,   twidget W, udat Code, tfn_listener Listener, void *Arg)
DECL(tlistener,AddClientMsgListener, twidget W, udat Code, tfn_listener Listener, void *Arg)
DECL(tlistener,AddDisplayListener,              udat Code, tfn_listener Listener, void *Arg)
DECL(tlistener,AddWidgetListener,    twidget W, udat Code, tfn_listener Listener, void *Arg)
DECL(tlistener,AddGadgetListener,    twidget W, udat Code, tfn_listener Listener, void *Arg)

DECL(tlistener,AddMenuListener,      twidget W, tmenu Menu, udat Code, tfn_listener Listener, void *Arg)

DECL(tlistener,AddSelectionListener, twidget W, tfn_listener Listener, void *Arg)
DECL(tlistener,AddSelectionNotifyListener,      tfn_listener Listener, void *Arg)
DECL(tlistener,AddSelectionRequestListener,     tfn_listener Listener, void *Arg)

c_doxygen(/** these five functions are used by Tw/Tw++.h */)
DECL(void,     InsertListener, tlistener L)
DECL(void,     RemoveListener, tlistener L)
DECL(tlistener,CreateListener, udat Type, tevent_any E, tfn_listener Listener, void *Arg)
DECL(void,     SetTEListener,  tlistener L, udat type, tevent_any E)
DECL(void,     DeleteListener, tlistener L)

DECL(void,SetDefaultListener, tfn_default_listener listener, void *arg)

DECL(byte,DispatchMsg, tmsg Msg)
DECL(byte,MainLoop)
DECL(void,ExitMainLoop)


DECL(tany, CallAExtension, textension eid, TW_CONST byte *proto, topaque args_n, TW_CONST tany *args)
c_doxygen(/** see Tw_CallLExtension() for the rules about how to pass arguments to this function */)
DECL(tany, CallVExtension, textension eid, TW_CONST byte *proto, topaque args_n, va_list vargs)

