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

#include "twautoconf.h" /* for TW_HAVE_* macros */
#include "compiler.h"
#include "obj/fwd.h"

#include <Tw/datatypes.h>

/* IDs */
#define NOID ((uldat)0)
#define BADID ((uldat)-1)

#define MAXID ((uldat)0x0FFFFFFFul)

byte AssignId(const fn_obj Fn_Obj, obj Obj);
byte AssignId_all(all Obj);
void DropId(obj Obj);
obj Id2Obj(byte i, uldat Id);
#define Obj2Id(o) ((o) ? (o)->Id : NOID)

#endif /* _TWIN_ID_H */
