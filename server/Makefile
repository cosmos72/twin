#
# Makefile for twin - Text mode WINdow manager
#
#

TOPDIR=..

BASEOBJS=data.o builtin.o methods.o util.o draw.o extensions.o \
	main.o hw_multi.o resize.o scroller.o remote.o alloc.o
HWOBJS=hw.o HW/hw.a
DLOBJS=dl.o
WMOBJS=wm.o rcrun.o
RCPARSEOBJS=rcparse_tab.o rcparse_lex.o shm.o
TERMOBJS=pty.o tty.o term.o 
SOCKETOBJS=socket.o md5.o
WRAPOBJS=wrapper.o
DISPLAYOBJS=display.o
ALLOBJS:=$(BASEOBJS) $(HWOBJS) $(DLOBJS) $(WMOBJS) $(TERMOBJS) \
	 $(SOCKETOBJS) $(WRAPOBJS) $(DISPLAYOBJS) $(RCPARSEOBJS)

OBJS_twin_real:=$(BASEOBJS) $(HWOBJS)
OBJS_wm.so:=$(WMOBJS)
OBJS_rcparse.so:=$(RCPARSEOBJS)
OBJS_term.so:=$(TERMOBJS)
OBJS_socket.so:=$(SOCKETOBJS)
OBJS_twin_wrapper:=$(WRAPOBJS)
OBJS_twdisplay:=$(DISPLAYOBJS) alloc.o $(HWOBJS)

BINS=twin_real twin_wrapper
MODBINS=

# don't use := here, as DESTDIR is not set yet
CFLAGS_data.o=$(strip $(patsubst %,-DCONF_DESTDIR=\"%\",$(DESTDIR)))
CFLAGS_wrapper.o=$(strip $(patsubst %,-DCONF_DESTDIR=\"%\",$(DESTDIR)))
CFLAGS_display.o=$(strip $(patsubst %,-DCONF_DESTDIR=\"%\",$(DESTDIR)))

LDFLAGS_wm.so=-lc -shared
LDFLAGS_rcparse.so=-lc -shared
LDFLAGS_rcrun.so=-lc -shared
LDFLAGS_term.so=-lc -shared
LDFLAGS_socket.so=-lc -shared
LDFLAGS_twdisplay=$(LIBTW)


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
  CFLAGS+=-DCONF__ALLOC
endif

ifeq ($(CONF_WM),y)
  OBJS_twin_real+=$(WMOBJS)
else
  ifeq ($(CONF_WM),m)
    MODBINS+=wm.so
  else
    EXCL_OBJS+=$(WMOBJS)
  endif
endif

ifeq ($(CONF_WM_RC),y)
  OBJS_twin_real+=$(RCPARSEOBJS)
else
  ifeq ($(CONF_WM_RC),m)
    MODBINS+=rcparse.so
  else
    EXCL_OBJS+=$(RCPARSEOBJS)
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
  SETPRIV_COMM=(i.e. suid root privileges)
else
  SETPRIV=
  SETPRIV_COMM=
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
    LDFLAGS_twin_real+=$(LIBTW)
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

rcparse_tab.c rcparse_tab.h: rcparse.y
	bison -d --no-lines -o $@ $<

rcparse_lex.c: rcparse.l
	flex -L -B -o$@ $<

mixdep: all
	for i in *.o; do nm $$i | grep 'U ' | cut -dU -f2- > $${i%%.o}.u; done; :
	for i in *.o; do nm $$i | grep -E 'B | C | D | T ' > $${i%%.o}.d; done; :
	for i in *.u; do echo; echo $$i; for j in `cat $$i`; do \
	grep -E " $${j}\$$" *.d; done | sort; done > $@; :
	
clean:
	$(MAKE) clean -C HW
	rm -f .*.flags .*.link .*.arlib gmon.out core log twin \
	      twin_real twin_wrapper twdisplay *.so \
	      $(ALLOBJS) \
	      rcparse.output y.tab.c lex.yy.c \
	      *.u *.d

pure:
	rm -f rcparse_tab.c rcparse_tab.h rcparse_lex.c

install:
	$(CP) $(BINS) twstart $(DESTDIR)/bin
	$(LN) twin_wrapper $(DESTDIR)/bin/twin
	if [ "$(MODBINS)" ]; then \
	  $(MKDIR) $(DESTDIR)/lib/twin/modules; \
	  $(CP) $(MODBINS) $(DESTDIR)/lib/twin/modules; \
	fi
	$(MAKE) install -C HW
	@if [ "$(SETPRIV)" ]; then \
	  echo [1m ; \
	  echo Twin may need special privileges for the terminal emulator to work. ; \
	  echo More exactly, the following command could be necessary: ; \
	  echo "        $(SETPRIV)" ; \
	  echo "$(SETPRIV_COMM)" ; \
	  echo Since twin has not been extensively tested against vulnerabilities, ; \
	  echo you will grant special privileges to twin AT YOUR OWN RISK. ; \
	  echo [0m ; \
	fi

