


all:
	@echo "You should run './configure [options]'" ; \
	 echo "to configure twin for your system" ; \
	 false

build:
	B="build-on-`hostname`" ; \
	mkdir "$$B" 2>/dev/null ; \
	cd "$$B" && \
	$(MAKE) -f ../Makefile.sub TOP_SRCDIR="../"

install:
	B="build-on-`hostname`" ; \
	cd "$$B" && \
	$(MAKE) install
