#
# Makefile for libTT, `The Text Toolkit' library sitting on top of libTw
#
TOPDIR=../..
THISDIR=$(TOPDIR)/libs/libTT

SUBDIRS=HW

all: everything

include makeversion

LINKS=libTT.so.$(SO_VER) libTT.so

libTT_SO:=libTT.so.$(VERSION)

BINS=$(libTT_SO)
MODBINS=
ARLIBS=libTT.a
COLLECTS=
ALLBINS=$(BINS) $(MODBINS) $(ARLIBS) $(COLLECTS)


HWOBJS=HW/hw.lst
HWOBJS_u=HW/hw_u.lst

OBJS_$(libTT_SO)=libTT.o missing.o $(HWOBJS)
OBJS_libTT.a=libTT_u.o missing_u.o $(HWOBJS_u)

CC_FLAGS_libTT.o+=-fPIC
CC_FLAGS+=-I. -D_REENTRANT 

LD_FLAGS_$(libTT_SO)=$(LD_FLAGS_SOLIB_NAME)libTT.so.$(SO_VER)

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
  ifeq ($(CONF__UNICODE),y)
    LD_FLAGS_$(libTT_SO)+=$(LD_LIBTUTF)
  endif
else
  everything: warn
  install: warn
  warn:
	@rm -f $(ALLBINS) $(LINKS); \
	 $(ECHO) ; \
	 $(ECHO) "You disabled \"Support remote socket connections\"," ; \
	 $(ECHO) "so the library libTT.so will NOT be compiled!" ; \
	 $(ECHO)
endif

ifeq ($(CONF_TT_HW_TWIN),y)
  LD_FLAGS_$(libTT_SO)+=$(LD_LIBTW)
  EXCL_OBJS+=avl.o avl_u.o
else
  # must manually provide AVL trees.
  OBJS_$(libTT_SO)+=avl.o
  OBJS_libTT.a+=avl_u.o
endif

ifeq ($(CONF_TT_HW_GTK),y)
  LD_FLAGS_$(libTT_SO)+=$(LD_LIBGTK)
endif


ifneq ($(libdir),)
  CC_FLAGS_libTT.o+=-DLIBDIR=\"$(libdir)\"
  CC_FLAGS_libTT_u.o+=-DLIBDIR=\"$(libdir)\"
endif


#
# handy targets
#

ifeq ($(CONF__SHLIBS),y)
  lib: $(ALLBINS) $(LINKS)
else
  lib: $(ARLIBS) $(MODBINS)
	@rm -f $(filter-out $(ARLIBS) $(MODBINS), $(ALLBINS)) $(LINKS) ; \
	 $(ECHO) ; \
	 $(ECHO) "You disabled \"Enable shared libraries (ELF)\"," ; \
	 $(ECHO) "so only the static library libTT.a will be compiled!" ; \
	 $(ECHO)
endif


#
# generate defsm4.h *before* entering HW/ subdirectory, otherwise
# wrong depends will be generated there since defsm4.h is missing
#
HW/hw_%_m4.c: defsm4.h force-remake 
	$(MAKE) -C HW $(patsubst HW/%,%,$@)

#$(HWOBJS) $(HWOBJS_u): dir-HW

HW/%: dir-HW
	@:

#
# include the do-everything file.
# Must correctly set all CC_FLAGS* and LD_FLAGS* before including this!
#
include $(TOPDIR)/makerules


#
# All done. Now just set a few special targets
#

do-autogen: defsm4.h

missing.c:
	$(LN) $(TOPDIR)/server/$@ $@

avl.c:
	$(LN) $(TOPDIR)/libs/libTw/$@ $@

links: $(LINKS)

modules: $(MODBINS)

libTT.so.$(SO_VER):
	$(LN) $(libTT_SO) libTT.so.$(SO_VER)

libTT.so:
	$(LN) libTT.so.$(SO_VER) libTT.so

install-fake:
	@echo ; \
	 echo "libTT will not be installed, since it is still experimental." ; \
	 echo

install-headers:
	$(INSTALL) -d $(DESTDIR)$(includedir)/TT && \
	$(INSTALL-TXT) $(TOPDIR)/include/TT/*.h $(DESTDIR)$(includedir)/TT

install-libdir:
	$(INSTALL) -d $(DESTDIR)$(libdir)

install-$(libTT_SO):
	$(INSTALL) $(libTT_SO) $(DESTDIR)$(libdir) && \
	$(LN) $(libTT_SO) $(DESTDIR)$(libdir)/libTT.so.$(SO_VER) && \
	$(LN) libTT.so.$(SO_VER)  $(DESTDIR)$(libdir)/libTT.so

install-libTT.a:
	$(INSTALL-TXT) $(ARLIBS) $(DESTDIR)$(libdir)

install-real: install-headers install-libdir $(foreach B, $(ALLBINS), install-$(B))

	
clean:
	rm -f .*.flags .*.link .*.arlib libTT.so* libTT.a *.o

