/*
 *  findtwin.c  --  find a running twin server
 *
 *  This program is placed in the public domain.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <Tw/Twautoconf.h>

#ifdef TW_HAVE_DIRENT_H
# include <dirent.h>
#else
# ifdef TW_HAVE_SYS_NDIR_H
#  include <sys/ndir.h>
# endif
# ifdef TW_HAVE_SYS_DIR_H
#  include <sys/dir.h>
# endif
# ifdef TW_HAVE_NDIR_H
#  include <ndir.h>
# endif
#endif

#include <Tw/Tw.h>
#include <Tw/Twerrno.h>

TW_DECL_MAGIC(findtwin_magic);

static void test(TW_CONST char *dpy) {
    if (dpy || (dpy = getenv("TWDISPLAY"))) {
	if (TwOpen(dpy)) {
	    printf("%s\n", dpy);
	    exit(0);
	}
    }
}

#define HX(c) (((c) >= '0' && (c) <= '9') || ((c) >= 'a' && (c) <= 'f'))

static int match_twsocket(TW_CONST struct dirent *d) {
    char *s = d->d_name;
    
    return !strncmp(s, ".Twin:", 6) &&
	HX(s[6]) && (!s[7] ||
		     (HX(s[7]) && (!s[8] ||
				   (HX(s[8]) && !s[9]))));
}

#if defined(TW_HAVE_SCANDIR) && defined(TW_HAVE_ALPHASORT)
static void unix_socket_test(void) {
    int alphasort();
    struct dirent **namelist;
    char *s;
    int n = scandir("/tmp", &namelist, match_twsocket, alphasort);

    while (n > 0) {
	s = namelist[0]->d_name;
	    
	test(s+5);

	namelist++;
	n--;
    }
}
#endif

int main(int argc, char *argv[]) {
    
    /* first: if given, check _ONLY_ command-line specified servers */
    if (*++argv) {
	do {
	    test(*argv);
	} while (*++argv);
	
	/* bomb out */
	return 1;
    }

    if (!TwCheckMagic(findtwin_magic)) {
	fprintf(stderr, "twfindtwin: %s%s\n", TwStrError(TwErrno), TwStrErrorDetail(TwErrno, TwErrnoDetail));
	return 1;
    }
    
    /* then, check for environment TWDISPLAY */
    test(NULL);
    
#if defined(TW_HAVE_SCANDIR) && defined(TW_HAVE_ALPHASORT)
    /* last resort: exhaustive search in /tmp */
    unix_socket_test();
#endif
    
    return 1;
}

