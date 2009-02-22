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

#ifndef _TT_UTILS_H
#define _TT_UTILS_H


#include "fire.h"

#include "create_defs_m4.h"


#define TNow				TTD.TNow
#define FNow				TTD.FNow

#define IdList				TTD.UTIL.IdList
#define IdSize				TTD.UTIL.IdSize
#define IdTop				TTD.UTIL.IdTop
#define IdBottom			TTD.UTIL.IdBottom

#ifdef CONF_SOCKET_PTHREADS
# define GetErrnoLocation		TTD.UTIL.GetErrnoLocation
#else
# define GetErrnoLocation()		(&TTD.rErrno_)
#endif

#define AssignId			TTD.UTIL.AssignId
#define DropId				TTD.UTIL.DropId
#define Id2Obj				TTD.UTIL.Id2Obj
#define FixedAssignId			TTD.UTIL.FixedAssignId
#define FixedAssignIdArray		TTD.UTIL.FixedAssignIdArray
#define FindNative			TTD.UTIL.FindNative
#define GetNow				TTD.UTIL.GetNow

#define FireEvent			TTD.UTIL.FireEvent
#define FireSimpleEvent			TTD.UTIL.FireSimpleEvent
#define FireChangeEvent			TTD.UTIL.FireChangeEvent

#define AddTo_ttlistener		TTD.UTIL.AddTo_ttlistener
#define Remove_ttlistener		TTD.UTIL.Remove_ttlistener
#define Activate_tttimer		TTD.UTIL.Activate_tttimer

#define AddTo_ttdata			TTD.UTIL.AddTo_ttdata
#define Remove_ttdata			TTD.UTIL.Remove_ttdata
#define SetData_ttdata			TTD.UTIL.SetData_ttdata
#define FindByKey_ttdata		TTD.UTIL.FindByKey_ttdata
#define protected_Create_ttdata		TTD.UTIL.protected_Create_ttdata

#define DelAllExtras_ttcomponent	TTD.UTIL.DelAllExtras_ttcomponent

#define Expose_ttvisible		TTD.UTIL.Expose_ttvisible

#define RealClose			TTD.UTIL.RealClose


#define BOUND(a, min, max) ((a) < (min) ? (min) : (a) > (max) ? (max) : (a))
#define minBOUND(a, min) ((a) < (min) ? (min) : (a))
#define MAXBOUND(a, max) ((a) > (max) ? (max) : (a))


#endif /* _TT_UTILS_H */


