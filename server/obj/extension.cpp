/*
 *  extension.cpp  --  define methods of server class s_extension
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "obj/extension.h"

extension s_extension::Create(fn_extension Fn, uldat namelen, const char *name) {
  return (extension)s_module::Create((fn_module)Fn, namelen, name);
}
