#
# Makefile for various libTT display targets
#
TOPDIR=../../..
THISDIR=$(TOPDIR)/libs/libTT/HW

#
# where to install modules (relative to $(libdir))
#
INSTALLSUBDIR=/TT/HW

SUBDIRS=

include $(TOPDIR)/include/TT/makeversion
-include $(TOPDIR)/makeautoconf

all: everything

BINS=
SHLIBS=
ARLIBS=
CLLIBS=hw.lst

ALLBINS=$(BINS) $(MODS) $(SHLIBS) $(ARLIBS) $(CLLIBS)
ALLBINS_FULL=hw.lst hw_l.lst hw_gtk.la hw_twin.la hw_xml.la

HWOBJS=hw_null_m4.o
HWOBJS_l=hw_null_m4.lo

GTKOBJS=hw_gtk_m4.o
GTKOBJS_l=hw_gtk_m4.lo
TWINOBJS=hw_twin_m4.o
TWINOBJS_l=hw_twin_m4.lo
XMLOBJS=hw_xml_m4.o
XMLOBJS_l=hw_xml_m4.lo

OBJS_hw.lst=$(HWOBJS)
OBJS_hw_l.lst=$(HWOBJS_l)
OBJS_hw_xml.la=$(XMLOBJS_l)
OBJS_hw_gtk.la=$(GTKOBJS_l)
OBJS_hw_twin.la=$(TWINOBJS_l)

ALLOBJS=$(HWOBJS) $(HWOBJS_l) $(XMLOBJS) $(XMLOBJS_l) $(GTKOBJS) $(GTKOBJS_l) $(TWINOBJS) $(TWINOBJS_l)

CC_FLAGS+=-I.. -D_REENTRANT

# also used for hw_gtk_m4.lo
CC_FLAGS_hw_gtk_m4.o+=$(CF_HEADERGTK)
LD_FLAGS_hw_gtk.la=$(LD_LIBGTK)


LD_FLAGS_hw_twin.la=$(LD_LIBTW)


# also used for hw_xml_m4.lo
CC_FLAGS_hw_xml_m4.o+=$(CF_HEADERXML)
LD_FLAGS_hw_xml.la=$(LD_LIBXML)


-include $(TOPDIR)/conf/conf.current
-include .modules
-include .depend
-include $(TOPDIR)/.hdepend

ifeq ($(CONF__SHLIBS),y)
  CLLIBS+=hw_l.lst
endif

#
# This is globally needed by all libTT sources
#
ifeq ($(CONF_SOCKET_PTHREADS),y)
  CC_FLAGS+=-DCONF_SOCKET_PTHREADS
endif

ifeq ($(CONF_TT_HW_TWIN),y)
  HWOBJS+=$(TWINOBJS)
  HWOBJS_l+=$(TWINOBJS_l)
else
  ifeq ($(CONF_TT_HW_TWIN),m)
    MODS+=hw_twin.la
    CC_FLAGS_hw_twin_m4.o+=-DTHIS_MODULE
  endif
endif

ifeq ($(CONF_TT_HW_GTK),y)
  HWOBJS+=$(GTKOBJS)
  HWOBJS_l+=$(GTKOBJS_l)
else
  ifeq ($(CONF_TT_HW_GTK),m)
    MODS+=hw_gtk.la
    CC_FLAGS_hw_gtk_m4.o+=-DTHIS_MODULE
  endif
endif

ifeq ($(CONF_TT_HW_XML),y)
  HWOBJS+=$(XMLOBJS)
  HWOBJS_l+=$(XMLOBJS_l)
else
  ifeq ($(CONF_TT_HW_XML),m)
    MODS+=hw_xml.la
    CC_FLAGS_hw_xml_m4.o+=-DTHIS_MODULE
  endif
endif


ifeq ($(CONF__MODULES),n)
  everything:
	@rm -f $(filter-out $(ALLBINS), $(ALLBINS_FULL)) ; \
	 echo ; \
	 echo "You disabled \"Enable modules (ELF)\", so only" ; \
	 echo "the built-in display targets $(CLLIBS) will be compiled!" ; \
	 echo
endif


#
# include the do-everything file.
# Must correctly set all CC_FLAGS* and LD_FLAGS* before including this!
#
include $(TOPDIR)/makerules

everything: $(ALLBINS) links

hw_%_m4.c: m4/hw.m4h hw_%_c $(wildcard m4/*.m4h $(TOPDIR)/include/m4/*.m4h)
	$(M4) '-Dtarget=$*' -I$(TOPDIR)/include $< > $@


#
# All done. Now just set a few special targets
#

do-autogen: $(patsubst hw_%_c,hw_%_m4.c,$(wildcard hw_*_c))

links: $(patsubst %.la,%.so.$(SHLIBS_VER),$(MODS))

install-dir:
	$(INSTALL-DIR) $(DESTDIR)$(libdir)/$(INSTALLSUBDIR)

install: install-dir $(foreach M,$(MODS),install-mod-$(M))
	
clean:
	rm -f .*.flags .*.mod .*.shlib .*.arlib .*.cllib *.lst *.la *.a *.o *.lo *.so *.so.* ; \
	rm -fr .libs


