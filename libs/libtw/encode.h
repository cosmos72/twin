/*
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 */

/*
 * encoding functions to serialize libtw call args in a buffer
 * that will be written in the socket connection to the server.
 */

/*
 * read (va_list va) and fill (tsfield a)
 *
 * let (tobj) fields decay into (uldat),
 * since from client side they are the same.
 */
TW_INLINE udat EncodeArgs(fn_order o, uldat *Space, va_list va, tsfield a) {
  const char *Format = Functions[o].format + 1;
  uldat arglen, space;
  udat N;
  byte c, t, variable_return_type = tfalse;

  for (N = space = 0; (c = *Format++); N++, a++) {
    t = (byte)*Format++;
    if (t >= TWS_highest)
      /*
       * let (tobj) fields decade into (uldat),
       * since from client side they are the same .
       */
      t = TWS_uldat;

    if (N)
      switch (c) {
      case '_':
      case 'x':
        space += Tw_MagicData[a->type = t];
        a->TWS_field_scalar = va_arg(va, tany);
        break;
      case 'W':
      case 'Y':
        a->type = TWS_vec | TWS_vecW | t;
        arglen = va_arg(va, tany);
        if (!(a->TWS_field_vecV = va_arg(va, void *)))
          arglen = 0;
        a->TWS_field_vecL = arglen;
        space += sizeof(topaque) + arglen;
        break;
      case 'V':
      case 'X':
        a->type = TWS_vec | t;
        space += a->TWS_field_vecL = va_arg(va, tany);
        a->TWS_field_vecV = va_arg(va, void *);
        break;
      default:
        return (udat)-1;
      }
    else
      switch (c) {
        /* parse arg 0 (return value) */
      case '_':
      case 'x':
      case 'v':
        a->type = t;
        a->label = Tw_MagicData[t]; /* sizeof(return type) */
        break;
      case 'O':
        /* will be filled by last arg */
        variable_return_type = ttrue;
        break;
      default:
        return (udat)-1;
      }
  }
  if (variable_return_type) {
    /* fill return type from first arg - which must be 'udat' */

    if (N > 1 && a[-1].type == (TWS_vec | TWS_byte) && a[-1].TWS_field_vecL == 2) {
      Format = (const char *)a[-1].TWS_field_vecV;
      switch ((byte)*Format++) {
      case '_':
      case 'x':
      case 'v':
        if ((t = (byte)*Format++) < TWS_highest) {
          a->type = t;
          a->label = Tw_MagicData[t]; /* sizeof(return type) */
          break;
        }
        return (udat)-1;
      default:
        return (udat)-1;
      }
    } else
      return (udat)-1;
  }
  *Space = space;
  return N - 1; /* arg 0 is not a call argument */
}

/* append (tsfield)a to the serialization buffer being constructed */
static byte *PushArg(byte *buf, tsfield a) {
  switch (a->type) {
#define ENC_CASE(type)                                                                             \
  case TW_CAT(TWS_, type): {                                                                       \
    type tmp = (type)a->TWS_field_scalar;                                                          \
    Push(buf, type, tmp);                                                                          \
  }; break
    ENC_CASE(byte);
    ENC_CASE(udat);
#if 0
        /* we never meet this here, as EncodeArgs() above turns (tobj) into (uldat) */
      case TWS_tobj:
        /* FALLTHROUGH */
#endif
    ENC_CASE(uldat);
    ENC_CASE(tcolor);
    ENC_CASE(topaque);
    ENC_CASE(tany);
    ENC_CASE(trune);
    ENC_CASE(tcell);
#undef ENC_CASE
  default:
    if (a->type & TWS_vec) {
      if (a->type & TWS_vecW)
        Push(buf, topaque, a->TWS_field_vecL);
      if (a->TWS_field_vecL)
        PushV(buf, a->TWS_field_vecL, a->TWS_field_vecV);
    }
    break;
  }
  return buf;
}

static void DecodeReply(byte *buf, tsfield a) {
  switch (a->type) {
#define DEC_CASE(type)                                                                             \
  case TW_CAT(TWS_, type): {                                                                       \
    type tmp;                                                                                      \
    Pop(buf, type, tmp);                                                                           \
    a->TWS_field_scalar = tmp;                                                                     \
  }; break
    DEC_CASE(byte);
    DEC_CASE(udat);
#if 0
        /* we never meet this here, as EncodeArgs() above turns (tobj) into (uldat) */
      case TWS_tobj:
        /* FALLTHROUGH */
#endif
    DEC_CASE(uldat);
    DEC_CASE(tcolor);
    DEC_CASE(topaque);
    DEC_CASE(tany);
    DEC_CASE(trune);
    DEC_CASE(tcell);
  default:
    break;
  }
}
