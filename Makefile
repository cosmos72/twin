#
# top Makefile
#
TOPDIR=.

all: boot.dir lib.dir twin clients.dir

#
ifneq ($(wildcard $(TOPDIR)/conf/config.status),)
  -include $(TOPDIR)/conf/config.status
endif
include MakeRules
#

torture: nullconfig scripts.dir
	@echo Starting torture test ...
	@echo
	if [ ! -f conf/config.backup ]; then cp conf/config.status conf/config.backup; fi
	scripts/Torture $(MAKE) `cat conf/config.list` > /dev/null
	
boot.dir: $(CONF)

lib.dir: $(CONF)

twin: $(CONF) server.dir

clients.dir: $(CONF)

ifneq ($(CONF_SOCKET),n)
  install-lib:
	$(MAKE) install -C lib
	$(CP) include/libTw*.h $(DESTDIR)/include
  install-clients:
	$(MAKE) install -C clients
else
  install-lib:
  install-clients:
endif


#
install: all install-lib install-clients
	$(MAKE) install -C server
	if [ -f $(DESTDIR)/lib/twin/system.twinrc ]; then \
	   $(CP) $(DESTDIR)/lib/twin/system.twinrc $(DESTDIR)/lib/twin/system.twinrc.bak; \
	fi && \
	$(CP) system.twinrc $(DESTDIR)/lib/twin && \
	$(LN) system.twinrc $(DESTDIR)/lib/twin/.twinrc
	@echo [1m ; \
	 echo If this is the first time you installed twin $(TW_VERSION) ; \
	 echo you probably need to run \"ldconfig\" now. ; \
	 echo [0m

clean:
	$(MAKE) clean -C boot
	$(MAKE) clean -C scripts
	$(MAKE) clean -C lib
	$(MAKE) clean -C server
	$(MAKE) clean -C clients

distclean: clean
	rm -f $(CONF) scripts/MkDep boot/sizes.h `find . -name .modules -o -name .depend`

pure: distclean
	rm -f server/rcparse_tab.c server/rcparse_tab.h server/rcparse_lex.c

