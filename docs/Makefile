#
# Makefile for docs/
#
#

TOPDIR=..
SUBDIRS=

ALLOBJS=
BINS=
MODBINS=

DOCS=Configure Tutorial libTw.txt libTwerrno.h twin.1
TOPDOCS=README BUGS INSTALL README.porting COPYING twin-current.lsm \
	COPYING.LIB Changelog.txt 

#
# target
#
all: 

#
# read configuration
#
-include $(TOPDIR)/conf/config.status
-include .modules
-include .depend

#
# include the do-everything file.
# Must correctly set all CFLAGS* and LDFLAGS* before including this!
#
#
include $(TOPDIR)/MakeRules


#
# All done. Now just set a few special targets
#

clean:

install:
	$(INSTALL) -d $(DESTDIR)$(mandir)/man1
	$(INSTALL-TXT) twin.1 $(DESTDIR)$(mandir)/man1
	$(INSTALL) -d $(DESTDIR)$(datadir)/twin/docs
	$(INSTALL-TXT) $(DOCS) $(DESTDIR)$(datadir)/twin/docs
	cd $(TOPDIR) && $(INSTALL-TXT) $(TOPDOCS) $(DESTDIR)$(datadir)/twin
