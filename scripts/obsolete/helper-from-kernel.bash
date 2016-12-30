#!/bin/bash

CHARSET="`echo \"$1\" | tr 'a-z' 'A-Z'`"

if [ "$2" = "" ]; then
  ORIGIN=Linux
else
  ORIGIN="$2"
fi

if [ "$CHARSET" = "" ]; then
  echo 'Usage: helper-from-kernel.bash CHARSET < KERNEL-NLS-SOURCE.c > INCLUDE.h'
  exit 0
fi

echo
echo '/*'
echo ' * This file was automatically generated'
echo ' * from '"$ORIGIN"' kernel sources with script twin/scripts/helper-from-kernel.bash'
echo ' *'
echo ' * PLEASE DO NOT EDIT!'
echo ' */'
echo

if [ -f utf_16.bash.cache ]; then
  . utf_16.bash.cache
else
  if [ ! -r include/utf_16.h ]; then
    echo "File 'include/utf_16.h' not found! please run the script '1-make-utf_16_h.bash' first" 1>&2
    exit 1
  fi
  while read define name value; do
    if [ "$define" = '#define' -a "${name:0:9}" = 'T_UTF_16_' ]; then
      rev["$(( value ))"]="${name:9}"
    fi
  done < include/utf_16.h
  set | grep ^rev= > utf_16.bash.cache
fi

i=0
list=
nlist=

def() {
  # echo "def($1, $2)" >&2

  a="${1%,}"
  a="$[ a ]"
  hex="`printf %04X $i`"
  revname=`eval "echo -n \"\\$rev_$a\""`
  
  if [ "$a" = 0 -a "$i" != 0 -o "$revname" = "" ]; then
    echo "#define T_${CHARSET}_UNDEFINED_$hex	0x$hex"
  else
    eval "echo \"#define T_${CHARSET}_\$rev_$a	0x$hex\""
  fi
  list="$list $a"
  if [ "$a" != "$i" ]; then
    if [ "$a" = 0 -a "$i" -ge 127 -a "$i" -lt 160 ]; then
      nlist="$nlist :"
    else
      nlist="$nlist $a"
    fi
  else
    nlist="$nlist :"
  fi
  : $[ i = i + 1 ]
}

echo
echo "#ifndef _TUTF_${CHARSET}_H"
echo "#define _TUTF_${CHARSET}_H"
echo

while read line; do
  if [ "$line" = 'static wchar_t charset2uni[256] = {' -o \
       "$line" = 'static const wchar_t charset2uni[256] = {' -o \
       "$line" = 'static const unsigned short charset2uni[256] = {' ]; then
    while read a; do
      if [ "${a}" = '};' ]; then
        break 2;
      elif [ "${a:0:2}" != '/*' ]; then
        for ai in $a; do
          def "$ai"
	done
      fi
    done
  fi
done


echo
echo "/* list of all characters */"
echo -n "#define T_LIST_${CHARSET}(EL)"

i=0
for a in $list; do
  revname=`eval "echo -n \"\\$rev_$a\""`

  echo " \\"
  if [ "$a" = 0 -a "$i" != 0 -o "$revname" = "" ]; then
    hex="`printf %04X $i`"
    echo -n "    EL(_UNDEFINED_$hex)"
  else
    echo -n "    EL(_$revname)"
  fi
  : $[ i = i + 1 ]
done
echo

echo
echo '/* list of all characters NOT IDENTICALLY MAPPED to UTF-16 */'
echo -n "#define T_NLIST_${CHARSET}(EL)"

i=0
for a in $nlist; do
  if [ "$a" != ":" ]; then
    revname=`eval "echo -n \"\\$rev_$a\""`
  
    echo " \\"
    if [ "$a" = 0 -a "$i" != 0 -o "$revname" = "" ]; then
      hex="`printf %04X $i`"
      echo -n "    EL(_UNDEFINED_$hex)"
    else
      echo -n "    EL(_$revname)"
    fi
  fi
  : $[ i = i + 1 ]
done
echo

echo
echo "#endif /* _TUTF_${CHARSET}_H */"
