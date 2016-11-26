#!/bin/sh
set -e
set -x
libtoolize --copy --install --force --ltdl=libs/libltdl
aclocal -I admin
autoheader
automake --foreign --add-missing --copy
autoconf
