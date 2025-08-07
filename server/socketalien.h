/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#define _obj .TWS_field_obj
#define _any .TWS_field_scalar
#define _vec .TWS_field_vecV
#define _len .TWS_field_vecL
#define _type .type

#define void_ TWS_void
#define obj_ TWS_obj
#define vec_ TWS_vec
#define vecW_ TWS_vecW

/* defines and wrappers for commonly used stuff */
#define SIZEOF(type) AlienSizeof(type, Slot)
#define REPLY(code, type, lval) alienReply(code, SIZEOF(type), sizeof(type), lval)

#define POP(s, type, lval) alienPOP(s, type, lval)
#define PUSH(s, type, lval) alienPUSH(s, type, lval)
/* wrap alienPUSH() to work with non-lvalue `val' */
#define PUSH_(s, type, rval)                                                                       \
  do {                                                                                             \
    type tmp__ = (rval);                                                                           \
    alienPUSH(s, type, tmp__);                                                                     \
  } while (0)
inline void FlipCopyMem(const void *srcv, void *dstv, uldat len) {
  const byte *src = (const byte *)srcv + len - 1;
  byte *dst = (byte *)dstv;
  while (len--)
    *dst++ = *src--;
}

/*translate from alien data, copying srclen bytes to dstlen bytes, optionally flipping byte order*/
static void alienRead(const byte *src, uldat srclen, byte *dst, uldat dstlen, byte flip) {

#if TW_IS_LITTLE_ENDIAN

  /* copy the least significant bits */
  if (flip) {
    FlipCopyMem(src + (srclen > dstlen ? srclen - dstlen : 0), dst, Min2(dstlen, srclen));
  } else {
    CopyMem(src, dst, Min2(dstlen, srclen));
  }
  /* and set the remaining to zero */
  if (dstlen > srclen) {
    memset(dst + srclen, '\0', dstlen - srclen);
  }
#else  /* TW_IS_BIG_ENDIAN */

  /* copy the least significant bits */
  if (flip) {
    FlipCopyMem(src, dst + (dstlen > srclen ? dstlen - srclen : 0), Min2(dstlen, srclen));
  } else if (dstlen > srclen) {
    CopyMem(src, dst + dstlen - srclen, srclen);
  } else {
    CopyMem(src + srclen - dstlen, dst, dstlen);
  }
  /* set the high bits to zero */
  if (dstlen > srclen) {
    memset(dst, '\0', dstlen - srclen);
  }
#endif /* TW_IS_LITTLE_ENDIAN */
}

/*translate to alien data, copying srclen bytes to dstlen bytes, optionally flipping byte order*/
static void alienWrite(const void *src, uldat srclen, byte *dst, uldat dstlen, byte flip) {

#if TW_IS_LITTLE_ENDIAN

  /* copy the least significant bits */
  if (flip)
    FlipCopyMem(src, dst + (dstlen > srclen ? dstlen - srclen : 0), Min2(dstlen, srclen));
  else
    CopyMem(src, dst, Min2(dstlen, srclen));
  /* and set the remaining to zero */
  if (dstlen > srclen)
    memset(dst + (flip ? 0 : srclen), '\0', dstlen - srclen);

#else /* TW_IS_BIG_ENDIAN */

  /* copy the least significant bits */
  if (flip)
    FlipCopyMem(src + (srclen > dstlen ? srclen - dstlen : 0), dst, Min2(dstlen, srclen));
  else if (dstlen > srclen)
    CopyMem(src, dst + dstlen - srclen, srclen);
  else
    CopyMem(src + srclen - dstlen, dst, dstlen);
  /* set the high bits to zero */
  if (dstlen > srclen)
    memset(dst + (flip ? srclen : 0), '\0', dstlen - srclen);

#endif /* TW_IS_LITTLE_ENDIAN */
}

/**
 * convert alien type at (*src) to native and write it at (*dst)
 * return src + alien_len
 */
static const byte *alienPop(const byte *src, uldat alien_len, void *dst, uldat len) {
  alienRead(src, alien_len, (byte *)dst, len, AlienXendian(Slot) == MagicAlienXendian);
  return src + alien_len;
}

/**
 * convert native type at (*src) to alien and write it at (*dst).
 * return dst + alien_len
 */
static byte *alienPush(const void *src, uldat len, byte *dst, uldat alien_len) {
  alienWrite((const byte *)src, len, dst, alien_len, AlienXendian(Slot) == MagicAlienXendian);
  return dst + alien_len;
}

