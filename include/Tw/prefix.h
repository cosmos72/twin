
#ifndef _TW_PREFIX_H
#define _TW_PREFIX_H

#ifdef _TW_H
# define TW_PREFIX	1
# define Tw(arg)	Tw_##arg
# define TW(arg)	TW_##arg
# ifndef TW_RETSIGTYPE
#  include <Tw/Twautoconf.h>
# endif
#else
# define TW_PREFIX	0
# define Tw(arg)	arg
# define TW(arg)	arg
# ifndef RETSIGTYPE
#  include "autoconf.h"
# endif
#endif

#endif /* _TW_PREFIX_H */

