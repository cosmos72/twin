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

typedef struct SobjList *TobjList;
typedef struct SobjEntry *TobjEntry;
struct ttydata;
typedef struct Sremotedata Tremotedata;

typedef struct Sobj *Tobj;
typedef struct Swidget *Twidget;
typedef struct Sgadget *Tgadget;
typedef struct Swindow *Twindow;
typedef struct Sscreen *Tscreen;
typedef struct Sgroup *Tgroup;
typedef struct Srow *Trow;
typedef struct Smenuitem *Tmenuitem;
typedef struct Smenu *Tmenu;
typedef struct Smsg *Tmsg;
typedef struct Smsgport *Tmsgport;
typedef struct Smutex *Tmutex;
typedef struct Smodule *Tmodule;
typedef struct Sdisplay *Tdisplay;

typedef struct SobjFn *TobjFn;
typedef struct SwidgetFn *TwidgetFn;
typedef struct SwindowFn *TwindowFn;
typedef struct SmsgFn *TmsgFn;
typedef struct SmsgportFn *TmsgportFn;
typedef struct SmutexFn *TmutexFn;
typedef struct SmoduleFn *TmoduleFn;

typedef struct Ssetup *Tsetup;
typedef struct Sselection *Tselection;
typedef struct Sall *Tall;

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
