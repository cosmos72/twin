/*
 *  wrapper.c  --  wrapper for the real twin program
 *
 *  This program is in the public domain
 *
 */

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
#ifdef BINDIR
    argv[0] = BINDIR "/twin_real";
#else
    argv[0] = "./twin_real";
#endif
    execv(argv[0], argv);
    printf("failed to exec %s: %s\n", argv[0], strerror(errno));
    return 1;
}
