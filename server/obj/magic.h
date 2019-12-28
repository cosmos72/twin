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

#ifndef _TWIN_MAGIC_H
#define _TWIN_MAGIC_H

#define magic_mask ((uldat)0xF0000000ul)
#define magic_shift 28

#define obj_magic_id 0
#define widget_magic_id 1
#define gadget_magic_id 2
#define window_magic_id 3
#define screen_magic_id 4
#define ggroup_magic_id 5
#define row_magic_id 6
#define menuitem_magic_id 7
#define menu_magic_id 8
#define msgport_magic_id 9
#define msg_magic_id 0xA
#define mutex_magic_id 0xB
#define module_magic_id 0xC
#define extension_magic_id 0xD
#define display_hw_magic_id 0xE
#define all_magic_id 0xF

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
#define extension_magic_STR "\x3D"
#define display_hw_magic_STR "\x3E"
#define all_magic_STR "\x3F"

#define obj_magic ((uldat)0x0DEAD0B1ul)
#define widget_magic ((uldat)0x161D9743ul)
#define gadget_magic ((uldat)0x29867551ul)
#define window_magic ((uldat)0x31357531ul)
#define screen_magic ((uldat)0x42659871ul)
#define ggroup_magic ((uldat)0x5741F326ul)
#define row_magic ((uldat)0x68074FFAul)
#define menuitem_magic ((uldat)0x7ABC8FDEul)
#define menu_magic ((uldat)0x8BAD0BEDul)
#define msgport_magic ((uldat)0x90981437ul)
#define msg_magic ((uldat)0xA3A61CE4ul) /* this gets compiled in libTw ! */
#define mutex_magic ((uldat)0xB0FADED0ul)
#define module_magic ((uldat)0xCB0F1278ul)
#define extension_magic ((uldat)0xDE81EC51ul)
#define display_hw_magic ((uldat)0xEDBCC609ul)
#define all_magic ((uldat)0xFA11FA11ul)

#define magic_n 16 /* max top hex digit of the above ones + 1 */

/*
 *   B I G   F A T   WARNING:
 *
 * msg_magic is the magic number for user-created (tmsg) structures,
 * while MSG_MAGIC (defined in sockproto.h) is the serial number reserved by
 * libTw to receive server messages (which are still (tmsg) structures).
 */

#endif /* _TWIN_MAGIC_H */
