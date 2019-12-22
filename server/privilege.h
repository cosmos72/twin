/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef _TWIN_PRIVILEGE_H
#define _TWIN_PRIVILEGE_H

#include "twautoconf.h"
#include "compiler.h"

#ifdef TW_HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef TW_HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

gid_t get_tty_grgid(void);
int init_groups(CONST char *user_name, gid_t group_id);

#endif /* _TWIN_PRIVILEGE_H */
