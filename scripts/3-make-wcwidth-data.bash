#!/bin/bash

exec >include/wcwidth_data.h

cat <<EOF
/*
 *  wcwidth_data.h  --  compute display width of UTF-32 (Unicode) characters
 *
 *  PLEASE DO NOT EDIT!
 *
 *  This file was automatically generated from http://unicode.org/Public/UNIDATA/EastAsianWidth.txt
 *  using the script twin/scripts/3-make-wcwidth-data.bash
 *
 *  All changes to this file will be lost when the script is re-executed.
 */

#ifndef TUTF_WCWIDTH_DATA_H
#define TUTF_WCWIDTH_DATA_H

static const trune wide[][2] = {
EOF

cat txt/EastAsianWidth.txt | grep '^[0-9A-F]' | cut -d' ' -f1 | grep -v ';N' | sed -e 's/\.\./ /' -e 's/;/ /'  | \
while read lo hi width; do
 if [ "$width" = "" ]; then
   width="$hi"
   hi="$lo"
 fi
 if [ "$width" = "F" -o "$width" = "W" ]; then
   echo "    { 0x$lo, 0x$hi },"
 fi
done

cat <<EOF
};

#endif /* TUTF_WCWIDTH_DATA_H */
EOF
