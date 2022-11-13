/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_PRINTK_H
#define TWIN_PRINTK_H

#include <Tw/Tw_defs.h> // TW_BIGBUFF

#ifndef SS
#define SS "%." STR(TW_SMALLBUFF) "s"
#endif

void printk_str(const char *s, size_t len);
int printk_receive_fd(int fd);
int flushk(void);

bool RegisterPrintkFd(int fd);
void UnRegisterPrintkFd(void);

extern char printk_buf[TW_BIGBUFF];

#endif /* TWIN_PRINTK_H */
