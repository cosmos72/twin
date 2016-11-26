m4_define([TW_OUTPUT], [[
  $SED -e 's/define \([A-Z]\)/define TW_\1/g' -e 's/undef \([A-Z]\)/undef TW_\1/g' < include/autoconf.h > include/tw_autoconf.h
]])
