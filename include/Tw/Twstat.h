/*
 *  Twstat.h  --  functions to get informations about libtw objects.
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef TW_STAT_H
#define TW_STAT_H

#include <stdarg.h>
#include <Tw/Twtypes.h>

#define TWS__CAT(a, b) a##b
#define TWS_CAT(a, b) TWS__CAT(a, b)

#define TWS__STR(a) #a
#define TWS_STR(a) TWS__STR(a)

/* the following are hardcoded in socket.c, libtw.c and libtw ABI, don't change! */
#define TWS_void 0
#define TWS_char 1
#define TWS_byte 1
#define TWS_num 1
#define TWS_dat 2
#define TWS_udat 2
#define TWS_ldat 3
#define TWS_uldat 3
#define TWS_tcolor 4
#define TWS_topaque 5
#define TWS_tany 6
#define TWS_trune 7
#define TWS_tcell 8
#define TWS_highest 9
#define TWS_tobj 0xFF
#define TWS_obj 0xFF
#define TWS_last 0xFF
#define TWS_vec 0x100  /* this is used as TWS_vec | TWS_<some other type> */
#define TWS_vecW 0x200 /* this is used as TWS_vecW | TWS_<some other type> */

#define TWS_array 0xFD /* shortcut for TWS_vec|TWS_vecW|TWS_byte in (byte *) format strings */

/* the following are hardcoded in libtw ABI, don't change! */
#define TWS_void_CHR '\xFE' /* '\xFF' would conflict with TWS_obj */
#define TWS_void_STR "\xFE"
#define TWS_char_STR "\x01"
#define TWS_byte_STR "\x01"
#define TWS_num_STR "\x01"
#define TWS_dat_STR "\x02"
#define TWS_udat_STR "\x02"
#define TWS_ldat_STR "\x03"
#define TWS_uldat_STR "\x03"
#define TWS_tcolor_STR "\x04"
#define TWS_topaque_STR "\x05"
#define TWS_tany_STR "\x06"
#define TWS_trune_STR "\x07"
#define TWS_tcell_STR "\x08"

/* used by server extensions as a shortcut for (TWS_vec | TWS_vecW | TWS_byte) */
#define TWS_array_STR "\xFD"
#define TWS_tobj_STR "\xFF"

#define TWS_field_byte val._
#define TWS_field_num val._
#define TWS_field_dat val._
#define TWS_field_udat val._
#define TWS_field_ldat val._
#define TWS_field_uldat val._
#define TWS_field_tcolor val._
#define TWS_field_trune val._
#define TWS_field_tcell val._
#define TWS_field_topaque val._
#define TWS_field_tany val._
#define TWS_field_tobj val._

#define TWS_field_scalar val._

#define TWS_field_obj val.o
#define TWS_field_vecV val.vec.V
#define TWS_field_vecCV val.vec.CV
#define TWS_field_vecL val.vec.L

#ifdef __cplusplus
extern "C" {
#endif
#if 0
  }
#endif

tany Tw_Stat(tdisplay TwD, tobj Id, udat h);
tslist Tw_StatL(tdisplay TwD, tobj Id, uldat hN, ...);
tslist Tw_StatA(tdisplay TwD, tobj Id, uldat hN, const udat *h);
tslist Tw_StatV(tdisplay TwD, tobj Id, uldat hN, va_list h);
tslist Tw_CloneStat(tdisplay TwD, tobj Id, uldat hN, ...);
tslist Tw_CloneStatL(tdisplay TwD, tobj Id, uldat hN, ...);
tslist Tw_CloneStatA(tdisplay TwD, tobj Id, uldat hN, const udat *h);
tslist Tw_CloneStatV(tdisplay TwD, tobj Id, uldat hN, va_list h);
void Tw_DeleteStat(tdisplay TwD, tslist TSL);
tsfield Tw_FindStat(tdisplay TwD, tslist TSL, udat label);
void Tw_ChangeField(tdisplay TwD, tobj Obj, udat field, tany clear_mask, tany xor_mask);

#define TwStat(Id, h) Tw_Stat(Tw_DefaultD, Id, h)

tslist TwStatL(tobj Id, uldat hN, ...);
#define TwStatA(Id, hN, h) Tw_StatA(Tw_DefaultD, Id, hN, h)
#define TwStatV(Id, hN, h) Tw_StatV(Tw_DefaultD, Id, hN, h)

tslist TwCloneStatL(tobj Id, uldat hN, ...);
#define TwCloneStatA(Id, hN, h) Tw_CloneStatA(Tw_DefaultD, Id, hN, h)
#define TwCloneStatV(Id, hN, h) Tw_CloneStatV(Tw_DefaultD, Id, hN, h)
#define TwDeleteStat(TSL) Tw_DeleteStat(Tw_DefaultD, TSL)
#define TwFindStat(TSL, label) Tw_FindStat(Tw_DefaultD, TSL, label)

#ifdef __cplusplus
}
#endif

#endif /* TW_STAT_H */
