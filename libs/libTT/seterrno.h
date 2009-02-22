/*
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 */

#ifndef _TT_SETERRNO_H
#define _TT_SETERRNO_H

/* error handling */
TT_INLINE void SetErrno(ttuint E, ttuint S) {
    s_tt_errno *L = GetErrnoLocation();
    L->E = E;
    L->S = S;
}

#endif /* _TT_SETERRNO_H */

