#
# Makefile for twin - Text mode WINdow manager
#
#

TOPDIR=..
THISDIR=$(TOPDIR)/server

SUBDIRS=HW

#
# where to install modules (relative to $(libdir))
#
INSTALLSUBDIR=/twin/modules

include $(TOPDIR)/include/makeversion
-include $(TOPDIR)/makeautoconf

BASEOBJS=data.o builtin.o methods.o util.o draw.o extensions.o main.o \
         missing.o hw_multi.o printk.o resize.o scroller.o remote.o alloc.o
HWOBJS=hw.o HW/hw.lst
DLOBJS=dl.o
WMOBJS=wm.o rcrun.o rcopt.o shm.o
RCPARSEOBJS=rcparse_tab.o rcparse_lex.o
TERMOBJS=pty.o tty.o tterm.o 
SOCKETOBJS=socket.o md5.o
WRAPOBJS=wrapper.o
DISPLAYOBJS=display.o
ALLOBJS:=$(BASEOBJS) $(HWOBJS) $(DLOBJS) $(WMOBJS) $(TERMOBJS) \
	 $(SOCKETOBJS) $(WRAPOBJS) $(DISPLAYOBJS) $(RCPARSEOBJS)
ALLOBJS+=$(patsubst %.o,%.lo,$(ALLOBJS))

OBJS_twin_real:=$(BASEOBJS) $(HWOBJS)
OBJS_wm.la:=$(patsubst %.o,%.lo,$(WMOBJS))
OBJS_rcparse.la:=$(patsubst %.o,%.lo,$(RCPARSEOBJS))
OBJS_term.la:=$(patsubst %.o,%.lo,$(TERMOBJS))
OBJS_socket.la:=$(patsubst %.o,%.lo,$(SOCKETOBJS))
OBJS_twin_wrapper:=$(WRAPOBJS)
OBJS_twdisplay:=$(DISPLAYOBJS) alloc.o missing.o $(HWOBJS)

BINS=
MODS=

ifeq ($(BUILD_PROG_TWIN),yes)
  BINS+=twin_real twin_wrapper
endif

LD_FLAGS_twdisplay=$(LD_LIBTW)


CC_FLAGS.lo+=-DTHIS_MODULE

#
# tweak flags based on current paths configuration
#
ifneq ($(libdir),)
  CC_FLAGS_data.o=-DLIBDIR=\"$(libdir)\"
  CC_FLAGS_display.o=-DLIBDIR=\"$(libdir)\"
  #
  # where to install modules?
  #
  # libdir_MODS=$(libdir)$(INSTALLSUBDIR)
endif
ifneq ($(bindir),)
  CC_FLAGS_wrapper.o=-DBINDIR=\"$(bindir)\"
endif


#
# target
#
all: build-subdirs bins modules twin links

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
ifeq ($(CONF__MODULES),y)
  #
  # help resolving symbols in modules loaded at runtime
  #
  LD_FLAGS_twin_real+=$(LD_LIBDL) -export-dynamic
  LD_FLAGS_twdisplay+=$(LD_LIBDL) -export-dynamic
  
  OBJS_twin_real+=$(DLOBJS)
endif

ifeq ($(CONF__ALLOC),y)
  CC_FLAGS+=-DCONF__ALLOC
endif

ifeq ($(CONF_WM),y)
  OBJS_twin_real+=$(WMOBJS)
else
  ifeq ($(CONF_WM),m)
    MODS+=wm.la
  endif
endif

ifeq ($(CONF_WM_RC),y)
  OBJS_twin_real+=$(RCPARSEOBJS)
else
  ifeq ($(CONF_WM_RC),m)
    MODS+=rcparse.la
  endif
endif

 
ifeq ($(CONF_TERM),y)
  OBJS_twin_real+=$(TERMOBJS)
else
  ifeq ($(CONF_TERM),m)
    MODS+=term.la
  endif
endif

ifneq ($(CONF_SOCKET),n)
  LD_FLAGS_twin_real+=$(LD_LIBSOCKET)
  LD_FLAGS_twdisplay+=$(LD_LIBSOCKET)
  ifeq ($(CONF_SOCKET),y)
    OBJS_twin_real+=$(SOCKETOBJS)
  else
    MODS+=socket.la
  endif
  ifeq ($(CONF_SOCKET_GZ),y)
    ifeq ($(CONF_SOCKET),y)
      LD_FLAGS_twin_real+=-lz
     else
      LD_FLAGS_socket.la+=-lz
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
  LD_FLAGS_twin_real+=$(LD_LIBTUTF)
  LD_FLAGS_twdisplay+=$(LD_LIBTUTF)
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
ifeq ($(CONF_HW_GFX),y)
  LD_FLAGS_twin_real+=$(LD_LIBGFX)
  LD_FLAGS_twdisplay+=$(LD_LIBGFX)
