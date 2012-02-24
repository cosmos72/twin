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
#define obj_  TWS_obj
#define vec_  TWS_vec
#define vecW_ TWS_vecW




/* defines and wrappers for commonly used stuff */
#define SIZEOF(type)			AlienSizeof(type,Slot)
#define REPLY(code, type, lval)		alienReply(code, SIZEOF(type), sizeof(type), lval)

#define POP(s,type,lval)		alienPOP(s,type,lval)
/* wrap alienPUSH() to work with non-lvalue `val' */
#define PUSH(s,type,val)		do {type tmp = (val); alienPUSH(s,type,tmp); } while(0)
#define POPADDR(s,type,len,ptr)		PopAddr(s,type,len,ptr)



#if TW_BYTE_ORDER == TW_LITTLE_ENDIAN
/* we can use hton?() functions to speed up translation */
# include <netinet/in.h>
#endif

#if TW_BYTE_ORDER == TW_LITTLE_ENDIAN && TW_CAN_UNALIGNED != 0 /* due to lack of alignment */
INLINE void FlipCopyMem(CONST byte *src, byte *dst, uldat len) {
    switch (len) {
      case 2:
	*(byte16 *)dst = htons(*(CONST byte16 *)src);
	break;
      case 4:
	*(byte32 *)dst = htonl(*(CONST byte32 *)src);
	break;
      case 8:
	((byte32 *)dst)[0] = htonl(((CONST byte32 *)src)[1]);
	((byte32 *)dst)[1] = htonl(((CONST byte32 *)src)[0]);
	break;
      default:
	src += len - 1;
	while (len--)
	    *dst++ = *src--;
	break;
    }
}

#else
INLINE void FlipCopyMem(CONST byte *src, byte *dst, uldat len) {
    src += len - 1;
    while (len--)
	*dst++ = *src--;
}
#endif /* TW_BYTE_ORDER == TW_LITTLE_ENDIAN */



/*translate from alien data, copying srclen bytes to dstlen bytes, optionally flipping byte order*/
static void alienRead(CONST byte *src, uldat srclen, byte *dst, uldat dstlen, byte flip) {
    
#if TW_BYTE_ORDER == TW_LITTLE_ENDIAN
    
    /* copy the least significant bits */
    if (flip)
	FlipCopyMem(src + (srclen > dstlen ? srclen - dstlen : 0), dst, Min2(dstlen, srclen));
    else
	CopyMem(src, dst, Min2(dstlen, srclen));
    /* and set the remaining to zero */
    if (dstlen > srclen)
	WriteMem(dst + srclen, '\0', dstlen - srclen);
    
#else /* TW_BYTE_ORDER == TW_BIG_ENDIAN */
    
    /* copy the least significant bits */
    if (flip)
	FlipCopyMem(src, dst + (dstlen > srclen ? dstlen - srclen : 0), Min2(dstlen, srclen));
    else if (dstlen > srclen)
	CopyMem(src, dst + dstlen - srclen, srclen);
    else
	CopyMem(src + srclen - dstlen, dst, dstlen);
    /* set the high bits to zero */
    if (dstlen > srclen)
	WriteMem(dst, '\0', dstlen - srclen);

#endif /* TW_BYTE_ORDER == TW_LITTLE_ENDIAN */
}

/*translate to alien data, copying srclen bytes to dstlen bytes, optionally flipping byte order*/
static void alienWrite(CONST byte *src, uldat srclen, byte *dst, uldat dstlen, byte flip) {
    
#if TW_BYTE_ORDER == TW_LITTLE_ENDIAN
    
    /* copy the least significant bits */
    if (flip)
	FlipCopyMem(src, dst + (dstlen > srclen ? dstlen - srclen : 0), Min2(dstlen, srclen));
    else
	CopyMem(src, dst, Min2(dstlen, srclen));
    /* and set the remaining to zero */
    if (dstlen > srclen)
	WriteMem(dst + (flip ? 0: srclen), '\0', dstlen - srclen);
    
#else /* TW_BYTE_ORDER == TW_BIG_ENDIAN */
    
    /* copy the least significant bits */
    if (flip)
	FlipCopyMem(src + (srclen > dstlen ? srclen - dstlen : 0), dst, Min2(dstlen, srclen));
    else if (dstlen > srclen)
	CopyMem(src, dst + dstlen - srclen, srclen);
    else
	CopyMem(src + srclen - dstlen, dst, dstlen);
    /* set the high bits to zero */
    if (dstlen > srclen)
	WriteMem(dst + (flip ? srclen : 0), '\0', dstlen - srclen);

#endif /* TW_BYTE_ORDER == TW_LITTLE_ENDIAN */
}

/* convert alien type at (*src) to native and put it at (dst) */
static void alienPop(CONST byte ** src, uldat alien_len, byte *dst, uldat len) {
    alienRead(*src, alien_len, dst, len, AlienXendian(Slot) == MagicAlienXendian);
    *src += alien_len;
}

