#
# Makefile for twin - Text mode WINdow manager
#
#

TOPDIR=..

BASEOBJS=data.o methods.o util.o draw.o main.o hw.o HW/hw.a resize.o scroller.o builtin.o wm.o remote.o
ALLOCOBJS=alloc.o
DLOBJS=dl.o
SOCKETOBJS=socket.o md5.o
TERMOBJS=pty.o tty.o term.o 
WRAPOBJS=wrapper.o
ALLOBJS:=$(BASEOBJS) $(ALLOCOBJS) $(DLOBJS) $(TERMOBJS) $(SOCKETOBJS) $(WRAPOBJS)

OBJS_twin_real:=$(BASEOBJS)
OBJS_socket.so:=$(SOCKETOBJS)
OBJS_term.so:=$(TERMOBJS)
OBJS_twin_wrapper:=$(WRAPOBJS)

BINS=twin_real twin_wrapper
MODBINS=

# don't use := here, as DESTDIR is not set yet
CFLAGS_dl.o=$(strip $(patsubst %,-DCONF_DESTDIR=\"%\",$(DESTDIR)))
CFLAGS_wrapper.o=$(strip $(patsubst %,-DCONF_DESTDIR=\"%\",$(DESTDIR)))

LDFLAGS_socket.so=-shared
LDFLAGS_term.so=-shared

#
# target
#
all: $(BINS) modules twin

twin_real: HW.dir

twin: twin_wrapper
	ln -sf $< $@

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
ifeq ($(CONF_MODULES),y)
  LDFLAGS_twin_real+=-rdynamic -ldl
  OBJS_twin_real+=$(DLOBJS)
else
  EXCL_OBJS+=$(DLOBJS)
endif

ifeq ($(CONF_SOCKET),y)
  OBJS_twin_real+=$(SOCKETOBJS)
  ifeq ($(CONF_SOCKET_GZ),y)
    LDFLAGS_twin_real+=-lz
  endif
else
  ifeq ($(CONF_SOCKET),m)
    MODBINS+=socket.so
    ifeq ($(CONF_SOCKET_GZ),y)
      LDFLAGS_socket.so+=-lz
    endif
  else
    EXCL_OBJS+=$(SOCKETOBJS)
  endif
endif

ifeq ($(CONF_TERM),y)
  OBJS_twin_real+=$(TERMOBJS)
else
  ifeq ($(CONF_TERM),m)
    MODBINS+=term.so
  else
    EXCL_OBJS+=$(TERMOBJS)
  endif
endif

ifeq ($(CONF_ALLOC),y)
  OBJS_twin_real+=$(ALLOCOBJS)
  CFLAGS+=-DCONF_ALLOC
else
  EXCL_OBJS+=$(ALLOCOBJS)
endif


#
# the final link nightmare...
#
ifeq ($(CONF_HW_TTY),y)
  ifeq ($(CONF_HW_TTY_LINUX),y)
    LDFLAGS_twin_real+=-lgpm #/usr/lib/libgpm.a
  endif
endif
ifeq ($(CONF_HW_X11),y)
  LDFLAGS_twin_real+=-L/usr/X11/lib -lX11
endif
ifeq ($(CONF_HW_GGI),y)
  LDFLAGS_twin_real+=-lggi
endif
ifeq ($(CONF_HW_TWIN),y)
  LDFLAGS_twin_real+=-L$(TOPDIR)/lib -lTw
endif


#
# common rules
#
include $(TOPDIR)/Rules

#
# All done. Now just set a few special targets
#

modules: $(MODBINS)

clean:
	make clean -C HW
	rm -f .*.flags .*.link .*.arlib gmon.out core twin_real twin_wrapper twin getsizes socket.so term.so $(ALLOBJS)

install:
	$(CP) twin_real twin_wrapper twin $(DESTDIR)/bin
	chmod u+s $(DESTDIR)/bin/twin_real
	if [ "$(MODBINS)" ]; then \
	  $(MKDIR) $(DESTDIR)/lib/twin/modules; \
	  $(CP) $(MODBINS) $(DESTDIR)/lib/twin/modules; \
	fi
	$(MAKE) install -C HW

