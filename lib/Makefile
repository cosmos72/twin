#
# Makefile for Twlib, the library that talks to Twin
#
TOPDIR=..

all: everything

CFLAGS+=-fPIC

VERSION=2.0.0
SO_VER=2

BINS=libTw.so.$(VERSION)
OBJS_libTw.so.$(VERSION)=libTw.o md5.o

LDFLAGS_libTw.so.$(VERSION)=-lc -shared -Wl,-soname,libTw.so.$(SO_VER)

include $(TOPDIR)/conf/config.status
-include .depend
-include .modules

ifneq ($(CONF_SOCKET),n)
  everything: lib links

  ifeq ($(CONF_SOCKET_GZ),y)
    LDFLAGS_libTw.so.$(VERSION)+=-lz
  endif
else
  everything:
	@echo
	@echo "You disabled \"Support remote socket connections\","
	@echo "so the library libTw.so will NOT be compiled!"
	@echo
endif

#
# include the do-everything file
#
include $(TOPDIR)/Rules

#
# handy targets
#
lib: $(BINS)

links: libTw.so.$(SO_VER) libTw.so

libTw.so.$(SO_VER):
	ln -sfn libTw.so.$(VERSION) libTw.so.$(SO_VER)

libTw.so:
	ln -sfn libTw.so.$(SO_VER) libTw.so

clean:
	rm -f .*.flags .*.link libTw.so* $(OBJS)

