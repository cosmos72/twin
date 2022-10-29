/*
 *  msg.cpp  --  define methods of server class Smsg
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "alloc.h" // AllocMem()
#include "log.h"
#include "fn.h"     // Fn_Tmsg
#include "obj/id.h" // AssignId()
#include "obj/msg.h"
#include "stl/fmt.h"

Tmsg Smsg::Create(udat type, size_t eventlen) {
  size_t headerlen = (size_t) & ((Tmsg)NULL)->Event;

  switch (type) {
  case msg_map:
    eventlen += SIZEOF_EVENT_MAP;
    break;
  case msg_display:
    eventlen += SIZEOF_EVENT_DISPLAY;
    break;
  case msg_key:
  case msg_widget_key:
    eventlen += SIZEOF_EVENT_KEYBOARD;
    break;
  case msg_widget_mouse:
  case msg_mouse:
    eventlen += SIZEOF_EVENT_MOUSE;
    break;
  case msg_widget_change:
    eventlen += SIZEOF_EVENT_WIDGET;
    break;
  case msg_widget_gadget:
    eventlen += SIZEOF_EVENT_GADGET;
    break;
  case msg_menu_row:
    eventlen += SIZEOF_EVENT_MENU;
    break;
  case msg_selection:
    eventlen += SIZEOF_EVENT_SELECTION;
    break;
  case msg_selection_notify:
    eventlen += SIZEOF_EVENT_SELECTIONNOTIFY;
    break;
  case msg_selection_request:
    eventlen += SIZEOF_EVENT_SELECTIONREQUEST;
    break;
  case msg_control:
  case msg_user_control:
    eventlen += SIZEOF_EVENT_CONTROL;
    break;
  case msg_user_clientmsg:
    eventlen += SIZEOF_EVENT_CLIENTMSG;
    break;

  case msg_selection_clear:
    eventlen += SIZEOF_EVENT_COMMON;
    break;
  default:
    log(ERROR) << "twin: CreateMsg(): unknown message type 0x" << hex(unsigned(type)) << "("
               << unsigned(type) << ")\n";
    return NULL;
  }
  if (eventlen > ((uldat)-1) - headerlen || eventlen > ((size_t)-1) - headerlen) {
    log(ERROR) << "twin: CreateMsg(): event length too large: " << eventlen << "\n";
    return NULL;
  }
  Tmsg m;
  if ((m = (Tmsg)AllocMem0(headerlen + eventlen))) {
    m->Fn = Fn_Tmsg;
    if (!m->Init(type, (uldat)eventlen)) {
      m->Delete();
      m = NULL;
    }
  }
  return m;
}

Tmsg Smsg::Init(udat type, uldat eventlen) {
  if (AssignId(e_id(Fn->Magic), (Tobj)this)) {
    // this->Prev = this->Next = NULL;
    // this->MsgPort = NULL;
    this->Type = type;
    this->Len = eventlen;
    return this;
  }
  return NULL;
}
