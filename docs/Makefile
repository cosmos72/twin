#
# Makefile for docs/
#
#

TOPDIR=..
THISDIR=$(TOPDIR)/docs

SUBDIRS=

ALLOBJS=
BINS=
SHLIBS=

DOCS=Configure Tutorial libTw.txt twin.1 ltrace.conf
TOPDOCS:=$(patsubst %,$(TOPDIR)/%, README BUGS INSTALL README.porting \
	COPYING twin-current.lsm COPYING.LIB Changelog.txt)

#
# target
#
all: autogen

#
# read configuration: currently not needed
#
#-include $(TOPDIR)/conf/conf.current
#-include .modules
#-include .depend
#-include $(TOPDIR)/.hdepend

#
# include the do-everything file.
# Must correctly set all CC_FLAGS* and LD_FLAGS* before including this!
#
#
include $(TOPDIR)/makerules


#
# All done. Now just set a few special targets
#

autogen: alldocs

alldocs: ltrace.conf

ltrace.conf: m4/ltrace.conf.m4 $(wildcard m4/ltrace.conf.*.in $(TOPDIR)/include/m4/TT*.m4)
	$(M4) -I$(TOPDIR)/include $< > $@

clean:

install:
	$(INSTALL-DIR) $(DESTDIR)$(datadir)/man/man1 && \
	$(INSTALL-TXT) twin.1 $(DESTDIR)$(datadir)/man/man1 ; \
	$(INSTALL-DIR) $(DESTDIR)$(datadir)/twin/docs
	$(INSTALL-TXT) $(TOPDOCS) $(DESTDIR)$(datadir)/twin
	$(INSTALL-TXT) $(DOCS) $(DESTDIR)$(datadir)/twin/docs

uninstall:
	cd $(DESTDIR)$(datadir) && rm -f man/man1/twin.1 && \
	   cd twin && rm -f $(notdir $(TOPDOCS)) && \
	   cd docs && rm -f $(notdir $(DOCS)) || true
	rmdir $(DESTDIR)$(datadir)/twin/docs || true
	rmdir $(DESTDIR)$(datadir)/twin || true
