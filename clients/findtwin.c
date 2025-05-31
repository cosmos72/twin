/*
 *  findtwin.c  --  find a running twin server
 *
 *  This program is placed in the public domain.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <Tw/autoconf.h>

#ifdef TW_HAVE_DIRENT_H
#include <dirent.h>
#else
#ifdef TW_HAVE_SYS_NDIR_H
#include <sys/ndir.h>
#endif
#ifdef TW_HAVE_SYS_DIR_H
#include <sys/dir.h>
#endif
#ifdef TW_HAVE_NDIR_H
#include <ndir.h>
#endif
#endif

#include <Tw/Tw.h>
#include <Tw/Twerrno.h>

TW_DECL_MAGIC(findtwin_magic);

static void try_TwOpen(const char *dpy) {
  if (dpy || (dpy = getenv("TWDISPLAY"))) {
    if (TwOpen(dpy)) {
      printf("%s\n", dpy);
      exit(0);
    }
  }
}

#define ishex(c) (((c) >= '0' && (c) <= '9') || ((c) >= 'a' && (c) <= 'f'))

static int match_twsocket(const struct dirent *d) {
  const char *s = d->d_name;

  return !strncmp(s, ".Twin:", 6) && ishex(s[6]) &&
         (!s[7] || (ishex(s[7]) && (!s[8] || (ishex(s[8]) && !s[9]))));
}

#if defined(TW_HAVE_SCANDIR) && (defined(TW_HAVE_VERSIONSORT) || defined(TW_HAVE_ALPHASORT))
static const char *tmpdir(void) {
  const char *tmp = getenv("TMPDIR");
  if (tmp == NULL)
    tmp = "/tmp";
  return tmp;
}

static void search_unix_socket(void) {

#ifdef TW_HAVE_VERSIONSORT
#define my_sort versionsort
#else
#define my_sort alphasort
#endif
  /* versionsort() declaration is tricky to pull from system headers */
  int my_sort(const struct dirent **, const struct dirent **);

  struct dirent **namelist;
  char *s;
  int n = scandir(tmpdir(), &namelist, match_twsocket, my_sort);

  while (n > 0) {
    s = namelist[0]->d_name;

    try_TwOpen(s + 5);

    namelist++;
    n--;
  }
}
#endif

int main(int argc, char *argv[]) {

  /* first: if given, check _ONLY_ command-line specified servers */
  if (*++argv) {
    do {
      try_TwOpen(*argv);
    } while (*++argv);

    /* bomb out */
    return 1;
  }

  if (!TwCheckMagic(findtwin_magic)) {
    fprintf(stderr, "twfindtwin: %s%s\n", TwStrError(TwErrno),
            TwStrErrorDetail(TwErrno, TwErrnoDetail));
    return 1;
  }

  /* then, check for environment TWDISPLAY */
  try_TwOpen(NULL);

#if defined(TW_HAVE_SCANDIR) && defined(TW_HAVE_ALPHASORT)
  /* last resort: exhaustive search in /tmp */
  search_unix_socket();
#endif

  return 1;
}
