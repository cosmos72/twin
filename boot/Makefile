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
all: $(TOPDIR)/include/Tw/datasizes.h

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

$(TOPDIR)/include/Tw/datasizes.h: getsizes.c
	$(CC) $(CFLAGS) getsizes.c $(LDFLAGS) -o getsizes
	./getsizes > $(TOPDIR)/include/Tw/datasizes.h

clean:
	rm -f .*.flags .*.link gmon.out core getsizes $(ALLOBJS)

