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

#include <dlfcn.h>

#include "twin.h"
#include "methods.h"
#include "data.h"
#include "util.h"
#include "dl.h"
#include "version.h"

static void WrongVer(uldat ver) {
    static byte buf[80];
    sprintf(buf, "version mismatch: module is %d.%d.%d, this twin is " TWIN_VERSION_STR,
	    TWVER_MAJOR(ver), TWVER_MINOR(ver), TWVER_PATCH(ver));
    ErrStr = buf;
}

byte DlOpen(module Module) {
    void *Handle = NULL;
    uldat len0 = LenStr(conf_destdir_lib_twin_modules_), len;
    byte *name;
    byte (*init_dl)(module);
    uldat *version_dl;
    
    if (Module && !Module->Handle && Module->Name && Module->NameLen) {
	/* dlopen(NULL, ...) returns a handle for the main program */
	if (Module->NameLen == 4 && !CmpMem(Module->Name, "main", 4))
	    name = NULL;
	else {
	    len = len0 + Module->NameLen;
	    if ((name = AllocMem(len+1))) {
		CopyMem(conf_destdir_lib_twin_modules_, name, len0);
		CopyMem(Module->Name, name+len0, Module->NameLen);
		name[len] = '\0';
	    } else
		return FALSE;
	}
	Handle = dlopen(name, RTLD_NOW|RTLD_GLOBAL);
	if (name)
	    FreeMem(name);
    }
    if (!Handle) {
	Error(DLERROR);
	ErrStr = dlerror();
	return FALSE;
    }

    if (name) {
	version_dl = dlsym(Handle, "VersionModule");
	if (version_dl && *version_dl == TWIN_VERSION) {
	    init_dl = dlsym(Handle, "InitModule");
	    if (!init_dl || init_dl(Module)) {
		Module->Handle = Handle;
		return TRUE;
	    }
	} else {
	    Error(USERERROR);
	    WrongVer(version_dl ? *version_dl : 0);
	}
	dlclose(Handle);
	return FALSE;
    }
    return TRUE;
}

void DlClose(module Module) {
    void (*quit_dl)(module);
    
    if (Module && Module->Handle) {
	if (Module->NameLen != 4 || CmpMem(Module->Name, "main", 4)) {
	    quit_dl = dlsym(Module->Handle, "QuitModule");
	    if (quit_dl)
		quit_dl(Module);
	}
	dlclose(Module->Handle);
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
	      case WMSo:      M = DlLoadAny(5, "wm.so"); break;
#endif
#ifndef CONF_TERM
	      case TermSo:    M = DlLoadAny(7, "term.so"); break;
#endif
#ifndef CONF_SOCKET
	      case SocketSo:  M = DlLoadAny(9, "socket.so"); break;
#endif
#ifndef CONF_SOCKET_ALIEN
	      case SocketAlienSo:  M = DlLoadAny(14, "socketalien.so"); break;
#endif
#ifndef CONF_WM_RC
	      case RCParseSo: M = DlLoadAny(10, "rcparse.so"); break;
#endif
	      case MainSo:
	      default:        M = DlLoadAny(4, "main"); break;
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
    if (!CmpStr(name, "wm.so"))
	return WMSo;
    if (!CmpStr(name, "term.so"))
	return TermSo;
    if (!CmpStr(name, "socket.so"))
	return SocketSo;
    if (!CmpStr(name, "socketalien.so"))
	return SocketAlienSo;
    if (!CmpStr(name, "rcparse.so"))
	return RCParseSo;
    return MainSo;
}
