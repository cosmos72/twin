#
# Makefile for twin hw drivers
#
#

TOPDIR=../..
THISDIR=$(TOPDIR)/server/HW

SUBDIRS=

include ../makeversion
-include $(TOPDIR)/makeautoconf

CC_FLAGS+=-I..

BASEOBJS=
TTYOBJS=hw_tty.o
X11OBJS=hw_X11.o
DISPLAYOBJS=hw_display.o
TWINOBJS=hw_twin.o
GGIOBJS=hw_ggi.o
ALLOBJS:=$(BASEOBJS) $(TTYOBJS) $(X11OBJS) $(DISPLAYOBJS) $(TWINOBJS) $(GGIOBJS)

OBJS_hw.a=$(BASEOBJS)
OBJS_hw_tty.so.$(VERSION)=$(TTYOBJS)
OBJS_hw_X11.so.$(VERSION)=$(X11OBJS)
OBJS_hw_display.so.$(VERSION)=$(DISPLAYOBJS)
OBJS_hw_twin.so.$(VERSION)=$(TWINOBJS)
OBJS_hw_ggi.so.$(VERSION)=$(GGIOBJS)

BINS=
MODBINS=
ARLIBS=hw.a

CC_FLAGS_hw_X11.o+=$(CF_HEADERX11)

LD_FLAGS_hw_tty.so.$(VERSION)=$(LD_FLAGS_SOLIB)
LD_FLAGS_hw_X11.so.$(VERSION)=$(LD_FLAGS_SOLIB) $(LD_LIBX11)
LD_FLAGS_hw_display.so.$(VERSION)=$(LD_FLAGS_SOLIB)
LD_FLAGS_hw_twin.so.$(VERSION)=$(LD_FLAGS_SOLIB) $(LD_LIBTW)
LD_FLAGS_hw_ggi.so.$(VERSION)=$(LD_FLAGS_SOLIB) -lggi

#
# target
#
all: $(ARLIBS) modules

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
  OBJS_hw.a+=$(TTYOBJS)
else
  ifeq ($(CONF_HW_TTY),m)
    MODBINS+=hw_tty.so.$(VERSION)
    ifeq ($(CONF_HW_TTY_LINUX),y)
      LD_FLAGS_hw_tty.so.$(VERSION)+=-lgpm
    endif
    ifeq ($(CONF_HW_TTY_TERMCAP),y)
      CC_FLAGS_hw_tty.o+=-I/usr/include/ncurses
      ifeq ($(CONF_HW_TTY_LINUX),y)
        ifeq ($(LD_LIBTERMCAP_IN_LIBGPM),)
          LD_FLAGS_hw_tty.so.$(VERSION)+=$(LD_LIBTERMCAP)
	# else necessary libs are already pulled in with -lgpm above
	endif
      else
        LD_FLAGS_hw_tty.so.$(VERSION)+=$(LD_LIBTERMCAP)
      endif
    endif
  else
    EXCL_OBJS+=$(TTYOBJS)
  endif
endif

ifeq ($(CONF_HW_X11),y)
  OBJS_hw.a+=$(X11OBJS)
else
  ifeq ($(CONF_HW_X11),m)
  
    all: hw_X.so.$(VERSION)
    
    MODBINS+=hw_X11.so.$(VERSION)
    
  else
    EXCL_OBJS+=$(X11OBJS)
  endif
endif

ifneq ($(CONF_SOCKET),n)
  ifeq ($(CONF_HW_TWIN),y)
    OBJS_hw.a+=$(TWINOBJS)
  else
    ifeq ($(CONF_HW_TWIN),m)
      MODBINS+=hw_twin.so.$(VERSION)
    else
      EXCL_OBJS+=$(TWINOBJS)
    endif
  endif
  
  ifeq ($(CONF_HW_DISPLAY),y)
    OBJS_hw.a+=$(DISPLAYOBJS)
  else
    ifeq ($(CONF_HW_DISPLAY),m)
      MODBINS+=hw_display.so.$(VERSION)
    else
      EXCL_OBJS+=$(DISPLAYOBJS)
    endif
  endif
endif

ifeq ($(CONF_HW_GGI),y)
  OBJS_hw.a+=$(GGIOBJS)
else
  ifeq ($(CONF_HW_GGI),m)
    MODBINS+=hw_ggi.so.$(VERSION)
  else
    EXCL_OBJS+=$(GGIOBJS)
  endif
endif

#
# common rules
#
include $(TOPDIR)/makerules

#
# All done. Now just set a few special targets
#

modules: $(MODBINS)

hw_X.so.$(VERSION): hw_X11.so.$(VERSION)
	$(LN) $< $@

clean:
	rm -f .*.flags .*.link .*.arlib hw.a hw_*.so.* $(ALLOBJS)

install:
	if [ "$(MODBINS)" ]; then \
	  $(INSTALL) -d $(DESTDIR)$(libdir)/twin/modules/HW; \
	  $(INSTALL) $(MODBINS) $(DESTDIR)$(libdir)/twin/modules/HW; \
	  $(LN) hw_X11.so.$(VERSION) $(DESTDIR)$(libdir)/twin/modules/HW/hw_X.so.$(VERSION); \
	fi
