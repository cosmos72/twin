#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([twin], [0.6.2.work], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([server/main.c])
AC_CONFIG_HEADERS([include/autoconf.h])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_AUX_DIR([admin])
AM_INIT_AUTOMAKE

LT_INIT
LT_CONFIG_LTDL_DIR([libs/libltdl])
LTDL_INIT

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_SED

# Checks for libraries.

# Checks for header files.
AC_PATH_X
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([arpa/inet.h fcntl.h inttypes.h libintl.h limits.h locale.h malloc.h memory.h netdb.h netinet/in.h shadow.h stddef.h stdint.h stdlib.h string.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h sys/timeb.h termio.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UID_T

AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_CHOWN
AC_FUNC_FORK
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_WAIT3
AC_CHECK_FUNCS([dup2 ftime gethostbyname gethostname getpagesize getspnam gettimeofday memchr memmove memset munmap putenv select setenv setlocale socket strchr strdup strerror strrchr strspn strstr strtol strtoul uname])

# Output
AC_CONFIG_FILES([Makefile clients/Makefile clients/mapscrn/Makefile contrib/Makefile contrib/twcd/Makefile docs/Makefile libs/Makefile libs/libTT/Makefile libs/libTT/HW/Makefile libs/libTutf/Makefile libs/libTw/Makefile server/Makefile server/extensions/Makefile server/HW/Makefile themes/Makefile themes/hw_gfx/Makefile])
AC_OUTPUT
TW_OUTPUT
