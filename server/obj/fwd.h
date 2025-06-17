/*
 *  fwd.h  --  forward declaration of server class Sobj and subclasses
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef TWIN_FWD_H
#define TWIN_FWD_H

typedef class SobjList *TobjList;
typedef class SobjEntry *TobjEntry;
struct ttydata;
typedef class Sremotedata Tremotedata;

typedef class Sobj *Tobj;
typedef class Swidget *Twidget;
typedef class Sgadget *Tgadget;
typedef class Swindow *Twindow;
typedef class Sscreen *Tscreen;
typedef class Sgroup *Tgroup;
typedef class Srow *Trow;
typedef class Smenuitem *Tmenuitem;
typedef class Smenu *Tmenu;
typedef class Smsg *Tmsg;
typedef class Smsgport *Tmsgport;
typedef class Smutex *Tmutex;
typedef class Smodule *Tmodule;
typedef class Sdisplay *Tdisplay;

typedef class SobjFn *TobjFn;
typedef class SwidgetFn *TwidgetFn;
typedef class SwindowFn *TwindowFn;
typedef class SmsgFn *TmsgFn;
typedef class SmsgportFn *TmsgportFn;
typedef class SmutexFn *TmutexFn;
typedef class SmoduleFn *TmoduleFn;

typedef class Ssetup *Tsetup;
typedef class Sselection *Tselection;
typedef class Sall *Tall;

typedef void (*HookFn)(Twidget);

struct HookData {
  HookFn Fn;
  Twidget W;

  void operator()() const {
    if (Fn && W) {
      Fn(W);
    }
  }
};

#endif /* TWIN_FWD_H */
