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
	if [ -d $(DESTDIR)/share/doc ]; then \
	  $(MKDIR) $(DESTDIR)/share/doc/twin/docs && \
	  $(CP) $(DOCS) $(DESTDIR)/share/doc/twin/docs; \
	  cd $(TOPDIR) && \
	  $(CP) $(TOPDOCS) $(DESTDIR)/share/doc/twin; \
	fi

