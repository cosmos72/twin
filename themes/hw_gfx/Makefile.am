themesdir = $(pkgdatadir)/themes/hw_gfx

themes_DATA = README Ganymede_8x16.xpm Ganymede_9x19.xpm ShinyMetal_6x13.xpm ShinyMetal_8x16.xpm absolute-e_8x16.xpm goldenblue_9x19.xpm granite_8x16.xpm granite_9x19.xpm icewm_8x16.xpm icewm_9x19.xpm kde2_9x19.xpm mac_8x16.xpm mac_9x19.xpm mono_8x16.xpm mono_9x19.xpm

install-data-hook:
	rm -f $(DESTDIR)$(themesdir)/default_6x13.xpm $(DESTDIR)$(themesdir)/default_8x16.xpm $(DESTDIR)$(themesdir)/default_9x19.xpm
	$(LN_S) ShinyMetal_6x13.xpm $(DESTDIR)$(themesdir)/default_6x13.xpm
	$(LN_S) Ganymede_8x16.xpm   $(DESTDIR)$(themesdir)/default_8x16.xpm
	$(LN_S) Ganymede_9x19.xpm   $(DESTDIR)$(themesdir)/default_9x19.xpm