/* convert native type at (src) to alien and put it at (*dst) */
static void alienPush(CONST byte *src, uldat len, byte **dst, uldat alien_len) {
    alienWrite(src, len, *dst, alien_len, AlienXendian(Slot) == MagicAlienXendian);
    *dst += alien_len;
}


/*
 * translate from alien data, copying len bytes from srcsize chunks to dstsize chunks, optionally flipping byte order.
 * assume dst is large enough to hold translated data.
 */
INLINE void alienReadVec(CONST byte *src, byte *dst, uldat len, uldat srcsize, uldat dstsize, byte flag) {
    /* round to srcsize multiple */
    len = (len / srcsize) * srcsize;
    
    /* optimize common cases */
    
#if TW_BYTE_ORDER == TW_LITTLE_ENDIAN
    if (srcsize == 1) {
	while (len--) {
	    WriteMem(dst+1, '\0', dstsize-1);
	    *dst = *src++;
	    dst += dstsize;
	}
    } else if (dstsize == 1) {
	if (flag) {
	    while (len--) {
		*dst++ = src[srcsize-1];
		src += srcsize;
	    }
	} else {
	    while (len--) {
		*dst++ = *src;
		src += srcsize;
	    }
	}
    }
#else /* TW_BYTE_ORDER == TW_BIG_ENDIAN */
    if (srcsize == 1) {
	while (len--) {
	    WriteMem(dst, '\0', dstsize-1);
	    dst[dstsize-1] = *src++;
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
		*dst++ = src[srcsize-1];
		src += srcsize;
	    }
	}
    }
#endif /* TW_BYTE_ORDER == TW_LITTLE_ENDIAN */
	
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
 * translate from alien data, copying len bytes from srcsize chunks to dstsize chunks, optionally flipping byte order.
 * allocate a new buffer to hold data.
 */
static CONST byte *alienAllocReadVec(CONST byte *src, uldat len, uldat srcsize, uldat dstsize, byte flag) {
    byte *dst;
    
    if ((dst = AllocMem(len / srcsize * dstsize)))
	alienReadVec(src, dst, len, srcsize, dstsize, flag);
    
    return dst;
}


/*
 * translate to alien data, copying len bytes from srcsize chunks to dstsize chunks, optionally flipping byte order.
 * assume dst is large enough to hold translated data.
 */
