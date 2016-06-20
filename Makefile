# Copyright (C) 2000 by Massimiliano Ghilardi
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
#



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
