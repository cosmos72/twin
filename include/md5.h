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

#include "autoconf.h"

#ifdef HAVE_STDDEF_H
# include <stddef.h> /* for size_t */
#endif

#ifdef HAVE_STDINT_H
# include <stdint.h> /* for uint32_t */
  typedef uint32_t md5_uint32;
#else
# warning <stdint.h> not found, checking that 'unsigned int' is 32 bits wide...
  typedef char md5_uint32_check [sizeof(unsigned int) == 4 ? 1 : -1];
  typedef unsigned int md5_uint32;
#endif

struct MD5Context {
    md5_uint32 buf[4];
    md5_uint32 bits[2];
    unsigned char in[64];
};

void MD5Init(struct MD5Context *context);
void MD5Update(struct MD5Context *context, unsigned char const *buf, size_t len);
void MD5Final(unsigned char digest[16], struct MD5Context *context);
void MD5Transform(md5_uint32 buf[4], md5_uint32 const in[16]);

/*
 * This is needed to make RSAREF happy on some MS-DOS compilers.
 */
typedef struct MD5Context MD5_CTX;

#endif /* MD5_H */
