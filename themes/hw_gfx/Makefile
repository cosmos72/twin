#
# Makefile for twin hw_gfx themes
#
#
TOPDIR=../..
THISDIR=$(TOPDIR)/themes/hw_gfx

SUBDIRS=

INSTALLSUBDIR=/twin/themes/hw_gfx

all: links


#
# common rules
#
include $(TOPDIR)/include/makeversion
-include $(TOPDIR)/makeautoconf
include $(TOPDIR)/makerules



fontsizes:=$(sort $(patsubst %.xpm,%,$(filter %.xpm,$(subst _, ,$(wildcard *_*.xpm)))))

default_gfx_themes:=$(patsubst %,default_%.xpm,$(fontsizes))






links: $(default_gfx_themes)

default_%.xpm:
	$(LN) $(word 1,$(filter-out $@,$(wildcard *_$*.xpm))) $@

install-dir:
	$(INSTALL-DIR) $(DESTDIR)$(datadir)$(INSTALLSUBDIR)

install-themes:	
	$(INSTALL-TXT) README *.xpm $(DESTDIR)$(datadir)$(INSTALLSUBDIR)
	$(foreach X,$(fontsizes),$(LN) \
	 $(notdir $(word 1,$(filter-out default_%.xpm,$(wildcard *_$(X).xpm)))) \
	 $(DESTDIR)$(datadir)/twin/themes/hw_gfx/default_$(X).xpm ; )


install: install-dir install-themes



uninstall-dir:
	rmdir $(DESTDIR)$(datadir)$(INSTALLSUBDIR) || true
	rmdir $(DESTDIR)$(datadir)/twin/themes || true
	rmdir $(DESTDIR)$(datadir)/twin || true

uninstall-themes:	
	cd $(DESTDIR)$(datadir)$(INSTALLSUBDIR) && \
	   rm -f README $(notdir $(wildcard *.xpm *_.xpm))

uninstall: uninstall-themes uninstall-dir

