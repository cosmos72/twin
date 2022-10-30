/*
 *  id.h  --  enums and functions to handle object's ids
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef TWIN_ID_H
#define TWIN_ID_H

#include "obj/fwd.h"
#include "obj/magic.h"

#include <Tw/datatypes.h>

/* IDs */
enum e_id /*: uldat*/ {
  NOID = 0,
  BADID = uldat(-1),
  MAXID = uldat(0x0FFFFFFFul),

  /*SelectionNotify Magic*/
  SEL_APPEND = 0x00000000,
  SEL_URLMAGIC = 0x2F4C5255,
  SEL_UTF8MAGIC = 0x38465455, /* UTF-8 */
  SEL_DATAMAGIC = 0x41544144, /* check MIME if you get this */
  SEL_IDMAGIC = 0x64496449,
  SEL_FILEMAGIC = 0x656C6946,

  obj_magic = 0x0DEAD0B1ul,
  widget_magic = 0x161D9743ul,
  gadget_magic = 0x29867551ul,
  window_magic = 0x31357531ul,
  screen_magic = 0x42659871ul,
  ggroup_magic = 0x5741F326ul,
  row_magic = 0x68074FFAul,
  menuitem_magic = 0x7ABC8FDEul,
  menu_magic = 0x8BAD0BEDul,
  msgport_magic = 0x90981437ul,
  /*
   *   B I G   F A T   WARNING:
   *
   * msg_magic is the magic number for user-created (tmsg) structures,
   * while MSG_MAGIC (defined in socklist_m4.h) is the serial number reserved by
   * libtw to receive server messages (which are still (tmsg) structures).
   */
  msg_magic = 0xA3A61CE4ul, /* this must match tmsg_magic in include/Tw/Tw_defs.h */
  mutex_magic = 0xB0FADED0ul,
  module_magic = 0xCB0F1278ul,
  display_hw_magic = 0xEDBCC609ul,
  all_magic = 0xFA11FA11ul,
};

bool AssignId(const e_id class_magic_id, Tobj Obj);
bool AssignId_all(Tall Obj);
void DropId(Tobj Obj);
Tobj Id2Obj(e_magic_byte expected_magic_byte, uldat Id);

#endif /* TWIN_ID_H */
