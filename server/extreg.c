
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

