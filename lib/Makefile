#
# Makefile for libTw, the library that talks to Twin
#
TOPDIR=..

all: everything

VERSION=2.0.1
SO_VER=2

BINS=libTw.so.$(VERSION)
ARLIBS=libTw.a
OBJS_libTw.so.$(VERSION)=libTw.o md5.o
OBJS_libTw.a=libTw_u.o md5_u.o

CFLAGS_libTw.o+=-fPIC -D_REENTRANT
CFLAGS_md5.o+=-fPIC -D_REENTRANT
CFLAGS_libTw_u.o+=-D_REENTRANT
CFLAGS_md5_u.o+=-D_REENTRANT
LDFLAGS_libTw.so.$(VERSION)=-lc -shared -Wl,-soname,libTw.so.$(SO_VER)

include $(TOPDIR)/conf/config.status
-include .depend
-include .modules

ifneq ($(CONF_SOCKET),n)
  everything: lib
  install: install-real
  ifeq ($(CONF_SOCKET_GZ),y)
    LDFLAGS_libTw.so.$(VERSION)+=-lz
  endif
else
  everything: warn
  install: warn
  warn:
	@echo ; \
	echo "You disabled \"Support remote socket connections\"," ; \
	echo "so the library libTw.so will NOT be compiled!" ; \
	echo
endif

#
# include the do-everything file
#
include $(TOPDIR)/MakeRules

#
# handy targets
#
ifeq ($(CONF__SHLIBS),y)
  lib: $(ARLIBS) $(BINS) links
else
  lib: $(ARLIBS)
	@echo ; \
	echo "You disabled \"Enable shared libraries (ELF)\"," ; \
	echo "so only the static library libTw.a will be compiled!" ; \
	echo
endif

links: libTw.so.$(SO_VER) libTw.so

libTw.so.$(SO_VER):
	$(LN) libTw.so.$(VERSION) libTw.so.$(SO_VER)

libTw.so:
	$(LN) libTw.so.$(SO_VER) libTw.so

install-real:
	if [ -f libTw.so.$(VERSION) ]; then \
	  $(CP) libTw.so.$(VERSION) $(DESTDIR)/lib ; \
	  $(LN) libTw.so.$(VERSION) $(DESTDIR)/lib/libTw.so.$(SO_VER) ; \
	  $(LN) libTw.so.$(SO_VER)  $(DESTDIR)/lib/libTw.so ; \
	else \
	  $(CP) libTw.a $(DESTDIR)/lib ; \
	fi
	
clean:
	rm -f .*.flags .*.link .*.arlib libTw.so* libTw.a $(_ALLOBJS)

