m4_define([TW_OUTPUT], [[
  echo '#ifndef _TWAUTOCONF_H'      >  include/twautoconf.h
  echo '#define _TWAUTOCONF_H'      >> include/twautoconf.h
  echo                              >> include/twautoconf.h
  $SED -e 's/define *\([A-Z]\)/define TW_\1/g' \
       -e 's/undef *\([A-Z]\)/undef TW_\1/g'   \
       -e 's/ifdef *\([A-Z]\)/ifdef TW_\1/g'   \
       -e 's/ifndef *\([A-Z]\)/ifndef TW_\1/g' \
                 < include/config.h >> include/twautoconf.h
  echo                              >> include/twautoconf.h
  echo '#endif /* _TWAUTOCONF_H */' >> include/twautoconf.h
  
  $SED -e 's/_TWAUTOCONF_H/_TW_TWAUTOCONF_H/g' < include/twautoconf.h > include/Tw/Twautoconf.h
]])
