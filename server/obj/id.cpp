/*
 *  obj.cpp  --  define methods of server class s_obj
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "obj/id.h"
#include "obj/magic.h"
#include "obj/obj.h"
#include "algo.h"  // Max2
#include "alloc.h" // Error(), ReAllocMem0()
#include "twin.h"  // NOSLOT

#include <Tw/Tw_defs.h> // TW_BIGBUFF

/* functions to manage Ids */

static obj *IdList[magic_n];
static uldat IdSize[magic_n], IdTop[magic_n], IdBottom[magic_n];

inline uldat IdListGrow(byte i) {
  uldat oldsize, size;
  obj *newIdList;

  oldsize = IdSize[i];
  if (oldsize >= MAXID || i == obj_magic_id || i == all_magic_id)
    return NOSLOT;

  size = oldsize < TW_SMALLBUFF / 3 ? TW_SMALLBUFF / 2 : oldsize + (oldsize >> 1);
  if (size > MAXID)
    size = MAXID;

  if (!(newIdList = (obj *)ReAllocMem0(IdList[i], sizeof(obj) * oldsize, sizeof(obj) * size)))
    return NOSLOT;

  IdList[i] = newIdList;
  IdSize[i] = size;

  return oldsize;
}

inline void IdListShrink(byte i) {
  obj *newIdList;
  uldat size = Max2(TW_BIGBUFF, IdTop[i] << 1);

  if (size < IdSize[i] &&
      (newIdList = (obj *)ReAllocMem0(IdList[i], sizeof(obj) * IdSize[i], sizeof(obj) * size))) {
    IdList[i] = newIdList;
    IdSize[i] = size;
  }
}

inline uldat IdListGet(byte i) {
  if (IdBottom[i] == IdSize[i])
    return IdListGrow(i);

  return IdBottom[i];
}

obj Id2Obj(byte i, uldat Id) {
  byte I = Id >> magic_shift;

  if (i < magic_n && I < magic_n) {
    /* everything is a valid (obj) */
    /* gadgets, windows, screens are valid (widget) */
    /* menuitems are valid (row) */
    if (i == I || i == obj_magic_id ||
        (i == widget_magic_id &&
         (I == gadget_magic_id || I == window_magic_id || I == screen_magic_id)) ||
        (i == row_magic_id && I == menuitem_magic_id)) {

      Id &= MAXID;
      if (Id < IdTop[I])
        return IdList[I][Id];
    }
  }
  return (obj)0;
}

inline byte _AssignId(byte i, obj o) {
  uldat Id, j;
  if ((Id = IdListGet(i)) != NOSLOT) {
    o->Id = Id | ((uldat)i << magic_shift);
    IdList[i][Id] = o;
    if (IdTop[i] <= Id)
      IdTop[i] = Id + 1;
    for (j = IdBottom[i] + 1; j < IdTop[i]; j++)
      if (!IdList[i][j])
        break;
    IdBottom[i] = j;
    return ttrue;
  }
  Error(NOTABLES);
  return tfalse;
}

inline void _DropId(byte i, obj o) {
  uldat Id = o->Id & MAXID, j;

  if (Id < IdTop[i] && IdList[i][Id] == o /* paranoia */) {
    o->Id = NOID;
    IdList[i][Id] = (obj)0;
    if (IdBottom[i] > Id)
      IdBottom[i] = Id;
    for (j = IdTop[i] - 1; j > IdBottom[i]; j--)
      if (IdList[i][j])
        break;
    IdTop[i] = (j == IdBottom[i]) ? j : j + 1;

    if (IdSize[i] > (IdTop[i] << 4) && IdSize[i] > TW_BIGBUFF)
      IdListShrink(i);
  }
}

byte AssignId(const fn_obj Fn_Obj, obj o) {
  byte i;
  if (o)
    switch (Fn_Obj->Magic) {
    case obj_magic:
      /* 'obj' is an abstract type, you can't create one */
      break;
    case row_magic:
    case module_magic:
    case display_hw_magic:
    case msg_magic:
      /*
       * We don't use Ids for rows and msgs as we expect to create *lots* of them.
       * Remote access to module and display_hw is unsafe too,
       * so no Ids for them too.
       */
      o->Id = Fn_Obj->Magic;
      return ttrue;
    case widget_magic:
    case gadget_magic:
    case window_magic:
    case screen_magic:
    case ggroup_magic:
    case menuitem_magic:
    case menu_magic:
    case msgport_magic:
    case mutex_magic:
    case extension_magic:
      i = Fn_Obj->Magic >> magic_shift;
      return _AssignId(i, o);
    default:
      break;
    }
  return tfalse;
}

void DropId(obj o) {
  obj_entry e = (obj_entry)o;
  byte i = 0;
  if (o && e->Fn)
    switch (e->Fn->Magic) {
    case obj_magic:
      /* 'obj' is just a template type, you can't create one */
      break;
    case row_magic:
    case module_magic:
    case display_hw_magic:
    case msg_magic:
      /* we don't use Ids for rows and msgs as we expect to create *lots* of them */
      /* it's unsafe to allow modules access remotely, so no Ids for them too */
      o->Id = NOID;
      break;
    case widget_magic:
    case gadget_magic:
    case window_magic:
    case screen_magic:
    case ggroup_magic:
    case menuitem_magic:
    case menu_magic:
    case msgport_magic:
    case mutex_magic:
      i = e->Fn->Magic >> magic_shift;
      if (i == (o->Id >> magic_shift))
        _DropId(i, o);
      break;
    default:
      break;
    }
}

static obj IdList_all[1];

byte AssignId_all(all a) {
  byte i = all_magic_id;

  if (!IdList[i]) {
    IdList[i] = IdList_all;
    IdSize[i] = 1;
    return _AssignId(i, (obj)a);
  }
  return tfalse;
}
