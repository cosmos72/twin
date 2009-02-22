/*  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */
define(`tolower', `translit(`$1', `A-Z', `a-z')')
define(`toupper', `translit(`$1', `a-z', `A-Z')')
define(`Toupper', `toupper(substr(`$1', 0, 1))`'patsubst(substr(`$1', 1), `_\(\w\)',``'toupper(`\1')')')

define(`Toupper2', `Toupper($1)`'Toupper($2)')

define(`get_field', `ifelse(index(`$4', `r'), -1, `', `getset_method(get,$3,this,1, _G0(_R_ifaddress($1,$2)`'$1,$2,), _G(_R,this,o))')')
define(`set_field', `ifelse(index(`$4', `w'), -1, `', `getset_method(set,$3,this,2, _G0(,ttbyte,), _G(,this,o), _G($1,$2,$3))')')
define(`getset_field', `get_field($@)`'set_field($@)')

