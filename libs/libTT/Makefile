#
# Makefile for libTT, `The Text Toolkit' library sitting on top of libTw
#
TOPDIR=../..
THISDIR=$(TOPDIR)/libs/libTT

SUBDIRS=HW

all: everything

include makeversion

BINS=
SHLIBS=
ARLIBS=libTT.a
COLLECTS=
ALLBINS=libTT.la libTT.a


AVLOBJS=avl.o
AVLOBJS_l=avl.lo

HWOBJS=HW/hw.lst
HWOBJS_l=HW/hw_l.lst

OBJS_libTT.a=libTT.o field.o handy.o wrap.o missing.o $(HWOBJS)
OBJS_libTT.la=libTT.lo field.lo handy.lo wrap.lo missing.lo $(HWOBJS_l)

ALLOBJS=$(AVLOBJS) $(AVLOBJS_l) $(HWOBJS) $(HWOBJS_l) $(OBJS_libTT.a) $(OBJS_libTT.la)


CC_FLAGS+=-I. -D_REENTRANT 

-include $(TOPDIR)/makeautoconf
-include $(TOPDIR)/conf/conf.current
-include .modules
-include .depend
-include $(TOPDIR)/.hdepend

#
# This is globally needed by all libTT sources
#
ifeq ($(CONF_SOCKET_PTHREADS),y)
  CC_FLAGS+=-DCONF_SOCKET_PTHREADS
endif

ifeq ($(CONF__MODULES),y)
  LD_FLAGS+=$(LD_LIBDL)
endif

ifneq ($(CONF_SOCKET),n)
  everything: lib
  install: install-real

  SHLIBS+=libTT.la
  
  ifeq ($(CONF__UNICODE),y)
    LD_FLAGS_libTT.la+=$(LD_LIBTUTF)
  endif
else
  everything: warn
  install: warn
  warn:
	@rm -f $(ALLBINS) ; \
	 echo ; \
	 echo "You disabled \"Support remote socket connections\"," ; \
	 echo "so the library libTT.so will NOT be compiled!" ; \
	 echo
endif

ifeq ($(CONF_TT_HW_TWIN),y)
  LD_FLAGS_libTT.la+=$(LD_LIBTW)
else
  # must manually provide AVL trees.
  OBJS_libTT.la+=$(AVLOBJS_l)
  OBJS_libTT.a+=$(AVLOBJS)
endif

ifeq ($(CONF_TT_HW_GTK),y)
  LD_FLAGS_libTT.la+=$(LD_LIBGTK)
endif


ifneq ($(libdir),)
  CC_FLAGS_libTT.lo+=-DLIBDIR=\"$(libdir)\"
  CC_FLAGS_libTT.o+=-DLIBDIR=\"$(libdir)\"
endif


#
# handy targets
#

ifeq ($(CONF__SHLIBS),y)
  lib: $(ALLBINS)
else
  lib: $(ARLIBS)
	@rm -f $(filter-out $(ARLIBS), $(ALLBINS)) ; \
	 echo ; \
	 echo "You disabled \"Enable shared libraries (ELF)\"," ; \
	 echo "so only the static library libTT.a will be compiled!" ; \
	 echo
endif


#
# generate defs_m4.h *before* entering HW/ subdirectory, otherwise
# wrong depends will be generated there since defs_m4.h is missing
#
HW/%: defs_m4.h dir-HW
	@:

#
# include the do-everything file.
# Must correctly set all CC_FLAGS* and LD_FLAGS* before including this!
#
include $(TOPDIR)/makerules


#
# All done. Now just set a few special targets
#

do-autogen: defs_m4.h

missing.c:
	$(LN) $(TOPDIR)/server/$@ $@

avl.c:
	$(LN) $(TOPDIR)/libs/libTw/$@ $@

install-headers:
	$(INSTALL-DIR) $(DESTDIR)$(includedir)/TT && \
	$(INSTALL-TXT) $(TOPDIR)/include/TT/*.h $(DESTDIR)$(includedir)/TT

install-dir:
	$(INSTALL-DIR) $(DESTDIR)$(libdir)

install-libTT.la:
	$(INSTALL) $(SHLIBS) $(DESTDIR)$(libdir)

install-libTT.a:
	$(INSTALL-TXT) $(ARLIBS) $(DESTDIR)$(libdir)

install-real: install-headers install-dir $(foreach B, $(SHLIBS) $(ARLIBS), install-$(B))


clean:
	rm -f .*.flags .*.shlib .*.arlib libTT.la libTT.a *.o *.lo ; \
	rm -fr .libs

