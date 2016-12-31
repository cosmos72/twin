#!/bin/bash

for path in txt/*.txt; do
  if [ "$path" = "txt/UnicodeData.txt" ]; then
    continue
  fi
  f="${path#txt/}"
  f="${f%.txt}"
  F="`echo \"$f\" | tr 'a-z' 'A-Z'`"
  OPTS=
  if [ "$F" = NEXTSTEP -o "${F:0:4}" = MAC_ ]; then
    OPTS="--insert-missing=128"
  elif [ "${F:0:6}" = ADOBE_ ]; then
    OPTS="--insert-missing=32"
  fi
  echo "./helper-from-unicode.org.bash $F $OPTS < $path > include/$f.h"
  ./helper-from-unicode.org.bash "$F" $OPTS < "$path" > include/"$f".h
done