/*
 * translate from alien data, copying len bytes from srcsize chunks to dstsize chunks, optionally
 * flipping byte order. assume dst is large enough to hold translated data.
 */
inline void alienReadVec(const byte *src, byte *dst, uldat len, uldat srcsize, uldat dstsize,
                         byte flag) {
  /* round to srcsize multiple */
  len = (len / srcsize) * srcsize;

  /* optimize common cases */

#if TW_IS_LITTLE_ENDIAN
  if (srcsize == 1) {
    while (len--) {
      memset(dst + 1, '\0', dstsize - 1);
      *dst = *src++;
      dst += dstsize;
    }
  } else if (dstsize == 1) {
    if (flag) {
      while (len--) {
        *dst++ = src[srcsize - 1];
        src += srcsize;
      }
    } else {
      while (len--) {
        *dst++ = *src;
        src += srcsize;
      }
    }
  }
#else  /* TW_IS_BIG_ENDIAN */
  if (srcsize == 1) {
    while (len--) {
      memset(dst, '\0', dstsize - 1);
      dst[dstsize - 1] = *src++;
      dst += dstsize;
    }
  } else if (dstsize == 1) {
    if (flag) {
      while (len--) {
        *dst++ = *src;
        src += srcsize;
      }
    } else {
      while (len--) {
        *dst++ = src[srcsize - 1];
        src += srcsize;
      }
    }
  }
#endif /* TW_IS_LITTLE_ENDIAN */

  else if (srcsize == dstsize) {
    if (flag) {
      while (len) {
        FlipCopyMem(src, dst, srcsize);
        src += srcsize;
        dst += dstsize;
        len -= srcsize;
      }
    } else
      CopyMem(src, dst, srcsize);
  } else {
    /* fallback. all other cases */
    while (len) {
      alienRead(src, srcsize, dst, dstsize, flag);
      src += srcsize;
      dst += dstsize;
      len -= srcsize;
    }
  }
}

/*
 * translate from alien data, copying len bytes from srcsize chunks to dstsize chunks, optionally
 * flipping byte order. allocate a new buffer to hold data.
 */
static byte *alienAllocReadVec(const byte *src, uldat len, uldat srcsize, uldat dstsize,
                               byte flag) {
  byte *dst;

  if ((dst = (byte *)AllocMem(len / srcsize * dstsize)))
    alienReadVec(src, dst, len, srcsize, dstsize, flag);

  return dst;
}

#if 0
/*
 * translate to alien data, copying len bytes from srcsize chunks to dstsize chunks, optionally
 * flipping byte order. assume dst is large enough to hold translated data.
 */
static void alienWriteVec(const byte *src, byte *dst, uldat len, uldat srcsize, uldat dstsize,
                          byte flag) {
  /* round to srcsize multiple */
  len = (len / srcsize) * srcsize;

  /* no optimization for srcsize == 1 or dstsize == 1 here */
  if (srcsize == dstsize) {
    if (flag) {
      while (len) {
        FlipCopyMem(src, dst, srcsize);
        src += srcsize;
        dst += dstsize;
        len -= srcsize;
      }
    } else
      CopyMem(src, dst, srcsize);
  } else {
    /* fallback. all other cases */
    while (len) {
      alienWrite(src, srcsize, dst, dstsize, flag);
      src += srcsize;
      dst += dstsize;
      len -= srcsize;
    }
  }
}
#endif /* 0 */

static void alienReply(uldat code, uldat alien_len, uldat len, const void *data) {
  byte AlienSizeofUldat = SIZEOF(uldat);

  if (RemoteWriteQueue(Slot, 3 * AlienSizeofUldat + alien_len, NULL) ==
      3 * AlienSizeofUldat + alien_len) {

    uldat queued;
    byte *T = RemoteWriteGetQueue(Slot, &queued);
    T += queued - 3 * AlienSizeofUldat - alien_len;

    alien_len += 2 * AlienSizeofUldat;
    PUSH(T, uldat, alien_len);
    PUSH(T, uldat, RequestN);
    PUSH(T, uldat, code);
    alien_len -= 2 * AlienSizeofUldat;

    if (alien_len && len && data) {
      T = RemoteWriteGetQueue(Slot, &queued);
      T += queued - alien_len;
      T = alienPush(data, len, T, alien_len);
    }
  }
}

