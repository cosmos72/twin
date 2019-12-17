/*
 *  wrapper.c  --  wrapper for the real twin program
 *
 *  This program is in the public domain
 *
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "twautoconf.h"

#ifdef TW_HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifndef BINDIR
# warning BINDIR is not #defined, assuming "/usr/local/bin"
# define BINDIR "/usr/local/bin"
#endif

int main(int argc, char *argv[]) {
    argv[0] = BINDIR "/twin_server";
    execv(argv[0], argv);
    printf("failed to exec %s: %s\n", argv[0], strerror(errno));
    return 1;
}
