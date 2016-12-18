/*
 *  util.c  --  utility functions
 *
 *  Copyright (C) 2016 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <Tw/Tw.h>

void NormalizeTime(timevalue *Time) {
    if (Time->Fraction >= FullSEC || Time->Fraction < 0) {
	tany delta = Time->Fraction / FullSEC;
	Time->Seconds += delta;
	Time->Fraction -= delta * FullSEC;
    }
}

timevalue *InstantNow(timevalue *Now) {
#if defined(TW_HAVE_GETTIMEOFDAY)
    struct timeval sysNow;

    gettimeofday(&sysNow, NULL);
    
    Now->Seconds = sysNow.tv_sec;
    Now->Fraction = sysNow.tv_usec MicroSECs;
#elif defined(TW_HAVE_FTIME)
    struct timeb sysNow;

    ftime(&sysNow);
    
    Now->Seconds = sysNow.time;
    Now->Fraction = sysNow.millitm MilliSECs;
#else
    Now->Seconds = time(NULL);
    Now->Fraction = 0;
#endif
    return Now;
}

timevalue *IncrTime(timevalue *Time, TW_CONST timevalue *Incr) {
    Time->Seconds += Incr->Seconds;
    Time->Fraction += Incr->Fraction;
    NormalizeTime(Time);
    return Time;
}

timevalue *DecrTime(timevalue *Time, TW_CONST timevalue *Decr) {
    Time->Seconds -= Decr->Seconds;
    if (Time->Fraction >= Decr->Fraction)
        Time->Fraction -= Decr->Fraction;
    else {
        Time->Seconds--;
        Time->Fraction += (FullSEC - Decr->Fraction);
    }
    return Time;
}

timevalue *SubTime(timevalue *Result, timevalue *Time, TW_CONST timevalue *Decr) {
    *Result = *Time; /* struct copy */
    return DecrTime(Result, Decr);
}

dat CmpTime(TW_CONST timevalue *T1, TW_CONST timevalue *T2) {
    if (T1->Seconds > T2->Seconds)
	return (dat)1;
    else if (T1->Seconds == T2->Seconds) {
	if (T1->Fraction > T2->Fraction)
	    return (dat)1;
	else if (T1->Fraction == T2->Fraction)
	    return (dat)0;
    }
    return (dat)-1;
}