static void alienTranslateTCellV_CP437_to_UTF_32(tcell *H, uldat Len) {
  trune f;
  while (Len--) {
    f = Tutf_CP437_to_UTF_32[TRUNE(*H) & 0xFF];
    *H = TCELL_COLMASK(*H) | TCELL(0, f);
    H++;
  }
}

#define alienFixIdentity(x) x

/*
 * twin < 0.8.0 used a different encoding {utf16_lo, color, utf16_hi, extra}
 * for tcell. detected by SIZEOF(trune) == 2 && SIZEOF(tcell) == 4
 */
inline tcell alienFixDecodeTCell(tcell cell) {
  trune rune = (cell & 0xFF) | ((cell >> 8) & 0xFF00);
  tcolor col = (cell >> 8) & 0xFF;
  return TCELL(col, rune);
}

inline tcell alienMaybeFixDecodeTCell(tcell cell) {
  if (SIZEOF(trune) == 2 && SIZEOF(tcell) == 4)
    cell = alienFixDecodeTCell(cell);
  return cell;
}

static void alienFixDecodeTCellV(tcell *H, uldat Len) {
  while (Len--) {
    *H = alienFixDecodeTCell(*H);
    H++;
  }
}

inline ldat alienDecodeArg(uldat id, const char *Format, uldat n, tsfield a, uldat mask[1],
                           byte flag[1], ldat fail) {
  void *A;
  const void *av;
  topaque nlen;
  byte c;

  switch ((c = (byte)*Format++)) {
  case '_':
    switch ((c = (byte)*Format)) {
#define CASE_fix(type, fixtype)                                                                    \
  case CAT(TWS_, type):                                                                            \
    /* ensure type size WAS negotiated */                                                          \
    if (SIZEOF(type) && Left(SIZEOF(type))) {                                                      \
      type an;                                                                                     \
      POP(s, type, an);                                                                            \
      a[n] _any = (tany)fixtype(an);                                                               \
      a[n] _type = c;                                                                              \
    } else                                                                                         \
      fail = -fail;                                                                                \
    break;
#define CASE_(type) CASE_fix(type, alienFixIdentity)
#define CASE_tcell() CASE_fix(tcell, alienMaybeFixDecodeTCell)

    case TWS_tcolor:
      /*FALLTHROUGH*/
      CASE_(byte);
      CASE_(dat);
      CASE_(ldat);
      CASE_(topaque);
      CASE_(tany);
      CASE_(trune);
      CASE_tcell();
#undef CASE_tcell
#undef CASE_fix
#undef CASE_
    default:
      break;
    }
    break;
  case 'x':
    /* all kind of pointers */
    if (Left(SIZEOF(uldat))) {
      uldat a0;
      POP(s, uldat, a0);
      c = (byte)*Format - Tbase_magic_CHR;
      a[n] _obj = Id2Obj(e_class_byte(c), a0);
      a[n] _type = obj_;
      break;
    }
    fail = -fail;
    break;
  case 'V':
    nlen = sockLengths(id, View<s_tsfield>(a, n));
    c = (byte)*Format;
    /* ensure type size WAS negotiated */
    if (AlienMagic(Slot)[c]) {
      nlen *= AlienMagic(Slot)[c];
      if (Left(nlen)) {
        PopAddr(s, const byte, nlen, av);
        a[n] _cvec = av;
        a[n] _len = nlen / AlienMagic(Slot)[c] * TwinMagicData[c];
        a[n] _type = vec_ | c;

        if (AlienMagic(Slot)[c] != TwinMagicData[c] ||
            (TwinMagicData[c] != 1 && AlienXendian(Slot) == MagicAlienXendian)) {

          a[n] _vec = alienAllocReadVec((const byte *)av, nlen, AlienMagic(Slot)[c],
                                        TwinMagicData[c], AlienXendian(Slot) == MagicAlienXendian);
          if (a[n] _vec) {
            if (c == TWS_tcell && SIZEOF(tcell) == 2)
              alienTranslateTCellV_CP437_to_UTF_32((tcell *)a[n] _vec, nlen);
            *mask |= 1 << n;
          } else
            fail = -fail;
        }
        if (c == TWS_tcell && SIZEOF(trune) == 2 && SIZEOF(tcell) == 4 && a[n] _vec)
          alienFixDecodeTCellV((tcell *)a[n] _vec, nlen / TW_SIZEOF_TCELL);
        break;
      }
    }
    fail = -fail;
    break;
  case 'W':
    /* ensure 'topaque' size WAS negotiated */
    if (SIZEOF(topaque) && Left(SIZEOF(topaque))) {
      POP(s, topaque, nlen);

      c = (byte)*Format;
      /* ensure type size WAS negotiated */
      if (AlienMagic(Slot)[c]) {
        if (!nlen ||
            (Left(nlen) && nlen == sockLengths(id, View<s_tsfield>(a, n)) * AlienMagic(Slot)[c])) {
          PopAddr(s, const byte, nlen, av);
          a[n] _cvec = av;
          a[n] _len = nlen / AlienMagic(Slot)[c] * TwinMagicData[c];
          a[n] _type = vec_ | vecW_ | c;

          if (AlienMagic(Slot)[c] != TwinMagicData[c] ||
              (TwinMagicData[c] != 1 && AlienXendian(Slot) == MagicAlienXendian)) {

            a[n] _vec =
                alienAllocReadVec((const byte *)av, nlen, AlienMagic(Slot)[c], TwinMagicData[c],
                                  AlienXendian(Slot) == MagicAlienXendian);

            if (a[n] _cvec) {
              if (c == TWS_tcell && SIZEOF(tcell) == 2)
                alienTranslateTCellV_CP437_to_UTF_32((tcell *)a[n] _vec, nlen);
              *mask |= 1 << n;
            } else
              fail = -fail;
          }
          if (c == TWS_tcell && SIZEOF(trune) == 2 && SIZEOF(tcell) == 4 && a[n] _vec)
            alienFixDecodeTCellV((tcell *)a[n] _vec, nlen / TW_SIZEOF_TCELL);
          break;
        }
      }
    }
    fail = -fail;
    break;
  case 'X':
    nlen = sockLengths(id, View<s_tsfield>(a, n)) * SIZEOF(uldat);
    c = (byte)*Format - Tbase_magic_CHR;
    if (Left(nlen)) {
      PopAddr(s, const byte, nlen, av);
      a[n] _cvec = av;
      a[n] _len = nlen / SIZEOF(uldat) * sizeof(uldat);
      a[n] _type = vec_ | obj_;

      if (SIZEOF(uldat) != sizeof(uldat) || AlienXendian(Slot) == MagicAlienXendian) {

        A = a[n] _vec = alienAllocReadVec((const byte *)av, nlen, SIZEOF(uldat), sizeof(uldat),
                                          AlienXendian(Slot) == MagicAlienXendian);
      } else
        A = a[n] _vec;

      if (A) {
        a[n] _vec = AllocId2ObjVec(flag, c, nlen / sizeof(uldat), (byte *)A);
        FreeMem((void *)A);
        if (a[n] _vec) {
          *mask |= *flag << n;
          break;
        }
      }
    }
    fail = -fail;
    break;
  case 'Y':
    c = (byte)*Format - Tbase_magic_CHR;

    /* ensure 'topaque' size WAS negotiated */
    if (SIZEOF(topaque) && Left(SIZEOF(topaque))) {
      POP(s, topaque, nlen);
      nlen *= sizeof(uldat);
      if (Left(nlen)) {
        PopAddr(s, const byte, nlen, av);
        a[n] _cvec = av;
        a[n] _len = nlen / SIZEOF(uldat) * sizeof(uldat);
        a[n] _type = vec_ | obj_;

        if (SIZEOF(uldat) != sizeof(uldat) || AlienXendian(Slot) == MagicAlienXendian) {

          A = a[n] _vec = alienAllocReadVec((const byte *)av, nlen, SIZEOF(uldat), sizeof(uldat),
                                            AlienXendian(Slot) == MagicAlienXendian);
        } else
          A = a[n] _vec;

        if (A) {
          a[n] _vec = AllocId2ObjVec(flag, c, nlen / sizeof(uldat), (byte *)A);
          FreeMem((void *)A);
          if (a[n] _vec) {
            *mask |= *flag << n;
            break;
          }
        }
      }
    }
    fail = -fail;
    break;
  default:
    fail = -fail;
    break;
  }
  return fail;
}

