/*
 *  msg.h  --  declare server class Smsg
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef TWIN_MSGPORT_H
#define TWIN_MSGPORT_H

#include "obj/fwd.h"
#include "obj/window.h" // struct Sremotedata
#include <Tw/datatypes.h>

struct SmsgportFn {
  uldat Magic;
  void (*Insert)(Tmsgport, Tall, Tmsgport Prev, Tmsgport Next);
  void (*Remove)(Tmsgport);
  void (*Delete)(Tmsgport);
  void (*ChangeField)(Tmsgport, udat field, uldat clear_mask, uldat xor_mask);
  /* Tmsgport */
  TobjFn Fn_Obj;
};

struct Smsgport : public Sobj {
  TmsgportFn Fn;
  Tmsgport Prev, Next; /* list in the same All */
  Tall All;
  /* Tmsgport */
  byte WakeUp, NameLen;
  char *Name;
  /* Note : a MsgPort is always woken up if it has pending messages. */
  void (*Handler)(Tmsgport);
  void (*ShutDownHook)(Tmsgport);
  timevalue CallTime, PauseDuration;
  Tremotedata RemoteData;
  Tmsg FirstMsg, LastMsg;
  Tmenu FirstMenu, LastMenu;    /* menus created by this MsgPort */
  Twidget FirstW, LastW;        /* widgets owned by this MsgPort */
  Tgroup FirstGroup, LastGroup; /* groups owned by this MsgPort */
  Tmutex FirstMutex, LastMutex; /* mutexes owned by this MsgPort */
  Tdisplay AttachHW;            /* that was attached as told by MsgPort */

  static Tmsgport Create(byte NameLen, const char *Name, tany PauseSec, tany PauseFraction,
                         byte WakeUp, void (*Handler)(Tmsgport));
  Tmsgport Init(byte NameLen, const char *Name, tany PauseSec, tany PauseFraction, byte WakeUp,
                void (*Handler)(Tmsgport));

  /* Tobj */
  uldat Magic() const {
    return Fn->Magic;
  }
  void Insert(Tall a, Tmsgport prev, Tmsgport next) {
    Fn->Insert(this, a, prev, next);
  }
  void Remove() {
    Fn->Remove(this);
  }
  void Delete() {
    Fn->Delete(this);
  }
  /* Tmsgport */
};

/* MsgPort->WakeUp: */
#define TIMER_ALWAYS ((byte)1)
#define TIMER_ONCE ((byte)2)

#endif /* TWIN_MSGPORT_H */
