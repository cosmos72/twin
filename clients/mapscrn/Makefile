#
# Makefile for twmapscrn libTw client
#
TOPDIR=../..
SUBDIRS=

CFLAGS+=-DDATADIR=\"/usr/lib/kbd\"
LDFLAGS+=$(LIBTW)

OBJS_twmapscrn=mapscrn.o xmalloc.o findfile.o

BINS=twmapscrn


all: $(BINS)

include $(TOPDIR)/conf/config.status
-include .modules
-include .depend

#
# include the do-everything file.
# Must correctly set all CFLAGS* and LDFLAGS* before including this!
#
include $(TOPDIR)/MakeRules


install:
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) $(BINS) $(DESTDIR)$(bindir)

clean:
	rm -f .*.flags .*.link gmon.out core $(OBJS) $(BINS)
	
