/*
 *  dl.c  --  load/unload modules
 *
 *  Copyright (C) 1999-2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <dlfcn.h>

#include "twin.h"
#include "methods.h"
#include "data.h"
#include "util.h"

#include "dl.h"

#ifdef CONF_DESTDIR
# define TWIN_MODULE_DIR CONF_DESTDIR "/lib/twin/modules/"
#else
# define TWIN_MODULE_DIR "./"
#endif

static byte *twindir = TWIN_MODULE_DIR;

byte DlOpen(module *Module) {
    void *Handle = NULL;
    uldat len0 = LenStr(twindir), len;
    byte *name;
    byte (*init_dl)(module *);
    
    if (Module && !Module->Handle && Module->Name &&
	(len = len0 + Module->NameLen, name = AllocMem(len+1))) {
	
	CopyMem(twindir, name, len0);
	CopyMem(Module->Name, name+len0, Module->NameLen);
	name[len] = '\0';
	Handle = dlopen(name, RTLD_NOW|RTLD_GLOBAL);
	FreeMem(name);
    }
    if (!Handle) {
	Error(DLERROR);
	ErrStr = dlerror();
	return FALSE;
    }
    
    init_dl = dlsym(Handle, "InitModule");
    if (!init_dl || init_dl(Module)) {
	Module->Handle = Handle;
	return TRUE;
    }
    dlclose(Handle);
    return FALSE;
}

void DlClose(module *Module) {
    void (*quit_dl)(module *);
    
    if (Module && Module->Handle) {
	quit_dl = dlsym(Module->Handle, "QuitModule");
	if (quit_dl)
	    quit_dl(Module);
	dlclose(Module->Handle);
	Module->Handle = NULL;
    }
}

module *DlLoadAny(uldat len, byte *name) {
    module *Module;
    if ((Module = Do(Create,Module)(FnModule, len, name))) {
	if (Act(DlOpen,Module)(Module))
	    return Module;
	Delete(Module);
    }
    return (module *)0;
}

static module *So[MAX_So];

module *DlLoad(uldat code) {
    if (code < MAX_So) {
	if (!So[code]) switch (code) {
#ifndef CONF_TERM
	  case TermSo:
	    return So[code] = DlLoadAny(7, "term.so");
#endif
#ifndef CONF_SOCKET
	  case SocketSo:
	    return So[code] = DlLoadAny(9, "socket.so");
#endif
	  default:
	    if (!So[MainSo])
		So[MainSo] = Do(Create,Module)(FnModule, 0, "");
	    return So[MainSo];
	}
	return So[code];
    }
    return (module *)0;
}

void DlUnLoad(uldat code) {
    if (code < MAX_So) {
	if (So[code]) {
	    Delete(So[code]);
	    So[code] = (module *)0;
	}
    }
}

