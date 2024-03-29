/*  Copyright (C) 2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */
#ifndef TWIN_OSINCLUDES_H
#define TWIN_OSINCLUDES_H

#include <time.h>
#include <sys/types.h>

#ifdef TW_HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef TW_HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef TW_HAVE_LIMITS_H
#include <limits.h>
#endif
#ifdef TW_HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef TW_HAVE_MACHINE_PARAM_H
#include <machine/param.h>
#endif
#ifdef TW_HAVE_SYS_FILIO_H
#include <sys/filio.h>
#endif
#ifdef TW_HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef TW_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#endif /* TWIN_OSINCLUDES_H */
