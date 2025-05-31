
;; This file is `twin/include/scheme/Tw/common_inc.scm' : scheme macros to autogenerate
;;    twin/include/Tw/common_gen.h
;;    twin/include/Tw/common1_gen.h
;; Tell the user about this.

(define (c_comment str)
  (display str)
  (newline))

(define-syntax EASY
  (syntax-rules ()
    ((_ name str) (F_EASY 'name str))))

(define-syntax DECL
  (syntax-rules ()
    ((_ ret name . args) (F_DECL 'ret 'name 'args))))


(c_comment "
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
")

(c_comment "/** check some magic data to ensure client and library have compatible types */")
(EASY CheckMagic "byte Tw_CheckMagic(const byte id[]);")

(c_comment "/** strip double hyphens from argv long options (i.e. --option -> -option) */")
(EASY MergeHyphensArgv "void Tw_MergeHyphensArgv(int argc, char **argv);")

(c_comment "/** change Tw_AllocMem(), Tw_ReAllocMem(), Tw_FreeMem() function pointers */")
(EASY ConfigMalloc "void Tw_ConfigMalloc(
             void *(*my_malloc)(size_t),
		     void *(*my_realloc)(void *, size_t),
		     void  (*my_free)(void *));")

(c_comment "/** pointer to custom malloc() function; use Tw_ConfigMalloc() to change it */")
(EASY AllocMem    "extern void *(*Tw_AllocMem)(size_t);")
(c_comment "/** pointer to custom realloc() function; use Tw_ConfigMalloc() to change it */")
(EASY ReAllocMem  "extern void *(*Tw_ReAllocMem)(void *, size_t);")
(c_comment "/** pointer to custom free() function; use Tw_ConfigMalloc() to change it */")
(EASY FreeMem     "extern void  (*Tw_FreeMem)(void *);")

(c_comment "/** equivalent to calloc() */")
(EASY AllocMem0   "void *Tw_AllocMem0(size_t Size);")
(c_comment "/** equivalent to realloc()+memset() */")
(EASY ReAllocMem0 "void *Tw_ReAllocMem0(void * Mem, size_t OldSize, size_t NewSize);")

(c_comment "/** custom malloc()+memcpy() function */")
(EASY CloneMem "extern void *Tw_CloneMem(const void *, size_t);")
(c_comment "/** custom strdup() function */")
(EASY CloneStr "extern char *Tw_CloneStr(const char *);")
(c_comment "/** custom byte-to-unicode conversion + strdup() function */")
(EASY CloneStr2TRune "extern trune *Tw_CloneStr2TRune(const char *, size_t);")


(EASY CopyStr "#define Tw_CopyStr(From,To)          strcpy(To, From)")
(EASY CopyMem "#define Tw_CopyMem(From, To, Size)    memcpy(To, From, Size)")
(EASY MoveMem "#define Tw_MoveMem(From, To, Size)    memmove(To, From, Size)")


(DECL byte FindFunctionV (va_list vargs))


(EASY DropPrivileges "
#ifdef __ANDROID__
# define Tw_DropPrivileges() 0
#else
# define Tw_DropPrivileges() (setegid(getgid()), seteuid(getuid()))
#endif")

(EASY GetPrivileges  "
#ifdef __ANDROID__
# define Tw_GetPrivileges() 0
#else
# define Tw_GetPrivileges() seteuid(0)
#endif")


(c_comment "/** try to enable compression (using zlib); return 1 if success or 0 if failed */")
(DECL byte EnableGzip)
(c_comment "/** try to disable compression (using zlib); return 1 if success or 0 if failed */")
(DECL byte DisableGzip)

(c_comment "/** return server diagnostic after Tw_AttachHW() */")
(DECL (const char *) AttachGetReply ((uldat *) len))
(c_comment "/** confirm to server it is ok to use newly opened display (used for synchronization) */")
(DECL void AttachConfirm)

(DECL tmsgport OwnerWidget   (twidget W))
(DECL twidget  O_PrevWidget  (twidget W))
(DECL twidget  O_NextWidget  (twidget W))

(DECL void SetFillWidget (twidget W) (tcell Fill))
(DECL void Draw2Widget   (twidget W) (dat XWidth) (dat YWidth) (dat Left) (dat Up) (dat Pitch) ((const char *) Chars) ((const trune *) Runes) ((const tcell *) Cells))

(DECL void DrawTextWidget  (twidget W) (dat XWidth) (dat YWidth) (dat Left) (dat Up) (dat Pitch)  ((const char *)  Chars))
(DECL void DrawTRuneWidget (twidget W) (dat XWidth) (dat YWidth) (dat Left) (dat Up) (dat Pitch)  ((const trune *) Runes))
(DECL void DrawTCellWidget (twidget W) (dat XWidth) (dat YWidth) (dat Left) (dat Up) (dat Pitch)  ((const tcell *) Cells))


(DECL void SetPressedGadget  (tgadget Gadget)  (byte on))
(DECL byte IsPressedGadget   (tgadget Gadget))
(DECL void SetToggleGadget   (tgadget Gadget)  (byte on))
(DECL byte IsToggleGadget    (tgadget Gadget))
(DECL void WriteTextGadget   (tgadget Gadget)               (dat XWidth) (dat YWidth)  ((const char *) Chars) (dat Left) (dat Up))
(DECL void SetTextGadget     (tgadget Gadget)               (dat XWidth) (dat YWidth)  ((const char *) Chars) (dat Left) (dat Up))
(DECL void SetTextsGadget    (tgadget Gadget) (byte bitmap) (dat XWidth) (dat YWidth)  ((const char *) Chars) (dat Left) (dat Up))

(DECL tgroup  GroupGadget     (tgadget Gadget))
(DECL tgadget G_PrevGadget    (tgadget Gadget))
(DECL tgadget G_NextGadget    (tgadget Gadget))

(DECL void WriteTRuneGadget  (tgadget Gadget)               (dat XWidth) (dat YWidth)  ((const trune *) Runes) (dat Left) (dat Up))
(DECL void SetTRuneGadget    (tgadget Gadget)               (dat XWidth) (dat YWidth)  ((const trune *) Tunes) (dat Left) (dat Up))
(DECL void SetTRunesGadget   (tgadget Gadget) (byte bitmap) (dat XWidth) (dat YWidth)  ((const trune *) Tunes) (dat Left) (dat Up))

(DECL tmenuitem Create4MenuRow      (twindow Window) (udat Code)      (byte Flags) (ldat Len)  ((const char *) Chars))
(DECL tmenuitem Create4MenuMenuItem (tobj    Parent) (twindow Window) (byte Flags) (dat Len)   ((const char *) Name))

(DECL tmsg CreateMsg       (uldat Type) (uldat Len))
(DECL void DeleteMsg       (tmsg Msg))
(DECL byte SendMsg         (tmsgport MsgPort) (tmsg Msg))
(DECL void BlindSendMsg    (tmsgport MsgPort) (tmsg Msg))

(DECL int ConnectionFd)

(DECL uldat LibraryVersion)
(DECL uldat ServerVersion)

(DECL byte InPanic)

(c_comment "/** this returns tfalse only after libtw has panicked */")
(DECL byte Flush)
(c_comment "/** this returns tfalse only after libtw has panicked */")
(DECL byte Sync)

(c_comment "
/**
 * try to write to the underlying socket.
 * if not all data could be written, write as much as possible,
 * keep the rest queued, then return.
 *
 * returns tfalse only after libtw has panicked,
 * returns ttrue+ttrue if not all data could be written.
 */")
(DECL byte TimidFlush)

(c_comment "
/**
 * This is the function you must call to check if there are pending Msgs,
 * i.e. already received from the socket.
 * Since Msgs can be received even during libtw calls, you cannot rely only
 * on select(Tw_ConnectionFd()) to detect incoming messages, as they could
 * be already queued in the library. So you must call either Tw_PendingMsg()
 * or Tw_PeekMsg() below to check for messages.
 * Returns the first pending Msg, without removing it from the queue,
 * ot (tmsg)0 if no Msgs are pending.
 */")
(DECL tmsg PendingMsg)

(c_comment "
/**
 * This is the function you must call to check if there are Msgs available,
 * i.e. either already received or that can be received non-blocking.
 * Returns the first available Msg, without removing it from the queue,
 * ot (tmsg)0 if no Msgs are available.
 */")
(DECL tmsg PeekMsg)

(c_comment "
/**
 * This is the function you must call to get a Msg from the server.
 * If Wait is ttrue  and no Msg is available, it waits until a Msg arrives.
 * If Wait is tfalse and no Msg is available, it tries non-blocking
 *                  to receive more Msgs.
 *
 * In both cases, if there is at least a Msgs available it is returned
 * without waiting.
 */")
(DECL tmsg ReadMsg  (byte Wait))

(c_comment "
/**
 * this is just like Tw_ReadMsg(), but returns a Tw_AllocMem()ed copy
 * of the message, to avoid concurrency problems with other threads.
 * You must Tw_FreeMem() it when done!
 */")
(DECL tmsg CloneReadMsg  (byte Wait))


(DECL tlistener AddKeyboardListener   (twidget W)  (udat Key)  (udat ShiftFlags) (tfn_listener Listener) ((void *) Arg))
(DECL tlistener AddMouseListener      (twidget W)  (udat Code) (udat ShiftFlags) (tfn_listener Listener) ((void *) Arg))

(DECL tlistener AddControlListener    (twidget W)  (udat Code) (tfn_listener Listener)  ((void *) Arg))
(DECL tlistener AddClientMsgListener  (twidget W)  (udat Code) (tfn_listener Listener)  ((void *) Arg))
(DECL tlistener AddDisplayListener                 (udat Code) (tfn_listener Listener)  ((void *) Arg))
(DECL tlistener AddWidgetListener     (twidget W)  (udat Code) (tfn_listener Listener)  ((void *) Arg))
(DECL tlistener AddGadgetListener     (twidget W)  (udat Code) (tfn_listener Listener)  ((void *) Arg))

(DECL tlistener AddMenuListener       (twidget W) (tmenu Menu) (udat Code) (tfn_listener Listener) ((void *) Arg))

(DECL tlistener AddSelectionListener  (twidget W) (tfn_listener Listener) ((void *) Arg))
(DECL tlistener AddSelectionNotifyListener        (tfn_listener Listener) ((void *) Arg))
(DECL tlistener AddSelectionRequestListener       (tfn_listener Listener) ((void *) Arg))

(c_comment "/** these five functions are used by Tw/Tw++.h */")
(DECL void      InsertListener  (tlistener L))
(DECL void      RemoveListener  (tlistener L))
(DECL tlistener CreateListener                (udat Type) (tevent_any E) (tfn_listener Listener) ((void *) Arg))
(DECL void      SetTEListener   (tlistener L) (udat type) (tevent_any E))
(DECL void      DeleteListener  (tlistener L))

(DECL void SetDefaultListener   (tfn_default_listener listener)  ((void *) Arg))

(DECL byte DispatchMsg  (tmsg Msg))
(DECL byte MainLoop)
(DECL void ExitMainLoop)