else
  ifeq ($(CONF_HW_X11),y)
    LD_FLAGS_twin_real+=$(LD_LIBX11)
    LD_FLAGS_twdisplay+=$(LD_LIBX11)
  endif
endif
ifeq ($(CONF_HW_GGI),y)
  LD_FLAGS_twin_real+=-lggi
  LD_FLAGS_twdisplay+=-lggi
endif
ifneq ($(CONF_SOCKET),n)
  ifeq ($(CONF_HW_DISPLAY),y)
    OBJS_twin_real+=HW/hw_display.lst
  endif
  ifneq ($(CONF_HW_DISPLAY),n)
    BINS+=twdisplay
  endif
  ifeq ($(CONF_HW_TWIN),y)
    LD_FLAGS_twin_real+=$(LD_LIBTW)
  endif
endif

ifeq ($(CONF_EXT_LIBTT),y)
  LD_FLAGS_twin_real+=$(LD_LIBTT)
endif

#
# common rules
#
include $(TOPDIR)/makerules


bins: $(BINS)

modules: $(MODS)


#
# All done. Now just set a few special targets
#

twin: twin_wrapper
	$(LN) $< $@

# the '@:' dummy command is to force relinking after HW/hw.lst is rebuilt
HW/hw.lst: build-dir-HW
	@:

do-autogen: rcparse_tab.c rcparse_tab.h rcparse_lex.c socket1_m4.h socket2_m4.h socket3_m4.h

links: .twinrc .twenvrc.sh $(patsubst %.la,%.so.$(SHLIBS_VER),$(MODS))

.twinrc:
	$(LN) ../system.twinrc .twinrc

.twenvrc.sh:
	$(LN) ../system.twenvrc.sh .twenvrc.sh

rcparse_tab.c rcparse_tab.h: rcparse.y
	bison -d --no-lines -o $@ $<

rcparse_lex.c: rcparse.l
	flex -L -B -o$@ $<

socketmacros%.h: m4/macros%.m4 $(TOPDIR)/include/sockproto.h
	m4 -I$(TOPDIR)/include $< > $@

symbolsdep: all
	for i in *.o; do nm $$i | grep 'U ' | cut -dU -f2- > $${i%%.o}.u; done; :
	for i in *.o; do nm $$i | grep 'B \| C \| D \| T ' > $${i%%.o}.d; done; :
	for i in *.u; do echo; echo $$i; for j in `cat $$i`; do \
	grep -e " $${j}\$$" *.d; done | sort; done > $@; :


install-dirs:
	$(INSTALL-DIR) $(DESTDIR)$(bindir) $(DESTDIR)$(libdir)$(INSTALLSUBDIR)

install-bins:
	$(INSTALL) $(BINS) $(DESTDIR)$(bindir) && \
	$(INSTALL-BIN) twstart $(DESTDIR)$(bindir) && \
	$(LN) twin_wrapper $(DESTDIR)$(bindir)/twin

install: install-dirs install-bins $(foreach M,$(MODS),install-mod-$(M))
	@if [ "$(SETPRIV)" ]; then \
	  echo [1m ; \
	  echo "# Twin may need special privileges for the terminal emulator to work." ; \
	  echo "# More exactly, the following command could be necessary:" ; \
	  echo "#    $(SETPRIV)" ; \
	  echo "# $(SETPRIV_COMM)" ; \
	  echo "# Since twin has not been extensively tested against vulnerabilities," ; \
	  echo "# you will grant special privileges to twin AT YOUR OWN RISK." ; \
	  echo [0m ; \
	fi

uninstall-dirs:
	rmdir $(DESTDIR)$(bindir) || true
	rmdir $(DESTDIR)$(libdir)$(INSTALLSUBDIR) || true
	rmdir $(DESTDIR)$(libdir)/twin || true
	rmdir $(DESTDIR)$(libdir) || true

uninstall-bins:
	cd $(DESTDIR)$(bindir) && \
	   rm -f $(BINS) twstart twin || true
	rmdir $(DESTDIR)$(bindir) || true
	rmdir $(DESTDIR)$(libdir)$(INSTALLSUBDIR) || true

uninstall: uninstall-bins $(foreach M,$(MODS),uninstall-mod-$(M)) uninstall-dirs


clean:
	rm -f .*.flags .*.link .*.arlib gmon.out core log twin \
	      twin_real twin_wrapper twdisplay *.o *.lo *.la *.so *.so.* \
	      rcparse.output y.tab.c lex.yy.c *.u *.d ; \
	rm -fr .libs
