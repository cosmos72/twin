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

#if defined(CONF_TERM) || !defined(CONF__MODULES)
# define FakeOpenTerm (void *)AlwaysNull
#else
# include "methods.h"
#endif

exts Exts = {
    { NULL },
    { remoteKillSlot },
    { (void *)NoOp, AlwaysTrue, (void *)AlwaysFalse, (void *)AlwaysNull, (void *)AlwaysNull },
    { FakeOpenTerm }
};
static exts OrigExts = {
    { NULL },
    { remoteKillSlot },
    { (void *)NoOp, AlwaysTrue, (void *)AlwaysFalse, (void *)AlwaysNull, (void *)AlwaysNull },
    { FakeOpenTerm }
};

#define OrigExt(where) ((void **)( (byte *)&OrigExts + ( (byte *)where - (byte *)&Exts)))

byte Register_Ext(void **where, void *data) {
    if (*where == *OrigExt(where)) {
	*where = data;
	return TRUE;
    }
    return FALSE;
}

byte UnRegister_Ext(void **where, void *data) {
    if (*where == data) {
	*where = *OrigExt(where);
	return TRUE;
    }
    return FALSE;
}

