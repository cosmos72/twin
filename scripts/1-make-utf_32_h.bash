#!/bin/bash

mkdir include 2>/dev/null

exec >include/utf_32.h

cat <<EOF
/*
 *  utf_32.h  --  include for UTF-32 (Unicode) characters database
 *
 *  PLEASE DO NOT EDIT!
 *
 *  This file was automatically generated from http://unicode.org/Public/UNIDATA/UnicodeData.txt
 *  using the script twin/scripts/1-make-utf_32_h.bash
 *
 *  All changes to this file will be lost when the script is re-executed.
 */

#ifndef TUTF_UTF_32_H
#define TUTF_UTF_32_H


/* auxiliary definitions */
#include <Tutf/utf_32_aux.h>


EOF


cut -d';' -f1,2,11 <txt/UnicodeData.txt | tr ', -' '___' | sed -e 's/<[^>]*>/_/g' -e 's/_([^)]*)//g' | tr ';' ' '  | \
while read i n1 n2; do
 if [ "$n1" != "" -a "$n1" != "_" ]; then
   echo "#define T_UTF_32_$n1	0x$i"
 elif [ "$n2" != "" -a "$n2" != "_" ]; then
   echo "#define T_UTF_32_CTRL_$n2	0x$i"
 fi
done

echo ""
echo ""
echo "#endif /* TUTF_UTF_32_H */"
