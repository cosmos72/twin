#
# Makefile for various libTw clients
#
TOPDIR=..
SUBDIRS=mapscrn

-include $(TOPDIR)/makeautoconf

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
OBJS_twterm=pty.o tterm.o
OBJS_twthreadtest=threadtest.o
OBJS_ttest=tt_test.o

CFLAGS_threadtest.o+=-D_REENTRANT

#
# LDFLAGS+=$(LIBTW)
# use fine-grained libs instead:
#
LDFLAGS_twattach+=$(LIBTW)
LDFLAGS_twcat+=$(LIBTW)
LDFLAGS_twclip+=$(LIBTW)
LDFLAGS_twevent+=$(LIBTW)
LDFLAGS_twdialog+=$(LIBTW)
LDFLAGS_twdm+=$(LIBTW)
LDFLAGS_twfindtwin+=$(LIBTW)
LDFLAGS_twlsobj+=$(LIBTW)
LDFLAGS_twsendmsg+=$(LIBTW)
LDFLAGS_twsetroot+=$(LIBTW)
LDFLAGS_twsysmon+=$(LIBTW)
LDFLAGS_twterm+=$(LIBTW)
LDFLAGS_twthreadtest+=$(LIBTW) $(LD_LIBPTHREAD)
LDFLAGS_ttest+=$(LIBTT)

BINS=twattach twcat twclip twdialog twevent twfindtwin twlsobj twsendmsg twsetroot twsysmon twterm

#BINS+=ttest

all: everything

-include $(TOPDIR)/conf/conf.current
-include .modules
-include .depend

ifneq ($(LD_LIBCRYPT),no)
 BINS+=twdm
 ifneq ($(LD_LIBCRYPT),)
   LDFLAGS_twdm+=$(LD_LIBCRYPT)
 endif
endif

ifneq ($(bindir),)
  CFLAGS_dm.o=-DBINDIR=\"$(bindir)\"
endif
 
ifneq ($(CONF_SOCKET),n)
  ifeq ($(CONF_SOCKET_PTHREADS),y)
    BINS+=twthreadtest
  endif
  everything: $(BINS) twdetach subdirs
else
  everything:
	@$(ECHO) ; \
	 $(ECHO) "You disabled \"Support remote socket connections\"," ; \
	 $(ECHO) "so the remote clients will NOT be compiled!" ; \
	 $(ECHO)
endif


#
# include the do-everything file.
# Must correctly set all BINS, MODBINS, ARLIBS, CFLAGS* and LDFLAGS* before including this!
#
include $(TOPDIR)/makerules



twdetach: twattach
	$(LN) $< $@

install:
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) $(BINS) $(DESTDIR)$(bindir)
	$(LN) twattach $(DESTDIR)$(bindir)/twdetach

clean:
	rm -f .*.flags .*.link gmon.out core *.o $(BINS)

