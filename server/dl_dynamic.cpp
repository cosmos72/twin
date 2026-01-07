/*
 *  dl_dynamic.c  --  load/unload modules
 *
 *  Copyright (C) 1999-2025  by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "dl_helper.h"

#include "dl_common.h"

bool DlOpen(Tmodule m) {
  String name;
  dlhandle Handle = NULL;
  byte (*init_func)(Tmodule);

  if (!dlinit_once()) {
    return false;
  }
  if (m && !m->Handle) {
    if (m->Name) {
      if (!name.format(plugindir, "/", DL_PREFIX, m->Name, DL_SUFFIX)) {
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
    init_func = (byte (*)(Tmodule))dlsym(Handle, "InitModule");
    if (init_func && init_func(m)) {
      m->Handle = (void *)Handle;
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

void DlClose(Tmodule m) {
  if (m && m->Handle) {
    if (m->Name) {
      void (*quit_func)(Tmodule) = (void (*)(Tmodule))dlsym((dlhandle)m->Handle, "QuitModule");
      if (quit_func) {
        quit_func(m);
      }
    }
    dlclose((dlhandle)m->Handle);
    m->Handle = NULL;
  }
}
