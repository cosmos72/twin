lib_LTLIBRARIES = libtstl.la

libtstl_la_SOURCES = alloc.cpp chars.cpp err.cpp fmt.cpp string.cpp to_chars.cpp utf8.cpp

# set version info to 4:0:3 before release, due to added Chars::trim0()

libtstl_la_LDFLAGS = -version-info 3:0:2 -no-undefined
libtstl_la_LIBADD  = $(LIBDL)

AM_CPPFLAGS=-I$(top_srcdir)/include
