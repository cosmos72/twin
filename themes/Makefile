#
# Makefile for twin themes
#
#

TOPDIR=..
THISDIR=$(TOPDIR)/themes

SUBDIRS=

all: links




fontsizes:=$(sort $(patsubst %.xpm,%,$(filter %.xpm,$(subst _, ,$(wildcard hw_gfx/*_*.xpm)))))

default_gfx_themes:=$(patsubst %,hw_gfx/default_%.xpm,$(fontsizes))



#
# common rules
#
include $(TOPDIR)/include/makeversion
-include $(TOPDIR)/makeautoconf
include $(TOPDIR)/makerules



links: $(default_gfx_themes)

hw_gfx/default_%.xpm:
	$(LN) $(notdir $(word 1,$(filter-out $@,$(wildcard hw_gfx/*_$*.xpm)))) $@

install-dir:
	$(INSTALL-DIR) $(DESTDIR)$(datadir)/twin/themes/hw_gfx

install-themes:	
	$(INSTALL-TXT) hw_gfx/* $(DESTDIR)$(datadir)/twin/themes/hw_gfx
	$(foreach S,$(fontsizes),$(LN) \
	 $(notdir $(word 1,$(filter-out hw_gfx/default_%.xpm,$(wildcard hw_gfx/*_$(S).xpm)))) \
	 $(DESTDIR)$(datadir)/twin/themes/hw_gfx/default_$(S).xpm ; )


install: install-dir install-themes


clean:
	rm -f hw_gfx/default_*.xpm
