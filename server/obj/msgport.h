/*
 *  msg.h  --  declare server class s_msg
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef _TWIN_MSGPORT_H
#define _TWIN_MSGPORT_H

#include "obj/fwd.h"

struct s_fn_msgport {
  uldat Magic, Size;
  msgport (*Create)(fn_msgport, byte NameLen, const char *Name, tany PauseSec, tany PauseFraction,
                    byte WakeUp, void (*Handler)(msgport));
  void (*Insert)(msgport, all, msgport Prev, msgport Next);
  void (*Remove)(msgport);
  void (*Delete)(msgport);
  void (*ChangeField)(msgport, udat field, uldat CLEARMask, uldat XORMask);
  /* msgport */
  fn_obj Fn_Obj;
  void (*UseExtension)(msgport, extension);
  void (*UnuseExtension)(msgport, extension);
};

struct s_msgport {
  uldat Id;
  fn_msgport Fn;
  msgport Prev, Next; /* list in the same All */
  all All;
  /* msgport */
  byte WakeUp, NameLen;
  char *Name;
  /* Note : a MsgPort is always woken up if it has pending messages. */
  void (*Handler)(msgport);
  void (*ShutDownHook)(msgport);
  timevalue CallTime, PauseDuration;
  remotedata RemoteData;
  msg FirstMsg, LastMsg;
  menu FirstMenu, LastMenu;     /* menus created by this MsgPort */
  widget FirstW, LastW;         /* widgets owned by this MsgPort */
  ggroup FirstGroup, LastGroup; /* groups owned by this MsgPort */
  mutex FirstMutex, LastMutex;  /* mutexes owned by this MsgPort */
  uldat CountE, SizeE;          /* number of extensions used by this MsgPort */
  extension *Es;                /* extensions used by this MsgPort */
  display_hw AttachHW;          /* that was attached as told by MsgPort */

  /* obj */
  uldat Magic() const {
    return Fn->Magic;
  }
  uldat Size() const {
    return Fn->Size;
  }
  void Remove() {
    Fn->Remove(this);
  }
  void Delete() {
    Fn->Delete(this);
  }
};

/* MsgPort->WakeUp: */
#define TIMER_ALWAYS ((byte)1)
#define TIMER_ONCE ((byte)2)

#endif /* _TWIN_MSGPORT_H */
