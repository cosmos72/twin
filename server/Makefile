#
# Makefile for twin - Text mode WINdow manager
#
#

TOPDIR=..

BASEOBJS=data.o methods.o util.o draw.o main.o hw_multi.o \
	 resize.o scroller.o builtin.o remote.o
HWOBJS=hw.o HW/hw.a
ALLOCOBJS=alloc.o
DLOBJS=dl.o
WMOBJS=wm.o
TERMOBJS=pty.o tty.o term.o 
SOCKETOBJS=socket.o md5.o
WRAPOBJS=wrapper.o
DISPLAYOBJS=display.o
ALLOBJS:=$(BASEOBJS) $(HWOBJS) $(ALLOCOBJS) $(DLOBJS) $(WMOBJS) $(TERMOBJS) $(SOCKETOBJS) $(WRAPOBJS) $(DISPLAYOBJS)

OBJS_twin_real:=$(BASEOBJS) $(HWOBJS)
OBJS_wm.so:=$(WMOBJS)
OBJS_term.so:=$(TERMOBJS)
OBJS_socket.so:=$(SOCKETOBJS)
OBJS_twin_wrapper:=$(WRAPOBJS)
OBJS_twdisplay:=$(DISPLAYOBJS) $(HWOBJS)

BINS=twin_real twin_wrapper
MODBINS=

# don't use := here, as DESTDIR is not set yet
CFLAGS_dl.o=$(strip $(patsubst %,-DCONF_DESTDIR=\"%\",$(DESTDIR)))
CFLAGS_wrapper.o=$(strip $(patsubst %,-DCONF_DESTDIR=\"%\",$(DESTDIR)))
CFLAGS_display.o=$(strip $(patsubst %,-DCONF_DESTDIR=\"%\",$(DESTDIR)))

LDFLAGS_wm.so=-lc -shared
LDFLAGS_term.so=-lc -shared
LDFLAGS_socket.so=-lc -shared
LDFLAGS_twdisplay=-L$(TOPDIR)/lib -lTw


#
# OS-specific stuff
#
OS=$(shell uname)
ifeq ($(OS),Linux)
  DLOPEN_LDFLAGS=-rdynamic -ldl
else
  DLOPEN_LDFLAGS=-rdynamic
endif

#
# target
#
all: bins modules twin

twin_real: HW.dir

twin: twin_wrapper
	$(LN) $< $@

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
ifeq ($(CONF__MODULES),y)
  LDFLAGS_twin_real+=$(DLOPEN_LDFLAGS)
  OBJS_twin_real+=$(DLOBJS)
  LDFLAGS_twdisplay+=$(DLOPEN_LDFLAGS)
else
  EXCL_OBJS+=$(DLOBJS)
endif

ifeq ($(CONF__ALLOC),y)
  OBJS_twin_real+=$(ALLOCOBJS)
  OBJS_twdisplay+=$(ALLOCOBJS)
  CFLAGS+=-DCONF__ALLOC
else
  EXCL_OBJS+=$(ALLOCOBJS)
endif

ifeq ($(CONF_WM),y)
  OBJS_twin_real+=$(WMOBJS)
else
  ifeq ($(CONF_WM),m)
    MODBINS+=wm.so
  else
    EXCL_OBJS+=$(WMOBJS)
    dummy:=$(shell echo "You disabled the window manager! Twin will not run!" > /dev/tty)
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

#
# suid root / sgid tty stuff
#
ifneq ($(CONF_TERM),n)
  ifeq ($(CONF_TERM_DEVPTS),y)
    PRIV=tty
  else
    PRIV=root
  endif
else
  ifeq ($(CONF_MODULES),y)
    PRIV=root
  else
    PRIV=
  endif
endif

ifeq ($(PRIV),root)
  SETPRIV=chmod u+s $(DESTDIR)/bin/twin_real
else
  ifeq ($(PRIV),tty)
    SETPRIV=chown :tty $(DESTDIR)/bin/twin_real && chmod g+s $(DESTDIR)/bin/twin_real
  else
    SETPRIV=
  endif
endif
  
#
# the final link nightmare...
#
ifeq ($(CONF_HW_TTY),y)
  ifeq ($(CONF_HW_TTY_LINUX),y)
    LDFLAGS_twin_real+=-lgpm #/usr/lib/libgpm.a
    LDFLAGS_twdisplay+=-lgpm #/usr/lib/libgpm.a
  endif
  ifeq ($(CONF_HW_TTY_TERMCAP),y)
    LIBTERMCAP=-ltermcap
    ifeq ($(CONF_HW_TTY_NCURSES),y)
      LIBTERMCAP=-lncurses
    endif
    LDFLAGS_twin_real+=$(LIBTERMCAP)
    LDFLAGS_twdisplay+=$(LIBTERMCAP)
  endif
endif
ifeq ($(CONF_HW_X11),y)
  LDFLAGS_twin_real+=-L/usr/X11R6/lib -lX11
  LDFLAGS_twdisplay+=-L/usr/X11R6/lib -lX11
endif
ifeq ($(CONF_HW_GGI),y)
  LDFLAGS_twin_real+=-lggi
  LDFLAGS_twdisplay+=-lggi
endif
ifneq ($(CONF_SOCKET),n)
  ifneq ($(CONF_HW_DISPLAY),n)
    BINS+=twdisplay
  endif
  ifeq ($(CONF_HW_TWIN),y)
    LDFLAGS_twin_real+=-L$(TOPDIR)/lib -lTw
  endif
endif


#
# common rules
#
include $(TOPDIR)/MakeRules


#
# All done. Now just set a few special targets
#

bins: $(BINS)

modules: $(MODBINS)

clean:
	$(MAKE) clean -C HW
	rm -f .*.flags .*.link .*.arlib gmon.out core log twin \
	      twin_real twin_wrapper twdisplay socket.so term.so wm.so \
	      $(ALLOBJS)

install:
	$(CP) $(BINS) $(DESTDIR)/bin
	$(LN) twin_real $(DESTDIR)/bin/twin
	$(SETPRIV)
	if [ "$(MODBINS)" ]; then \
	  $(MKDIR) $(DESTDIR)/lib/twin/modules; \
	  $(CP) $(MODBINS) $(DESTDIR)/lib/twin/modules; \
	fi
	$(MAKE) install -C HW