static void alienMultiplexB(uldat id) {
  static struct s_tsfield a[TW_MAX_ARGS_N];
  static byte warned = tfalse;
  uldat mask = 0; /* at least 32 bits. we need 20... */
  uldat nlen, n = 1;
  ldat fail = 1;
  const char *Format = sockF[id].Format;
  char retT[2];
  byte c, self, flag, tmp;

  self = (byte)*Format++;
  retT[0] = *Format++;
  retT[1] = *Format++;

  while (fail > 0 && *Format) {
    if (n < TW_MAX_ARGS_N) {
      fail = alienDecodeArg(id, Format, n, a, &mask, &flag, fail);

    } else /* (n >= TW_MAX_ARGS_N) */ {
      if (!warned) {
        warned = ttrue;
        log(ERROR) << "twin: alienMultiplexB(): got a call with " << n << " args, only "
                   << TW_MAX_ARGS_N << " supported!\n";
      }
      fail = -fail;
    }

    if (fail > 0) {
      Format += 2;
      fail++;
      n++;
    } else
      break;
  }

  if ((flag = (fail > 0 && s == end && !*Format && (self != '2' || a[1] _obj)))) {

    if (retT[0] == 'O' && a[n - 1] _type == (TWS_vec | TWS_byte) &&
        a[n - 1] _len == 2 * sizeof(byte)) {
      /*
       * variable return type. store it in last arg,
       * and let function implementation overwrite it
       *
       * evil trick: only a[n-1]_vec will be passed to the function,
       * but it points to a[n-1] itself!
       */
      a[n - 1] _type = proto_2_TWS((const char *)(a[n - 1] _vec));
      if (mask & 1 << (n - 1))
        FreeMem(a[n - 1].TWS_field_vecV);

      a[n - 1] _vec = &a[n - 1];
      a[n - 1] _len = 0;
    }

    fullMultiplexS(id, Span<s_tsfield>(a, n));
  }

  for (nlen = 0; mask; mask >>= 1, nlen++) {
    if (mask & 1)
      FreeMem(a[nlen].TWS_field_vecV);
  }

  if (flag) {
    if (retT[0] == 'O') {
      /* variable return type. get it from last arg */
      /* FIXME: currently, only '_' (scalar) and 'v' (void) return types are supported */

      TWS_2_proto(a[n - 1] _type, retT);
    }

    switch ((byte)retT[0]) {
    case '_':
      switch ((byte)retT[1]) {
#define CASE_(type)                                                                                \
  case CAT(TWS_, type):                                                                            \
    /* ensure type size WAS negotiated */                                                          \
    if (CAT(TWS_, type) <= TWS_tcolor || SIZEOF(type)) {                                           \
      /* move to first bytes on MSB machines */                                                    \
      const type a0 = (type)a[0] _any;                                                             \
      memcpy(&a[0] _any, &a0, sizeof(type));                                                       \
      c = SIZEOF(type);                                                                            \
      tmp = sizeof(type);                                                                          \
      break;                                                                                       \
    }                                                                                              \
    fail = 0;                                                                                      \
    break

      case TWS_tcolor:
        /*FALLTHROUGH*/
        CASE_(byte);
        CASE_(dat);
        CASE_(ldat);
        CASE_(topaque);
        CASE_(tany);
        CASE_(trune);
        CASE_(tcell);
#undef CASE_
      default:
        c = self = 0;
        break;
      }
      if (c && fail > 0) {
        alienReply(OK_MAGIC, tmp, c, &a[0] _any);
        return;
      }
      break;

    case 'x': {
      const uldat a0 = a[0] _obj ? a[0] _obj->Id : NOID;
      REPLY(OK_MAGIC, uldat, &a0);
      return;
    }
    case 'S':
    case 'v':
      return;

    default:
      break;
    }
  }
  if (retT[0] != 'v') {
    if (fail > 0) {
      if (self != '2' || a[1] _obj)
        fail = FAIL_MAGIC;
      else
        fail = 1;
    }
    alienReply(fail, 0, 0, NULL);
  }
}

