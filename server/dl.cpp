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
#include "alloc.h"
#include "methods.h"
#include "data.h"
#include "dl.h"
#include "log.h"
#include "util.h"
#include "version.h"

#include "dl_helper.h"

bool DlOpen(Tmodule Module) {
  String name;
  dlhandle Handle = NULL;
  byte (*init_func)(Tmodule);

  if (!dlinit_once()) {
    return false;
  }
  if (Module && !Module->Handle) {
    if (Module->Name) {
      if (!name.format(plugindir, "/", DL_PREFIX, Module->Name, DL_SUFFIX)) {
        return false;
      }
    } else {
      /* dlopen(NULL, ...) returns a handle for the main program */
    }
    Handle = dlopen(name.data());
  }
  if (!Handle) {
    Error(DLERROR);
    return false;
  }

  if (name) {
    init_func = (byte(*)(Tmodule))dlsym(Handle, "InitModule");
    if (init_func && init_func(Module)) {
      Module->Handle = (void *)Handle;
      return true;
    }
    dlclose(Handle);

    if (init_func) {
      if (!Errstr) {
        Error(DLERROR);
        Errstr = "InitModule() failed";
      }
    } else {
      Error(DLERROR);
      Errstr = "InitModule() not found in Tmodule";
    }
    return false;
  }
  return true;
}

void DlClose(Tmodule Module) {
  if (Module && Module->Handle) {
    if (Module->Name) {
      void (*quit_func)(Tmodule) = (void (*)(Tmodule))dlsym((dlhandle)Module->Handle, "QuitModule");
      if (quit_func)
        quit_func(Module);
    }
    dlclose((dlhandle)Module->Handle);
    Module->Handle = NULL;
  }
}

Tmodule DlLoadAny(Chars name) {
  Tmodule Module;

  for (Module = All->Modules.First; Module; Module = Module->Next) {
    if (name == Module->Name) {
      return Module; // already loaded!
    }
  }
  if ((Module = Smodule::Create(name))) {
    if (Module->DlOpen()) {
      return Module;
    }
    Module->Delete();
  }
  return NULL;
}

static Tmodule So[MAX_So];

IdSo DlName2Code(const Chars name) {
  if (name == Chars("socket")) {
    return SocketSo;
  } else if (name == Chars("rcparse")) {
    return RCParseSo;
  } else {
    return MainSo;
  }
}

static Chars DlCode2Name(IdSo code) {
  switch (code) {
  case SocketSo:
    return Chars("socket");
  case RCParseSo:
    return Chars("rcparse");
  default:
  case MainSo:
    return Chars();
  }
}

Tmodule DlLoad(IdSo code) {
  Tmodule m = NULL;
  if (code >= 0 && code < MAX_So && !(m = So[code])) {
    const Chars name = DlCode2Name(code);
    m = DlLoadAny(name);
    if ((So[code] = m)) {
      All->HookModule();
    } else {
      log(ERROR) << "failed to load module " << name << ": " //
                 << (Errstr ? Errstr : Chars("unknown error"));
    }
  }
  return m;
}

void DlUnload(IdSo code) {
  if (code >= 0 && code < MAX_So && So[code]) {
    So[code]->Delete();
    So[code] = NULL;
    All->HookModule();
  }
}

Tmodule DlIsLoaded(IdSo code) {
  if (code >= 0 && code < MAX_So) {
    return So[code];
  }
  return (Tmodule)0;
}

void *DlSym(Tmodule Module, const char *name) {
  if (Module && name) {
    return (void *)dlsym((dlhandle)Module->Handle, name);
  }
  return NULL;
}
