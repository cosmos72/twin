#
# Makefile for libTutf, the UTF-16 charset conversion library
#
TOPDIR=../..
SUBDIRS=

all: everything

VERSION=0.0.1
SO_VER=0

BINS=libTutf.so.$(VERSION)
ARLIBS=libTutf.a

OBJS_libTutf.so.$(VERSION)=libTutf.o
OBJS_libTutf.a=libTutf_u.o

CFLAGS_libTutf.o+=-fPIC -D_REENTRANT
CFLAGS_libTutf_u.o+=-D_REENTRANT

LDFLAGS_libTutf.so.$(VERSION)=$(LDFLAGS_SOLIB)libTutf.so.$(SO_VER)

include $(TOPDIR)/conf/config.status
-include .modules
-include .depend


ifneq ($(CONF_SOCKET),n)
  ifeq ($(CONF__UNICODE),y)
    everything: lib
    install: install-real
  else
    everything install:
	@$(ECHO) ; \
	 $(ECHO) "You disabled \"Build all code with Unicode support\"," ; \
	 $(ECHO) "so the library libTutf will NOT be compiled!" ; \
	 $(ECHO)
  endif
else
  everything install:
	@$(ECHO) ; \
	 $(ECHO) "You disabled \"Support remote socket connections\"," ; \
	 $(ECHO) "so the library libTw.so will NOT be compiled!" ; \
	 $(ECHO)
endif

#
# handy targets
#


ifeq ($(CONF__SHLIBS),y)
  lib: $(ARLIBS) $(BINS) links
else
  lib: $(ARLIBS)
	@$(ECHO) ; \
	 $(ECHO) "You disabled \"Enable shared libraries (ELF)\"," ; \
	 $(ECHO) "so only the static library libTutf.a will be compiled!" ; \
	 $(ECHO)
endif


#
# include the do-everything file.
# Must correctly set all CFLAGS* and LDFLAGS* before including this!
#
#
include $(TOPDIR)/MakeRules



links: libTutf.so.$(SO_VER) libTutf.so

libTutf.so.$(SO_VER):
	$(LN) libTutf.so.$(VERSION) libTutf.so.$(SO_VER)

libTutf.so:
	$(LN) libTutf.so.$(SO_VER) libTutf.so

libTutf_u.c:
	$(LN) libTutf.c libTutf_u.c

install-headers:
	$(INSTALL) -d $(DESTDIR)$(includedir)/Tutf && \
	$(INSTALL-TXT) $(TOPDIR)/include/Tutf/*.h $(DESTDIR)$(includedir)/Tutf

install-real: install-headers
	$(INSTALL) -d $(DESTDIR)$(libdir) && \
	$(INSTALL) libTutf.a $(DESTDIR)$(libdir) && \
	if [ -f libTutf.so.$(VERSION) ]; then \
		$(INSTALL) libTutf.so.$(VERSION) $(DESTDIR)$(libdir) && \
		$(LN) libTutf.so.$(VERSION) $(DESTDIR)$(libdir)/libTutf.so.$(SO_VER) && \
		$(LN) libTutf.so.$(SO_VER)  $(DESTDIR)$(libdir)/libTutf.so; \
	fi
	
clean:
	rm -f .*.flags .*.link .*.arlib libTutf.so* libTutf.a $(_ALLOBJS)

