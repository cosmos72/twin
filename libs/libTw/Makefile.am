lib_LTLIBRARIES = libTw.la

libTw_la_SOURCES = avl.c libTw.c md5.c missing.c util.c
libTw_la_LDFLAGS = -version-info 5:0:0 -no-undefined
libTw_la_LIBADD  = $(LIBSOCK) $(LIBZ)

AM_CPPFLAGS=-I$(top_srcdir)/include
