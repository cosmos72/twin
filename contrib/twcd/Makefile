#
# Makefile for various contributed libTw clients
#

#
# list of subdirectories to build
#
SUBDIRS=

# add your binary to this list
#
BINS=twcd

# place the object files for your binary here: you must add a line
# OBJS_<your binary>=<list of object files>
#
OBJS_twcd=twcd.o
LD_FLAGS_twcd+=-lcdaudio


#
# where is this directory with respect of source topdir?
#
TOPDIR=../..
THISDIR=$(TOPDIR)/contrib/twcd

#
# --------------- no user-serviceable parts below this line ----------------
#
-include $(TOPDIR)/makeautoconf

LD_FLAGS+=$(LD_LIBTW)

all: everything


-include $(TOPDIR)/conf/conf.current
-include .modules
-include .depend
-include $(TOPDIR)/.hdepend

#
# include the do-everything file.
# Must correctly set all CC_FLAGS* and LD_FLAGS* before including this!
#
include $(TOPDIR)/makerules


ifneq ($(CONF_SOCKET),n)
  everything: $(BINS)
else
  everything:
	@echo ; \
	 echo "You disabled \"Support remote socket connections\"," ; \
	 echo "so the clients will NOT be compiled!" ; \
	 echo
endif




install:
	$(INSTALL-DIR) $(DESTDIR)$(bindir)
	$(INSTALL-BIN) $(BINS) $(DESTDIR)$(bindir)

clean:
	rm -f .*.flags .*.bin gmon.out core $(BINS) *.la *.a *.o *.lo *.so *.so.* ; \
	rm -fr .libs

