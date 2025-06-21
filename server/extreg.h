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

#include <Tw/stattypes.h> // s_tsfield

typedef struct exts exts;

struct exts {
  struct {
    Tmsgport MsgPort;
  } WM;
  struct {
    void (*KillSlot)(uldat slot);
  } Remote;
  struct {
    void (*SendMsg)(Tmsgport MsgPort, Tmsg msg);
    bool (*InitAuth)(void);
    void (*MultiplexS)(uldat order, Span<s_tsfield> args);
  } Socket;
  struct {
    Twindow (*Open)(const char *arg0, const char *const *argv);
  } Term;
};

extern exts Exts;
extern const exts OrigExts;

template <class T> byte Register_Ext(T &where, const T &orig_data, T data) {
  if (where == orig_data) {
    where = data;
    return ttrue;
  }
  return tfalse;
}
template <class T> byte UnRegister_Ext(T &where, const T &orig_data, T data) {
  if (where == data) {
    where = orig_data;
    return ttrue;
  }
  return tfalse;
}

#define RegisterExt(kind, member, data) Register_Ext(Exts.kind.member, OrigExts.kind.member, data)
#define UnRegisterExt(kind, member, data)                                                          \
  UnRegister_Ext(Exts.kind.member, OrigExts.kind.member, data)

#define Ext(kind, member) (Exts.kind.member)

#endif /* TWIN_EXTREG_H */
