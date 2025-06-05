/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef TWIN_SOCKET_ID_H
#define TWIN_SOCKET_ID_H

enum sockid {
#define EL(name) CAT(order_, name),
#include "socklist_gen.h"
#undef EL
  order_StatObj
};

#endif /* TWIN_SOCKET_ID_H */
