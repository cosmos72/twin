
#ifndef _TT_UTILS_H
#define _TT_UTILS_H

#ifdef CONF_SOCKET_PTHREADS
# define GetErrnoLocation	TTD.UTIL.GetErrnoLocation
#else
# define GetErrnoLocation()	(&TTD.rErrno_)
#endif
#define CloneStr2TTFont		TTD.UTIL.CloneStr2TTFont
#define AssignId		TTD.UTIL.AssignId
#define DropId			TTD.UTIL.DropId
#define Id2Obj			TTD.UTIL.Id2Obj
#define FindNative		TTD.UTIL.FindNative

#define DispatchEvent		TTD.UTIL.DispatchEvent
#define DispatchSimpleEvent	TTD.UTIL.DispatchSimpleEvent

#define DelAllListeners_ttobj	TTD.UTIL.DelAllListeners_ttobj
#define DelAllCallbacks_ttobj	TTD.UTIL.DelAllCallbacks_ttobj
#define AddTo_ttcallback	TTD.UTIL.AddTo_ttcallback
#define Remove_ttcallback	TTD.UTIL.Remove_ttcallback
#define Expose_ttvisible	TTD.UTIL.Expose_ttvisible

#include "create_defsm4.h"

#endif /* _TT_UTILS_H */


