#
# Makefile for various libTw clients
#
TOPDIR=..
THISDIR=$(TOPDIR)/clients
SUBDIRS=

-include $(TOPDIR)/makeautoconf

#
# twsysmon and twmapscrn are Linux-specific
#
ifeq ($(OS),Linux)
  SUBDIRS+=mapscrn
  OS_CLIENTS=twsysmon
else
  OS_CLIENTS=
endif

OBJS_twattach=attach.o
OBJS_twcat=cat.o
OBJS_twclip=clip.o
OBJS_twevent=event.o
OBJS_twdialog=dialog.o
OBJS_twdm=dm.o
OBJS_twfindtwin=findtwin.o
OBJS_twlsobj=lsobj.o
OBJS_twsendmsg=sendmsg.o
OBJS_twsetroot=setroot.o
OBJS_twsysmon=sysmon.o
OBJS_twterm=pty.o term.o
OBJS_twthreadtest=threadtest.o
OBJS_ttdemo=demo.o

CC_FLAGS_threadtest.o+=-D_REENTRANT

#
# LD_FLAGS+=$(LD_LIBTW)
# use fine-grained libs instead:
#
LD_FLAGS_twattach+=$(LD_LIBTW)
LD_FLAGS_twcat+=$(LD_LIBTW)
LD_FLAGS_twclip+=$(LD_LIBTW)
LD_FLAGS_twevent+=$(LD_LIBTW)
LD_FLAGS_twdialog+=$(LD_LIBTW)
LD_FLAGS_twdm+=$(LD_LIBTW)
LD_FLAGS_twfindtwin+=$(LD_LIBTW)
LD_FLAGS_twlsobj+=$(LD_LIBTW)
LD_FLAGS_twsendmsg+=$(LD_LIBTW)
LD_FLAGS_twsetroot+=$(LD_LIBTW)
LD_FLAGS_twsysmon+=$(LD_LIBTW)
LD_FLAGS_twterm+=$(LD_LIBTW)
LD_FLAGS_twthreadtest+=$(LD_LIBTW) $(LD_LIBPTHREAD)
LD_FLAGS_ttdemo+=$(LD_LIBTT)

BINS=twattach twcat twclip twdialog twevent twfindtwin twlsobj twsendmsg twsetroot twterm

BINS+=$(OS_CLIENTS) ttdemo

all: everything

-include $(TOPDIR)/conf/conf.current
-include .modules
-include .depend
-include $(TOPDIR)/.hdepend

ifneq ($(LD_LIBCRYPT),no)
 BINS+=twdm
 ifneq ($(LD_LIBCRYPT),)
   LD_FLAGS_twdm+=$(LD_LIBCRYPT)
 endif
endif

ifneq ($(bindir),)
  CC_FLAGS_dm.o+=-DBINDIR=\"$(bindir)\"
endif
 
ifneq ($(CONF_SOCKET),n)
  ifeq ($(CONF_SOCKET_PTHREADS),y)
    BINS+=twthreadtest
  endif
  everything: $(BINS) twdetach subdirs
else
  everything:
	@echo ; \
	 echo "You disabled \"Support remote socket connections\"," ; \
	 echo "so the remote clients will NOT be compiled!" ; \
	 echo
endif

ifeq ($(CONF__UNICODE),y)
  LD_FLAGS_twterm+=$(LD_LIBTUTF)
  LD_FLAGS_twdm+=$(LD_LIBTUTF)
endif

#
# include the do-everything file.
# Must correctly set all BINS, SHLIBS, ARLIBS, CC_FLAGS* and LD_FLAGS* before including this!
#
include $(TOPDIR)/makerules

twdetach: twattach
	$(LN) $< $@

install:
	$(INSTALL-DIR) $(DESTDIR)$(bindir) && \
	$(INSTALL-DIR) $(DESTDIR)$(sbindir) && \
	$(INSTALL) $(filter-out twdm,$(BINS)) $(DESTDIR)$(bindir) && \
	$(INSTALL) twdm $(DESTDIR)$(sbindir) && \
	$(LN) twattach $(DESTDIR)$(bindir)/twdetach

clean:
	rm -f .*.flags .*.bin gmon.out core *.o tw* tt* ; \
	rm -fr .libs

