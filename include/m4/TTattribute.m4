/*  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */
define(`encode_attribute', `0`'patsubst(`$1', `TT_ARG_', `encode_')')


define(`encode_READ',  `|e_R')
define(`encode_WRITE', `|e_W')
define(`encode_DIE',   `|e_D')
define(`encode_ARRAY_Z',  `|e_Z')

define(`encode_ARRAY',  `|e_A(encode_array$1)')

define(`encode_array', `ifelse(`$1', `', `', `encode_len($1)`'dnl
`'ifelse(`$3', `', `', `|(_encode_array(shift(shift($@))))<<e_WFP')`'dnl
`'|encode_op(`$2')')')

define(`_encode_array', `ifelse(`$1', `', `', `encode_len($1)`'dnl
`'ifelse(`$3', `', `', `|(_encode_array(shift(shift($@))))<<e_WFP')`'dnl
`'ifelse(`$2', `', `', `|encode_op(`$2')')')')

define(`encode_len', `patsubst($1, `_[FP]', `encode_len\&')')
define(`encode_len_F', `e_F(this, $1)')
define(`encode_len_P', `e_P($1)')

define(`encode_op', `e_OP(ifelse($1, `', e_SET, $1, +, e_ADD, $1, *, e_MUL))')



