lib_LTLIBRARIES = libTw.la

libTw_la_SOURCES = avl.c libTw.c libTw2_i386.S md5.c missing.c util.c
libTw_la_LDFLAGS = -version-info 11:4:7 -no-undefined
libTw_la_LIBADD  = $(LIBSOCK) $(LIBZ)

AM_CPPFLAGS=-I$(top_srcdir)/include
