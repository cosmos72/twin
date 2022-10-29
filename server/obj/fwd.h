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

typedef struct s_draw_ctx draw_ctx;
typedef struct s_obj_list *obj_list;
typedef struct s_obj_entry *obj_entry;
struct ttydata;
typedef struct s_remotedata remotedata;

typedef struct Sobj *obj;
typedef struct Swidget *Twidget;
typedef struct Sgadget *gadget;
typedef struct Swindow *Twindow;
typedef struct Sscreen *screen;
typedef struct Sgroup *ggroup;
typedef struct Srow *row;
typedef struct Smenuitem *Tmenuitem;
typedef struct Smenu *menu;
typedef struct Smsg *msg;
typedef struct Smsgport *Tmsgport;
typedef struct Smutex *mutex;
typedef struct Smodule *module;
typedef struct Sdisplay *Tdisplay;

typedef struct SobjFn *TobjFn;
typedef struct SwidgetFn *TwidgetFn;
typedef struct SgadgetFn *TgadgetFn;
typedef struct SwindowFn *TwindowFn;
typedef struct SscreenFn *TscreenFn;
typedef struct SgroupFn *TgroupFn;
typedef struct SrowFn *TrowFn;
typedef struct SmenuitemFn *TmenuitemFn;
typedef struct SmenuFn *TmenuFn;
typedef struct SmsgFn *TmsgFn;
typedef struct SmsgportFn *TmsgportFn;
typedef struct SmutexFn *TmutexFn;
typedef struct SmoduleFn *TmoduleFn;
typedef struct SdisplayFn *TdisplayFn;

typedef struct SstructFn TstructFn;
typedef struct Ssetup setup;
typedef struct Sselection *selection;
typedef struct Sall *all;

typedef void (*HookFn)(Twidget);

#endif /* TWIN_FWD_H */
