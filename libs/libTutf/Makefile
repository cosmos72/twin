#
# Makefile for libTutf, the UTF-16 charset conversion library
#
TOPDIR=../..
THISDIR=$(TOPDIR)/libs/libTutf

include $(TOPDIR)/include/Tutf/makeversion
-include $(TOPDIR)/makeautoconf

SUBDIRS=

all: everything

SHLIBS=libTutf.la
ARLIBS=libTutf.a

OBJS_libTutf.la=libTutf.lo
OBJS_libTutf.a=libTutf.o

CC_FLAGS+=-D_REENTRANT

-include $(TOPDIR)/conf/conf.current
-include .modules
-include .depend
-include $(TOPDIR)/.hdepend


ifeq ($(CONF__UNICODE),y)
  everything: lib
  install: install-real
else
  everything install:
	@rm -f $(SHLIBS) $(ARLIBS) ; \
	 echo ; \
	 echo "You disabled \"Build all code with Unicode support\"," ; \
	 echo "so the library libTutf will NOT be compiled!" ; \
	 echo
endif

#
# handy targets
#


ifeq ($(CONF__SHLIBS),y)
  lib: $(ARLIBS) $(SHLIBS)
else
  lib: $(ARLIBS)
	@rm -f $(SHLIBS) ; \
	 echo ; \
	 echo "You disabled \"Enable shared libraries (ELF)\"," ; \
	 echo "so only the static library libTutf.a will be compiled!" ; \
	 echo
endif


#
# include the do-everything file.
# Must correctly set all CC_FLAGS* and LD_FLAGS* before including this!
#
#
include $(TOPDIR)/makerules


install-headers:
	$(INSTALL-DIR) $(DESTDIR)$(includedir)/Tutf && \
	$(INSTALL-TXT) $(TOPDIR)/include/Tutf/*.h $(DESTDIR)$(includedir)/Tutf

install-dir:
	$(INSTALL-DIR) $(DESTDIR)$(libdir)

install-real: install-headers install-dir $(foreach B,$(SHLIBS) $(ARLIBS),install-lib-$(B))
	
clean:
	rm -f .*.flags .*.shlib .*.arlib *.la *.a *.o *.lo *.so *.so.* ; \
	rm -fr .libs

