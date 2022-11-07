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

  Tobj_class_id = 0x0DEAD0B1ul,
  Twidget_class_id = 0x161D9743ul,
  Tgadget_class_id = 0x29867551ul,
  Twindow_class_id = 0x31357531ul,
  Tscreen_class_id = 0x42659871ul,
  Tgroup_class_id = 0x5741F326ul,
  Trow_class_id = 0x68074FFAul,
  Tmenuitem_class_id = 0x7ABC8FDEul,
  Tmenu_class_id = 0x8BAD0BEDul,
  Tmsgport_class_id = 0x90981437ul,
  /*
   *   B I G   F A T   WARNING:
   *
   * Tmsg_class_id is the magic number for user-created (tmsg) structures,
   * while MSG_MAGIC (defined in socklist_m4.h) is the serial number reserved by
   * libtw to receive server messages (which are still (tmsg) structures).
   */
  Tmsg_class_id = 0xA3A61CE4ul, /* this must match tmsg_magic in include/Tw/Tw_defs.h */
  Tmutex_class_id = 0xB0FADED0ul,
  Tmodule_class_id = 0xCB0F1278ul,
  Tdisplay_class_id = 0xEDBCC609ul,
  Tall_class_id = 0xFA11FA11ul,
};

bool AssignId(const e_id class_id, Tobj o);
bool AssignId_all(Tall all);
void DropId(Tobj o);
Tobj Id2Obj(e_class_byte expected_magic_byte, uldat id);

#endif /* TWIN_ID_H */
