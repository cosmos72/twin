#
# Makefile for twmapscrn libTw client
#
TOPDIR=../..
THISDIR=$(TOPDIR)/clients/mapscrn
SUBDIRS=

CC_FLAGS+=-DDATADIR=\"/usr/lib/kbd\"
CC_FLAGS_mapscrn.o+=-DMAIN

LD_FLAGS+=$(LD_LIBTW)

OBJS_twmapscrn=mapscrn.o findfile.o utf8.o xmalloc.o getfd.o kdmapop.o
BINS=twmapscrn


all: $(BINS)

-include $(TOPDIR)/conf/conf.current
-include .modules
-include .depend
-include $(TOPDIR)/.hdepend

#
# include the do-everything file.
# Must correctly set all CFLAGS* and LDFLAGS* before including this!
#
include $(TOPDIR)/makerules


install:
	$(INSTALL-DIR) $(DESTDIR)$(bindir)
	$(INSTALL) $(BINS) $(DESTDIR)$(bindir)

clean:
	rm -f .*.flags .*.bin gmon.out core tw* *.la *.a *.o *.lo *.so *.so.* ; \
	rm -fr .libs
	
