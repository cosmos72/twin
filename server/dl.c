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
#include "dl.h"
#include "util.h"
#include "printk.h"
#include "version.h"

#include "dl_helper.h"

byte DlOpen(module Module) {
    dlhandle Handle = NULL;
    uldat len, len0 = 1 + LenStr(pkg_libdir) + LenStr(DL_PREFIX) + LenStr(DL_SUFFIX);
    byte *name = NULL;
    byte (*init_func)(module);
    
    if (!dlinit_once()) {
        return FALSE;
    }
        
    if (Module && !Module->Handle && (!Module->NameLen || Module->Name)) {
	/* dlopen(NULL, ...) returns a handle for the main program */
	if (Module->NameLen) {
	    len = len0 + Module->NameLen;
	    if ((name = AllocMem(len+1)))
		sprintf(name, "%s/%s%.*s%s", pkg_libdir, DL_PREFIX, (int)Module->NameLen, Module->Name, DL_SUFFIX);
	    else {
		return FALSE;
	    }
	}
	Handle = dlopen(name);
	if (name)
	    FreeMem(name);
    }
    if (!Handle) {
	Error(DLERROR);
	ErrStr = dlerror();
	return FALSE;
    }

    if (name) {
	init_func = (byte (*)(module)) dlsym(Handle, "InitModule");
	if (init_func && init_func(Module)) {
	    Module->Handle = (void *)Handle;
	    return TRUE;
	}
	dlclose(Handle);

        if (init_func) {
            if (ErrStr == NULL || *ErrStr == '\0') {
                Error(DLERROR);
                ErrStr = "InitModule() failed";
            }
        } else {
            Error(DLERROR);
            ErrStr = "InitModule() not found in module";
        }
	return FALSE;
    }
    return TRUE;
}

void DlClose(module Module) {
    if (Module && Module->Handle) {
	if (Module->NameLen != 0) {
            void (*quit_func)(module) = (void (*)(module)) dlsym((dlhandle)Module->Handle, "QuitModule");
	    if (quit_func)
		quit_func(Module);
	}
	dlclose((dlhandle)Module->Handle);
	Module->Handle = NULL;
    }
}

module DlLoadAny(uldat len, CONST byte *name) {
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

udat DlName2Code(CONST byte *name) {
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

static CONST byte * DlCode2Name(uldat code) {
    switch (code)
    {
    case WMSo:      return "wm";
    case TermSo:    return "term";
    case SocketSo:  return "socket";
    case RCParseSo: return "rcparse";
    default:
    case MainSo:    return NULL;
    }
}



module DlLoad(uldat code) {
    module M = (module)0;
    if (code < MAX_So && !(M = So[code])) {
        CONST byte * name = DlCode2Name(code);
        M = DlLoadAny(name ? LenStr(name) : 0, name);
        if ((So[code] = M)) {
            if (All->FnHookModule)
                All->FnHookModule(All->HookModule);
        } else {
            printk("failed to load module %s: %s\n",
                   name ? name : (CONST byte *)"(NULL)",
                   ErrStr ? ErrStr : (CONST byte *)"unknown error");
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

void *DlSym(module Module, CONST byte *name) {
    if (Module && name)
	return (void *)dlsym((dlhandle)Module->Handle, name);
    
    return NULL;
}

