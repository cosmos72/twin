/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef TWIN_EXTREG_H
#define TWIN_EXTREG_H

#include "stl/span.h"

typedef struct exts exts;

struct exts {
  struct {
    msgport MsgPort;
  } WM;
  struct {
    void (*KillSlot)(uldat slot);
  } Remote;
  struct {
    void (*SendMsg)(msgport MsgPort, msg Msg);
    byte (*InitAuth)(void);
    void (*MultiplexS)(uldat order, Span<s_tsfield> args);
  } Socket;
  struct {
    window (*Open)(const char *arg0, const char *const *argv);
  } Term;
};

extern exts Exts;

byte Register_Ext(void **where, void *data);
byte UnRegister_Ext(void **where, void *data);

#define RegisterExt(kind, member, data) Register_Ext((void **)&(Exts.kind.member), (void *)data)
#define UnRegisterExt(kind, member, data) UnRegister_Ext((void **)&(Exts.kind.member), (void *)data)

#define Ext(kind, member) (Exts.kind.member)

#endif /* TWIN_EXTREG_H */
