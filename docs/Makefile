#
# Makefile for docs/
#
#

TOPDIR=..

ALLOBJS=
BINS=
MODBINS=

DOCS=Configure Tutorial libTw.txt libTwerrno.h twin.1
TOPDOCS=README BUGS INSTALL README.porting COPYING twin-$(TWIN_VERSION_STR).lsm \
	COPYING.LIB Changelog.txt 

#
# target
#
all: 

#
# read configuration
#
include $(TOPDIR)/conf/config.status
-include .depend
-include .modules

#
# common rules
#
include $(TOPDIR)/MakeRules


#
# All done. Now just set a few special targets
#

Torture: torture

clean:

install:
	$(INSTALL) -d $(DESTDIR)$(mandir)/man1
	$(INSTALL) twin.1 $(DESTDIR)$(mandir)/man1
	$(INSTALL) -d $(DESTDIR)$(datadir)/twin/docs
	$(INSTALL) $(DOCS) $(DESTDIR)$(datadir)/twin/docs
	cd $(TOPDIR) && $(INSTALL) $(TOPDOCS) $(DESTDIR)$(datadir)/twin
