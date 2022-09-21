/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_PRIVILEGE_H
#define TWIN_PRIVILEGE_H

#include <Tw/autoconf.h>
#include "compiler.h"

#ifdef TW_HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef TW_HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

gid_t get_tty_grgid(void);
int init_groups(const char *user_name, gid_t group_id);

#endif /* TWIN_PRIVILEGE_H */
