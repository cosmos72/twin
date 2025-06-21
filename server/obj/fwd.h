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

class SobjList;
class SobjEntry;
class ttydata;
class Sremotedata;

class Sobj;
class Swidget;
class Sgadget;
class Swindow;
class Sscreen;
class Sgroup;
class Srow;
class Smenuitem;
class Smenu;
class Smsg;
class Smsgport;
class Smutex;
class Smodule;
class Sdisplay;

class SobjFn;
class SwidgetFn;
class SwindowFn;
class SmsgFn;
class SmsgportFn;
class SmutexFn;
class SmoduleFn;

class Ssetup;
class Sselection;
class Sall;

typedef SobjList *TobjList;
typedef SobjEntry *TobjEntry;
typedef Sremotedata Tremotedata;

typedef Sobj *Tobj;
typedef Swidget *Twidget;
typedef Sgadget *Tgadget;
typedef Swindow *Twindow;
typedef Sscreen *Tscreen;
typedef Sgroup *Tgroup;
typedef Srow *Trow;
typedef Smenuitem *Tmenuitem;
typedef Smenu *Tmenu;
typedef Smsg *Tmsg;
typedef Smsgport *Tmsgport;
typedef Smutex *Tmutex;
typedef Smodule *Tmodule;
typedef Sdisplay *Tdisplay;

typedef SobjFn *TobjFn;
typedef SwidgetFn *TwidgetFn;
typedef SwindowFn *TwindowFn;
typedef SmsgFn *TmsgFn;
typedef SmsgportFn *TmsgportFn;
typedef SmutexFn *TmutexFn;
typedef SmoduleFn *TmoduleFn;

typedef Ssetup *Tsetup;
typedef Sselection *Tselection;
typedef Sall *Tall;

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
