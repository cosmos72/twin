#
# Makefile for twin - Text mode WINdow manager
#
#

TOPDIR=..

OBJS_getsizes=getsizes.o
ALLOBJS:=$(OBJS_getsizes)

BINS=getsizes
MODBINS=

#
# target
#
all: sizes.h

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

sizes.h: getsizes.c $(TOPDIR)/include/twin.h
	$(CC) $(CFLAGS) getsizes.c $(LDFLAGS) -o getsizes
	./getsizes > $(TOPDIR)/include/sizes.h
	$(LN) $(TOPDIR)/include/sizes.h sizes.h

clean:
	rm -f .*.flags .*.link gmon.out core getsizes $(ALLOBJS)

