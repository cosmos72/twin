#
# Makefile for various libTT display targets
#
TOPDIR=../../..
THISDIR=$(TOPDIR)/libs/libTT/HW

SUBDIRS=

include ../makeversion

all: everything

BINS=
MODBINS=
ARLIBS=
COLLECTS=hw_u.lst

ALLBINS=$(ARLIBS) $(BINS) $(MODBINS) $(COLLECTS)
ALLBINS_FULL=hw.lst hw_u.lst hw_twin.so.$(VERSION) hw_gtk.so.$(VERSION)

HWOBJS=
HWOBJS_u=
TWINOBJS=hw_twin_m4.o
TWINOBJS_u=hw_twin_m4_u.o
GTKOBJS=hw_gtk_m4.o
GTKOBJS_u=hw_gtk_m4_u.o

OBJS_hw.lst=$(HWOBJS)
OBJS_hw_u.lst=$(HWOBJS_u)
OBJS_hw_twin.so.$(VERSION)=$(TWINOBJS)
OBJS_hw_gtk.so.$(VERSION)=$(GTKOBJS)

CC_FLAGS+=-I.. -D_REENTRANT

CC_FLAGS_hw_gtk_m4.o+=-fPIC $(CF_HEADERGTK)
CC_FLAGS_hw_gtk_m4_u.o+=$(CF_HEADERGTK)
LD_FLAGS_hw_gtk.so.$(VERSION)=$(LD_FLAGS_SOLIB) $(LD_LIBGTK)

CC_FLAGS_hw_twin_m4.o+=-fPIC
LD_FLAGS_hw_twin.so.$(VERSION)=$(LD_FLAGS_SOLIB) $(LD_LIBTW)

-include $(TOPDIR)/conf/conf.current
-include .modules
-include .depend
-include $(TOPDIR)/.hdepend

ifeq ($(CONF__SHLIBS),y)
  COLLECTS+=hw.lst
endif

#
# This is globally needed by all libTT sources
#
ifeq ($(CONF_SOCKET_PTHREADS),y)
  CC_FLAGS+=-DCONF_SOCKET_PTHREADS
endif

ifeq ($(CONF_TT_HW_TWIN),y)
  HWOBJS+=$(TWINOBJS)
  HWOBJS_u+=$(TWINOBJS_u)
else
  ifeq ($(CONF_TT_HW_TWIN),m)
    MODBINS+=hw_twin.so.$(VERSION)
  else
    EXCL_OBJS+=$(TWINOBJS)
  endif
endif

ifeq ($(CONF_TT_HW_GTK),y)
  HWOBJS+=$(GTKOBJS)
  HWOBJS_u+=$(GTKOBJS_u)
else
  ifeq ($(CONF_TT_HW_GTK),m)
    MODBINS+=hw_gtk.so.$(VERSION)
  else
    EXCL_OBJS+=$(GTKOBJS)
  endif
endif


ifeq ($(CONF__MODULES),n)
  everything:
	@rm -f $(filter-out $(ALLBINS), $(ALLBINS_FULL)) ; \
	 $(ECHO) ; \
	 $(ECHO) "You disabled \"Enable modules (ELF)\", so only" ; \
	 $(ECHO) "the static display targets $(COLLECTS) will be compiled!" ; \
	 $(ECHO)
endif


#
# include the do-everything file.
# Must correctly set all CC_FLAGS* and LD_FLAGS* before including this!
#
include $(TOPDIR)/makerules

everything: $(ALLBINS)

hw_%_m4.c: m4/hw.m4h hw_%_c $(wildcard m4/*.m4h $(TOPDIR)/include/m4/*.m4h)
	$(M4) '-Dtarget=$*' -I$(TOPDIR)/include $< > $@


#
# All done. Now just set a few special targets
#

do-autogen: $(patsubst hw_%_c,hw_%_m4.c,$(wildcard hw_*_c))

install: install-real

install-fake:
	@echo ; \
	 echo "libTT display targets will not be installed," ; \
	 echo "since they are still experimental." ; \
	 echo

install-real:
	if [ "$(MODBINS)" ]; then \
	  $(INSTALL) -d $(DESTDIR)$(libdir)/TT/HW; \
	  $(INSTALL) $(MODBINS) $(DESTDIR)$(libdir)/TT/HW; \
	fi
	
clean:
	rm -f .*.flags .*.link .*.arlib .*.collect hw_*.o hw_*.so.* hw*.lst


