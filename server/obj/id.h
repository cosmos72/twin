/*
 *  obj.h  --  declare server class s_obj
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef _TWIN_ID_H
#define _TWIN_ID_H

#include "obj/fwd.h"

#include <Tw/datatypes.h>

/* IDs */
enum : uldat {
  NOID = 0,
  BADID = uldat(-1),
  MAXID = uldat(0x0FFFFFFFul),
};

byte AssignId(const fn_obj Fn_Obj, obj Obj);
byte AssignId_all(all Obj);
void DropId(obj Obj);
obj Id2Obj(byte i, uldat Id);

#endif /* _TWIN_ID_H */
