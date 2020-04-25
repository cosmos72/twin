/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

/*
 * extreg.c -- code for simple-minded internal server extensions bookkeeping
 */

#include "twin.h"
#include "main.h"
#include "remote.h"
#include "extreg.h"
#include "methods.h"

exts Exts = {{NULL},
             {remoteKillSlot},
             {
                 (void (*)(msgport, msg))NoOp,
                 AlwaysTrue,
                 (byte(*)(topaque *, CONST byte **, topaque *, tsfield))AlwaysFalse,
                 (void (*)(uldat, topaque, tsfield))NoOp,
                 (tany(*)(uldat, ...))AlwaysNull,
             },
             {FakeOpenTerm}};
static exts OrigExts = {{NULL},
                        {remoteKillSlot},
                        {
                            (void (*)(msgport, msg))NoOp,
                            AlwaysTrue,
                            (byte(*)(topaque *, CONST byte **, topaque *, tsfield))AlwaysFalse,
                            (void (*)(uldat, topaque, tsfield))NoOp,
                            (tany(*)(uldat, ...))AlwaysNull,
                        },
                        {FakeOpenTerm}};

#define OrigExt(where) ((void **)((byte *)&OrigExts + ((byte *)where - (byte *)&Exts)))

byte Register_Ext(void **where, void *data) {
  if (*where == *OrigExt(where)) {
    *where = data;
    return ttrue;
  }
  return tfalse;
}

byte UnRegister_Ext(void **where, void *data) {
  if (*where == data) {
    *where = *OrigExt(where);
    return ttrue;
  }
  return tfalse;
}
