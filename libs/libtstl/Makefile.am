lib_LTLIBRARIES = libtstl.la

libtstl_la_SOURCES = alloc.cpp chars.cpp err.cpp fmt.cpp string.cpp to_chars.cpp utf8.cpp
libtstl_la_LDFLAGS = -version-info 3:0:2 -no-undefined
libtstl_la_LIBADD  = $(LIBDL)

AM_CPPFLAGS=-I$(top_srcdir)/include
