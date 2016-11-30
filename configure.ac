#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([twin], [0.6.2.work], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([server/main.c])
AC_CONFIG_HEADERS([include/config.h])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_AUX_DIR([admin])
AM_INIT_AUTOMAKE([1.15 foreign subdir-objects])

m4_define([AH_HEADER], [config.h]) dnl filename of first AC_CONFIG_HEADERS. works around a bug in included libltdl Makefiles

LT_INIT
LT_CONFIG_LTDL_DIR([libs/libltdl])
LTDL_INIT([recursive])

# Checks for programs.
AC_PROG_CC
AM_PROG_AS
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_SED

# Checks for libraries.

# Checks for header files.
AC_PATH_X
AC_FUNC_ALLOCA
AC_HEADER_TIME
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([arpa/inet.h asm/page.h crypt.h dlfcn.h fcntl.h grp.h inttypes.h libintl.h limits.h locale.h machine/param.h malloc.h memory.h netdb.h netinet/in.h shadow.h signal.h stdarg.h stddef.h stdint.h stdlib.h string.h sys/filio.h sys/ioctl.h sys/mman.h sys/param.h sys/select.h sys/socket.h sys/stat.h sys/timeb.h sys/un.h sys/utsname.h termio.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_C_BIGENDIAN()
AC_TYPE_PID_T
AC_TYPE_SIGNAL
AC_TYPE_SIZE_T
AC_TYPE_UID_T

AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

AC_CHECK_SIZEOF([char])
AC_CHECK_SIZEOF([short])
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])
AC_CHECK_SIZEOF([size_t])
AC_CHECK_SIZEOF([void *])

# Checks for library functions.
AC_FUNC_CHOWN
AC_FUNC_FORK
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_WAIT3
AC_CHECK_FUNCS([dup2 ftime gethostbyname gethostname getpagesize getspnam gettimeofday grantpt memchr memmove memset munmap ptsname putenv select setenv setlocale socket strchr strdup strerror strrchr strspn strstr strtol strtoul tcsetpgrp uname unlockpt wait4])

AC_SEARCH_LIBS(crypt, crypt)                  dnl crypt() is needed by twdm
AC_SEARCH_LIBS(deflate, z)
AC_SEARCH_LIBS(dlopen, dl)
AC_SEARCH_LIBS(ggiOpen, ggi)
AC_SEARCH_LIBS(Gpm_Open, gpm)
AC_SEARCH_LIBS(pthread_create, [pthread c_r])         dnl -lc_r is needed on FreeBSD to use pthread functions
AC_SEARCH_LIBS(tgetent, [gpm tinfo ncurses termcap])  dnl search AFTER Gpm_Open in -lgpm, since we look in -lgpm too

# Output
if test "$with_included_ltdl" = yes; then
  AC_DEFINE(HAVE_INCLUDED_LTDL, 1, [Define to 1 if using the libltdl included with this package sources])
  AC_SUBST(SUBDIR_LIBLTDL, libltdl)
fi
AC_SUBST(SUBDIR_LIBTT, ) dnl libTT is disabled

if test "$ac_cv_search_dlopen" != "no"; then
  AC_DEFINE(HAVE_DLOPEN, 1, [Define to 1 if you have the `dlopen'  function.])
fi

AC_SUBST(LIBCRYPT,   "`echo \"$ac_cv_search_crypt\"          | $SED -e 's/^none required$//g' -e 's/^no$//g'`")
AC_SUBST(LIBDL,      "`echo \"$ac_cv_search_dlopen\"         | $SED -e 's/^none required$//g' -e 's/^no$//g'`")
AC_SUBST(LIBGGI,     "`echo \"$ac_cv_search_ggiOpen\"        | $SED -e 's/^none required$//g' -e 's/^no$//g'`")
AC_SUBST(LIBGPM,     "`echo \"$ac_cv_search_Gpm_Open\"       | $SED -e 's/^none required$//g' -e 's/^no$//g'`")
AC_SUBST(LIBPTHREAD, "`echo \"$ac_cv_search_pthread_create\" | $SED -e 's/^none required$//g' -e 's/^no$//g'`")
AC_SUBST(LIBTERMCAP, "`echo \"$ac_cv_search_tgetent\"        | $SED -e 's/^none required$//g' -e 's/^no$//g'`")
AC_SUBST(LIBZ,       "`echo \"$ac_cv_search_deflate\"        | $SED -e 's/^none required$//g' -e 's/^no$//g'`")

LIBS=

AC_CONFIG_FILES([Makefile clients/Makefile clients/mapscrn/Makefile contrib/Makefile contrib/twcd/Makefile docs/Makefile libs/Makefile libs/libTT/Makefile libs/libTT/HW/Makefile libs/libTutf/Makefile libs/libTw/Makefile libs/libltdl/Makefile server/Makefile server/extensions/Makefile server/HW/Makefile themes/Makefile themes/hw_gfx/Makefile])
AC_OUTPUT
TW_OUTPUT

