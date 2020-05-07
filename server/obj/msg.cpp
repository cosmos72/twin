/*
 *  msg.cpp  --  define methods of server class s_msg
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "alloc.h"  // AllocMem()
#include "fn.h"     // Fn_msg
#include "obj/id.h" // AssignId()
#include "obj/msg.h"

#define Delta ((size_t) & (((msg)0)->Event))

msg s_msg::Create(udat type, udat eventlen) {
  msg m;

  switch (type) {
  case msg_map:
    eventlen += sizeof(event_map);
    break;
  case msg_display:
    eventlen += sizeof(event_display);
    break;
  case msg_key:
  case msg_widget_key:
    eventlen += sizeof(event_keyboard);
    break;
  case msg_widget_mouse:
  case msg_mouse:
    eventlen += sizeof(event_mouse);
    break;
  case msg_widget_change:
    eventlen += sizeof(event_widget);
    break;
  case msg_widget_gadget:
    eventlen += sizeof(event_gadget);
    break;
  case msg_menu_row:
    eventlen += sizeof(event_menu);
    break;
  case msg_selection:
    eventlen += sizeof(event_selection);
    break;
  case msg_selection_notify:
    eventlen += sizeof(event_selectionnotify) - sizeof(uldat);
    break;
  case msg_selection_request:
    eventlen += sizeof(event_selectionrequest);
    break;
  case msg_control:
  case msg_user_control:
    eventlen += sizeof(event_control) - sizeof(uldat);
    break;
  case msg_user_clientmsg:
    eventlen += sizeof(event_clientmsg) - sizeof(uldat);
    break;

  case msg_selection_clear:
    eventlen += sizeof(event_common);
    break;
  default:
    printk("twin: CreateMsg(): internal error: unknown Msg->Type 0x%04x(%d)\n", (int)type,
           (int)type);
    return NULL;
  }

  if ((m = (msg)AllocMem0(eventlen + Delta))) {
    m->Fn = Fn_msg;
    if (!m->Init(type, eventlen)) {
      m->Delete();
      m = NULL;
    }
  }
  return m;
}

#undef Delta

msg s_msg::Init(udat type, udat eventlen) {
  if (AssignId((fn_obj)Fn, (obj)this)) {
    // this->Prev = this->Next = NULL;
    // this->MsgPort = NULL;
    this->Type = type;
    this->Len = eventlen;
    return this;
  }
  return NULL;
}
