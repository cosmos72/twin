/*
 *  magic.h  --  define magic constants
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef TWIN_MAGIC_H
#define TWIN_MAGIC_H

#define class_byte_mask ((uldat)0xF0000000ul)
#define class_byte_shift 28

enum e_class_byte /*: unsigned char*/ {
  Tobj_class_byte = 0,
  Twidget_class_byte = 1,
  Tgadget_class_byte = 2,
  Twindow_class_byte = 3,
  Tscreen_class_byte = 4,
  Tgroup_class_byte = 5,
  Trow_class_byte = 6,
  Tmenuitem_class_byte = 7,
  Tmenu_class_byte = 8,
  Tmsgport_class_byte = 9,
  Tmsg_class_byte = 0xA,
  Tmutex_class_byte = 0xB,
  Tmodule_class_byte = 0xC,
  Tdisplay_class_byte = 0xE,
  Tall_class_byte = 0xF,

  class_byte_n = 16, /* 1 + max of the *_class_byte values above */
};

/*
 * These must have consecutive values, but obj_magic_STR can be changed
 * as long as it has the same value as obj_magic_CHR.
 * To avoid troubles with strlen(), you should not use '\0' or "\0"
 * for any of the values below.
 */
#define base_magic_CHR '\x30'
#define obj_magic_STR "\x30"
#define widget_magic_STR "\x31"
#define gadget_magic_STR "\x32"
#define window_magic_STR "\x33"
#define screen_magic_STR "\x34"
#define ggroup_magic_STR "\x35"
#define row_magic_STR "\x36"
#define menuitem_magic_STR "\x37"
#define menu_magic_STR "\x38"
#define Tmsgport_magic_STR "\x39"
#define msg_magic_STR "\x3A"
#define mutex_magic_STR "\x3B"
#define module_magic_STR "\x3C"
#define display_hw_magic_STR "\x3E"
#define all_magic_STR "\x3F"

#endif /* TWIN_MAGIC_H */
