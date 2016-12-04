/*
 *  printk.c -- report messages from twin core.
 *
 *  Copyright (C) 2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twin.h"
#include "remote.h"
#include "methods.h"
#include "builtin.h"
#include "draw.h"

#ifdef TW_HAVE_STDARG_H
# include <stdarg.h>
#endif

static byte buf[TW_BIGBUFF]; /* hope it's enough */
static int printk_fd = NOFD;



void printk_str(int len, CONST byte *s) {
    int chunk;

    if (len > 0) {

#ifdef CONF_PRINTK
	if (MessagesWin) {
	    if (MessagesWin->HLogic > TW_SMALLBUFF) {
		while (MessagesWin->HLogic > TW_SMALLBUFF) {
		    Delete(MessagesWin->USE.R.FirstRow);
		    MessagesWin->CurY--;
		}
		if (MessagesWin->Parent)
		    DrawFullWindow2(MessagesWin);
	    }
	    Act(RowWriteAscii,MessagesWin)(MessagesWin, len, s);
	}
#endif /* CONF_PRINTK */
	
	if (printk_fd == NOFD)
	    fwrite(s, len, 1, stderr);
	else {
	    do {
		do {
		    chunk = write(printk_fd, s, len);
		} while (chunk < 0 && errno == EINTR);
		if (chunk > 0) {
		    s += chunk;
		    len -= chunk;
		}
	    } while (len > 0 && chunk > 0);
	}
    }
}



int printk(CONST byte *format, ...) {
    int len = 0;
#if defined(TW_HAVE_VSNPRINTF) || defined(TW_HAVE_VSPRINTF)
    va_list ap;
    
    va_start(ap, format);
# ifdef TW_HAVE_VSNPRINTF
    len = vsnprintf(buf, sizeof(buf), format, ap);
    va_end(ap);
# else
    len = vsprintf(buf, format, ap); /* hopefully i < sizeof(buf) */
    va_end(ap);
    
    if (len > sizeof(buf)) {
	fputs("twin: internal error: printk() overflow! \033[1mQUIT NOW !\033[0m\n", stderr);
	return sizeof(buf);
    }
#endif /* TW_HAVE_VSNPRINTF */
    printk_str(len, buf);

#endif /* defined(TW_HAVE_VSNPRINTF) || defined(TW_HAVE_VPRINTF) */
    return len;
}

    
int flushk(void) {
    return printk_fd == NOFD ? fflush(stderr) : TRUE;
}

int printk_receive_fd(int fd) {
    int got = 0, len;
    
    do {
	do {
	    len = read(fd, buf, sizeof(buf));
	} while (len < 0 && errno == EINTR);
	if (len > 0) {
	    got += len;
	    printk_str(len, buf);
	}
    } while (len > 0);
    
    return got;
}



byte RegisterPrintk(int fd) {
    if (printk_fd == NOFD) {
	printk_fd = fd;
	return TRUE;
    }
    return FALSE;
}

void UnRegisterPrintk(void) {
    printk_fd = NOFD;
}

