#!/usr/bin/env bash

declare -a wide

cat txt/EastAsianWidth.txt | grep '^[0-9A-F]' | cut -b1-18 | grep '; [FW]' | sed -e 's/\.\./ /' -e 's/;/ /' > txt/wide.txt

exec > include/Tutf/wcwidth_data.h

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

static const unsigned char wide_bitmap[0x4000] = {
EOF

while read lox hix width; do
 lo=$(printf '%d' "0x$lox")
 if [ "$lo" -ge 131072 ]; then
   break
 fi
 if [ "$width" = "" ]; then
   width="$hix"
   hix="$lox"
 fi
 if [ "$width" = "F" -o "$width" = "W" ]; then
   hi=$(printf '%d' "0x$hix")
   while [ "$lo" -le "$hi" ]; do
     wide[$lo]=x
     : $(( lo = lo + 1 ))
   done
 fi
done < txt/wide.txt

lo=0
while [ "$lo" -lt 131072 ]; do
  bits=""
  bit=0
  while [ "$bit" -lt 8 ]; do
    i=$(( lo + bit ))
    bits="${#wide[$i]}$bits"
    : $(( bit = bit + 1 ))
  done
  if [ "$bits" = "00000000" ]; then
    echo '0,'
  else
    printf '0x%x,\n' "0b$bits"
  fi
  : $(( lo = lo + 8 ))
done

cat << EOF
};

#endif /* TUTF_WCWIDTH_DATA_H */
EOF
