# Copyright (C) 2000 by Massimiliano Ghilardi
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
#

#
# remove any trailing slash from top_srcdir (more robust) and add a single trailing slash
#

top_srcdir="`echo \"$1\" | sed 's%\(/\)*$%%'`/"


if test "$top_srcdir" = "/" -o ! -d "$top_srcdir"; then
  echo "Usage: scripts/Makefiles.sh <top_srcdir>"
  exit 1
fi

echo "creating all the Makefiles..."

MAKEFILES="`find \"$top_srcdir\" -name Makefile.sub | sed -e 's%^'\"$top_srcdir\"'%%' -e 's%.sub$%%'`"

if test "$top_srcdir" = "./"; then
  top_srcdir=
fi

for i in $MAKEFILES; do
  if echo "$i" | grep '/' >/dev/null 2>/dev/null; then
    i_DIR="`echo $i|sed 's%/[^/][^/]*$%%'`/"
    i_REVERSE_DIR="`echo \"$i_DIR\" | sed 's%[^/][^/]*/%../%g'`"
    mkdir -p "$i_DIR"
  else
    i_DIR=
    i_REVERSE_DIR=
  fi
  if test "`echo \"$top_srcdir\" | cut -b1`" = "/"; then
    # $top_srcdir is an absolute path
    i_TOP_SRCDIR="$top_srcdir"
  else
    # $top_srcdir is a relative path, fix it for each subdirectory
    i_TOP_SRCDIR="$i_REVERSE_DIR$top_srcdir"
  fi
  if test "$top_srcdir"; then
    i_THIS_SRCDIR="$i_TOP_SRCDIR$i_DIR"
  else
    i_THIS_SRCDIR=
  fi
  echo "
#
# This file was generated automatically by scripts/Makefiles.sh. Do not edit.
#

# TOP_SRCDIR
T:=$i_TOP_SRCDIR

# THIS_SRCDIR
S:=$i_THIS_SRCDIR

# TOP_BUILDDIR
B:=$i_REVERSE_DIR

include \$(S)Makefile.sub
" > $i

done

