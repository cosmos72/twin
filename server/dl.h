/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef _TWIN_DL_H
#define _TWIN_DL_H

#ifndef _TWAUTOCONF_H
# include "twautoconf.h" /* for TW_HAVE_* macros */
#endif

#if defined(TW_HAVE_LTDL) || defined(TW_HAVE_INCLUDED_LTDL)
#  ifdef TW_LT_LIBPREFIX
#    define my_PREFIX TW_LT_LIBPREFIX
#  else
#    define my_PREFIX ""
#  endif
#elif defined(TW_HAVE_DLFCN_H) && defined(TW_HAVE_DLOPEN)
#  define my_PREFIX "lib"
#endif

#define my_LEN(str)        (sizeof(str) - 1)
#define my_PREFIX_LEN      my_LEN(my_PREFIX)
#define with_PREFIX(str)   my_LEN(my_PREFIX str), my_PREFIX str


byte DlOpen(module Module);
void DlClose(module Module);

#define MainSo		((uldat)0)
#define WMSo		((uldat)1)
#define TermSo		((uldat)2)
#define SocketSo	((uldat)3)
#define RCParseSo	((uldat)4)
#define MAX_So		((uldat)5)

module DlLoad(uldat code);
void   DlUnLoad(uldat code);
module DlIsLoaded(uldat code);
udat   DlName2Code(byte *name);

module DlLoadAny(uldat len, byte *name);

void *DlSym(module Module, CONST byte *name);

#endif /* _TWIN_DL_H */
