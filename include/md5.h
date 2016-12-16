/*
 * This code implements the MD5 message-digest algorithm.
 * The algorithm is due to Ron Rivest.  This code was
 * written by Colin Plumb in 1993, no copyright is claimed.
 * This code is in the public domain; do with it what you wish.
 *
 * Equivalent code is available from RSA Data Security, Inc.
 * This code has been tested against that, and is equivalent,
 * except that you don't need to include two pages of legalese
 * with every copy.
 *
 * To compute the message digest of a chunk of bytes, declare an
 * MD5Context structure, pass it to MD5Init, call MD5Update as
 * needed on buffers full of bytes, and then call MD5Final, which
 * will fill a supplied 16-byte array with the digest.
 */

#ifndef MD5_H
#define MD5_H

#include "twautoconf.h" /* for TW_HAVE_*, uint32_t */
#include "compiler.h"   /* for INLINE, CONST */

#ifdef TW_HAVE_STDINT_H
# include <stdint.h> /* for uint32_t */
#endif

#ifdef TW_HAVE_STDDEF_H
# include <stddef.h> /* for size_t */
#endif

typedef uint32_t md5_uint32;

typedef char assert_md5_uint32_is_really_32_bits_wide [sizeof(md5_uint32) == 4 ? 1 : -1];

struct MD5Context {
    md5_uint32 buf[4];
    md5_uint32 bits[2];
    md5_uint32 in[16];
};

void MD5Init(struct MD5Context *context);
void MD5Update(struct MD5Context *context, unsigned char CONST *buf, size_t len);
void MD5Final(unsigned char digest[16], struct MD5Context *context);
void MD5Transform(md5_uint32 buf[4], md5_uint32 CONST in[16]);

/*
 * This is needed to make RSAREF happy on some MS-DOS compilers.
 */
typedef struct MD5Context MD5_CTX;

#endif /* MD5_H */