#ifdef CONF__UNICODE
static void alienWriteVec(CONST byte *src, byte *dst, uldat len, uldat srcsize, uldat dstsize, byte flag) {
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
#endif

static void alienReply(uldat code, uldat alien_len, uldat len, CONST void *data) {
    byte AlienSizeofUldat = SIZEOF(uldat);
    
    if (RemoteWriteQueue(Slot, 3*AlienSizeofUldat + alien_len, NULL)
	== 3*AlienSizeofUldat + alien_len) {
	
	uldat queued;
	byte *T = RemoteWriteGetQueue(Slot, &queued);
	T += queued - 3*AlienSizeofUldat - alien_len;
	
	alien_len += 2*AlienSizeofUldat;
	alienPUSH(T,uldat,alien_len);
	alienPUSH(T,uldat,RequestN);
	alienPUSH(T,uldat,code);
	alien_len -= 2*AlienSizeofUldat;
	
	if (alien_len && len && data) {
	    T = RemoteWriteGetQueue(Slot, &queued);
	    T += queued - alien_len;
	    alienPush(data, len, &T, alien_len);
	}
    }
}

#ifdef CONF__UNICODE
static void alienTranslateHWAttrV_CP437_to_UTF_16(hwattr *H, uldat Len) {
    hwfont f;
    while (Len--) {
	f = Tutf_CP437_to_UTF_16[HWFONT(*H) & 0xFF];
	*H = HWATTR_COLMASK(*H) | HWATTR(0, f);
	H++;
    }
}
#endif

TW_INLINE ldat alienDecodeArg(uldat id, CONST byte * Format, uldat n, tsfield a, uldat mask[1], byte flag[1], ldat fail) {
    CONST void *A;
    void *av;
    topaque nlen;
    uldat a0;
    byte c;
    
    switch ((c = *Format++)) {
      case '_':
	switch ((c = *Format)) {
#define CASE_(type) \
	  case CAT(TWS_,type): \
	    /* ensure type size WAS negotiated */ \
	    if (SIZEOF(type) && Left(SIZEOF(type))) { \
		type an; \
		POP(s,type,an); \
		a[n]_any = (tany)an; \
		a[n]_type = c; \
		break; \
	    } \
	    fail = -fail; \
	    break
		
	  case TWS_hwcol:
	    /*FALLTHROUGH*/
	    CASE_(byte);
	    CASE_(dat);
	    CASE_(ldat);
	    CASE_(topaque);
	    CASE_(tany);
#ifndef CONF__UNICODE
	    CASE_(hwfont);
#else
	  case TWS_hwfont:
	    /* ensure hwattr size WAS negotiated */
	    if (SIZEOF(hwfont) && Left(SIZEOF(hwfont))) {
		hwfont an;
		POP(s,hwfont,an);
		a[n]_any = (tany)an & 0xFF;
		if (SIZEOF(hwfont) == 1)
		    a[n]_any = Tutf_CP437_to_UTF_16[a[n]_any];
		a[n]_type = c;
		break;
	    }
	    fail = -fail;
	    break;
#endif
#ifndef CONF__UNICODE
	    CASE_(hwattr);
#else
	  case TWS_hwattr:
	    /* ensure hwattr size WAS negotiated */
	    if (SIZEOF(hwattr) && Left(SIZEOF(hwattr))) {
		hwattr an;
		POP(s,hwattr,an);
		a[n]_any = HWATTR_COLMASK(an);
		an = HWFONT(an) & 0xFF;
		if (SIZEOF(hwattr) == 2)
		    an = Tutf_CP437_to_UTF_16[an];
		a[n]_any |= HWATTR(0, an);
		a[n]_type = c;
		break;
	    }
	    fail = -fail;
	    break;
#endif
#undef CASE_
	  default:
	    break;
	}
	break;
      case 'x':
	/* all kind of pointers */
	if (Left(SIZEOF(uldat))) {
	    POP(s,uldat,a0);
	    c = *Format - base_magic_CHR;
	    a[n]_obj = Id2Obj(c, a0);
	    a[n]_type = obj_;
	    break;
	}
	fail = -fail;
	break;
      case 'V':
	nlen = sockLengths(id, n, a);
	c = *Format;
	/* ensure type size WAS negotiated */
	if (AlienMagic(Slot)[c]) {
	    nlen *= AlienMagic(Slot)[c];
	    if (Left(nlen)) {
		PopAddr(s,byte,nlen,av);
		a[n]_vec = av;
		a[n]_len = nlen / AlienMagic(Slot)[c] * TwinMagicData[c];
		a[n]_type = vec_|c;
		
		if (AlienMagic(Slot)[c] != TwinMagicData[c] ||
		    (TwinMagicData[c] != 1 && AlienXendian(Slot) == MagicAlienXendian)) {
			
		    a[n]_vec = alienAllocReadVec
			(av, nlen, AlienMagic(Slot)[c], TwinMagicData[c],
			 AlienXendian(Slot) == MagicAlienXendian);
		    if (a[n]_vec) {
#ifdef CONF__UNICODE
			if (c == TWS_hwattr && SIZEOF(hwattr) == 2)
			    alienTranslateHWAttrV_CP437_to_UTF_16((hwattr *)a[n]_vec, nlen);
#endif
			*mask |= 1 << n;
		    } else
			fail = -fail;
		}
		break;
	    }
	}
	fail = -fail;
	break;
      case 'W':
        /* ensure 'topaque' size WAS negotiated */
        if (SIZEOF(topaque) && Left(SIZEOF(topaque))) {
	    POP(s,topaque,nlen);
		
	    c = *Format;
	    /* ensure type size WAS negotiated */
	    if (AlienMagic(Slot)[c]) {
		if (!nlen || (Left(nlen) && nlen == sockLengths(id, n, a) * AlienMagic(Slot)[c])) {
		    PopAddr(s,byte,nlen,av);
		    a[n]_vec = av;
		    a[n]_len = nlen / AlienMagic(Slot)[c] * TwinMagicData[c];
		    a[n]_type = vec_|vecW_|c;
		    
		    if (AlienMagic(Slot)[c] != TwinMagicData[c] ||
			(TwinMagicData[c] != 1 && AlienXendian(Slot) == MagicAlienXendian)) {
			
			a[n]_vec = alienAllocReadVec
			    (av, nlen, AlienMagic(Slot)[c], TwinMagicData[c],
			     AlienXendian(Slot) == MagicAlienXendian);
			    
			if (a[n]_vec) {
#ifdef CONF__UNICODE
			    if (c == TWS_hwattr && SIZEOF(hwattr) == 2)
				alienTranslateHWAttrV_CP437_to_UTF_16((hwattr *)a[n]_vec, nlen);
#endif
			    *mask |= 1 << n;
			} else
			    fail = -fail;
		    }
		    break;
		}
	    }
	}
	fail = -fail;
	break;
    case 'X':
	nlen = sockLengths(id, n, a) * SIZEOF(uldat);
	c = *Format - base_magic_CHR;
	if (Left(nlen)) {
	    PopAddr(s,byte,nlen,av);
	    a[n]_vec = av;
	    a[n]_len = nlen / SIZEOF(uldat) * sizeof(uldat);
	    a[n]_type = vec_|obj_;
	    
	    if (SIZEOF(uldat) != sizeof(uldat) ||
		AlienXendian(Slot) == MagicAlienXendian) {

		A = a[n]_vec = alienAllocReadVec
		    (av, nlen, SIZEOF(uldat), sizeof(uldat),
		     AlienXendian(Slot) == MagicAlienXendian);
	    } else
		A = a[n]_vec;
		
	    if (A) {
		a[n]_vec = AllocId2ObjVec(flag, c, nlen/sizeof(uldat), (byte *)A);
		FreeMem((void *)A);
		if (a[n]_vec) {
		    *mask |= *flag << n;
		    break;
		}
	    }
	}
	fail = -fail;
	break;
      case 'Y':
	c = *Format - base_magic_CHR;
        
        /* ensure 'topaque' size WAS negotiated */
        if (SIZEOF(topaque) && Left(SIZEOF(topaque))) {
	    POP(s,topaque,nlen);
	    nlen *= sizeof(uldat);
	    if (Left(nlen)) {
		PopAddr(s,byte,nlen,av);
		a[n]_vec = av;
		a[n]_len = nlen / SIZEOF(uldat) * sizeof(uldat);
		a[n]_type = vec_|obj_;

		if (SIZEOF(uldat) != sizeof(uldat) ||
		    AlienXendian(Slot) == MagicAlienXendian) {
			
		    A = a[n]_vec = alienAllocReadVec
			(av, nlen, SIZEOF(uldat), sizeof(uldat),
			 AlienXendian(Slot) == MagicAlienXendian);
		} else
		    A = a[n]_vec;
		    
		if (A) {
		    a[n]_vec = AllocId2ObjVec(flag, c, nlen/sizeof(uldat), (byte *)A);
		    FreeMem((void *)A);
		    if (a[n]_vec) {
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
    static byte warned = FALSE;
    uldat mask = 0; /* at least 32 bits. we need 20... */
    uldat nlen, n = 1;
    ldat fail = 1;
    CONST byte *Format = sockF[id].Format;
    uldat a0;
    byte c, self, flag, tmp, retT[2];
    
    self = *Format++;
    retT[0] = *Format++;
    retT[1] = *Format++;
    
    while (fail > 0 && *Format) {
	if (n < TW_MAX_ARGS_N) {
	    fail = alienDecodeArg(id, Format, n, a, &mask, &flag, fail);
	
	} else /* (n >= TW_MAX_ARGS_N) */ {
	    if (!warned) {
		warned = TRUE;
		printk("twin: alienMultiplexB(): got a call with %d args, only %d supported!\n",
		       n, TW_MAX_ARGS_N);
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

    if ((flag = (fail > 0 && s == end && !*Format && (self != '2' || a[1]_obj)))) {
	
	if (retT[0] == 'O' && a[n-1]_type == (TWS_vec|TWS_byte) && a[n-1]_len == 2*sizeof(byte)) {
	    /*
	     * variable return type. store it in last arg,
	     * and let function implementation overwrite it
	     * 
	     * evil trick: only a[n-1]_vec will be passed to the function,
	     * but it points to a[n-1] itself!
	     */
	    a[n-1]_type = proto_2_TWS(a[n-1]_vec);
	    if (mask & 1<<(n-1))
		FreeMem(a[n-1].TWS_field_vecVV);
	    
	    a[n-1]_vec = &a[n-1];
	    a[n-1]_len = 0;
	}
	
	fullMultiplexS(id, n, a);
    }
    
    for (nlen = 0; mask; mask >>= 1, nlen++) {
	if (mask & 1)
	    FreeMem(a[nlen].TWS_field_vecVV);
    }

    if (flag) {
        if (retT[0] == 'O') {
	    /* variable return type. get it from last arg */
	    /* FIXME: currently, only '_' (scalar) and 'v' (void) return types are supported */
		
	    TWS_2_proto(a[n-1]_type, retT);
	}
	
	switch (retT[0]) {
	  case '_':
	    switch (retT[1]) {
#define CASE_(type) \
	      case CAT(TWS_,type): \
    		/* ensure type size WAS negotiated */ \
		if (CAT(TWS_,type) <= TWS_hwcol || SIZEOF(type)) { \
		    /* move to first bytes on MSB machines */ \
		    *(type *)&a[0]_any = (type)a[0]_any; \
		    c = SIZEOF(type); \
		    tmp = sizeof(type); \
		    break; \
		} \
		fail = 0; \
		break
		
		case TWS_hwcol:
		/*FALLTHROUGH*/
		CASE_(byte);
		CASE_(dat);
		CASE_(ldat);
		CASE_(topaque);
		CASE_(tany);
		CASE_(hwfont);
#ifndef CONF__UNICODE
		CASE_(hwattr);
#else
	      case TWS_hwattr:
		/* ensure hwattr size WAS negotiated */
		if (TWS_hwattr <= TWS_hwcol || SIZEOF(hwattr)) {
		    if (SIZEOF(hwattr) == 1) {
			hwfont f = Tutf_UTF_16_to_CP437(HWFONT(a[0]_any));
			a[0]_any = HWATTR_COLMASK(a[0]_any) | HWATTR(0, f);
		    }
		    /* move to first bytes on MSB machines */
		    *(hwattr *)&a[0]_any = (hwattr)a[0]_any;
		    c = SIZEOF(hwattr);
		    tmp = sizeof(hwattr);
		} else
		    fail = -fail;
		break;
#endif
#undef CASE_
	      default:
		c = self = 0;
		break;
	    }
	    if (c && fail > 0) {
		alienReply(OK_MAGIC, tmp, c, &a[0]_any);
		return;
	    }
	    break;
	    
	  case 'x':
	    a0 = a[0]_obj ? a[0]_obj->Id : NOID;
	    REPLY(OK_MAGIC, uldat, &a0);
	    return;
	    
	  case 'S':
	  case 'v':
	    return;
	    
	  default:
	    break;
	}
    }
    if (retT[0] != 'v') {
	if (fail > 0) {
	    if (self != '2' || a[1]_obj)
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
    byte *t, *tend;
    int tot = 0;
#ifdef CONF_SOCKET_GZ
    uldat gzSlot;
#endif
    byte AlienSizeofUldat;
    
    Fd = fd;
    Slot = slot;

    if (ioctl(Fd, FIONREAD, &tot) != 0 || tot == 0)	
	tot = SMALLBUFF;
    else if (tot > BIGBUFF*BIGBUFF)
	tot = BIGBUFF*BIGBUFF;
    
    if (!(t = RemoteReadGrowQueue(Slot, tot)))
	return;
    
    if ((len = read(Fd, t, tot)) && len && len != (uldat)-1) {
	if (len < tot)
	    RemoteReadShrinkQueue(Slot, tot - len);
	
	/* ok, now process the data */

#ifdef CONF_SOCKET_GZ
	if ((gzSlot = LS.pairSlot) != NOSLOT) {
	    /* hmmm, a compressed socket. */
	    if (RemoteGunzip(Slot))
		Slot = gzSlot;
	    else {
		Ext(Remote,KillSlot)(Slot);
		return;
	    }
	}
#endif
	
	s = t = RemoteReadGetQueue(Slot, &len);
	tend = s + len;

	AlienSizeofUldat = SIZEOF(uldat);
	
	while (s + 3*AlienSizeofUldat <= tend) {
	    POP(s,uldat,len);
	    if (len < 2*AlienSizeofUldat) {
		s += len;
		continue;
	    }
	    if (s + len > s && s + len <= tend) {
		end = s + len;
		POP(s, uldat, RequestN);
		POP(s, uldat, Funct);
		if (Funct < MaxFunct) {
		    slot = Slot;
		    alienMultiplexB(Funct); /* Slot is the uncompressed socket here ! */
		    Slot = slot;	/*
					 * restore, in case alienF[Funct].F() changed it;
					 * without this, tw* clients can freeze
					 * if twdisplay is in use
					 */
		}
		else if (Funct == FIND_MAGIC)
		    alienMultiplexB(0);
		s = end;
	    } else if (s + len < s) {
		s = tend;
		break;
	    } else { /* if (s + len > tend) */
		/* must wait for rest of packet... unpop len */
		s -= AlienSizeofUldat;
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
	Ext(Remote,KillSlot)(Slot);
    }
}


/*move chunk bytes to chunk bytes at time, flipping byte order*/
static void FlipMoveMem(byte *mem, uldat len, uldat chunk) {
    uldat i, j;
    byte c;
#if TW_BYTE_ORDER == TW_LITTLE_ENDIAN
    byte32 t;
    
    switch (chunk) {
      case 1:
	return;
      case 2:
	while (len >= 2) {
	    *(byte16 *)mem = htons(*(CONST byte16 *)mem);
	    mem += 2;
	    len -= 2;
	}
	return;
      case 4:
	while (len >= 4) {
	    *(byte32 *)mem = htonl(*(CONST byte32 *)mem);
	    mem += 4;
	    len -= 4;
	}
	return;
      case 8:
	while (len >= 8) {
	    t = htonl(((CONST byte32 *)mem)[0]);
	    ((byte32 *)mem)[0] = htonl(((CONST byte32 *)mem)[1]);
	    ((byte32 *)mem)[1] = t;
	    mem += 8;
	    len -= 8;
	}
	return;
      default:
	break;
    }
#endif
    while (len >= chunk) {
	for (i = 0; i < chunk / 2; i++) {
	    j = chunk - i - 1;
	    c = mem[i];
	    mem[i] = mem[j];
	    mem[j] = c;
	}
	mem += chunk;
	len -= chunk;
    }
}

/*
 * Turn the (msg) into a (tmsg) and write it on the MsgPort file descriptor.
 * Used when MsgPort slot is using non-native data sizes and endianity.
 * 
 * this can be called in nasty places like detaching non-exclusive displays
 * when an exclusive one is started. Must preserve Slot, Fd and other globals!
 */
static void alienSendMsg(msgport MsgPort, msg Msg) {
    uldat save_Slot = Slot;
    int save_Fd = Fd;
    uldat Len = 0, Tot;
    byte *t;
    uldat N;
#if defined(CONF__MODULES) || defined (CONF_HW_DISPLAY) || defined(CONF__UNICODE)
    byte *Src;
#endif
#if defined(CONF__MODULES) || defined (CONF_HW_DISPLAY)
    byte Type;
#endif
#ifdef CONF__UNICODE
    byte16 h;
    hwattr H;
#endif
    
    RequestN = MSG_MAGIC;
    Fd = MsgPort->RemoteData.Fd;
    Slot = MsgPort->RemoteData.FdSlot;

    switch (Msg->Type) {
#if defined(CONF__MODULES) || defined (CONF_HW_DISPLAY)
      case MSG_DISPLAY:
	Src = Msg->Event.EventDisplay.Data;
	N = 1;
	
	switch (Msg->Event.EventDisplay.Code) {
	  case DPY_DrawHWAttr:
	    Type = TWS_hwattr;
	    N = Msg->Event.EventDisplay.Len / sizeof(hwattr);
	    break;
	  case DPY_Helper:
	  case DPY_SetCursorType:
	    Type = TWS_uldat;
	    break;
	  case DPY_DragArea:
	  case DPY_SetPalette:
	    Type = TWS_dat;
	    N = 4;
	    break;
	  default:
	    Type = TWS_byte;
	    N = Msg->Event.EventDisplay.Len;
	    break;
	}
	
	Len = AlienMagic(Slot)[Type] * N + SIZEOF(uldat) + 4*SIZEOF(udat);
	alienReply(Msg->Type, Len, 0, NULL);

	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    
	    PUSH(t, uldat, NOID); /* not used here */
	    alienPUSH(t, udat,  Msg->Event.EventDisplay.Code);
	    Len -= SIZEOF(uldat) + 4*SIZEOF(udat);
	    alienPUSH(t, udat,  Len);
	    alienPUSH(t, udat,  Msg->Event.EventDisplay.X);
	    alienPUSH(t, udat,  Msg->Event.EventDisplay.Y);
	    
	    if (Type == TWS_byte) {
		PushV(t, N, Src);
# ifdef CONF__UNICODE
	    } else if (Type == TWS_hwattr && AlienMagic(Slot)[Type] == 2) {
		/* on the fly conversion from Unicode to CP437 */
		while (N--) {
		    Pop(Src,hwattr,H);
		    
		    h = HWATTR16(HWCOL(H),Tutf_UTF_16_to_CP437(HWFONT(H)));
		    alienPush((CONST byte *)&h, sizeof(hwattr), &t, 2);
		}
# endif
	    } else {
		Tot = TwinMagicData[Type];
		Len = AlienMagic(Slot)[Type];
		while (N--) {
		    alienPush(Src, Tot, &t, Len);
		    Src += Tot;
		}
	    }
	}
	
	break;
#endif /* defined(CONF__MODULES) || defined (CONF_HW_DISPLAY) */
      case MSG_WIDGET_KEY:
	alienReply(Msg->Type, Len = SIZEOF(uldat) + 3*SIZEOF(udat) + 2*SIZEOF(byte) + Msg->Event.EventKeyboard.SeqLen, 0, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	         PUSH(t, uldat, Obj2Id(Msg->Event.EventKeyboard.W));
	    alienPUSH(t, udat,  Msg->Event.EventKeyboard.Code);
	    alienPUSH(t, udat,  Msg->Event.EventKeyboard.ShiftFlags);
	    alienPUSH(t, udat,  Msg->Event.EventKeyboard.SeqLen);
	    alienPUSH(t, byte,  Msg->Event.EventKeyboard.pad);
	    PushV(t, Msg->Event.EventKeyboard.SeqLen + 1, /* the final '\0' */
		  Msg->Event.EventKeyboard.AsciiSeq);
	}
	break;
      case MSG_WIDGET_MOUSE:
	alienReply(Msg->Type, Len = SIZEOF(uldat) + 4*SIZEOF(udat), 0, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	         PUSH(t, uldat, Obj2Id(Msg->Event.EventMouse.W));
	    alienPUSH(t, udat,  Msg->Event.EventMouse.Code);
	    alienPUSH(t, udat,  Msg->Event.EventMouse.ShiftFlags);
	    alienPUSH(t, dat,   Msg->Event.EventMouse.X);
	    alienPUSH(t, dat,   Msg->Event.EventMouse.Y);
	}
	break;
      case MSG_WIDGET_CHANGE:
	alienReply(Msg->Type, Len = SIZEOF(uldat) + 4*SIZEOF(dat), 0, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	         PUSH(t, uldat, Obj2Id(Msg->Event.EventWidget.W));
	    alienPUSH(t, udat,  Msg->Event.EventWidget.Code);
	    alienPUSH(t, udat,  Msg->Event.EventWidget.Flags);
	    alienPUSH(t, udat,  Msg->Event.EventWidget.XWidth);
	    alienPUSH(t, udat,  Msg->Event.EventWidget.YWidth);
	    alienPUSH(t, udat,  Msg->Event.EventWidget.XWidth);
	    alienPUSH(t, udat,  Msg->Event.EventWidget.Y);
	}
	break;
      case MSG_WIDGET_GADGET:
	alienReply(Msg->Type, Len = SIZEOF(uldat) + 2*SIZEOF(dat), 0, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	    PUSH(t, uldat, Obj2Id(Msg->Event.EventGadget.W));
	    alienPUSH(t, udat,  Msg->Event.EventGadget.Code);
	    alienPUSH(t, udat,  Msg->Event.EventGadget.Flags);
	}
	break;
      case MSG_MENU_ROW:
	alienReply(Msg->Type, Len = SIZEOF(uldat) + 2*SIZEOF(dat) + SIZEOF(uldat), 0, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	         PUSH(t, uldat, Obj2Id(Msg->Event.EventMenu.W));
	    alienPUSH(t, udat,  Msg->Event.EventMenu.Code);
	    alienPUSH(t, udat,  Msg->Event.EventMenu.pad);
	         PUSH(t, uldat, Obj2Id(Msg->Event.EventMenu.Menu));
	}
	break;
      case MSG_SELECTION:
	alienReply(Msg->Type, Len = SIZEOF(uldat) + 4*SIZEOF(dat), 0, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	         PUSH(t, uldat, Obj2Id(Msg->Event.EventSelection.W));
	    alienPUSH(t, udat,  Msg->Event.EventSelection.Code);
	    alienPUSH(t, udat,  Msg->Event.EventSelection.pad);
	    alienPUSH(t, dat,   Msg->Event.EventSelection.X);
	    alienPUSH(t, dat,   Msg->Event.EventSelection.Y);
	}
	break;
      case MSG_SELECTIONNOTIFY:
	N = Msg->Event.EventSelectionNotify.Len;
#ifdef CONF__UNICODE
	if (Msg->Event.EventSelectionNotify.Magic == SEL_HWFONTMAGIC)
	    N = (N / sizeof(hwfont)) * SIZEOF(hwfont);
#endif
	alienReply(Msg->Type, Len = 4*SIZEOF(uldat) + 2*SIZEOF(dat) + MAX_MIMELEN + N, 0, NULL);

	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	         PUSH(t, uldat, Obj2Id(Msg->Event.EventSelectionNotify.W));
	    alienPUSH(t, udat,  Msg->Event.EventSelectionNotify.Code);
	    alienPUSH(t, udat,  Msg->Event.EventSelectionNotify.pad);
	    alienPUSH(t, uldat, Msg->Event.EventSelectionNotify.ReqPrivate);
	    alienPUSH(t, uldat, Msg->Event.EventSelectionNotify.Magic);
	    PushV(t, MAX_MIMELEN, Msg->Event.EventSelectionNotify.MIME);
	    
#ifdef CONF__UNICODE
	    /* client may be not unicode aware while we are */
	    if (Msg->Event.EventSelectionNotify.Magic == SEL_HWFONTMAGIC) {
		N = Msg->Event.EventSelectionNotify.Len;
		
		if (SIZEOF(hwfont) == 1) {
		    /* on the fly conversion from Unicode to CP437 */
		    Src = Msg->Event.EventSelectionNotify.Data;
		
		    N /= sizeof(hwfont);
		    alienPUSH(t, uldat, N);
		
		    while (N--) {
			Pop(Src,hwattr,H);
			
			h = HWATTR16(HWCOL(H),Tutf_UTF_16_to_CP437(HWFONT(H)));
			alienPush((CONST byte *)&h, sizeof(hwattr), &t, 2);
		    }
		} else {
		    N = (N / sizeof(hwfont)) * SIZEOF(hwfont);
		    alienPUSH(t, uldat, N);
		    N = Msg->Event.EventSelectionNotify.Len;
		    alienWriteVec(Msg->Event.EventSelectionNotify.Data, t, N,
				  sizeof(hwfont), SIZEOF(hwfont),
				  AlienXendian(Slot) == MagicAlienXendian);
		    t += N;
		}
	    } else
#endif
	    {
		alienPUSH(t, uldat, Msg->Event.EventSelectionNotify.Len);
		PushV(t, Msg->Event.EventSelectionNotify.Len, Msg->Event.EventSelectionNotify.Data);
	    }
	}
	break;
      case MSG_SELECTIONREQUEST:
	alienReply(Msg->Type, Len = SIZEOF(uldat) + 2*SIZEOF(dat) + 2*SIZEOF(ldat), 0, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	         PUSH(t, uldat, Obj2Id(Msg->Event.EventSelectionRequest.W));
	    alienPUSH(t, udat,  Msg->Event.EventSelectionRequest.Code);
	    alienPUSH(t, udat,  Msg->Event.EventSelectionRequest.pad);
	         PUSH(t, uldat, Obj2Id(Msg->Event.EventSelectionRequest.Requestor));
	    alienPUSH(t, uldat, Msg->Event.EventSelectionRequest.ReqPrivate);
	}
	break;
	
      case MSG_USER_CONTROL:
	alienReply(Msg->Type, Len = SIZEOF(uldat) + 4*SIZEOF(dat) + SIZEOF(byte)
	      + Msg->Event.EventControl.Len, 0, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    t += Tot - Len;
	         PUSH(t, uldat, Obj2Id(Msg->Event.EventControl.W));
	    alienPUSH(t, udat,  Msg->Event.EventControl.Code);
	    alienPUSH(t, udat,  Msg->Event.EventControl.Len);
	    alienPUSH(t, dat,   Msg->Event.EventControl.X);
	    alienPUSH(t, dat,   Msg->Event.EventControl.Y);
	    PushV(t, Msg->Event.EventControl.Len, Msg->Event.EventControl.Data);
	}
	break;
	
      case MSG_USER_CLIENTMSG:
	alienReply(Msg->Type, Len = 2*SIZEOF(uldat) + 2*SIZEOF(dat) + Msg->Event.EventClientMsg.Len, 0, NULL);
	if ((t = RemoteWriteGetQueue(Slot, &Tot)) && Tot >= Len) {
	    event_clientmsg *E = &Msg->Event.EventClientMsg;
	    
	    t += Tot - Len;
	         PUSH(t, uldat, Obj2Id(E->W));
	    alienPUSH(t, udat,  E->Code);
	    alienPUSH(t, udat,  E->Format);
	    alienPUSH(t, uldat, E->Len);
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

#ifdef CONF_EXT

#define _obj .TWS_field_obj
#define _any .TWS_field_scalar
#define _vec .TWS_field_vecV
#define _len .TWS_field_vecL
#define _type .type

static byte alienDecodeExtension(tany *Len, CONST byte **Data, tany *Args_n, tsfield a) {
    static byte type_warned = 0;
    topaque n = 0;
    ldat fail = 1;
    tany len, left = *Len;
    CONST byte *data = *Data;
    tany args_n = *Args_n;
    udat t;

    while (fail > 0 && n < args_n) {
	switch ((t = a[n]_type)) {

# define CASE_(type) \
case CAT(TWS_,type): \
    /* ensure type size WAS negotiated */ \
    if ((len = AlienSizeof(type, Slot)) && left >= len) { \
	type an; \
	\
	left -= len; \
	POP(data,type,an); \
	a[n]_any = (tany)an; \
    } else \
	fail = -fail; \
    break

	  case TWS_hwcol:
	    /*FALLTHROUGH*/
	    CASE_(byte);
	    CASE_(dat);
	    CASE_(ldat);
	    CASE_(topaque);
	    CASE_(tany);
	    CASE_(hwfont);
	    CASE_(hwattr);
#undef CASE_

	  case TWS_vec|TWS_vecW|TWS_byte:
	    /* ensure (topaque) size WAS negotiated */
	    if ((len = AlienSizeof(topaque, Slot)) && left >= len) {
		topaque nlen;
		
		left -= len;
		POP(data,topaque,nlen);
		a[n]_len = nlen;
		
		if (!nlen || Left(nlen)) {
		    void *addr;
		    left -= nlen;
		    POPADDR(data,byte,nlen,addr);
		    a[n]_vec = addr;
		    break;
		}
	    }
	    fail = -fail;
	    break;
	  default:
	    if (type_warned < 5) {
		type_warned = 5;
		printk("twin: sockDecodeExtension(): got a call with unknown type 0x%02X' !\n",
		       (int)t);
	    }
	    fail = -fail;
	    break;
	}
	
	if (fail <= 0)
	    break;
	
	fail++;
	n++;
    }
    
    if (fail > 0) {
	*Len -= data - *Data;
	*Data = data;
	*Args_n = n;
    }
    
    return fail > 0;
}

#undef _obj
#undef _any
#undef _vec
#undef _len
#undef _type

#endif /* CONF_EXT */

#undef SIZEOF
#undef REPLY
#undef POP
#undef POPADDR
#undef PUSH

