/*
 *  hw_all.cpp  --  aggregates all functions to let twin display on various hardware
 *
 *  Copyright (C) 1999-2025 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "obj/fwd.h"

/**
 * pretend to dlopen() a hw display driver.
 * Actually only knows about hw display drivers enabled at compile time.
 */
bool DlOpenHW(Tmodule m);