#undef _obj
#undef _any
#undef _vec
#undef _len
#undef _type

#undef void_
#undef obj_
#undef vec_
#undef vecW_

static void AlienIO(int fd, uldat slot) {
  uldat len, Funct;
  byte *t;
  const byte *tend;
  int tot = 0;
#ifdef CONF_SOCKET_GZ
  uldat gzSlot;
#endif
  byte AlienSizeofUldat;

  Fd = fd;
  Slot = slot;

  if (ioctl(Fd, FIONREAD, &tot) != 0 || tot <= 0)
    tot = TW_SMALLBUFF;
  else if (tot > TW_HUGEBUFF)
    tot = TW_HUGEBUFF;

  if (!(t = RemoteReadGrowQueue(Slot, tot)))
    return;

  if ((len = read(Fd, t, tot)) && len && len != (uldat)-1) {
    if (len < (uldat)tot)
      RemoteReadShrinkQueue(Slot, (uldat)tot - len);

    /* ok, now process the data */

#ifdef CONF_SOCKET_GZ
    if ((gzSlot = LS.pairSlot) != NOSLOT) {
      /* hmmm, a compressed socket. */
      if (RemoteGunzip(Slot))
        Slot = gzSlot;
      else {
        Ext(Remote, KillSlot)(Slot);
        return;
      }
    }
#endif

    s = t = RemoteReadGetQueue(Slot, &len);
    tend = s + len;

    AlienSizeofUldat = SIZEOF(uldat);

    while (s + 3 * AlienSizeofUldat <= tend) {
      POP(s, uldat, len);
      if (len < 2 * AlienSizeofUldat) {
        s += len;
        continue;
      } else if (len <= tend - s) {
        end = s + len;
        POP(s, uldat, RequestN);
        POP(s, uldat, Funct);
        if (Funct < MaxFunct) {
          slot = Slot;
          alienMultiplexB(Funct); /* Slot is the uncompressed socket here ! */
          Slot = slot;            /*
                                   * restore, in case alienF[Funct].F() changed it;
                                   * without this, tw* clients can freeze
                                   * if twdisplay is in use
                                   */
        } else if (Funct == FIND_MAGIC) {
          alienMultiplexB(0);
        }
        s = end;
      } else if (len <= 1048576) {
        /* must wait for rest of packet... unpop len */
        s -= AlienSizeofUldat;
        break;
      } else {
        /* ignore invalid packet, is too long */
        s = tend;
        break;
      }
    }
    RemoteReadDeQueue(Slot, (uldat)(s - t));

#ifdef CONF_SOCKET_GZ
    if (gzSlot != NOSLOT)
      /* compressed socket, restore Slot */
      Slot = gzSlot;
#endif

  } else if (!len || (len == (uldat)-1 && errno != EINTR && errno != EWOULDBLOCK)) {
    /* let's close this sucker */
    Ext(Remote, KillSlot)(Slot);
  }
}

