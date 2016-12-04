#!/bin/sh
set -e
set -x
libtoolize --copy --install --force --ltdl=libs/libltdl
aclocal -I admin
autoheader
automake --add-missing --copy
autoconf
