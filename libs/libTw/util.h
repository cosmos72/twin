#ifndef _TW_UTIL_H
#define _TW_UTIL_H

/*
 *  Copyright (C) 2016 by Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */


void NormalizeTime(timevalue *Time);
timevalue *InstantNow(timevalue *Now);

timevalue *IncrTime(timevalue *Time, TW_CONST timevalue *Incr);
timevalue *DecrTime(timevalue *Time, TW_CONST timevalue *Decr);

dat CmpTime(TW_CONST timevalue *T1, TW_CONST timevalue *T2);

#endif /* _TW_UTIL_H */
