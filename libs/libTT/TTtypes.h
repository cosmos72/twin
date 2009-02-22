
/*
 *  TTtypes.h  --  INTERNAL (!!) header for libTT types declarations
 *
 *  Note: you cannot include both this file and <TT/TTtypes.h>, as they conflict!
 *
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 */

#ifndef _TT_TYPES_H
#define _TT_TYPES_H

#include <Tw/Tw_defs.h>
#include <Tw/Twtypes.h>
#include <Tw/Twavl.h>

#include <Tutf/Tutf.h>

#include <TT/osincludes.h>
#include <TT/compiler.h>
#include <TT/version.h>

/* include our internal copy of headers instead of the public ones */
#include <TT/datatypes.h>
#include <TT/datasizes.h>
#include "internaltypes.h"
#include "limits_m4.h"

#include "flags.h"
#include "tree_m4.h"
#include "decls_m4.h"

#endif /* _TT_TYPES_H */

