#
# Makefile for various libTw clients
#
TOPDIR=..

LDFLAGS+=$(LIBTW)

OBJS_twfindtwin=findtwin.o
OBJS_twcat=cat.o
OBJS_twclip=clip.o
OBJS_twsendmsg=sendmsg.o
OBJS_twsysmon=sysmon.o
OBJS_twterm=pty.o term.o
OBJS_twattach=attach.o
OBJS_twevent=event.o
OBJS_twsetroot=setroot.o
OBJS_twthreadtest=threadtest.o

CFLAGS_threadtest.o+=-D_REENTRANT
LDFLAGS_twthreadtest+=-lpthread

BINS=twfindtwin twcat twclip twsysmon twsendmsg twterm twattach twevent twsetroot

all: everything


include $(TOPDIR)/conf/config.status
-include .depend
-include .modules

ifneq ($(CONF_SOCKET),n)
  ifeq ($(CONF_SOCKET_PTHREADS),y)
    BINS+=twthreadtest
  endif
  everything: $(BINS) twdetach mapscrn.dir
else
  everything:
	@echo
	@echo "You disabled \"Support remote socket connections\","
	@echo "so the remote clients will NOT be compiled!"
	@echo
endif

include $(TOPDIR)/MakeRules

twdetach: twattach
	$(LN) $< $@

Torture:

install:
	$(CP) $(BINS) */tw* $(DESTDIR)/bin 
	$(LN) twattach $(DESTDIR)/bin/twdetach

clean:
	rm -f .*.flags .*.link gmon.out core $(OBJS) $(BINS) twdetach
	$(MAKE) clean -C mapscrn

