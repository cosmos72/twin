#
# Makefile for Twlib, the library that talks to Twin
#
TOPDIR=..

LDFLAGS+=-L../lib -lTw

OBJS_twcat=cat.o
OBJS_twedit=edit.o
OBJS_twterm=pty.o term.o
OBJS_twsysmon=sysmon.o
OBJS_twattach=attach.o
OBJS_twevent=event.o
OBJS_twsetroot=setroot.o

BINS=twcat twedit twterm twsysmon twattach twevent twsetroot

all: everything


include $(TOPDIR)/conf/config.status
-include .depend
-include .modules

include $(TOPDIR)/MakeRules

ifneq ($(CONF_SOCKET),n)
  everything: $(BINS) twdetach mapscrn.dir
else
  everything:
	@echo
	@echo "You disabled \"Support remote socket connections\","
	@echo "so the remote clients will NOT be compiled!"
	@echo
endif

twdetach: twattach
	$(LN) $< $@

install:
	$(CP) $(BINS) */tw* $(DESTDIR)/bin 
	$(LN) twattach $(DESTDIR)/bin/twdetach

clean:
	rm -f .*.flags .*.link gmon.out core $(OBJS) $(BINS) twdetach
	$(MAKE) clean -C mapscrn

