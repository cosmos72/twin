m4_define([TW_OUTPUT], [[
  mkdir include 2>/dev/null
  $SED -e 's/define \([A-Z]\)/define TW_\1/g' -e 's/undef \([A-Z]\)/undef TW_\1/g' < config.h > include/tw_autoconf.h
]])
