#
# top Makefile
#
TOPDIR=.

make_VERSION:=$(shell $(MAKE) -v)
ifneq ($(wordlist 1,2,$(make_VERSION)),GNU Make)
 all: make_is_not_GNU
endif

all: boot.dir docs.dir lib.dir twin clients.dir

make_is_not_GNU:
	@echo "Warning: GNU Make is required to build twin, but the '$(MAKE)' you are using" ; \
	 echo "does not seem to be GNU Make. build will probably fail... " ; \
	 echo "running '$(MAKE) -v' to get version information resulted in: " ; \
	 echo "--------------------------------------------------------" ; \
	 $(MAKE) -v ; \
	 echo "--------------------------------------------------------"

#
ifneq ($(wildcard $(TOPDIR)/conf/config.status),)
  include $(TOPDIR)/conf/config.status
endif
include MakeRules
#
	

boot.dir: $(CONF)

lib.dir: $(CONF)

twin: $(CONF) server.dir

clients.dir: $(CONF)

ifneq ($(CONF_SOCKET),n)
  install-lib:
	$(MAKE) install -C lib DESTDIR=$(DESTDIR)
	$(INSTALL) -d $(DESTDIR)$(includedir)/Tw
	$(INSTALL) include/Tw/*.h $(DESTDIR)$(includedir)/Tw
	$(LN) Tw/Tw++.h $(DESTDIR)$(includedir)/libTw++.h
	$(LN) Tw/Tw.h $(DESTDIR)$(includedir)/libTw.h
	$(LN) Tw/Twerrno.h $(DESTDIR)$(includedir)/libTwerrno.h
	$(LN) Tw/Twkeys.h $(DESTDIR)$(includedir)/libTwkeys.h
  install-clients:
	$(MAKE) install -C clients DESTDIR=$(DESTDIR)
else
  install-lib:
  install-clients:
endif

#
Torture: torture
	$(MAKE) Torture -C lib
	$(MAKE) Torture -C server
	$(MAKE) Torture -C clients
	
#
install: all install-lib install-clients
	$(MAKE) install -C docs DESTDIR=$(DESTDIR) 
	$(MAKE) install -C server DESTDIR=$(DESTDIR) 
	$(INSTALL) -d $(DESTDIR)$(libdir)/twin
	$(INSTALL) system.twinrc $(DESTDIR)$(libdir)/twin
	$(LN) system.twinrc $(DESTDIR)$(libdir)/twin/.twinrc
	@echo [1m ; \
	 echo If this is the first time you installed twin $(TWIN_VERSION_STR) ; \
	 echo you probably need to run \"ldconfig\" now. ; \
	 echo [0m

clean:
	$(MAKE) clean -C boot
	$(MAKE) clean -C scripts
	$(MAKE) clean -C docs
	$(MAKE) clean -C lib
	$(MAKE) clean -C server
	$(MAKE) clean -C clients

distclean: clean
	rm -f $(CONF) scripts/MkDep boot/sizes.h `find . -name .modules -o -name .depend`

pure: clean
	rm -f $(CONF) scripts/MkDep boot/sizes.h `find . -name .modules -o -name .depend` \
	      server/rcparse_tab.c server/rcparse_tab.h server/rcparse_lex.c

