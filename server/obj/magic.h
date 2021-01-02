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

#define magic_mask ((uldat)0xF0000000ul)
#define magic_shift 28

enum e_magic_id {
  obj_magic_id = 0,
  widget_magic_id = 1,
  gadget_magic_id = 2,
  window_magic_id = 3,
  screen_magic_id = 4,
  ggroup_magic_id = 5,
  row_magic_id = 6,
  menuitem_magic_id = 7,
  menu_magic_id = 8,
  msgport_magic_id = 9,
  msg_magic_id = 0xA,
  mutex_magic_id = 0xB,
  module_magic_id = 0xC,
  display_hw_magic_id = 0xE,
  all_magic_id = 0xF,
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
#define msgport_magic_STR "\x39"
#define msg_magic_STR "\x3A"
#define mutex_magic_STR "\x3B"
#define module_magic_STR "\x3C"
#define display_hw_magic_STR "\x3E"
#define all_magic_STR "\x3F"

enum e_magic {
  obj_magic = 0x0DEAD0B1ul,
  widget_magic = 0x161D9743ul,
  gadget_magic = 0x29867551ul,
  window_magic = 0x31357531ul,
  screen_magic = 0x42659871ul,
  ggroup_magic = 0x5741F326ul,
  row_magic = 0x68074FFAul,
  menuitem_magic = 0x7ABC8FDEul,
  menu_magic = 0x8BAD0BEDul,
  msgport_magic = 0x90981437ul,
  /*
   *   B I G   F A T   WARNING:
   *
   * msg_magic is the magic number for user-created (tmsg) structures,
   * while MSG_MAGIC (defined in socklist_m4.h) is the serial number reserved by
   * libtw to receive server messages (which are still (tmsg) structures).
   */
  msg_magic = 0xA3A61CE4ul, /* this must match tmsg_magic in include/Tw/Tw_defs.h */
  mutex_magic = 0xB0FADED0ul,
  module_magic = 0xCB0F1278ul,
  display_hw_magic = 0xEDBCC609ul,
  all_magic = 0xFA11FA11ul,
};

enum {
  magic_n = 16, /* max top hex digit of the above ones + 1 */
};

#endif /* TWIN_MAGIC_H */
