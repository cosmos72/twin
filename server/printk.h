/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef _TWIN_PRINTK_H
#define _TWIN_PRINTK_H

int printk(CONST byte *format, ...);
void printk_str(int len, CONST byte *s);
int printk_receive_fd(int fd);
int flushk(void);

byte RegisterPrintk(int fd);
void UnRegisterPrintk(void);


#endif /* _TWIN_PRINTK_H */
