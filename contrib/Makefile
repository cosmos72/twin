#
# Makefile for various contributed libTw clients
#

#
# list of subdirectories to build
#
SUBDIRS=

# add your binary to this list
#
BINS=twbiff

# place the object files for your binary here: you must add a line
# OBJS_<your binary>=<list of object files>
#
OBJS_twbiff=biff.o


#
# --------------- no user-serviceable parts below this line ----------------
#
TOPDIR=..
THISDIR=$(TOPDIR)/contrib

LD_FLAGS+=-L$(TOPDIR)/lib -lTw

all: everything


-include $(TOPDIR)/conf/conf.current
-include .modules
-include .depend
-include $(TOPDIR)/.hdepend

include $(TOPDIR)/makerules


ifneq ($(CONF_SOCKET),n)
  everything: $(BINS)
else
  everything:
	@$(ECHO) ; \
	 $(ECHO) "You disabled \"Support remote socket connections\"," ; \
	 $(ECHO) "so the clients will NOT be compiled!" ; \
	 $(ECHO)
endif


#
# include the do-everything file.
# Must correctly set all CC_FLAGS* and LD_FLAGS* before including this!
#
include $(TOPDIR)/makerules


install:
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) $(BINS) $(DESTDIR)$(bindir)

clean:
	rm -f .*.flags .*.link gmon.out core $(OBJS) $(BINS)

