#
# Makefile for twin - Text mode WINdow manager
#
#

TOPDIR=..
SUBDIRS=HW

-include $(TOPDIR)/makeautoconf

BASEOBJS=data.o builtin.o methods.o util.o draw.o extensions.o main.o \
         missing.o hw_multi.o printk.o resize.o scroller.o remote.o alloc.o
HWOBJS=hw.o HW/hw.a
DLOBJS=dl.o
WMOBJS=wm.o rcrun.o rcopt.o
RCPARSEOBJS=rcparse_tab.o rcparse_lex.o shm.o
TERMOBJS=pty.o tty.o tterm.o 
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
OBJS_twdisplay:=$(DISPLAYOBJS) alloc.o missing.o $(HWOBJS)

BINS=twin_real twin_wrapper
MODBINS=

LD_FLAGS_wm.so=-lc -shared
LD_FLAGS_rcparse.so=-lc -shared
LD_FLAGS_rcrun.so=-lc -shared
LD_FLAGS_term.so=-lc -shared
LD_FLAGS_socket.so=-lc -shared
LD_FLAGS_twdisplay=$(LIBTW)

#
# tweak flags based on current paths configuration
#
ifneq ($(libdir),)
  CC_FLAGS_data.o=-DLIBDIR=\"$(libdir)\"
  CC_FLAGS_display.o=-DLIBDIR=\"$(libdir)\"
endif
ifneq ($(bindir),)
  CC_FLAGS_wrapper.o=-DBINDIR=\"$(bindir)\"
endif


#
# OS-specific stuff
#
ifneq ($(GNULD),)
  LD_FLAGS_rdynamic:=-rdynamic
endif

#
# target
#
all: bins modules twin handy

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
ifeq ($(CONF__MODULES),y)
  LD_FLAGS_twin_real+=$(LD_LIBDL) $(LD_FLAGS_rdynamic)
  OBJS_twin_real+=$(DLOBJS)
  LD_FLAGS_twdisplay+=$(LD_LIBDL) $(LD_FLAGS_rdynamic)
else
  EXCL_OBJS+=$(DLOBJS)
endif

ifeq ($(CONF__ALLOC),y)
  CC_FLAGS+=-DCONF__ALLOC
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

ifeq ($(CONF_SOCKET),n)
  EXCL_OBJS+=$(SOCKETOBJS)
else
  LD_FLAGS_twin_real+=$(LD_LIBSOCKET)
  LD_FLAGS_twdisplay+=$(LD_LIBSOCKET)
  ifeq ($(CONF_SOCKET),y)
    OBJS_twin_real+=$(SOCKETOBJS)
  else
    MODBINS+=socket.so
  endif
  ifeq ($(CONF_SOCKET_GZ),y)
    ifeq ($(CONF_SOCKET),y)
      LD_FLAGS_twin_real+=-lz
     else
      LD_FLAGS_socket.so+=-lz
    endif
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
  SETPRIV=chmod u+s $(DESTDIR)$(bindir)/twin_real
  SETPRIV_COMM=(i.e. suid root privileges)
else
  SETPRIV=chown .tty $(DESTDIR)$(bindir)/twin_real && chmod g+s $(DESTDIR)$(bindir)/twin_real
  SETPRIV_COMM=(i.e. sgid tty privileges)
endif

#
# the final link nightmare...
#
ifeq ($(CONF__UNICODE),y)
  LD_FLAGS_twin_real+=$(LIBTUTF)
  LD_FLAGS_twdisplay+=$(LIBTUTF)
endif

