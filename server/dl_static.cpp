/*
 *  dl_static.c  --  pretend to load/unload modules
 *
 *  Copyright (C) 1999-2026 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "dl_helper.h"
#include "dl_common.h"
#include "hw/hw_all.h"
#include "socket.h"
#include "rcproto.h"

bool DlOpen(Tmodule m) {
  bool ok = false;
  switch (DlName2Id(m->Name)) {
#ifdef TWIN_SOCKET_STATIC
  case SocketSo:
    ok = InitSocket();
    break;
#endif
#ifdef TWIN_RCPARSE_STATIC
  case RCParseSo:
    m->DoInit = &rcload;
    ok = true;
    break;
#endif
  default:
    ok = DlOpenHW(m);
  }
  if (!ok && !Errstr) {
    Error(DLERROR);
    Errstr = "InitModule() failed";
  }
  return ok;
}

void DlClose(Tmodule m) {
  switch (DlName2Id(m->Name)) {
#ifdef TWIN_SOCKET_STATIC
  case SocketSo:
    QuitSocket();
    break;
#endif
  default:
    break;
  }
}
