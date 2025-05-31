#ifndef TW_UTIL_H
#define TW_UTIL_H

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

timevalue *IncrTime(timevalue *Time, const timevalue *Incr);
timevalue *DecrTime(timevalue *Time, const timevalue *Decr);

dat CmpTime(const timevalue *T1, const timevalue *T2);

#endif /* TW_UTIL_H */
