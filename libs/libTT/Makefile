#
# Makefile for libTT, `The Text Toolkit' library sitting on top of libTw
#
TOPDIR=../..
SUBDIRS=

all: everything

VERSION=0.0.4
SO_VER=0

BINS=libTT.so.$(VERSION)
ARLIBS=libTT.a


OBJS_libTT.so.$(VERSION)=libTT.o missing.o
OBJS_libTT.a=libTT_u.o missing_u.o

CC_FLAGS_libTT.o+=-fPIC -D_REENTRANT
CC_FLAGS_missing.o+=-fPIC -D_REENTRANT
CC_FLAGS_libTT_u.o+=-D_REENTRANT
CC_FLAGS_missing_u.o+=-D_REENTRANT

LD_FLAGS_libTT.so.$(VERSION)=$(LIBTW) $(LD_FLAGS_SOLIB)libTT.so.$(SO_VER)

-include $(TOPDIR)/conf/conf.current
-include .modules
-include .depend

ifneq ($(CONF_SOCKET),n)
  everything: lib
  install: install-real
  ifeq ($(CONF__UNICODE),y)
    LD_FLAGS_libTT.so.$(VERSION)+=$(LIBTUTF)
  endif
else
  everything: warn
  install: warn
  warn:
	@$(ECHO) ; \
	 $(ECHO) "You disabled \"Support remote socket connections\"," ; \
	 $(ECHO) "so the library libTT.so will NOT be compiled!" ; \
	 $(ECHO)
endif

#
# include the do-everything file.
# Must correctly set all CC_FLAGS* and LD_FLAGS* before including this!
#
include $(TOPDIR)/makerules


#
# handy targets
#
ifeq ($(CONF__SHLIBS),y)
  lib: $(ARLIBS) $(BINS) links
else
  lib: $(ARLIBS)
	@$(ECHO) ; \
	 $(ECHO) "You disabled \"Enable shared libraries (ELF)\"," ; \
	 $(ECHO) "so only the static library libTT.a will be compiled!" ; \
	 $(ECHO)
endif

autogen: methodsm4.h TTm4.h

methodsm4.h: methods_base.h

missing.c missing_u.c:
	$(LN) $(TOPDIR)/server/missing.c missing.c && \
	$(LN) $(TOPDIR)/server/missing.c missing_u.c

links: libTT.so.$(SO_VER) libTT.so

libTT.so.$(SO_VER):
	$(LN) libTT.so.$(VERSION) libTT.so.$(SO_VER)

libTT.so:
	$(LN) libTT.so.$(SO_VER) libTT.so

libTT_u.c:
	$(LN) libTT.c libTT_u.c

install-headers:
	$(INSTALL) -d $(DESTDIR)$(includedir)/TT && \
	$(INSTALL-TXT) $(TOPDIR)/include/TT/*.h $(DESTDIR)$(includedir)/TT && \
	$(LN) TT/TT++.h $(DESTDIR)$(includedir)/libTT++.h && \
	$(LN) TT/TT.h $(DESTDIR)$(includedir)/libTT.h

install-real: install-headers
	$(INSTALL) -d $(DESTDIR)$(libdir) && \
	$(INSTALL-TXT) $(ARLIBS) $(DESTDIR)$(libdir) && \
	if [ -f libTT.so.$(VERSION) ]; then \
		$(INSTALL) libTT.so.$(VERSION) $(DESTDIR)$(libdir) && \
		$(LN) libTT.so.$(VERSION) $(DESTDIR)$(libdir)/libTT.so.$(SO_VER) && \
		$(LN) libTT.so.$(SO_VER)  $(DESTDIR)$(libdir)/libTT.so; \
	fi
	
clean:
	rm -f .*.flags .*.link .*.arlib libTT.so* libTT.a $(_ALLOBJS)

