
#ifndef _TWIN_SOCKET_ID_H
#define _TWIN_SOCKET_ID_H



enum sockid {
#define EL(name) CAT(order_,name),
#include "socklist_m4.h"
#undef EL
    order_StatObj
};



#endif /* _TWIN_SOCKET_ID_H */

