#
# Makefile for twin - Text mode WINdow manager
#
#

TOPDIR=../..

CFLAGS+=-I..

BASEOBJS=hw_null.o
TTYOBJS=hw_tty.o
X11OBJS=hw_X11.o
GGIOBJS=hw_ggi.o
TWINOBJS=hw_twin.o
ALLOBJS:=$(BASEOBJS) $(TTYOBJS) $(X11OBJS) $(TWINOBJS) $(GGIOBJS)

OBJS_hw.a:=$(BASEOBJS)
OBJS_hw_tty.so=$(TTYOBJS)
OBJS_hw_X11.so=$(X11OBJS)
OBJS_hw_ggi.so=$(GGIOBJS)
OBJS_hw_twin.so=$(TWINOBJS)

BINS=
MODBINS=
ARLIBS=hw.a

LDFLAGS_hw_tty.so=-shared
LDFLAGS_hw_X11.so=-shared -L/usr/X11/lib -lX11
LDFLAGS_hw_ggi.so=-shared -lggi
LDFLAGS_hw_twin.so=-shared -L$(TOPDIR)/lib -lTw

#
# target
#
all: $(ARLIBS) modules

#
# read configuration
#
include $(TOPDIR)/conf/config.status
-include .depend
-include .modules

#
# set variables according config.status
# and group together all *.o objects needed by each binary
#

ifeq ($(CONF_HW_TTY),y)
  OBJS_hw.a+=$(TTYOBJS)
else
  ifeq ($(CONF_HW_TTY),m)
    MODBINS+=hw_tty.so
    ifeq ($(CONF_HW_TTY_LINUX),y)
      LDFLAGS_hw_tty.so+=-lgpm
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

ifeq ($(CONF_HW_GGI),y)
  OBJS_hw.a+=$(GGIOBJS)
else
  ifeq ($(CONF_HW_GGI),m)
    MODBINS+=hw_ggi.so
  else
    EXCL_OBJS+=$(GGIOBJS)
  endif
endif

ifeq ($(CONF_HW_TWIN),y)
  OBJS_hw.a+=$(TWINOBJS)
else
  ifeq ($(CONF_HW_TWIN),m)
    MODBINS+=hw_twin.so
  else
    EXCL_OBJS+=$(TWINOBJS)
  endif
endif

#
# common rules
#
include $(TOPDIR)/Rules

#
# All done. Now just set a few special targets
#

modules: $(MODBINS)

hw_X.so: hw_X11.so
	ln -sfn $< $@

clean:
	rm -f .*.flags .*.link .*.arlib hw.a hw_*.so $(ALLOBJS)

install:
	if [ "$(MODBINS)" ]; then \
	  $(MKDIR) $(DESTDIR)/lib/twin/modules/HW; \
	  $(CP) $(MODBINS) $(wildcard hw_X.so) $(DESTDIR)/lib/twin/modules/HW; \
	fi
