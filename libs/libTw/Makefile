#
# Makefile for libTw, the library that talks to Twin
#
TOPDIR=../..
THISDIR=$(TOPDIR)/libs/libTw

SUBDIRS=

include $(TOPDIR)/include/Tw/makeversion
-include $(TOPDIR)/makeautoconf

LD_FLAGS+=$(LD_LIBSOCKET)

all: everything

SHLIBS=libTw.la
ARLIBS=libTw.a

OBJS_libTw.la=libTw.lo avl.lo md5.lo missing.lo
OBJS_libTw.a=libTw.o avl.o md5.o missing.o

CC_FLAGS+=-D_REENTRANT

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
    LD_FLAGS_libTw.la+=$(LD_LIBPTHREAD_WEAK)
  endif
  everything: lib
  install: install-real
  ifeq ($(CONF_SOCKET_GZ),y)
    LD_FLAGS_libTw.la+=-lz
  endif
else
  everything: warn
  install: warn
  warn:
	@rm -f $(ARLIBS) $(SHLIBS) ; \
	 echo ; \
	 echo "You disabled \"Support remote socket connections\"," ; \
	 echo "so the library libTw will NOT be compiled!" ; \
	 echo
endif

ifeq ($(GCC_I386_ASM),yes)
  OBJS_libTw.la+=libTw2_i386.lo
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
  lib: $(ARLIBS) $(SHLIBS)
else
  lib: $(ARLIBS)
	@rm -f $(SHLIBS) ; \
	 echo ; \
	 echo "You disabled \"Enable shared libraries (ELF)\"," ; \
	 echo "so only the static library libTw.a will be compiled!" ; \
	 echo
endif

md5.c:
	$(LN) $(TOPDIR)/server/md5.c $@

missing.c:
	$(LN) $(TOPDIR)/server/missing.c $@

do-autogen: libTw1_m4.h libTw2_m4.h libTw2_i386_m4.h libTw3_m4.h

install-headers:
	$(INSTALL-DIR) $(DESTDIR)$(includedir)/Tw && \
	$(INSTALL-TXT) $(TOPDIR)/include/Tw/*.h $(DESTDIR)$(includedir)/Tw
	
install-dir:
	$(INSTALL-DIR) $(DESTDIR)$(libdir)

install-real: install-headers install-dir $(foreach B,$(SHLIBS) $(ARLIBS),install-lib-$(B))

clean:
	rm -f .*.flags .*.shlib .*.arlib *.a *.la *.o *.lo *.so *.so.* ; \
	rm -fr .libs

