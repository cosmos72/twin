#
# Makefile for twin hw drivers
#
#

TOPDIR=../..
THISDIR=$(TOPDIR)/server/HW

SUBDIRS=

#
# where to install modules (relative to $(libdir))
#
INSTALLSUBDIR=/twin/modules/HW

include $(TOPDIR)/include/makeversion
-include $(TOPDIR)/makeautoconf

BASEOBJS=
GFXOBJS=hw_gfx.o
GFXOBJS-L=hw_gfx.lo
X11OBJS=hw_X11.o
X11OBJS-L=hw_X11.lo
DISPLAYOBJS=hw_display.o
DISPLAYOBJS-L=hw_display.lo
TWINOBJS=hw_twin.o
TWINOBJS-L=hw_twin.lo
TTYOBJS=hw_tty.o
TTYOBJS-L=hw_tty.lo
GGIOBJS=hw_ggi.o
GGIOBJS-L=hw_ggi.lo
ALLOBJS:=$(BASEOBJS) $(GFXOBJS) $(GFXOBJS-L) $(X11OBJS) $(X11OBJS-L) \
  $(DISPLAYOBJS) $(DISPLAYOBJS-L) $(TWINOBJS) $(TWINOBJS-L) $(TTYOBJS) $(TTYOBJS-L) \
  $(GGIOBJS) $(GGIOBJS-L)

OBJS_hw.lst=$(BASEOBJS)
OBJS_hw_display.lst=
OBJS_hw_gfx.la=$(GFXOBJS-L)
OBJS_hw_X11.la=$(X11OBJS-L)
OBJS_hw_display.la=$(DISPLAYOBJS-L)
OBJS_hw_twin.la=$(TWINOBJS-L)
OBJS_hw_tty.la=$(TTYOBJS-L)
OBJS_hw_ggi.la=$(GGIOBJS-L)

BINS=
MODS=
SHLIBS=
ARLIBS=
CLLIBS=hw.lst hw_display.lst


CC_FLAGS+=-I..

CC_FLAGS.lo+=-DTHIS_MODULE

CC_FLAGS_hw_gfx.o+=$(CF_HEADERGFX)
CC_FLAGS_hw_X11.o+=$(CF_HEADERX11)

LD_FLAGS_hw_gfx.la=$(LD_LIBGFX) 
LD_FLAGS_hw_X11.la=$(LD_LIBX11)
LD_FLAGS_hw_twin.la=$(LD_LIBTW)
LD_FLAGS_hw_ggi.la=-lggi

ifneq ($(datadir),)
  CC_FLAGS_hw_gfx.o+=-DDATADIR=\"$(datadir)\"
endif

#
# target
#
all: everything

#
# read configuration
#
-include $(TOPDIR)/conf/conf.current
-include .modules
-include .depend
-include $(TOPDIR)/.hdepend

#
# set variables according conf.current
# and group together all *.o objects needed by each binary
#

ifeq ($(CONF__ALLOC),y)
  CC_FLAGS+=-DCONF__ALLOC
endif

#ifeq ($(CONF__UNICODE),y)
#  LD_FLAGS+=$(LD_LIBTUTF)
#endif

ifeq ($(CONF_HW_TTY),y)
  OBJS_hw.lst+=$(TTYOBJS)
else
  ifeq ($(CONF_HW_TTY),m)
    MODS+=hw_tty.la
    ifeq ($(CONF_HW_TTY_LINUX),y)
      LD_FLAGS_hw_tty.la+=-lgpm
    endif
    ifeq ($(CONF_HW_TTY_TERMCAP),y)
      CC_FLAGS_hw_tty.o+=-I/usr/include/ncurses
      ifeq ($(CONF_HW_TTY_LINUX),y)
        ifeq ($(LD_LIBTERMCAP_IN_LIBGPM),)
          LD_FLAGS_hw_tty.la+=$(LD_LIBTERMCAP)
	# else necessary libs are already pulled in with -lgpm above
	endif
      else
        LD_FLAGS_hw_tty.la+=$(LD_LIBTERMCAP)
      endif
    endif
  endif
endif

ifeq ($(CONF_HW_GFX),y)
  OBJS_hw.lst+=$(GFXOBJS)
else
  ifeq ($(CONF_HW_GFX),m)
    MODS+=hw_gfx.la
  endif
endif

ifeq ($(CONF_HW_X11),y)
  OBJS_hw.lst+=$(X11OBJS)
else
  ifeq ($(CONF_HW_X11),m)
    MODS+=hw_X11.la
  endif
endif

ifneq ($(CONF_SOCKET),n)
  ifeq ($(CONF_HW_TWIN),y)
    OBJS_hw.lst+=$(TWINOBJS)
  else
    ifeq ($(CONF_HW_TWIN),m)
      MODS+=hw_twin.la
    endif
  endif
  
  #
  # do NOT add $(DISPLAYOBJS) to hw.lst, since twdisplay does not link with it
  #
  ifeq ($(CONF_HW_DISPLAY),y)
    OBJS_hw_display.lst+=$(DISPLAYOBJS)
  else
    ifeq ($(CONF_HW_DISPLAY),m)
      MODS+=hw_display.la
    endif
  endif
endif

ifeq ($(CONF_HW_GGI),y)
  OBJS_hw.lst+=$(GGIOBJS)
else
  ifeq ($(CONF_HW_GGI),m)
    MODS+=hw_ggi.la
  endif
endif

#
# common rules
#
include $(TOPDIR)/makerules

#
# All done. Now just set a few special targets
#

everything: $(BINS) $(MODS) $(SHLIBS) $(ARLIBS) $(CLLIBS) links

links: $(subst hw_X11.la,hw_X.so.$(SHLIBS_VER),$(filter hw_X11.la,$(MODS)))


modules: $(MODS) links

hw_X.so.$(SHLIBS_VER):
	$(LN) hw_X11.so.$(SHLIBS_VER) $@

clean:
	rm -f .*.flags .*.bin .*.mod .*.arlib .*.cllib *.lst *.o *.lo *.la *.so *.so.* \
	rm -fr .libs

install-dir:
	$(INSTALL-DIR) $(DESTDIR)$(libdir)/twin/modules/HW

install-mod-link-hw_X.la:
	$(LN) hw_X11.so.$(SHLIBS_VER) $(DESTDIR)$(libdir)/twin/modules/HW/hw_X.so.$(SHLIBS_VER)

install-mod-hw_X11.la: install-mod-link-hw_X.la

install: install-dir $(foreach M,$(MODS),install-mod-$(M))

