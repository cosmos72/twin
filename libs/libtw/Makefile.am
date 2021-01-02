lib_LTLIBRARIES = libtw.la

libtw_la_SOURCES = avl.c libtw.c md5.c missing.c util.c
libtw_la_LDFLAGS = -version-info 5:0:0 -no-undefined
libtw_la_LIBADD  = $(LIBSOCK) $(LIBZ)

AM_CPPFLAGS=-I$(top_srcdir)/include
