#
# top Makefile
#
TOPDIR=.

all: boot.dir lib.dir twin clients.dir

#
include Rules
#

config $(CONF): 
	$(BASH) scripts/Configure

reconfig: config all

boot.dir: $(CONF)

lib.dir: $(CONF)

twin: $(CONF) server.dir

clients.dir: $(CONF)

#
# `cp -df' : `cp' with force overwrite, don't dereference symlinks/hardlinks
#
# `mkdir -p' or `mkdirhier' : make a directory and all missing directory parents
#
install: all
	cp -df server/twin_wrapper server/twin_real server/twin clients/tw* $(DESTDIR)/bin 
	chmod u+s $(DESTDIR)/bin/twin_real
	cp -df lib/libTw.so* $(DESTDIR)/lib
	cp -df include/libTw*.h $(DESTDIR)/include
	mkdir -p $(DESTDIR)/lib/twin/modules/HW
	if [ "$(wildcard server/*.so)" ]; then \
	  cp -df server/*.so $(DESTDIR)/lib/twin/modules; \
	fi
	if [ "$(wildcard server/HW/*.so)" ]; then \
	  cp -df server/HW/*.so $(DESTDIR)/lib/twin/modules/HW; \
	fi
	
clean:
	$(MAKE) clean -C boot
	$(MAKE) clean -C lib
	$(MAKE) clean -C server
	$(MAKE) clean -C clients

distclean: clean
	rm -f $(CONF) scripts/MkDep boot/sizes.h `find . -name .modules -o -name .depend`

