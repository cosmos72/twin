#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([twin], [0.6.3], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([server/main.c])
AC_CONFIG_HEADERS([include/config.h])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_AUX_DIR([admin])
AM_INIT_AUTOMAKE([1.15 foreign subdir-objects])

m4_define([AH_HEADER], [config.h]) dnl filename of first AC_CONFIG_HEADERS. works around a bug in included libltdl Makefiles

TW_OPTIONS

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
AC_CHECK_HEADERS([arpa/inet.h asm/page.h crypt.h dlfcn.h fcntl.h grp.h inttypes.h libintl.h limits.h \
       linux/kd.h linux/keyboard.h linux/vt.h locale.h machine/param.h malloc.h memory.h \
       ncurses.h ncurses/ncurses.h ncurses/termcap.h netdb.h netinet/in.h pthread.h \
       shadow.h signal.h stdarg.h stddef.h stdint.h stdlib.h string.h \
       sys/filio.h sys/ioctl.h sys/mman.h sys/param.h sys/select.h sys/socket.h sys/stat.h \
       sys/timeb.h sys/un.h sys/utsname.h termcap.h termio.h termios.h unistd.h X11/xpm.h zlib.h])
AC_STRUCT_TM

# Checks for typedefs, structures, and compiler characteristics.
AC_C_BIGENDIAN
AC_C_CONST
AC_C_INLINE

TW_C_FEATURES

# CC flags '-g -O2' are already tested by AC_PROG_CC
# CC flags '-p -pg' are incompatible with -fomit-frame-pointer
for try_cflag in -fomit-frame-pointer -fno-strict-aliasing -pipe -Wall -Wshadow \
                 -Wno-uninitialized -Wno-pointer-sign -Wno-unused -Wundef -Wstrict-prototypes \
                 -Wcast-qual -Wcast-align -Wpointer-arith -Wbad-function-cast -Waggregate-return
do
  save_CFLAGS="$CFLAGS"
  CFLAGS="$CFLAGS $try_cflag"
  try_cflag_id="`echo \"$try_cflag\" | tr - _`"
  AC_CACHE_CHECK(for compiler flag $try_cflag, ac_cv_cflag$try_cflag_id, AC_TRY_COMPILE(
  [
    static inline int ret0(void) {
      return 0;
    }
  ] , [
    int a = ret0();
  ], [
    eval "ac_cv_cflag$try_cflag_id=yes"
  ], [
    eval "ac_cv_cflag$try_cflag_id=no"
    CFLAGS="$save_CFLAGS"
  ]))
done


AC_CHECK_TYPE(gid_t,int)
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
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_SETVBUF_REVERSED
dnl we never use the 'struct rusage' returned by wait3()
dnl AC_FUNC_WAIT3
AC_CHECK_FUNCS([dup2 ftime gethostname getpagesize getspnam gettimeofday grantpt \
      memchr memmove memset munmap ptsname putenv select setenv setlocale socket strchr strdup \
      strerror strrchr strspn strstr strtol strtoul sysconf tcsetpgrp uname unlockpt \
      vsprintf vsnprintf wait3 wait4])


AC_SEARCH_LIBS(crypt,          crypt)                   dnl crypt() is needed by twdm
AC_SEARCH_LIBS(deflate,        z)
AC_SEARCH_LIBS(dlopen,         dl)
AC_SEARCH_LIBS(socket,         [socket wsock32])
AC_SEARCH_LIBS(connect,        [socket wsock32])
AC_SEARCH_LIBS(gethostbyname,  [nsl    wsock32])
AC_SEARCH_LIBS(ggiOpen,        ggi)
AC_SEARCH_LIBS(Gpm_Open,       gpm)
AC_SEARCH_LIBS(pthread_create, [pthread c_r])         dnl -lc_r is needed on FreeBSD to use pthread functions
AC_SEARCH_LIBS(XpmReadFileToPixmap, Xpm)
                                                      dnl reset LIBS, because we check -lgpm again
LIBS=
AC_SEARCH_LIBS(tgetent, [gpm tinfo ncurses termcap])


# Output
if test "$with_included_ltdl" = yes; then
  AC_DEFINE(HAVE_INCLUDED_LTDL, 1, [Define to 1 if using the libltdl included with this package sources])
  AC_SUBST(SUBDIR_LIBLTDL, libltdl)
fi
AC_SUBST(SUBDIR_LIBTT, ) dnl libTT is disabled

