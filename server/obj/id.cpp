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

class s_idvec {
public:
  Tobj *Vec;
  uldat Bottom, Top, Size;

  bool assign_id(e_class_byte class_byte, Tobj o);
  void drop_id(e_class_byte class_byte, Tobj o);

private:
  // returns old size, or NOSLOT if resize failed
  uldat grow(e_class_byte class_byte);
  void shrink(e_class_byte class_byte);

  // return first available id, growing if needed
  uldat get(e_class_byte class_byte);
};

static s_idvec IdVec[class_byte_n];

uldat s_idvec::grow(e_class_byte class_byte) {
  uldat old_size = Size;

  if (old_size >= MAXID || class_byte == Tobj_class_byte || class_byte == Tall_class_byte) {
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

void s_idvec::shrink(e_class_byte class_byte) {
  Tobj *new_vec;
  uldat new_size = Max2(TW_BIGBUFF, Top << 1);

  if (new_size < Size &&
      (new_vec = (Tobj *)ReAllocMem0(Vec, sizeof(Tobj) * Size, sizeof(Tobj) * new_size))) {
    Vec = new_vec;
    Size = new_size;
  }
}

uldat s_idvec::get(e_class_byte class_byte) {
  return Bottom < Size ? Bottom : grow(class_byte);
}

bool s_idvec::assign_id(e_class_byte class_byte, Tobj o) {
  const uldat id = get(class_byte);
  if (id != NOSLOT) {
    o->Id = id | ((uldat)class_byte << class_byte_shift);
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

void s_idvec::drop_id(e_class_byte class_byte, Tobj o) {
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
      shrink(class_byte);
  }
}

bool AssignId(const e_id class_id, Tobj o) {
  if (o) {
    e_class_byte class_byte = e_class_byte(class_id >> class_byte_shift);

    switch (class_byte) {
    case Tobj_class_byte:
      /* 'Tobj' is an abstract type, you can't create one */
      break;
    case Trow_class_byte:
    case Tmodule_class_byte:
    case Tdisplay_class_byte:
    case Tmsg_class_byte:
      // We don't use Ids for rows and msgs as we expect to create *lots* of them.
      //
      // Remote access to module and Tdisplay is unsafe, so no Ids for them too.
      o->Id = class_id;
      return true;
    case Twidget_class_byte:
    case Tgadget_class_byte:
    case Twindow_class_byte:
    case Tscreen_class_byte:
    case Tgroup_class_byte:
    case Tmenuitem_class_byte:
    case Tmenu_class_byte:
    case Tmsgport_class_byte:
    case Tmutex_class_byte:
      return IdVec[class_byte].assign_id(class_byte, o);
    default:
      break;
    }
  }
  return false;
}

void DropId(Tobj o) {
  if (o) {
    const e_class_byte class_byte = e_class_byte(o->Id >> class_byte_shift);

    switch (class_byte) {
    case Tobj_class_byte:
      /* 'Tobj' is an abstract class, you can't create one */
      break;
    case Trow_class_byte:
    case Tmodule_class_byte:
    case Tdisplay_class_byte:
    case Tmsg_class_byte:
      /* we don't use Ids for rows and msgs as we expect to create *lots* of them */
      /* it's unsafe to allow modules access remotely, so no Ids for them too */
      o->Id = NOID;
      break;
    case Twidget_class_byte:
    case Tgadget_class_byte:
    case Twindow_class_byte:
    case Tscreen_class_byte:
    case Tgroup_class_byte:
    case Tmenuitem_class_byte:
    case Tmenu_class_byte:
    case Tmsgport_class_byte:
    case Tmutex_class_byte:
      if (class_byte == e_class_byte(o->Id >> class_byte_shift)) {
        IdVec[class_byte].drop_id(class_byte, o);
      }
      break;
    default:
      break;
    }
  }
}

Tobj Id2Obj(e_class_byte expected_magic_byte, uldat id) {
  e_class_byte class_byte = e_class_byte(id >> class_byte_shift);

  if (expected_magic_byte < class_byte_n && class_byte < class_byte_n) {
    /* everything is a valid (Tobj) */
    /* gadgets, windows, screens are valid (widget) */
    /* menuitems are valid (Trow) */
    if (expected_magic_byte == class_byte || expected_magic_byte == Tobj_class_byte ||
        (expected_magic_byte == Twidget_class_byte &&
         (class_byte == Tgadget_class_byte || class_byte == Twindow_class_byte ||
          class_byte == Tscreen_class_byte)) ||
        (expected_magic_byte == Trow_class_byte && class_byte == Tmenuitem_class_byte)) {

      id &= MAXID;
      if (id < IdVec[class_byte].Top) {
        return IdVec[class_byte].Vec[id];
      }
    }
  }
  return NULL;
}

static Tobj IdVec_all[1];

bool AssignId_all(Tall a) {
  const e_class_byte class_byte = Tall_class_byte;
  s_idvec &my = IdVec[class_byte];

  if (!my.Vec) {
    my.Vec = IdVec_all;
    my.Size = 1;
    return my.assign_id(class_byte, a);
  }
  return false;
}
