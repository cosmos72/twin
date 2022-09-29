/*
 *  util.c  --  utility functions
 *
 *  Copyright (C) 1993-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <Tw/autoconf.h>

#include "privilege.h"

#ifdef TW_HAVE_GRP_H
#include <grp.h>
#endif

gid_t getTtyGrgid(void) {
  struct group *gr;
  if ((gr = getgrnam("tty")))
    return gr->gr_gid;
  else
    return (gid_t)-1;
}

int init_groups(const char *user_name, gid_t group_id) {
  return initgroups(user_name, group_id);
}
