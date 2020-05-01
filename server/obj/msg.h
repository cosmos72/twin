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

#ifndef _TWIN_MSG_H
#define _TWIN_MSG_H

#include "obj/event.h"

struct s_msg {
  uldat Id;
  fn_msg Fn;
  msg Prev, Next;
  msgport MsgPort;
  /* msg */
  udat Type; /* See note above */
  udat Len;  /* length of Event */
  event_any Event;
};
struct s_fn_msg {
  uldat Magic, Size;
  msg (*Create)(fn_msg, udat Type, udat EventLen);
  void (*Insert)(msg, msgport, msg Prev, msg Next);
  void (*Remove)(msg);
  void (*Delete)(msg);
  void (*ChangeField)(msg, udat field, uldat CLEARMask, uldat XORMask);
  /* msg */
  fn_obj Fn_Obj;
};

#endif /* _TWIN_MSG_H */
