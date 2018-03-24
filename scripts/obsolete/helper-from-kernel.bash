#!/bin/bash

i=0

while read line; do
  if [ "$line" = 'static wchar_t charset2uni[256] = {' -o \
       "$line" = 'static const wchar_t charset2uni[256] = {' -o \
       "$line" = 'static const unsigned short charset2uni[256] = {' ]; then
    while read a; do
      if [ "${a}" = '};' ]; then
        break 2;
      elif [ "${a:0:2}" != '/*' ]; then
        for ai in $a; do
          ai="${ai%,}"
          printf "0x%02x %s\n" "$i" "$ai"
          : $(( i=i+1 ))
	done
      fi
    done
  fi
done
