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
#include "obj/window.h" // class Sremotedata
#include "stl/list.h"

#include <Tw/datatypes.h>

class Smsgport : public Sobj {
public:
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
  List<Tmsg> Msgs;
  List<Tmenu> Menus;     /* menus created by this MsgPort */
  List<Twidget> Widgets; /* widgets owned by this MsgPort */
  List<Tgroup> Groups;   /* groups owned by this MsgPort */
  List<Tmutex> Mutexes;  /* mutexes owned by this MsgPort */
  Tdisplay AttachHW;     /* that was attached as told by MsgPort */

  static Tmsgport Create(byte NameLen, const char *Name, tany PauseSec, tany PauseFraction,
                         byte WakeUp, void (*Handler)(Tmsgport));
  Tmsgport Init(byte NameLen, const char *Name, tany PauseSec, tany PauseFraction, byte WakeUp,
                void (*Handler)(Tmsgport));

  /* Tobj */
  virtual void Delete() OVERRIDE;
  virtual void Remove() OVERRIDE;

  /* Tmsgport */
  void Insert(Tall a, Tmsgport prev, Tmsgport next);
};

/* MsgPort->WakeUp: */
#define TIMER_ALWAYS ((byte)1)
#define TIMER_ONCE ((byte)2)

#endif /* TWIN_MSGPORT_H */
