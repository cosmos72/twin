
target="$1"

if [ "$target" = "" ]; then
  echo 'Usage: bash from-kernel.bash CHARSET < KERNEL-NLS-SOURCE.c > INCLUDE.h'
  exit 0
fi

echo
echo '/*'
echo ' * This file was automatically generated'
echo ' * from '"$2"' sources with "from-kernel.bash"'
echo ' * Do not edit!'
echo ' */'
echo

if [ -f utf_16.sh ]; then
  . utf_16.sh
else
  while read define name value; do
    if [ "$define" = '#define' -a "${name:0:9}" = 'T_UTF_16_' ]; then
      value="$[ value ]"
      if [ "$value" -gt 65535 ]; then
        break
      fi
      eval "${name:9}=$value"
      eval "rev_$value=${name:9}"
    fi
  done < utf_16.h
  set | grep "^rev_" > utf_16.sh
fi

i=0
list=
nlist=

function def() {
  a="${1%,}"
  a="$[ a ]"
  hex="`printf %04X $i`"
  if [ "$a" = 0 -a "$i" != 0 ]; then
    echo "#define T_${target}_UNDEFINED_$hex	0x$hex"
  else
    eval "echo \"#define T_${target}_\$rev_$a	0x$hex\""
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
echo "#ifndef _TUTF_${target}_H"
echo "#define _TUTF_${target}_H"
echo

while read line; do
  if [ "$line" = 'static wchar_t charset2uni[256] = {' -o \
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
echo -n "#define T_LIST_${target}(EL)"

i=0
for a in $list; do
  echo " \\"
  if [ "$a" = 0 -a "$i" != 0 ]; then
    hex="`printf %04X $i`"
    echo -n "    EL(_UNDEFINED_$hex)"
  else
    eval "echo -n \"    EL(_\$rev_$a)\""
  fi
  : $[ i = i + 1 ]
done
echo

echo
echo '/* list of all characters NOT IDENTICALLY MAPPED to UTF-16 */'
echo -n "#define T_NLIST_${target}(EL)"

i=0
for a in $nlist; do
  if [ "$a" != ":" ]; then
    echo " \\"
    if [ "$a" = 0 -a "$i" != 0 ]; then
      hex="`printf %04X $i`"
      echo -n "    EL(_UNDEFINED_$hex)"
    else
      eval "echo -n \"    EL(_\$rev_$a)\""
    fi
  fi
  : $[ i = i + 1 ]
done
echo

echo
echo "#endif /* _TUTF_${target}_H */"
