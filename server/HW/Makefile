#
# Makefile for twin - Text mode WINdow manager
#
#

TOPDIR=../..
SUBDIRS=

-include $(TOPDIR)/makeautoconf

CFLAGS+=-I..

BASEOBJS=
TTYOBJS=hw_tty.o
X11OBJS=hw_X11.o
DISPLAYOBJS=hw_display.o
TWINOBJS=hw_twin.o
GGIOBJS=hw_ggi.o
ALLOBJS:=$(BASEOBJS) $(TTYOBJS) $(X11OBJS) $(DISPLAYOBJS) $(TWINOBJS) $(GGIOBJS)

OBJS_hw.a:=$(BASEOBJS)
OBJS_hw_tty.so=$(TTYOBJS)
OBJS_hw_X11.so=$(X11OBJS)
OBJS_hw_display.so=$(DISPLAYOBJS)
OBJS_hw_twin.so=$(TWINOBJS)
OBJS_hw_ggi.so=$(GGIOBJS)

BINS=
MODBINS=
ARLIBS=hw.a

CFLAGS_hw_X11.o+=$(CF_HEADERX11)

LDFLAGS_hw_tty.so=-lc -shared
LDFLAGS_hw_X11.so=-lc -shared $(LD_LIBX11)
LDFLAGS_hw_display.so=-lc -shared
LDFLAGS_hw_twin.so=-lc -shared $(LIBTW)
LDFLAGS_hw_ggi.so=-lc -shared -lggi

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

#
# set variables according conf.current
# and group together all *.o objects needed by each binary
#

ifeq ($(CONF__ALLOC),y)
  CFLAGS+=-DCONF__ALLOC
endif

#ifeq ($(CONF__UNICODE),y)
#  LDFLAGS+=$(LIBTUTF)
#endif

ifeq ($(CONF_HW_TTY),y)
  OBJS_hw.a+=$(TTYOBJS)
else
  ifeq ($(CONF_HW_TTY),m)
    MODBINS+=hw_tty.so
    ifeq ($(CONF_HW_TTY_LINUX),y)
      LDFLAGS_hw_tty.so+=-lgpm
    endif
    ifeq ($(CONF_HW_TTY_TERMCAP),y)
      CFLAGS_hw_tty.o+=-I/usr/include/ncurses
      ifeq ($(CONF_HW_TTY_LINUX),y)
        ifeq ($(LD_LIBTERMCAP_IN_LIBGPM),)
          LDFLAGS_hw_tty.so+=$(LD_LIBTERMCAP)
	# else necessary libs are already pulled in with -lgpm above
	endif
      else
        LDFLAGS_hw_tty.so+=$(LD_LIBTERMCAP)
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
  
    all: hw_X.so
    
    MODBINS+=hw_X11.so
    
  else
    EXCL_OBJS+=$(X11OBJS)
  endif
endif

ifneq ($(CONF_SOCKET),n)
  ifeq ($(CONF_HW_TWIN),y)
    OBJS_hw.a+=$(TWINOBJS)
  else
    ifeq ($(CONF_HW_TWIN),m)
      MODBINS+=hw_twin.so
    else
      EXCL_OBJS+=$(TWINOBJS)
    endif
  endif
  
  ifeq ($(CONF_HW_DISPLAY),y)
    OBJS_hw.a+=$(DISPLAYOBJS)
  else
    ifeq ($(CONF_HW_DISPLAY),m)
      MODBINS+=hw_display.so
    else
      EXCL_OBJS+=$(DISPLAYOBJS)
    endif
  endif
endif

ifeq ($(CONF_HW_GGI),y)
  OBJS_hw.a+=$(GGIOBJS)
else
  ifeq ($(CONF_HW_GGI),m)
    MODBINS+=hw_ggi.so
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

hw_X.so: hw_X11.so
	$(LN) $< $@

clean:
	rm -f .*.flags .*.link .*.arlib hw.a hw_*.so $(ALLOBJS)

install:
	if [ "$(MODBINS)" ]; then \
	  $(INSTALL) -d $(DESTDIR)$(libdir)/twin/modules/HW; \
	  $(INSTALL) $(MODBINS) $(DESTDIR)$(libdir)/twin/modules/HW; \
	  $(LN) hw_X11.so $(DESTDIR)$(libdir)/twin/modules/HW/hw_X.so; \
	fi
