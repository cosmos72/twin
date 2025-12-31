#!/bin/bash

for path in txt/*.txt; do
  if [ "$path" = "txt/UnicodeData.txt" -o "$path" = "txt/EastAsianWidth.txt" ]; then
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
  echo "scripts/helper-from-unicode.org.bash $F $OPTS < $path > include/Tutf/$f.h"
  scripts/helper-from-unicode.org.bash "$F" $OPTS < "$path" > "include/Tutf/$f.h"
done

