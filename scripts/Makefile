#
# scripts Makefile
#
TOPDIR=..


all: bins

OBJS_Torture:=torture.o
OBJS_Bin2c:=bin2c.o

ALLOBJS:=$(OBJS_Torture) $(OBJS_Bin2c)


BINS=Torture Bin2c
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
	rm -f .*.flags .*.link .*.arlib $(ALLOBJS) Torture

install:

