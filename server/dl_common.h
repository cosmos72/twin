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

IdSo DlName2Id(const Chars name) {
  if (name == Chars("socket")) {
    return SocketSo;
  } else if (name == Chars("rcparse")) {
    return RCParseSo;
  } else {
    return MainSo;
  }
}

static Chars DlId2Name(IdSo id) {
  switch (id) {
  case SocketSo:
    return Chars("socket");
  case RCParseSo:
    return Chars("rcparse");
  default:
  case MainSo:
    return Chars();
  }
}

Tmodule DlLoadAny(Chars name) {
  Tmodule m;

  for (m = All->Modules.First; m; m = m->Next) {
    if (name == m->Name) {
      return m; // already loaded!
    }
  }
  if ((m = Smodule::Create(name))) {
    if (m->DlOpen()) {
      return m;
    }
    m->Delete();
  }
  return NULL;
}

static Tmodule So[MAX_So];

Tmodule DlLoad(IdSo id) {
  Tmodule m = NULL;
  if (id >= 0 && id < MAX_So && (m = So[id]) == NULL) {
    const Chars name = DlId2Name(id);
    m = DlLoadAny(name);
    if ((So[id] = m) != NULL) {
      All->HookModule();
    } else {
      log(ERROR) << "failed to load module " << name << ": " //
                 << (Errstr ? Errstr : Chars("unknown error"));
    }
  }
  return m;
}

void DlUnload(IdSo id) {
  if (id >= 0 && id < MAX_So && So[id]) {
    So[id]->Delete();
    So[id] = NULL;
    All->HookModule();
  }
}

Tmodule DlIsLoaded(IdSo id) {
  if (id >= 0 && id < MAX_So) {
    return So[id];
  }
  return (Tmodule)0;
}
