/*
 *  dl.c  --  load/unload modules
 *
 *  Copyright (C) 1999-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twin.h"
#include "methods.h"
#include "data.h"
#include "util.h"
#include "dl.h"
#include "version.h"

#if defined(HAVE_DLFCN_H) && defined(HAVE_DLOPEN)
# include <dlfcn.h>
# define my(fn) fn
# define my_handle void *
# define my_dlopen_extra_args , RTLD_NOW|RTLD_GLOBAL
# define my_VERSION ".so." TWIN_VERSION_STR

#elif defined(HAVE_LTDL_H) && defined(HAVE_LT_DLOPEN)

# include <ltdl.h>
# define my(fn) lt_##fn
# define my_handle lt_dlhandle
# define my_dlopen_extra_args
# define my_VERSION ".la"

#else
# error nor dlopen() nor lt_dlopen() module loading API available!  
#endif


byte DlOpen(module Module) {
    my_handle Handle = NULL;
    uldat len0 = LenStr(conf_destdir_lib_twin_modules_) + LenStr(my_VERSION), len;
    byte *name = NULL;
    byte (*init_dl)(module);
    
    if (Module && !Module->Handle && (!Module->NameLen || Module->Name)) {
	/* dlopen(NULL, ...) returns a handle for the main program */
	if (Module->NameLen) {
	    len = len0 + Module->NameLen;
	    if ((name = AllocMem(len+1)))
		sprintf(name, "%s%.*s%s", conf_destdir_lib_twin_modules_, (int)Module->NameLen, Module->Name, my_VERSION);
	    else {
		Error(NOMEMORY);
		return FALSE;
	    }
	}
	Handle = my(dlopen)(name my_dlopen_extra_args);
	if (name)
	    FreeMem(name);
    }
    if (!Handle) {
	Error(DLERROR);
	ErrStr = my(dlerror)();
	return FALSE;
    }

    if (name) {
	init_dl = (byte (*)(module)) my(dlsym)(Handle, "InitModule");
	if (!init_dl || init_dl(Module)) {
	    Module->Handle = (void *)Handle;
	    return TRUE;
	}
	my(dlclose)(Handle);
	return FALSE;
    }
    return TRUE;
}

void DlClose(module Module) {
    void (*quit_dl)(module);
    
    if (Module && Module->Handle) {
	if (Module->NameLen != 0) {
	    quit_dl = (void (*)(module)) my(dlsym)((my_handle)Module->Handle, "QuitModule");
	    if (quit_dl)
		quit_dl(Module);
	}
	my(dlclose)((my_handle)Module->Handle);
	Module->Handle = NULL;
    }
}

module DlLoadAny(uldat len, byte *name) {
    module Module;
    
    for (Module = All->FirstModule; Module; Module = Module->Next) {
	if (len == Module->NameLen && !CmpMem(name, Module->Name, len))
	    /* already loaded! */
	    return Module;
    }
    
    if ((Module = Do(Create,Module)(FnModule, len, name))) {
	if (Act(DlOpen,Module)(Module))
	    return Module;
	Delete(Module);
    }
    return (module)0;
}

static module So[MAX_So];

module DlLoad(uldat code) {
    module M;
    if (code < MAX_So) {
	if (!(M = So[code])) {
	    switch (code) {
#ifndef CONF_WM
	      case WMSo:      M = DlLoadAny(6, "wm"); break;
#endif
#ifndef CONF_TERM
	      case TermSo:    M = DlLoadAny(8, "term"); break;
#endif
#ifndef CONF_SOCKET
	      case SocketSo:  M = DlLoadAny(10, "socket"); break;
#endif
#ifndef CONF_WM_RC
	      case RCParseSo: M = DlLoadAny(11, "rcparse"); break;
#endif
	      case MainSo:
	      default:        M = DlLoadAny(0, NULL); break;
	    }
	    if ((So[code] = M)) {
		if (All->FnHookModule)
		    All->FnHookModule(All->HookModule);
	    }
	}
    }
    return M;
}

void DlUnLoad(uldat code) {
    if (code < MAX_So) {
	if (So[code]) {
	    Delete(So[code]);
	    So[code] = (module)0;
	    if (All->FnHookModule)
		All->FnHookModule(All->HookModule);
	}
    }
}

module DlIsLoaded(uldat code) {
    if (code < MAX_So)
	return So[code];
    return (module)0;
}

udat DlName2Code(byte *name) {
    if (!CmpStr(name, "wm"))
	return WMSo;
    if (!CmpStr(name, "term"))
	return TermSo;
    if (!CmpStr(name, "socket"))
	return SocketSo;
    if (!CmpStr(name, "rcparse"))
	return RCParseSo;
    return MainSo;
}


void *DlSym(module Module, CONST byte *name) {
    if (Module && name)
	return (void *)my(dlsym)((my_handle)Module->Handle, name);
    
    return NULL;
}

