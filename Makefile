#
# top Makefile
#
TOPDIR=.

all: boot.dir lib.dir twin clients.dir

#
ifneq ($(wildcard $(TOPDIR)/conf/config.status),)
  -include $(TOPDIR)/conf/config.status
endif
include Rules
#

config $(CONF):
	@echo Starting configure for twin $(TW_VERSION)...
	@echo
	$(BASH) scripts/Configure

reconfig: config all

boot.dir: $(CONF)

lib.dir: $(CONF)

twin: $(CONF) server.dir

clients.dir: $(CONF)


ifneq ($(CONF_SOCKET),n)
  install-lib:
	$(CP) lib/libTw.so* $(DESTDIR)/lib
	$(CP) include/libTw*.h $(DESTDIR)/include
  install-clients:
	$(CP) clients/tw* clients/*/tw* $(DESTDIR)/bin 
else
  install-lib:
  install-clients:
endif


#
install: all install-lib install-clients
	$(MAKE) install -C server
	@echo
	@echo If this is the first time you installed twin $(TW_VERSION)
	@echo you probably need to run \"ldconfig\" now.
	@echo

clean:
	$(MAKE) clean -C boot
	$(MAKE) clean -C lib
	$(MAKE) clean -C server
	$(MAKE) clean -C clients

distclean: clean
	rm -f $(CONF) scripts/MkDep boot/sizes.h `find . -name .modules -o -name .depend`