ifeq ($(CONF_HW_TTY),y)
  ifeq ($(CONF_HW_TTY_LINUX),y)
    LD_FLAGS_twin_real+=-lgpm
    LD_FLAGS_twdisplay+=-lgpm
  endif
  ifeq ($(CONF_HW_TTY_TERMCAP),y)
    ifeq ($(CONF_HW_TTY_LINUX),y)
      ifeq ($(LD_LIBTERMCAP_IN_LIBGPM),)
        LD_FLAGS_twin_real+=$(LD_LIBTERMCAP)
        LD_FLAGS_twdisplay+=$(LD_LIBTERMCAP)
      # else necessary libs are already pulled in with -lgpm above
      endif
    else
      LD_FLAGS_twin_real+=$(LD_LIBTERMCAP)
      LD_FLAGS_twdisplay+=$(LD_LIBTERMCAP)
    endif
  endif
endif
ifeq ($(CONF_HW_X11),y)
  LD_FLAGS_twin_real+=-L/usr/X11R6/lib -lX11
  LD_FLAGS_twdisplay+=-L/usr/X11R6/lib -lX11
endif
ifeq ($(CONF_HW_GGI),y)
  LD_FLAGS_twin_real+=-lggi
  LD_FLAGS_twdisplay+=-lggi
endif
ifneq ($(CONF_SOCKET),n)
  ifneq ($(CONF_HW_DISPLAY),n)
    BINS+=twdisplay
  endif
  ifeq ($(CONF_HW_TWIN),y)
    LD_FLAGS_twin_real+=$(LIBTW)
  endif
endif


#
# common rules
#
include $(TOPDIR)/makerules


bins: $(BINS)

modules: $(MODBINS)


#
# All done. Now just set a few special targets
#

twin: twin_wrapper
	$(LN) $< $@

# the '@:' dummy command is to force relinking after HW/hw.a is rebuilt
HW/hw.a: dir-HW
	@:

autogen: rcparse_tab.c rcparse_tab.h rcparse_lex.c socket1m4.h socket2m4.h socket3m4.h

handy: .twinrc

.twinrc:
	$(LN) ../system.twinrc .twinrc

rcparse_tab.c rcparse_tab.h: rcparse.y
	bison -d --no-lines -o $@ $<

rcparse_lex.c: rcparse.l
	flex -L -B -o$@ $<

socketmacros%.h: m4/macros%.m4 $(TOPDIR)/include/sockproto.h
	m4 -I$(TOPDIR)/include $< > $@

symbolsdep: all
	for i in *.o; do nm $$i | grep 'U ' | cut -dU -f2- > $${i%%.o}.u; done; :
	for i in *.o; do nm $$i | grep 'B \| C \| D \| T ' > $${i%%.o}.d; done; :
	for i in *.u; do $(ECHO); $(ECHO) $$i; for j in `cat $$i`; do \
	grep -e " $${j}\$$" *.d; done | sort; done > $@; :

clean:
	rm -f .*.flags .*.link .*.arlib gmon.out core log twin \
	      twin_real twin_wrapper twdisplay *.so \
	      *.o \
	      rcparse.output y.tab.c lex.yy.c \
	      *.u *.d


install:
	$(INSTALL) -d $(DESTDIR)$(bindir) $(DESTDIR)/$(libdir)/twin/modules
	$(INSTALL) $(BINS) twstart $(DESTDIR)$(bindir)
	$(LN) twin_wrapper $(DESTDIR)$(bindir)/twin
	if [ "$(MODBINS)" ]; then \
	  $(INSTALL) $(MODBINS) $(DESTDIR)$(libdir)/twin/modules; \
	fi
	@if [ "$(SETPRIV)" ]; then \
	  $(ECHO) [1m ; \
	  $(ECHO) Twin may need special privileges for the terminal emulator to work. ; \
	  $(ECHO) More exactly, the following command could be necessary: ; \
	  $(ECHO) "   $(SETPRIV)" ; \
	  $(ECHO) "$(SETPRIV_COMM)" ; \
	  $(ECHO) Since twin has not been extensively tested against vulnerabilities, ; \
	  $(ECHO) you will grant special privileges to twin AT YOUR OWN RISK. ; \
	  $(ECHO) [0m ; \
	fi
