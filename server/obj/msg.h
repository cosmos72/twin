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

#ifndef TWIN_MSG_H
#define TWIN_MSG_H

#include "obj/event.h"
#include "obj/fwd.h"
#include "obj/obj.h"

#include <Tw/datatypes.h>

struct SmsgFn {
  uldat Magic;
  void (*Insert)(Tmsg, Tmsgport, Tmsg Prev, Tmsg Next);
  void (*Remove)(Tmsg);
  void (*Delete)(Tmsg);
  void (*ChangeField)(Tmsg, udat field, uldat clear_mask, uldat xor_mask);
  /* Tmsg */
  TobjFn Fn_Obj;
};

struct Smsg : public Sobj {
  TmsgFn Fn;
  Tmsg Prev, Next;
  Tmsgport MsgPort;
  /* Tmsg */
  udat Type; /* See note above */
  uldat Len; /* length of Event, in bytes */
  event_any Event;

  static Tmsg Create(udat type, size_t eventlen);
  Tmsg Init(udat type, uldat eventlen);

  /* Tobj */
  uldat Magic() const {
    return Fn->Magic;
  }
  void Insert(Tmsgport port, Tmsg prev, Tmsg next) {
    Fn->Insert(this, port, prev, next);
  }
  void Remove() {
    Fn->Remove(this);
  }
  void Delete() {
    Fn->Delete(this);
  }
};

#endif /* TWIN_MSG_H */
