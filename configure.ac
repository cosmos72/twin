#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([twin], [0.6.2.work], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([server/main.c])
AC_CONFIG_HEADERS([include/config.h])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_AUX_DIR([admin])
AM_INIT_AUTOMAKE([1.15 foreign subdir-objects])

m4_define([AH_HEADER], [config.h]) dnl filename of first AC_CONFIG_HEADERS. works around a bug in current libltdl

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
AC_CHECK_HEADERS([arpa/inet.h asm/page.h crypt.h fcntl.h inttypes.h libintl.h limits.h locale.h machine/param.h malloc.h memory.h netdb.h netinet/in.h shadow.h signal.h stdarg.h stddef.h stdint.h stdlib.h string.h sys/filio.h sys/ioctl.h sys/mman.h sys/param.h sys/select.h sys/socket.h sys/timeb.h termio.h termios.h unistd.h])

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
AC_CHECK_FUNCS([dup2 ftime gethostbyname gethostname getpagesize getspnam gettimeofday memchr memmove memset munmap putenv select setenv setlocale socket strchr strdup strerror strrchr strspn strstr strtol strtoul uname wait4])

dnl crypt() is needed by twdm
AC_CHECK_FUNC(crypt,,AC_CHECK_LIB(crypt,crypt))

dnl -lc_r is needed on FreeBSD to use pthread functions
AC_CHECK_FUNC(pthread_self,,AC_CHECK_LIB(pthread,pthread_self,,AC_CHECK_LIB(c_r,pthread_self)))

AC_CHECK_LIB(z, deflate)
AC_CHECK_LIB(gpm, Gpm_Open)
AC_CHECK_LIB(ggi, ggiOpen)

dnl libgpm is often linked against ncurses...
dnl no need for additional libraries in that case
AC_CHECK_LIB(gpm,tgetent,,AC_CHECK_LIB(tinfo,tgetent,,AC_CHECK_LIB(ncurses,tgetent,,AC_CHECK_LIB(termcap,tgetent))))

# Output
LIBS=

AC_CONFIG_FILES([Makefile clients/Makefile clients/mapscrn/Makefile contrib/Makefile contrib/twcd/Makefile docs/Makefile libs/Makefile libs/libTT/Makefile libs/libTT/HW/Makefile libs/libTutf/Makefile libs/libTw/Makefile libs/libltdl/Makefile server/Makefile server/extensions/Makefile server/HW/Makefile themes/Makefile themes/hw_gfx/Makefile])
AC_OUTPUT
TW_OUTPUT

