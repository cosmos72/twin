/*
 *  dl_helper.c  --  wrapper around lt_dlopen() and dlopen()
 *
 *  Copyright (C) 2016 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#ifndef _TWIN_DL_HELPER_H
#define _TWIN_DL_HELPER_H

#ifndef _TWAUTOCONF_H
#  include "twautoconf.h"
#endif


/* development: prefer dlopen() to lt_dlopen() */
#define TW_PREFER_DLOPEN


#if defined(TW_PREFER_DLOPEN) && defined(TW_HAVE_DLFCN_H) && defined(TW_HAVE_DLOPEN)
#  undef TW_HAVE_LTDL
#  undef TW_HAVE_INCLUDED_LTDL
#endif


#if defined(TW_HAVE_LTDL) || defined(TW_HAVE_INCLUDED_LTDL)
#  include <ltdl.h>
#  define dlinit         lt_dlinit
byte dlinit_once(void);                     /* from dl_helper.c */
#  define dlerror        lt_dlerror
#  define dlhandle       lt_dlhandle
#  define dlopen(name)   lt_dlopenext(name) /* let ltdl decide library name suffix: .la .so .dll or whatever is on the target platform */
#  define dlclose        lt_dlclose
#  define dlsym          lt_dlsym
#  ifdef TW_LT_LIBPREFIX
#    define DL_PREFIX TW_LT_LIBPREFIX
#  else
#    define DL_PREFIX ""
#  endif
#  define DL_SUFFIX   ""
#elif defined(TW_HAVE_DLFCN_H) && defined(TW_HAVE_DLOPEN)
#  include <dlfcn.h>
#  undef  dlinit           /* dlopen() requires no initialization */
#  define dlinit_once()    TRUE
#  define dlhandle         void *
#  define dlopen(name)     dlopen((name), RTLD_NOW|RTLD_GLOBAL)
#  define DL_PREFIX        "lib"
#  define DL_SUFFIX        "-" TWIN_VERSION_STR ".so"
#else
#  error nor dlopen() nor lt_dlopen() module loading API available!  
#endif

#endif /* _TWIN_DL_HELPER_H */
