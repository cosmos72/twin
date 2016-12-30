#!/bin/bash

for path in txt/*.txt; do
  if [ "$path" = "txt/UnicodeData.txt" ]; then
    continue
  fi
  f="${path#txt/}"
  f="${f%.txt}"
  F="`echo \"$f\" | tr 'a-z' 'A-Z'`"
  echo "./helper-from-unicode.org.bash $F < $path > include/$f.h"
  OPTS=
  if [ "$F" = nextstep ]; then
    OPTS="--insert-missing=128"
  elif [ "${F:0:6}" = adobe_ -o "${F:0:6}" = apple_ ]; then
    OPTS="--insert-missing=32"
  fi
  ./helper-from-unicode.org.bash "$F" $OPTS < "$path" > include/"$f".h
done

