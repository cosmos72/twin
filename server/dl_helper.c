/*
 *  dl_helper.c  --  wrapper around lt_dlopen() and dlopen()
 *
 *  Copyright (C) 2016 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "dl_helper.h"
#include "util.h"

#ifdef dlinit
byte dlinit_once(void) {
    static enum { no_init = 0, init_ok = 1, init_error = 2 } initialized = no_init;
    
    if (initialized == no_init) {
        int err = dlinit();
        if (err != 0) {
            Error(DLERROR);
            ErrStr = dlerror();
            initialized = init_error;
        } else {
            initialized = init_ok;
        }
    }
    return initialized == init_ok ? TRUE : FALSE;
}
#endif

