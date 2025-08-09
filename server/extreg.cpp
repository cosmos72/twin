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
                 (void (*)(Tmsgport, Tmsg))NoOp,
                 AlwaysTrue,
             },
             {FakeOpenTerm}};
const exts OrigExts = {{NULL},
                       {remoteKillSlot},
                       {
                           (void (*)(Tmsgport, Tmsg))NoOp,
                           AlwaysTrue,
                       },
                       {FakeOpenTerm}};
