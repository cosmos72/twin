#
# Makefile for Twlib, the library that talks to Twin
#
TOPDIR=..

CFLAGS+=-I$(TOPDIR)/include
LDFLAGS+=-L../lib -lTw

OBJS_twcat=cat.o
OBJS_twedit=edit.o
OBJS_twterm=pty.o term.o
OBJS_twsysmon=sysmon.o
OBJS_twattach=attach.o
OBJS_twevent=event.o

BINS=twcat twedit twterm twsysmon twattach twevent


all: $(BINS) twdetach

include $(TOPDIR)/conf/config.status
-include .depend
-include .modules

include $(TOPDIR)/Rules


twdetach: twattach
	ln -sfn $< $@

clean:
	rm -f .*.flags .*.link gmon.out core $(OBJS) $(BINS) twdetach
	
