
#include "twin.h"
#include "main.h"
#include "remote.h"
#include "extensions.h"

#if defined(CONF_TERM) || !defined(CONF__MODULES)
# define FakeOpenTerm (window (*)(CONST byte *arg0, byte * CONST * argv))AlwaysNull
#else
# include "methods.h"
#endif

exts Exts = {
    { NULL },
    { remoteKillSlot },
    { (void *)NoOp },
    { FakeOpenTerm }
};
static exts OrigExts = {
    { NULL },
    { remoteKillSlot },
    { (void *)NoOp },
    { FakeOpenTerm }
};

#define OrigExt(where) ((void **)( (byte *)&OrigExts + ( (byte *)where - (byte *)&Exts)))

byte RegisterExt(void **where, void *data) {
    if (*where == *OrigExt(where)) {
	*where = data;
	return TRUE;
    }
    return FALSE;
}

byte UnRegisterExt(void **where, void *data) {
    if (*where == data) {
	*where = *OrigExt(where);
	return TRUE;
    }
    return FALSE;
}

