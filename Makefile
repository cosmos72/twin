#
# top Makefile
#
TOPDIR=.
SUBDIRS=$(wildcard docs libs clients server)

make_VERSION:=$(wordlist 1,2,$(shell $(MAKE) -v))
ifneq ($(make_VERSION),GNU Make)
 all: make_is_not_GNU
endif

all: subdirs

make_is_not_GNU:
	@echo "Warning: GNU Make is required to build twin, but the '$(MAKE)' you are using" ; \
	 echo "does not seem to be GNU Make. build will probably fail... " ; \
	 echo "running '$(MAKE) -v' to get version information resulted in: " ; \
	 echo "--------------------------------------------------------" ; \
	 $(MAKE) -v ; \
	 echo "--------------------------------------------------------"


#
ifneq ($(wildcard $(TOPDIR)/conf/config.status),)
  include $(TOPDIR)/conf/config.status
endif


include $(TOPDIR)/MakeRules



#
# ensure libraries are built first
#
dir-clients dir-server: dir-libs

fakeconf:
	if [ ! -e $(CONF) ]; then touch $(CONF); fi

#
Torture: fakeconf autogen
	
#
install:
	$(INSTALL) -d $(DESTDIR)$(libdir)/twin
	$(INSTALL) system.twinrc $(DESTDIR)$(libdir)/twin
	$(LN) system.twinrc $(DESTDIR)$(libdir)/twin/.twinrc
	@echo [1m ; \
	 echo If this is the first time you installed twin $(TWIN_VERSION_STR) ; \
	 echo you probably need to run \"ldconfig\" now. ; \
	 echo [0m

clean: fakeconf autogen
	rm -f .*.flags .*.link .*.arlib gmon.out core log
	if [ ! -s $(CONF) ]; then rm -f $(CONF); fi


distclean: clean
	rm -f $(CONF) $(LINKS) \
	      scripts/Mkdep scripts/Bitmap scripts/Getsizes \
	      include/Tw/datasizes.h include/Tw/uni_types.h \
	      `find . -name .modules -o -name .depend` \
	      `find . -type l`

pure: clean
	rm -f $(CONF) $(LINKS) \
	      scripts/Mkdep scripts/Bitmap scripts/Getsizes \
	      include/Tw/datasizes.h include/Tw/uni_types.h \
	      `find . -name .modules -o -name .depend -o -name \*macros.h` \
	      `find . -type l` server/rcparse_*.?

