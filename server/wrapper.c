/*
 *  wrapper.c  --  wrapper for the real twin program
 *
 *  This program is in the public domain
 *
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "autoconf.h"
#include "version.h"
#include <Tw/Tw.h>

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifndef BINDIR
# define BINDIR "."
#endif

int main(int argc, char *argv[]) {
    argv[0] = BINDIR "/twin_real";
    execv(argv[0], argv);
    printf("failed to exec %s: %s\n", argv[0], strerror(errno));
    return 1;
}
