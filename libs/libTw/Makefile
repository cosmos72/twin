#
# Makefile for libTw, the library that talks to Twin
#
TOPDIR=../..
THISDIR=$(TOPDIR)/libs/libTw

SUBDIRS=

-include $(TOPDIR)/makeautoconf

LD_FLAGS+=$(LD_LIBSOCKET)

all: everything

VERSION=3.0.5
SO_VER=3

BINS=libTw.so.$(VERSION)
ARLIBS=libTw.a

LINKS=libTw.so.$(SO_VER) libTw.so


OBJS_libTw.so.$(VERSION)=libTw.o avl.o md5.o missing.o
OBJS_libTw.a=libTw_u.o avl_u.o md5_u.o missing_u.o

CC_FLAGS_libTw.o+=-fPIC
CC_FLAGS_avl.o+=-fPIC
CC_FLAGS_md5.o+=-fPIC
CC_FLAGS_missing.o+=-fPIC
CC_FLAGS+=-D_REENTRANT

LD_FLAGS_libTw.so.$(VERSION)=$(LD_FLAGS_SOLIB_NAME)libTw.so.$(SO_VER)

-include $(TOPDIR)/conf/conf.current
-include .modules
-include .depend
-include $(TOPDIR)/.hdepend

ifneq ($(CONF_SOCKET),n)
  # do not link against libpthread: let client programs specify it explicitly
  # if they want to use threads, so that if they don't,
  # fallback, do-nothing functions will be picked from libc (if they exist)
  # avoiding overhead.
  #
  ifeq ($(CONF_SOCKET_PTHREADS),y)
    LD_FLAGS_libTw.so.$(VERSION)+=$(LD_LIBPTHREAD_WEAK)
  endif
  everything: lib
  install: install-real
  ifeq ($(CONF_SOCKET_GZ),y)
    LD_FLAGS_libTw.so.$(VERSION)+=-lz
  endif
else
  everything: warn
  install: warn
  warn:
	@rm -f (ARLIBS) $(BINS) $(LINKS) ; \
	 $(ECHO) ; \
	 $(ECHO) "You disabled \"Support remote socket connections\"," ; \
	 $(ECHO) "so the library libTw.so will NOT be compiled!" ; \
	 $(ECHO)
endif

#
# hand-optimized assembler functions for i386 with gcc
# 
# makerules autodetect that libTw2_i386.o must be compiled from libTw2_i386.S
# and also generates dependencies scanning libTw2_i386.S :-)
#
ifeq ($(CC_ARCH_I386_ASM),y)
  CC_FLAGS_libTw.o+=-DTW_HAVE_I386_ASM
  CC_FLAGS_libTw_u.o+=-DTW_HAVE_I386_ASM
  CC_FLAGS_libTw2_i386.o+=-DTW_HAVE_I386_ASM
    
  OBJS_libTw.so.$(VERSION)+=libTw2_i386.o
  OBJS_libTw.a+=libTw2_i386.o
endif


#
# include the do-everything file.
# Must correctly set all CC_FLAGS* and LD_FLAGS* before including this!
#
include $(TOPDIR)/makerules


#
# handy targets
#
ifeq ($(CONF__SHLIBS),y)
  lib: $(ARLIBS) $(BINS) links
else
  lib: $(ARLIBS)
	@rm -f $(BINS) $(LINKS) ; \
	 $(ECHO) ; \
	 $(ECHO) "You disabled \"Enable shared libraries (ELF)\"," ; \
	 $(ECHO) "so only the static library libTw.a will be compiled!" ; \
	 $(ECHO)
endif

links: $(LINKS)

libTw.so.$(SO_VER):
	$(LN) libTw.so.$(VERSION) $@

libTw.so:
	$(LN) libTw.so.$(SO_VER) $@

md5.c:
	$(LN) $(TOPDIR)/server/md5.c $@

missing.c:
	$(LN) $(TOPDIR)/server/missing.c $@

do-autogen: paranoiam4.h libTw1m4.h libTw2m4.h libTw2_i386m4.h libTw3m4.h

install-headers:
	$(INSTALL) -d $(DESTDIR)$(includedir)/Tw && \
	$(INSTALL-TXT) $(TOPDIR)/include/Tw/*.h $(DESTDIR)$(includedir)/Tw

install-real: install-headers
	$(INSTALL) -d $(DESTDIR)$(libdir) && \
	$(INSTALL-TXT) $(ARLIBS) $(DESTDIR)$(libdir) && \
	if [ -f libTw.so.$(VERSION) ]; then \
		$(INSTALL) libTw.so.$(VERSION) $(DESTDIR)$(libdir) && \
		$(LN) libTw.so.$(VERSION) $(DESTDIR)$(libdir)/libTw.so.$(SO_VER) && \
		$(LN) libTw.so.$(SO_VER)  $(DESTDIR)$(libdir)/libTw.so; \
	fi

clean:
	rm -f .*.flags .*.link .*.arlib libTw.so* libTw.a *.o