if test "$ac_cv_search_crypt" != "no"; then
  AC_DEFINE(HAVE_CRYPT, 1, [Define to 1 if you have the `crypt'  function.])
fi
if test "$ac_cv_search_deflate" != "no"; then
  AC_DEFINE(HAVE_DEFLATE, 1, [Define to 1 if you have the `deflate' function.])
fi
if test "$ac_cv_search_dlopen" != "no"; then
  AC_DEFINE(HAVE_DLOPEN, 1, [Define to 1 if you have the `dlopen'  function.])
fi
if test "$ac_cv_search_gethostbyname" != "no"; then
  AC_DEFINE(HAVE_GETHOSTBYNAME, 1, [Define to 1 if you have the `gethostbyname'  function.])
fi
if test "$ac_cv_search_ggiOpen" != "no"; then
  AC_DEFINE(HAVE_GGIOPEN, 1, [Define to 1 if you have the `ggiOpen'  function.])
fi
if test "$ac_cv_search_Gpm_Open" != "no"; then
  AC_DEFINE(HAVE_GPM_OPEN, 1, [Define to 1 if you have the `Gpm_Open'  function.])
fi
if test "$ac_cv_search_pthread_create" != "no"; then
  AC_DEFINE(HAVE_PTHREAD_CREATE, 1, [Define to 1 if you have the `pthread_create'  function.])
fi
if test "$ac_cv_search_tgetent" != "no"; then
  AC_DEFINE(HAVE_TGETENT, 1, [Define to 1 if you have the `tgetent'  function.])
fi
if test "$ac_cv_search_XpmReadFileToPixmap" != "no"; then
  AC_DEFINE(HAVE_XPMREADFILETOPIXMAP, 1, [Define to 1 if you have the `XpmReadFileToPixmap'  function.])
fi

AC_SUBST(LIBCRYPT,   "`echo \"$ac_cv_search_crypt\"          | $SED -e 's/^none required$//g' -e 's/^no$//g'`")
AC_SUBST(LIBDL,      "`echo \"$ac_cv_search_dlopen\"         | $SED -e 's/^none required$//g' -e 's/^no$//g'`")
AC_SUBST(LIBGGI,     "`echo \"$ac_cv_search_ggiOpen\"        | $SED -e 's/^none required$//g' -e 's/^no$//g'`")
AC_SUBST(LIBGPM,     "`echo \"$ac_cv_search_Gpm_Open\"       | $SED -e 's/^none required$//g' -e 's/^no$//g'`")
AC_SUBST(LIBSOCK,    "`echo \"$ac_cv_search_gethostbyname\"  | $SED -e 's/^none required$//g' -e 's/^no$//g'` `echo \"$ac_cv_search_socket\" | $SED -e 's/^none required$//g' -e 's/^no$//g'` `echo \"$ac_cv_search_connect\"  | $SED -e 's/^none required$//g' -e 's/^no$//g'`")
AC_SUBST(LIBPTHREAD, "`echo \"$ac_cv_search_pthread_create\" | $SED -e 's/^none required$//g' -e 's/^no$//g'`")
AC_SUBST(LIBTERMCAP, "`echo \"$ac_cv_search_tgetent\"        | $SED -e 's/^none required$//g' -e 's/^no$//g'`")
AC_SUBST(LIBXPM,     "`echo \"$ac_cv_search_XpmReadFileToPixmap\"  | $SED -e 's/^none required$//g' -e 's/^no$//g'`")
AC_SUBST(LIBZ,       "`echo \"$ac_cv_search_deflate\"        | $SED -e 's/^none required$//g' -e 's/^no$//g'`")

AC_SUBST(LIBTUTF,    '$(top_builddir)/libs/libTutf/libTutf.la')
AC_SUBST(LIBTW,      '$(top_builddir)/libs/libTw/libTw.la')

LIBS=

TW_FIX_CONFIG

AM_CONDITIONAL(LIBHW_DISPLAY_la, [test "$enable_hw_display" = yes])
AM_CONDITIONAL(LIBHW_GFX_la,     [test "$enable_hw_gfx"     = yes])
AM_CONDITIONAL(LIBHW_GGI_la,     [test "$enable_hw_ggi"     = yes])
AM_CONDITIONAL(LIBHW_TTY_la,     [test "$enable_hw_tty"     = yes])
AM_CONDITIONAL(LIBHW_TWIN_la,    [test "$enable_hw_twin"    = yes])
AM_CONDITIONAL(LIBHW_X11_la,     [test "$enable_hw_x11"     = yes])
AM_CONDITIONAL(LIBRCPARSE_la,    [test "$enable_wm_rc"      = yes])
AM_CONDITIONAL(LIBSOCKET_la,     [test "$enable_socket"     = yes])
AM_CONDITIONAL(LIBTERM_la,       [test "$enable_term"       = yes])
AM_CONDITIONAL(LIBWM_la,         [test "$enable_wm"         = yes])

AC_CONFIG_FILES([
      Makefile clients/Makefile clients/mapscrn/Makefile contrib/Makefile contrib/twcd/Makefile docs/Makefile \
      libs/Makefile libs/libTutf/Makefile libs/libTw/Makefile libs/libltdl/Makefile \
      server/Makefile server/HW/Makefile themes/Makefile themes/hw_gfx/Makefile])

AC_OUTPUT

function tw_cfg()
{
  if test "$2" = yes; then
    echo "#define $1" >> include/twconfig.h
  else
    echo "#undef  $1" >> include/twconfig.h
  fi
}

TW_OUTPUT

