/*
 *  group.cpp  --  define methods of server class s_group
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "obj/group.h"
#include "obj/msgport.h"
#include "methods.h" // InsertLast()

ggroup s_group::Create(fn_group Fn, msgport MsgPort) {
  ggroup g = NULL;

  if (MsgPort && (g = (ggroup)s_obj::Create((fn_obj)Fn))) {

    g->FirstG = g->LastG = g->SelectG = NULL;
    g->MsgPort = NULL;

    InsertLast(Group, g, MsgPort);
  }
  return g;
}
