
TOPDIR=..
THISDIR=$(TOPDIR)/include

SUBDIRS=Tutf Tw TT

all: autogen

do-autogen: m4_incs version.h

include makeversion
include $(TOPDIR)/makerules

m4_incs: $(call M4_INCS,)

