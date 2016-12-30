#!/bin/bash

CHARSET="`echo \"$1\" | tr 'a-z' 'A-Z'`"
if [ "$CHARSET" = "" ]; then
  echo 'Usage: helper-from-unicode.org.bash CHARSET-NAME < UNICODE.ORG-CHARSET-DEF.TXT > INCLUDE.h'
  exit 0
fi

INSERT_MISSING=0
if [ "${2:0:17}" = "--insert-missing=" ]; then
  INSERT_MISSING="${2:17}"
fi

echo
echo '/*'
echo ' * This file was automatically generated from unicode'
echo ' * charset definition files http://unicode.org/Public/MAPPINGS/'
echo ' * using the script twin/scripts/helper-from-unicode.org.bash'
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
      names["$(( value ))"]="${name:9}"
    fi
  done < include/utf_16.h
  set | grep ^names= > utf_16.bash.cache
fi

parse_hex() {
  local token="$1"
  local value=
  if [ "${token:0:2}" = "0x" ]; then
    value="$token"
  else
    # skip <LR>+ and <RL>+ and any other prefix...
    value="`expr match \"$token\" '.*\(0x[0-9a-fA-F]*\)'`"
    if [ "$value" = "" ]; then
      # check for hexadecimal value without 0x prefix
      value="`expr match \"$token\" '\([0-9a-fA-F]*\)'`"
      if [ "$value" = "$token" ]; then
        value="0x$token"
      fi
    fi
  fi
  result="$value"
}

list=
nlist=
ch=
chinv=
missing=0

def() {
  if [ "$INSERT_MISSING" != 0 ]; then
    while [ "$missing" -lt "$INSERT_MISSING" -a "$missing" -lt "$1" ]; do
      ch["$missing"]="$missing"
      chinv["$missing"]="$missing"
      : $(( missing = missing+1 ))
    done
  fi
  # do not create mappings for 0xFFFD = REPLACEMENT_CHARACTER, because unicode.org uses them to mean "character not defined"
  if [ "$2" != 65533 ]; then
    ch["$1"]="$2"
    if [ "$2" != "" -a "$2" != "." ]; then
      if [ "${chinv[$2]}" = "" ]; then
        # used to catch duplicates later
        chinv["$2"]="$1"
      fi
    fi
  fi
}

echo
echo "#ifndef _TUTF_${CHARSET}_H"
echo "#define _TUTF_${CHARSET}_H"
echo

while read hex_index hex_value comments; do
  if [ "$hex_index" = "" -o "${hex_index:0:1}" = "#" ]; then
    # skip empty lines and comments
    continue
  fi
  
  parse_hex "$hex_index"
  # echo "parse_hex($hex_index) -> $result # $hex_value $comments"
  hex_index="$result"
  
  if [ "$hex_index" = "" ]; then
    continue
  fi
  index="$(( hex_index ))"
  if [ "$index" -ge 256 ]; then
    # this is a byte-to-utf16 mapping... cannot define mappings from (> 0xFF)
    continue
  fi
  
  parse_hex "$hex_value"
  hex_value="$result"
  if [ "$hex_value" != "" ]; then
    value="$(( hex_value ))"
    if [ "$value" -ge 65536 ]; then
      # this is a byte-to-utf16 mapping... cannot define mappings to (> 0xFFFF)
      continue
    fi
  else
    value="."
  fi
  def "$index" "$value"
done

to_name() {
  local value="${ch[$1]}"
  result=
  if [ "$value" != "" -a "$value" != "." ]; then
    # skip duplicates
    if [ "${chinv[$value]}" = "$1" ]; then
      result="${names[$value]}"
    fi
  fi
  if [ "$result" = "" -o "$result" = "." ]; then
    result="UNDEFINED_${2:2}" # "$2" is hex_index, skip its "0x" prefix
  fi
}

index=0
while [ "$index" -le 255 ]; do
  hex_index="`printf 0x%04X \"$index\"`"
  
  to_name "$index" "$hex_index"
  name="$result"
  if [ "$name" != "" ]; then
    echo "#define T_${CHARSET}_$name	$hex_index"
  fi
  : $(( index = index + 1 ))
done
  
echo
echo "/* list of all characters */"
echo -n "#define T_LIST_${CHARSET}(EL)"

index=0
while [ "$index" -le 255 ]; do
  hex_index="`printf 0x%04X \"$index\"`"
  value="${ch[index]}"
  
  if [ "$value" != "" ]; then
    to_name "$index" "$hex_index"
    name="$result"
    if [ "$name" != "" ]; then
      echo " \\"
      echo -n "    EL(_$name)"
    fi
  fi
  : $(( index = index + 1 ))
done
echo

echo
echo '/* list of all characters NOT IDENTICALLY MAPPED to UTF-16 */'
echo -n "#define T_NLIST_${CHARSET}(EL)"

index=0
while [ "$index" -le 255 ]; do
  hex_index="`printf 0x%04X \"$index\"`"
  value="${ch[index]}"
  
  if [ "$value" != "" -a "$value" != "$index" ]; then
    to_name "$index" "$hex_index"
    name="$result"
    if [ "$name" != "" ]; then
      echo " \\"
      echo -n "    EL(_$name)"
    fi
  fi
  : $(( index = index + 1 ))
done
echo

echo
echo
echo "#endif /* _TUTF_${CHARSET}_H */"
