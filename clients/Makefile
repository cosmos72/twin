#
# Makefile for various libTw clients
#
TOPDIR=..
SUBDIRS=mapscrn

-include $(TOPDIR)/makeautoconf

#
# twsysmon is Linux-specific
#
ifeq ($(OS),Linux)
  TWSYSMON=twsysmon
else
  TWSYSMON=
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
OBJS_twterm=pty.o tterm.o
OBJS_twthreadtest=threadtest.o
OBJS_ttest=tt_test.o

CC_FLAGS_threadtest.o+=-D_REENTRANT

#
# LD_FLAGS+=$(LIBTW)
# use fine-grained libs instead:
#
LD_FLAGS_twattach+=$(LIBTW)
LD_FLAGS_twcat+=$(LIBTW)
LD_FLAGS_twclip+=$(LIBTW)
LD_FLAGS_twevent+=$(LIBTW)
LD_FLAGS_twdialog+=$(LIBTW)
LD_FLAGS_twdm+=$(LIBTW)
LD_FLAGS_twfindtwin+=$(LIBTW)
LD_FLAGS_twlsobj+=$(LIBTW)
LD_FLAGS_twsendmsg+=$(LIBTW)
LD_FLAGS_twsetroot+=$(LIBTW)
LD_FLAGS_twsysmon+=$(LIBTW)
LD_FLAGS_twterm+=$(LIBTW)
LD_FLAGS_twthreadtest+=$(LIBTW) $(LD_LIBPTHREAD)
LD_FLAGS_ttest+=$(LIBTT)

BINS=twattach twcat twclip twdialog twevent twfindtwin twlsobj twsendmsg twsetroot $(TWSYSMON) twterm

#BINS+=ttest

all: everything

-include $(TOPDIR)/conf/conf.current
-include .modules
-include .depend

ifneq ($(LD_LIBCRYPT),no)
 BINS+=twdm
 ifneq ($(LD_LIBCRYPT),)
   LD_FLAGS_twdm+=$(LD_LIBCRYPT)
 endif
endif

ifneq ($(bindir),)
  CC_FLAGS_dm.o=-DBINDIR=\"$(bindir)\"
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
# Must correctly set all BINS, MODBINS, ARLIBS, CC_FLAGS* and LD_FLAGS* before including this!
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

