#
# Makefile for libTutf, the UTF-16 charset conversion library
#
TOPDIR=../..
THISDIR=$(TOPDIR)/libs/libTutf

SUBDIRS=

all: everything

VERSION=0.0.5
SO_VER=0

BINS=libTutf.so.$(VERSION)
ARLIBS=libTutf.a

LINKS=libTutf.so.$(SO_VER) libTutf.so

OBJS_libTutf.so.$(VERSION)=libTutf.o
OBJS_libTutf.a=libTutf_u.o

CC_FLAGS_libTutf.o+=-fPIC
CC_FLAGS+=-D_REENTRANT

LD_FLAGS_libTutf.so.$(VERSION)=$(LD_FLAGS_SOLIB_NAME)libTutf.so.$(SO_VER)

-include $(TOPDIR)/conf/conf.current
-include .modules
-include .depend
-include $(TOPDIR)/.hdepend


ifeq ($(CONF__UNICODE),y)
  everything: lib
  install: install-real
else
  everything install:
	@rm -f $(ARLIBS) $(BINS) $(LINKS) ; \
	 $(ECHO) ; \
	 $(ECHO) "You disabled \"Build all code with Unicode support\"," ; \
	 $(ECHO) "so the library libTutf will NOT be compiled!" ; \
	 $(ECHO)
endif

#
# handy targets
#


ifeq ($(CONF__SHLIBS),y)
  lib: $(ARLIBS) $(BINS) links
else
  lib: $(ARLIBS)
	@rm -f $(BINS) $(LINKS) ; \
	 $(ECHO) ; \
	 $(ECHO) "You disabled \"Enable shared libraries (ELF)\"," ; \
	 $(ECHO) "so only the static library libTutf.a will be compiled!" ; \
	 $(ECHO)
endif


#
# include the do-everything file.
# Must correctly set all CC_FLAGS* and LD_FLAGS* before including this!
#
#
include $(TOPDIR)/makerules



links: $(LINKS)

libTutf.so.$(SO_VER):
	$(LN) libTutf.so.$(VERSION) libTutf.so.$(SO_VER)

libTutf.so:
	$(LN) libTutf.so.$(SO_VER) libTutf.so

install-headers:
	$(INSTALL) -d $(DESTDIR)$(includedir)/Tutf && \
	$(INSTALL-TXT) $(TOPDIR)/include/Tutf/*.h $(DESTDIR)$(includedir)/Tutf

install-real: install-headers
	$(INSTALL) -d $(DESTDIR)$(libdir) && \
	$(INSTALL-TXT) $(ARLIBS) $(DESTDIR)$(libdir) && \
	if [ -f libTutf.so.$(VERSION) ]; then \
		$(INSTALL) libTutf.so.$(VERSION) $(DESTDIR)$(libdir) && \
		$(LN) libTutf.so.$(VERSION) $(DESTDIR)$(libdir)/libTutf.so.$(SO_VER) && \
		$(LN) libTutf.so.$(SO_VER)  $(DESTDIR)$(libdir)/libTutf.so; \
	fi
	
clean:
	rm -f .*.flags .*.link .*.arlib libTutf.so* libTutf.a $(_ALLOBJS)

