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

config $(CONF):
	@echo Starting configure for twin $(TW_VERSION) ...
	@echo
	$(BASH) scripts/Configure

menuconf menuconfig:
	@echo Starting configure for twin $(TW_VERSION) ...
	@echo
	$(BASH) scripts/Configure.dialog

reconfig: config all

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

