
#ifndef _TW_PREFIX_H
#define _TW_PREFIX_H

#ifdef _TW_H
# define TW_PREFIX	1
# define Tw(arg)	Tw_##arg
# define TW(arg)	TW_##arg
# ifndef _TW_AUTOCONF_H
#  include <Tw/autoconf.h>
# endif
#else
# define TW_PREFIX	0
# define Tw(arg)	arg
# define TW(arg)	arg
# ifndef _TWAUTOCONF_H
#  include "twautoconf.h"
# endif
#endif

#endif /* _TW_PREFIX_H */

