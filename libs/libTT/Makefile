#
# Makefile for libTT, `The Text Toolkit' library sitting on top of libTw
#
TOPDIR=../..
THISDIR=$(TOPDIR)/libs/libTT

SUBDIRS=HW

include $(TOPDIR)/include/TT/makeversion
-include $(TOPDIR)/makeautoconf

all: everything

BINS=
SHLIBS=libTT.la
ARLIBS=libTT.a
COLLECTS=
#
# we MUST place static library first, as libtool also links
# *.o files to *.lo ones if they don't already exist
#
ALLBINS=libTT.a libTT.la


AVLOBJS=avl.o
AVLOBJS_l=avl.lo

HWOBJS=HW/hw.lst
HWOBJS_l=HW/hw_l.lst

OBJS_libTT.a=libTT.o mem.o field.o handy.o wrap.o missing.o $(HWOBJS)
OBJS_libTT.la=libTT.lo mem.lo field.lo handy.lo wrap.lo missing.lo $(HWOBJS_l)

ALLOBJS=$(AVLOBJS) $(AVLOBJS_l) $(HWOBJS) $(HWOBJS_l) $(OBJS_libTT.a) $(OBJS_libTT.la)


CC_FLAGS+=-I. -D_REENTRANT 

-include $(TOPDIR)/conf/conf.current
-include .modules
-include .depend
-include $(TOPDIR)/.hdepend

ifeq ($(CONF_TT),y)
  ifeq ($(CONF__SHLIBS),y)
    everything: $(ALLBINS)
  else
    everything: $(ARLIBS)
	@rm -f $(filter-out $(ARLIBS), $(ALLBINS)) ; \
	 echo ; \
	 echo "You disabled \"Enable shared libraries\"," ; \
	 echo "so only the static library libTT.a will be compiled!" ; \
	 echo
  endif

  install: install-real
else
  everything: warn
  install: warn
  warn:
	@rm -f $(ALLBINS) ; \
	 echo ; \
	 echo "You disabled \"Support Text Toolkit library (libTT)\"," ; \
	 echo "so the library libTT will NOT be compiled!" ; \
	 echo
endif


#
# This is globally needed by all libTT sources
#
ifeq ($(CONF_SOCKET_PTHREADS),y)
  CC_FLAGS+=-DCONF_SOCKET_PTHREADS
endif

ifeq ($(CONF__MODULES),y)
  LD_FLAGS+=$(LD_LIBDL)
endif

ifeq ($(CONF__UNICODE),y)
  LD_FLAGS_libTT.la+=$(LD_LIBTUTF)
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

ifeq ($(CONF_TT_HW_XML),y)
  LD_FLAGS_libTT.la+=$(LD_LIBXML)
endif


ifneq ($(libdir),)
  # also used for libTT.lo
  CC_FLAGS_libTT.o+=-DLIBDIR=\"$(libdir)\"
endif


#
# handy targets
#

#
# generate decls_m4.h *before* entering HW/ subdirectory, otherwise
# wrong depends will be generated there since decls_m4.h is missing
#
HW/%: decls_m4.h dir-HW
	@:

#
# include the do-everything file.
# Must correctly set all CC_FLAGS* and LD_FLAGS* before including this!
#
include $(TOPDIR)/makerules


#
# All done. Now just set a few special targets
#

do-autogen: decls_m4.h

missing.c:
	$(LN) $(TOPDIR)/server/$@ $@

avl.c:
	$(LN) $(TOPDIR)/libs/libTw/$@ $@

install-headers:
	$(INSTALL-DIR) $(DESTDIR)$(includedir)/TT && \
	$(INSTALL-TXT) $(TOPDIR)/include/TT/*.h $(DESTDIR)$(includedir)/TT

install-dir:
	$(INSTALL-DIR) $(DESTDIR)$(libdir)


install-real: install-headers install-dir $(foreach B,$(SHLIBS) $(ARLIBS),install-lib-$(B))


clean:
	rm -f .*.flags .*.shlib .*.arlib *.la *.a *.o *.lo *.so *.so.* ; \
	rm -fr .libs

