#
# Makefile for Twlib, the library that talks to Twin
#
TOPDIR=../..

CFLAGS+=-I$(TOPDIR)/include -DDATADIR=\"/usr/lib/kbd\"
LDFLAGS+=-L$(TOPDIR)/lib -lTw

OBJS_twmapscrn=mapscrn.o xmalloc.o findfile.o

BINS=twmapscrn


all: $(BINS)

include $(TOPDIR)/conf/config.status
-include .depend
-include .modules

include $(TOPDIR)/Rules


clean:
	rm -f .*.flags .*.link gmon.out core $(OBJS) $(BINS)
	
