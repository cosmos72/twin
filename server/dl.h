/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_DL_H
#define TWIN_DL_H

byte DlOpen(module Module);
void DlClose(module Module);

#define MainSo ((uldat)0)
#define TermSo ((uldat)2)
#define SocketSo ((uldat)3)
#define RCParseSo ((uldat)4)
#define MAX_So ((uldat)5)

module DlLoad(uldat code);
void DlUnload(uldat code);
module DlIsLoaded(uldat code);
udat DlName2Code(const char *name);

module DlLoadAny(uldat len, const char *name);

void *DlSym(module Module, const char *name);

#endif /* TWIN_DL_H */
