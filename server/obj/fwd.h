/*
 *  fwd.h  --  forward declaration of server class s_obj and subclasses
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
typedef struct s_ttydata ttydata;
typedef struct s_remotedata remotedata;

typedef struct s_obj *obj;
typedef struct s_widget *widget;
typedef struct s_gadget *gadget;
typedef struct s_window *window;
typedef struct s_screen *screen;
typedef struct s_group *ggroup;
typedef struct s_row *row;
typedef struct s_menuitem *menuitem;
typedef struct s_menu *menu;
typedef struct s_msg *msg;
typedef struct s_msgport *msgport;
typedef struct s_mutex *mutex;
typedef struct s_module *module;
typedef struct s_display_hw *display_hw;

typedef struct s_fn_obj *fn_obj;
typedef struct s_fn_widget *fn_widget;
typedef struct s_fn_gadget *fn_gadget;
typedef struct s_fn_window *fn_window;
typedef struct s_fn_screen *fn_screen;
typedef struct s_fn_group *fn_group;
typedef struct s_fn_row *fn_row;
typedef struct s_fn_menuitem *fn_menuitem;
typedef struct s_fn_menu *fn_menu;
typedef struct s_fn_msg *fn_msg;
typedef struct s_fn_msgport *fn_msgport;
typedef struct s_fn_mutex *fn_mutex;
typedef struct s_fn_module *fn_module;
typedef struct s_fn_display_hw *fn_display_hw;

typedef struct s_fn_struct fn_struct;
typedef struct s_setup setup;
typedef struct s_all *all;

typedef void (*fn_hook)(widget);

#endif /* TWIN_FWD_H */
