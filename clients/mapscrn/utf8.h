/*  
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
extern unsigned long from_utf8(char **inptr, int cnt, int *err);

#define UTF8_BAD	(-1)
#define UTF8_SHORT	(-2)
