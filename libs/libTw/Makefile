#
# Makefile for libTw, the library that talks to Twin
#
TOPDIR=../..
SUBDIRS=

-include $(TOPDIR)/makeautoconf

LDFLAGS+=$(LD_LIBSOCKET)

all: everything

VERSION=3.0.1
SO_VER=3

BINS=libTw.so.$(VERSION)
ARLIBS=libTw.a



OBJS_libTw.so.$(VERSION)=libTw.o avl.o md5.o missing.o
OBJS_libTw.a=libTw_u.o avl_u.o md5_u.o missing_u.o

CFLAGS_libTw.o+=-fPIC -D_REENTRANT
CFLAGS_avl.o+=-fPIC -D_REENTRANT
CFLAGS_md5.o+=-fPIC -D_REENTRANT
CFLAGS_missing.o+=-fPIC -D_REENTRANT
CFLAGS_libTw_u.o+=-D_REENTRANT
CFLAGS_avl_u.o+=-D_REENTRANT
CFLAGS_md5_u.o+=-D_REENTRANT
CFLAGS_missing_u.o+=-D_REENTRANT

LDFLAGS_libTw.so.$(VERSION)=$(LDFLAGS_SOLIB)libTw.so.$(SO_VER)

-include $(TOPDIR)/conf/conf.current
-include .modules
-include .depend

ifneq ($(CONF_SOCKET),n)
  # do not link against libpthread: let client programs specify it explicitly
  # if they want to use threads, so that if they don't,
  # fallback, do-nothing functions will be picked from libc (if they exist)
  # avoiding overhead.
  #
  ifeq ($(CONF_SOCKET_PTHREADS),y)
    LDFLAGS_libTw.so.$(VERSION)+=$(LD_LIBPTHREAD_WEAK)
  endif
  everything: lib
  install: install-real
  ifeq ($(CONF_SOCKET_GZ),y)
    LDFLAGS_libTw.so.$(VERSION)+=-lz
  endif
else
  everything: warn
  install: warn
  warn:
	@$(ECHO) ; \
	 $(ECHO) "You disabled \"Support remote socket connections\"," ; \
	 $(ECHO) "so the library libTw.so will NOT be compiled!" ; \
	 $(ECHO)
endif


#
# include the do-everything file.
# Must correctly set all CFLAGS* and LDFLAGS* before including this!
#
include $(TOPDIR)/makerules



#
# handy targets
#
ifeq ($(CONF__SHLIBS),y)
  lib: $(ARLIBS) $(BINS) links
else
  lib: $(ARLIBS)
	@$(ECHO) ; \
	 $(ECHO) "You disabled \"Enable shared libraries (ELF)\"," ; \
	 $(ECHO) "so only the static library libTw.a will be compiled!" ; \
	 $(ECHO)
endif

links: libTw.so.$(SO_VER) libTw.so

libTw.so.$(SO_VER):
	$(LN) libTw.so.$(VERSION) libTw.so.$(SO_VER)

libTw.so:
	$(LN) libTw.so.$(SO_VER) libTw.so

libTw_u.c avl_u.c md5.c md5_u.c missing.c missing_u.c:
	$(LN) libTw.c libTw_u.c && \
	$(LN) avl.c avl_u.c && \
	$(LN) $(TOPDIR)/server/md5.c md5.c && \
	$(LN) $(TOPDIR)/server/md5.c md5_u.c && \
	$(LN) $(TOPDIR)/server/missing.c missing.c && \
	$(LN) $(TOPDIR)/server/missing.c missing_u.c

autogen: paranoiam4.h libTw1m4.h libTw2m4.h

install-headers:
	$(INSTALL) -d $(DESTDIR)$(includedir)/Tw && \
	$(INSTALL-TXT) $(TOPDIR)/include/Tw/*.h $(DESTDIR)$(includedir)/Tw

install-real: install-headers
	$(INSTALL) -d $(DESTDIR)$(libdir) && \
	$(INSTALL) libTw.a $(DESTDIR)$(libdir) && \
	if [ -f libTw.so.$(VERSION) ]; then \
		$(INSTALL) libTw.so.$(VERSION) $(DESTDIR)$(libdir) && \
		$(LN) libTw.so.$(VERSION) $(DESTDIR)$(libdir)/libTw.so.$(SO_VER) && \
		$(LN) libTw.so.$(SO_VER)  $(DESTDIR)$(libdir)/libTw.so; \
	fi

clean:
	rm -f .*.flags .*.link .*.arlib libTw.so* libTw.a $(_ALLOBJS)

