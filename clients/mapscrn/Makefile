#
# Makefile for Twlib, the library that talks to Twin
#
TOPDIR=../..

CFLAGS+=-DDATADIR=\"/usr/lib/kbd\"
LDFLAGS+=$(LIBTW)

OBJS_twmapscrn=mapscrn.o xmalloc.o findfile.o

BINS=twmapscrn


all: $(BINS)

include $(TOPDIR)/conf/config.status
-include .depend
-include .modules

include $(TOPDIR)/MakeRules


clean:
	rm -f .*.flags .*.link gmon.out core $(OBJS) $(BINS)
	
