#
# Makefile for various contributed libTw clients
#

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

LDFLAGS+=-L$(TOPDIR)/lib -lTw

all: everything


include $(TOPDIR)/conf/config.status
-include .depend
-include .modules

include $(TOPDIR)/MakeRules

ifneq ($(CONF_SOCKET),n)
  everything: $(BINS)
else
  everything:
	@echo
	@echo "You disabled \"Support remote socket connections\","
	@echo "so the clients will NOT be compiled!"
	@echo
endif

install:
	$(CP) $(BINS) $(DESTDIR)/bin 

clean:
	rm -f .*.flags .*.link gmon.out core $(OBJS) $(BINS)