/*move chunk bytes to chunk bytes at time, flipping byte order*/
static void FlipMoveMem(byte *mem, uldat len, uldat chunk) {
#if TW_IS_LITTLE_ENDIAN

  switch (chunk) {
  case 1:
    return;
  case 2:
    while (len >= 2) {
      serialize(mem, 0, htons(deserialize<uint16_t>(mem)));
      mem += 2;
      len -= 2;
    }
    return;
  case 4:
    while (len >= 4) {
      serialize(mem, 0, htonl(deserialize<uint16_t>(mem)));
      mem += 4;
      len -= 4;
    }
    return;
  case 8:
    while (len >= 8) {
      uint32_t tmp = htonl(deserialize<uint32_t>(mem, 0));
      serialize(mem, 0, htonl(deserialize<uint32_t>(mem, 4)));
      serialize(mem, 4, tmp);
      mem += 8;
      len -= 8;
    }
    return;
  default:
    break;
  }
#endif
  while (len >= chunk) {
    for (uldat i = 0; i < chunk / 2; i++) {
      uldat j = chunk - i - 1;
      byte c = mem[i];
      mem[i] = mem[j];
      mem[j] = c;
    }
    mem += chunk;
    len -= chunk;
  }
}

/*
 * Turn the (Tmsg) into a (tmsg) and write it on the MsgPort file descriptor.
 * Used when MsgPort slot is using non-native data sizes and endianity.
 *
 * this can be called in nasty places like detaching non-exclusive displays
 * when an exclusive one is started. Must preserve Slot, Fd and other globals!
 */
