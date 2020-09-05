
#ifndef TW_PREFIX_H
#define TW_PREFIX_H

#ifdef TW_H
#define TW_PREFIX 1
#define Tw(arg) Tw_##arg
#define TW(arg) TW_##arg
#ifndef TW_AUTOCONF_H
#include <Tw/autoconf.h>
#endif
#else
#define TW_PREFIX 0
#define Tw(arg) arg
#define TW(arg) arg
#ifndef TWAUTOCONF_H
#include "twautoconf.h"
#endif
#endif

#endif /* TW_PREFIX_H */
