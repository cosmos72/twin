#
# Makefile for Twlib, the library that talks to Twin
#
TOPDIR=..

all: everything

CFLAGS+=-fPIC

BINS=libTw.so.1.0.0
OBJS_libTw.so.1.0.0=libTw.o md5.o

LDFLAGS_libTw.so.1.0.0=-shared -Wl,-soname,libTw.so.1

include $(TOPDIR)/conf/config.status
-include .depend
-include .modules

ifneq ($(CONF_SOCKET),n)
  everything: lib links

  ifeq ($(CONF_SOCKET_GZ),y)
    LDFLAGS_libTw.so.1.0.0+=-lz
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

links: libTw.so.1 libTw.so

libTw.so.1:
	ln -sfn libTw.so.1.0.0 libTw.so.1

libTw.so:
	ln -sfn libTw.so.1     libTw.so

clean:
	rm -f .*.flags .*.link libTw.so* $(OBJS)

