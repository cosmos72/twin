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

class Chars;

bool DlOpen(Tmodule Module);
void DlClose(Tmodule Module);

enum IdSo {
  MainSo = 0,
  SocketSo = 3,
  RCParseSo = 4,
  MAX_So = 5,
};

Tmodule DlLoad(IdSo code);
void DlUnload(IdSo code);
Tmodule DlIsLoaded(IdSo code);
IdSo DlName2Id(const Chars name);

Tmodule DlLoadAny(Chars name);

#endif /* TWIN_DL_H */
