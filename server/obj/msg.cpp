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

msg s_msg::Create(udat type, size_t eventlen) {
  size_t headerlen = ((size_t) & (((msg) nullptr)->Event));

  switch (type) {
  case msg_map:
    headerlen += sizeof(event_map);
    break;
  case msg_display:
    headerlen += sizeof(event_display);
    break;
  case msg_key:
  case msg_widget_key:
    headerlen += sizeof(event_keyboard);
    break;
  case msg_widget_mouse:
  case msg_mouse:
    headerlen += sizeof(event_mouse);
    break;
  case msg_widget_change:
    headerlen += sizeof(event_widget);
    break;
  case msg_widget_gadget:
    headerlen += sizeof(event_gadget);
    break;
  case msg_menu_row:
    headerlen += sizeof(event_menu);
    break;
  case msg_selection:
    headerlen += sizeof(event_selection);
    break;
  case msg_selection_notify:
    headerlen += sizeof(event_selectionnotify) - sizeof(uldat);
    break;
  case msg_selection_request:
    headerlen += sizeof(event_selectionrequest);
    break;
  case msg_control:
  case msg_user_control:
    headerlen += sizeof(event_control) - sizeof(uldat);
    break;
  case msg_user_clientmsg:
    headerlen += sizeof(event_clientmsg) - sizeof(uldat);
    break;

  case msg_selection_clear:
    headerlen += sizeof(event_common);
    break;
  default:
    printk("twin: CreateMsg(): unknown message type 0x%04x(%d)\n", (int)type, (int)type);
    return NULL;
  }
  if (eventlen > ((uldat)-1) - headerlen || eventlen > ((size_t)-1) - headerlen) {
    printk("twin: CreateMsg(): event length too large: %lu\n", (unsigned long)eventlen);
    return NULL;
  }
  msg m;
  if ((m = (msg)AllocMem0(eventlen + headerlen))) {
    m->Fn = Fn_msg;
    if (!m->Init(type, (uldat)eventlen)) {
      m->Delete();
      m = NULL;
    }
  }
  return m;
}

msg s_msg::Init(udat type, uldat eventlen) {
  if (AssignId(Fn->Magic, (obj)this)) {
    // this->Prev = this->Next = NULL;
    // this->MsgPort = NULL;
    this->Type = type;
    this->Len = eventlen;
    return this;
  }
  return NULL;
}
