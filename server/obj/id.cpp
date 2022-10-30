/*
 *  obj.cpp  --  define methods of server class Sobj
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

struct s_idvec {
  Tobj *Vec;
  uldat Bottom, Top, Size;

  bool assign_id(e_magic_byte magic_byte, Tobj o);
  void drop_id(e_magic_byte magic_byte, Tobj o);

private:
  // returns old size, or NOSLOT if resize failed
  uldat grow(e_magic_byte magic_byte);
  void shrink(e_magic_byte magic_byte);

  // return first available id, growing if needed
  uldat get(e_magic_byte magic_byte);
};

static s_idvec IdVec[magic_n];

uldat s_idvec::grow(e_magic_byte magic_byte) {
  uldat old_size = Size;

  if (old_size >= MAXID || magic_byte == Tobj_magic_byte || magic_byte == Tall_magic_byte) {
    // do not assign IDs to Obj and All
    return NOSLOT;
  }
  uldat new_size = old_size < TW_SMALLBUFF / 3 ? TW_SMALLBUFF / 2 : old_size + (old_size >> 1);
  if (new_size > MAXID) {
    new_size = MAXID;
  }
  Tobj *new_vec;
  if (!(new_vec = (Tobj *)ReAllocMem0(Vec, sizeof(Tobj) * old_size, sizeof(Tobj) * new_size))) {
    return NOSLOT;
  }
  Vec = new_vec;
  Size = new_size;
  return old_size;
}

void s_idvec::shrink(e_magic_byte magic_byte) {
  Tobj *new_vec;
  uldat new_size = Max2(TW_BIGBUFF, Top << 1);

  if (new_size < Size &&
      (new_vec = (Tobj *)ReAllocMem0(Vec, sizeof(Tobj) * Size, sizeof(Tobj) * new_size))) {
    Vec = new_vec;
    Size = new_size;
  }
}

uldat s_idvec::get(e_magic_byte magic_byte) {
  return Bottom < Size ? Bottom : grow(magic_byte);
}

bool s_idvec::assign_id(e_magic_byte magic_byte, Tobj o) {
  const uldat id = get(magic_byte);
  if (id != NOSLOT) {
    o->Id = id | ((uldat)magic_byte << magic_shift);
    Vec[id] = o;
    if (Top <= id) {
      Top = id + 1;
    }
    uldat lo;
    for (lo = Bottom + 1; lo < Top; lo++) {
      if (!Vec[lo]) {
        break;
      }
    }
    Bottom = lo;
    return true;
  }
  Error(NOTABLES);
  return false;
}

void s_idvec::drop_id(e_magic_byte magic_byte, Tobj o) {
  uldat id = o->Id & MAXID;

  if (id < Top && Vec[id] == o /* paranoia */) {
    o->Id = NOID;
    Vec[id] = NULL;
    if (Bottom > id) {
      Bottom = id;
    }
    uldat hi;
    for (hi = Top - 1; hi > Bottom; hi--) {
      if (Vec[hi]) {
        break;
      }
    }
    Top = (hi == Bottom) ? hi : hi + 1;

    if (Size > (Top << 4) && Size > TW_BIGBUFF)
      shrink(magic_byte);
  }
}

bool AssignId(const e_id class_magic_id, Tobj o) {
  if (o) {
    const e_magic_byte magic_byte = e_magic_byte(class_magic_id >> magic_shift);
    switch (magic_byte) {
    case Tobj_magic_byte:
      /* 'Tobj' is an abstract type, you can't create one */
      break;
    case Trow_magic_byte:
    case Tmodule_magic_byte:
    case Tdisplay_magic_byte:
    case Tmsg_magic_byte:
      // We don't use Ids for rows and msgs as we expect to create *lots* of them.
      //
      // Remote access to module and Tdisplay is unsafe, so no Ids for them too.
      o->Id = class_magic_id;
      return true;
    case Twidget_magic_byte:
    case Tgadget_magic_byte:
    case Twindow_magic_byte:
    case Tscreen_magic_byte:
    case Tgroup_magic_byte:
    case Tmenuitem_magic_byte:
    case Tmenu_magic_byte:
    case Tmsgport_magic_byte:
    case Tmutex_magic_byte:
      return IdVec[magic_byte].assign_id(magic_byte, o);
    default:
      break;
    }
  }
  return false;
}

void DropId(Tobj o) {
  TobjEntry e = (TobjEntry)o;
  if (o && e->Fn) {
    const e_magic_byte magic_byte = e_magic_byte(e->Fn->Magic >> magic_shift);

    switch (magic_byte) {
    case Tobj_magic_byte:
      /* 'Tobj' is an abstract class, you can't create one */
      break;
    case Trow_magic_byte:
    case Tmodule_magic_byte:
    case Tdisplay_magic_byte:
    case Tmsg_magic_byte:
      /* we don't use Ids for rows and msgs as we expect to create *lots* of them */
      /* it's unsafe to allow modules access remotely, so no Ids for them too */
      o->Id = NOID;
      break;
    case Twidget_magic_byte:
    case Tgadget_magic_byte:
    case Twindow_magic_byte:
    case Tscreen_magic_byte:
    case Tgroup_magic_byte:
    case Tmenuitem_magic_byte:
    case Tmenu_magic_byte:
    case Tmsgport_magic_byte:
    case Tmutex_magic_byte:
      if (magic_byte == e_magic_byte(o->Id >> magic_shift)) {
        IdVec[magic_byte].drop_id(magic_byte, o);
      }
      break;
    default:
      break;
    }
  }
}

Tobj Id2Obj(e_magic_byte expected_magic_byte, uldat id) {
  e_magic_byte magic_byte = e_magic_byte(id >> magic_shift);

  if (expected_magic_byte < magic_n && magic_byte < magic_n) {
    /* everything is a valid (Tobj) */
    /* gadgets, windows, screens are valid (widget) */
    /* menuitems are valid (Trow) */
    if (expected_magic_byte == magic_byte || expected_magic_byte == Tobj_magic_byte ||
        (expected_magic_byte == Twidget_magic_byte &&
         (magic_byte == Tgadget_magic_byte || magic_byte == Twindow_magic_byte ||
          magic_byte == Tscreen_magic_byte)) ||
        (expected_magic_byte == Trow_magic_byte && magic_byte == Tmenuitem_magic_byte)) {

      id &= MAXID;
      if (id < IdVec[magic_byte].Top) {
        return IdVec[magic_byte].Vec[id];
      }
    }
  }
  return NULL;
}

static Tobj IdVec_all[1];

bool AssignId_all(all a) {
  const e_magic_byte magic_byte = Tall_magic_byte;
  s_idvec &my = IdVec[magic_byte];

  if (!my.Vec) {
    my.Vec = IdVec_all;
    my.Size = 1;
    return my.assign_id(magic_byte, a);
  }
  return false;
}