static void alienSendMsg(Tmsgport MsgPort, Tmsg msg) {
  byte *t;
  char *Src;
  uldat save_Slot = Slot, Len = 0, Tot, N;
  int save_Fd = Fd;
  tcell H;
  uint16_t h;
  byte Type;

  RequestN = MSG_MAGIC;
  Fd = MsgPort->RemoteData.Fd;
  Slot = MsgPort->RemoteData.FdSlot;

  switch (msg->Type) {
  case msg_display:
    Src = (char *)msg->Event.EventDisplay.Data;
    N = 1;

    switch (msg->Event.EventDisplay.Code) {
    case ev_dpy_DrawTCell:
      Type = TWS_tcell;
      N = msg->Event.EventDisplay.Len / sizeof(tcell);
      break;
    case ev_dpy_Helper:
    case ev_dpy_SetCursorType:
      Type = TWS_uldat;
      break;
    case ev_dpy_DragArea:
    case ev_dpy_SetPalette:
      Type = TWS_dat;
      N = 4;
      break;
    default:
      Type = TWS_byte;
      N = msg->Event.EventDisplay.Len;
      break;
    }

    Len = AlienMagic(Slot)[Type] * N + SIZEOF(uldat) + 4 * SIZEOF(udat);
    alienReply(msg->Type, Len, 0, NULL);

    if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
      t += Tot - Len;

      PUSH_(t, uldat, NOID); /* not used here */
      PUSH(t, udat, msg->Event.EventDisplay.Code);
      Len -= SIZEOF(uldat) + 4 * SIZEOF(udat);
      PUSH(t, udat, Len);
      PUSH(t, udat, msg->Event.EventDisplay.X);
      PUSH(t, udat, msg->Event.EventDisplay.Y);

      if (Type == TWS_byte) {
        PushV(t, N, Src);
      } else if (Type == TWS_tcell && AlienMagic(Slot)[Type] == 2) {
        /* on the fly conversion from Unicode to CP437 */
        while (N--) {
          Pop(Src, tcell, H);

          h = ((uint16_t)TCOLOR(H) << 8) | Tutf_UTF_32_to_CP437(TRUNE(H));
          t = alienPush(&h, sizeof(tcell), t, 2);
        }
      } else {
        Tot = TwinMagicData[Type];
        Len = AlienMagic(Slot)[Type];
        while (N--) {
          t = alienPush(Src, Tot, t, Len);
          Src += Tot;
        }
      }
    }

    break;
  case msg_widget_key:
    alienReply(msg->Type,
               Len = SIZEOF(uldat) + 3 * SIZEOF(udat) + 2 * SIZEOF(byte) +
                     msg->Event.EventKeyboard.SeqLen,
               0, NULL);
    if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
      t += Tot - Len;
      PUSH_(t, uldat, Obj2Id(msg->Event.EventKeyboard.W));
      PUSH(t, udat, msg->Event.EventKeyboard.Code);
      PUSH(t, udat, msg->Event.EventKeyboard.ShiftFlags);
      PUSH(t, udat, msg->Event.EventKeyboard.SeqLen);
      PUSH(t, byte, msg->Event.EventKeyboard.pad);
      PushV(t, msg->Event.EventKeyboard.SeqLen + 1, /* the final '\0' */
            msg->Event.EventKeyboard.AsciiSeq);
    }
    break;
  case msg_widget_mouse:
    alienReply(msg->Type, Len = SIZEOF(uldat) + 4 * SIZEOF(udat), 0, NULL);
    if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
      t += Tot - Len;
      PUSH_(t, uldat, Obj2Id(msg->Event.EventMouse.W));
      PUSH(t, udat, msg->Event.EventMouse.Code);
      PUSH(t, udat, msg->Event.EventMouse.ShiftFlags);
      PUSH(t, dat, msg->Event.EventMouse.X);
      PUSH(t, dat, msg->Event.EventMouse.Y);
    }
    break;
  case msg_widget_change:
    alienReply(msg->Type, Len = SIZEOF(uldat) + 4 * SIZEOF(dat), 0, NULL);
    if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
      t += Tot - Len;
      PUSH_(t, uldat, Obj2Id(msg->Event.EventWidget.W));
      PUSH(t, udat, msg->Event.EventWidget.Code);
      PUSH(t, udat, msg->Event.EventWidget.Flags);
      PUSH(t, udat, msg->Event.EventWidget.XWidth);
      PUSH(t, udat, msg->Event.EventWidget.YWidth);
      PUSH(t, udat, msg->Event.EventWidget.XWidth);
      PUSH(t, udat, msg->Event.EventWidget.Y);
    }
    break;
  case msg_widget_gadget:
    alienReply(msg->Type, Len = SIZEOF(uldat) + 2 * SIZEOF(dat), 0, NULL);
    if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
      t += Tot - Len;
      PUSH_(t, uldat, Obj2Id(msg->Event.EventGadget.W));
      PUSH(t, udat, msg->Event.EventGadget.Code);
      PUSH(t, udat, msg->Event.EventGadget.Flags);
    }
    break;
  case msg_menu_row:
    alienReply(msg->Type, Len = SIZEOF(uldat) + 2 * SIZEOF(dat) + SIZEOF(uldat), 0, NULL);
    if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
      t += Tot - Len;
      PUSH_(t, uldat, Obj2Id(msg->Event.EventMenu.W));
      PUSH(t, udat, msg->Event.EventMenu.Code);
      PUSH(t, udat, msg->Event.EventMenu.pad);
      PUSH_(t, uldat, Obj2Id(msg->Event.EventMenu.Menu));
    }
    break;
  case msg_selection:
    alienReply(msg->Type, Len = SIZEOF(uldat) + 4 * SIZEOF(dat), 0, NULL);
    if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
      t += Tot - Len;
      PUSH_(t, uldat, Obj2Id(msg->Event.EventSelection.W));
      PUSH(t, udat, msg->Event.EventSelection.Code);
      PUSH(t, udat, msg->Event.EventSelection.pad);
      PUSH(t, dat, msg->Event.EventSelection.X);
      PUSH(t, dat, msg->Event.EventSelection.Y);
    }
    break;
  case msg_selection_notify:
    N = msg->Event.EventSelectionNotify.DataLen;
    alienReply(msg->Type, Len = 4 * SIZEOF(uldat) + 2 * SIZEOF(dat) + TW_MAX_MIMELEN + N, 0, NULL);

    if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
      t += Tot - Len;
      PUSH_(t, uldat, Obj2Id(msg->Event.EventSelectionNotify.W));
      PUSH(t, udat, msg->Event.EventSelectionNotify.Code);
      PUSH(t, udat, msg->Event.EventSelectionNotify.pad);
      PUSH(t, uldat, msg->Event.EventSelectionNotify.ReqPrivate);
      PUSH(t, uldat, msg->Event.EventSelectionNotify.Magic);

      char mimeBuf[TW_MAX_MIMELEN] = {};
      const size_t mimeLen = Min2u(msg->Event.EventSelectionNotify.MimeLen, TW_MAX_MIMELEN);
      CopyMem(msg->Event.EventSelectionNotify.MIME().data(), mimeBuf, mimeLen);
      PushV(t, TW_MAX_MIMELEN, mimeBuf);

      Chars data = msg->Event.EventSelectionNotify.Data();
      const uldat dataLen = uldat(data.size());
      PUSH(t, uldat, dataLen);
      PushV(t, dataLen, data.data());
    }
    break;
  case msg_selection_request:
    alienReply(msg->Type, Len = SIZEOF(uldat) + 2 * SIZEOF(dat) + 2 * SIZEOF(ldat), 0, NULL);
    if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
      t += Tot - Len;
      PUSH_(t, uldat, Obj2Id(msg->Event.EventSelectionRequest.W));
      PUSH(t, udat, msg->Event.EventSelectionRequest.Code);
      PUSH(t, udat, msg->Event.EventSelectionRequest.pad);
      PUSH_(t, uldat, Obj2Id(msg->Event.EventSelectionRequest.Requestor));
      PUSH(t, uldat, msg->Event.EventSelectionRequest.ReqPrivate);
    }
    break;

  case msg_user_control:
    alienReply(msg->Type,
               Len = SIZEOF(uldat) + 4 * SIZEOF(dat) + SIZEOF(byte) + msg->Event.EventControl.Len,
               0, NULL);
    if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
      t += Tot - Len;
      PUSH_(t, uldat, Obj2Id(msg->Event.EventControl.W));
      PUSH(t, udat, msg->Event.EventControl.Code);
      PUSH(t, udat, msg->Event.EventControl.Len);
      PUSH(t, dat, msg->Event.EventControl.X);
      PUSH(t, dat, msg->Event.EventControl.Y);
      PushV(t, msg->Event.EventControl.Len, msg->Event.EventControl.Data);
    }
    break;

  case msg_user_clientmsg:
    alienReply(msg->Type, Len = 2 * SIZEOF(uldat) + 2 * SIZEOF(dat) + msg->Event.EventClientMsg.Len,
               0, NULL);
    if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
      event_clientmsg *E = &msg->Event.EventClientMsg;

      t += Tot - Len;
      PUSH_(t, uldat, Obj2Id(E->W));
      PUSH(t, udat, E->Code);
      PUSH(t, udat, E->Format);
      PUSH(t, uldat, E->Len);
      if (AlienXendian(Slot) == MagicAlienXendian && E->Format > 1)
        FlipMoveMem(E->Data.b, E->Len, E->Format);
      PushV(t, E->Len, E->Data.b);
    }
    break;

  default:
    break;
  }
  Slot = save_Slot;
  Fd = save_Fd;
}

#undef SIZEOF
#undef REPLY
#undef POP
#undef PUSH_
