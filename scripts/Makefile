#
# scripts Makefile
#
TOPDIR=..


all: bins

OBJS_Bitmap:=bitmap.o
OBJS_Bin2c:=bin2c.o

ALLOBJS:=$(OBJS_Bitmap)


BINS=Bitmap
MODBINS=

-include .depend
-include .modules

#
# common rules
#
include $(TOPDIR)/MakeRules


bins: $(BINS)

modules: $(MODBINS)

clean:
	rm -f .*.flags .*.link .*.arlib $(ALLOBJS) $(BINS) $(MODBINS)

install:

