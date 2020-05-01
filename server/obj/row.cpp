/*
 *  row.cpp  --  define methods of server class s_row
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "obj/menuitem.h" // COD_RESERVED
#include "obj/row.h"

row s_row::Create(fn_row Fn, udat code, byte flags) {
  row Row = NULL;
  if (code < COD_RESERVED && (Row = (row)s_obj::Create((fn_obj)Fn))) {
    Row->Code = code;
    Row->Flags = flags;
    Row->Gap = Row->LenGap = Row->Len = Row->MaxLen = 0;
    Row->Text = NULL;
    Row->ColText = NULL;
  }
  return Row;
}
