/*  
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
/* Error-free versions of some libc routines */
extern void *xmalloc(size_t sz);
extern void *xrealloc(void *p, size_t sz);
extern char *xstrdup(char *p);
