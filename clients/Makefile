#
# Makefile for various libTw clients
#
TOPDIR=..
SUBDIRS=mapscrn

LDFLAGS+=$(LIBTW)

OBJS_twattach=attach.o
OBJS_twcat=cat.o
OBJS_twclip=clip.o
OBJS_twevent=event.o
OBJS_twdialog=dialog.o
OBJS_twfindtwin=findtwin.o
OBJS_twlsobj=lsobj.o
OBJS_twsendmsg=sendmsg.o
OBJS_twsetroot=setroot.o
OBJS_twsysmon=sysmon.o
OBJS_twterm=pty.o tterm.o
OBJS_twthreadtest=threadtest.o

CFLAGS_threadtest.o+=-D_REENTRANT
LDFLAGS_twthreadtest+=-lpthread

BINS=twattach twcat twclip twdialog twevent twfindtwin twlsobj twsendmsg twsetroot twsysmon twterm

all: everything


include $(TOPDIR)/conf/config.status
-include .modules
-include .depend


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
include $(TOPDIR)/MakeRules



twdetach: twattach
	$(LN) $< $@

install:
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) $(BINS) $(DESTDIR)$(bindir)
	$(LN) twattach $(DESTDIR)$(bindir)/twdetach

clean:
	rm -f .*.flags .*.link gmon.out core *.o tw*

