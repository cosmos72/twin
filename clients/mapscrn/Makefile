#
# Makefile for twmapscrn libTw client
#
TOPDIR=../..
SUBDIRS=

CC_FLAGS+=-DDATADIR=\"/usr/lib/kbd\"
CC_FLAGS_mapscrn.o+=-DMAIN

LD_FLAGS+=$(LIBTW)

OBJS_twmapscrn=mapscrn.o findfile.o utf8.o xmalloc.o getfd.o kdmapop.o
BINS=twmapscrn


all: $(BINS)

-include $(TOPDIR)/conf/conf.current
-include .modules
-include .depend

#
# include the do-everything file.
# Must correctly set all CFLAGS* and LDFLAGS* before including this!
#
include $(TOPDIR)/makerules


install:
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) $(BINS) $(DESTDIR)$(bindir)

clean:
	rm -f .*.flags .*.link gmon.out core $(OBJS) $(BINS)
	
