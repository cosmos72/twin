#!/bin/sh
# Copyright (C) 2000 by Massimiliano Ghilardi
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
#

while [ "$1" ]; do
  eval "$1"
  shift
done

echo_shlibs="<none>"
echo_op=
echo_cl=
if test "$enable__shlibs" = y; then
  echo_op='('
  echo_cl=')'
  echo_shlibs="${OS}-${SYS_SHLIBS}"
fi

echo_cc_feat=
if test "$enable__asm" = y; then
  echo_cc_feat="$echo_cc_feat $enable__asm_style"
fi
if test "$enable__long_long" = y; then
  echo_cc_feat="$echo_cc_feat long-long"
fi
if test "$echo_cc_feat" = ""; then
  echo_cc_feat="<none>"
else
  echo_cc_feat="`echo $echo_cc_feat`"
fi


echo_feat=
for i in socket wm wm_rc term term_devpts; do
  eval "echo_val=\${enable_$i}"
  if test "$echo_val" = y; then
    echo_feat="$echo_feat $i"
  elif test "$echo_val" = m; then
    echo_feat="$echo_feat $echo_op""$i""$echo_cl"
  fi
done
if test "$echo_feat" = ""; then
  echo_feat="<none>"
else
  echo_feat="`echo $echo_feat`"
fi

echo_socket=
for i in gz pthreads alien; do
  eval "echo_val=\${enable_socket_$i}"
  if test "$echo_val" = y; then
    echo_socket="$echo_socket $i"
  elif test "$echo_val" = m; then
    echo_socket="$echo_socket $echo_op""$i""$echo_cl"
  fi
done
if test "$echo_socket" = ""; then
  echo_socket="<none>"
else
  echo_socket="`echo $echo_socket`"
fi

echo_hw=
for i in gfx x11 twin display tty ggi; do
  eval "echo_val=\${enable_hw_$i}"
  if test "$echo_val" = y; then
    echo_hw="$echo_hw $i"
  elif test "$echo_val" = m; then
    echo_hw="$echo_hw $echo_op""$i""$echo_cl"
  fi
done
if test "$echo_hw" = ""; then
  echo_hw="<none>"
else
  echo_hw="`echo $echo_hw`"
fi

echo_hw_tty=
if test "$enable_hw_tty" != n; then
  for i in linux lrawkbd twterm termcap; do
    eval "echo_val=\${enable_hw_tty_$i}"
    if test "$echo_val" = y; then
      echo_hw_tty="$echo_hw_tty $i"
    elif test "$echo_val" = m; then
      echo_hw_tty="$echo_hw_tty $echo_op""$i""$echo_cl"
    fi
  done
fi
if test "$echo_hw_tty" = ""; then
  echo_hw_tty="<none>"
else
  echo_hw_tty="`echo $echo_hw_tty`"
fi

echo_libs=
if test "$enable__unicode" != n; then
  echo_libs="$echo_libs $echo_op""libTutf""$echo_cl"
fi
if test "$enable_socket" != n; then
  echo_libs="$echo_libs $echo_op""libTw""$echo_cl"
fi
if test "$enable_TT" != n; then
  echo_libs="$echo_libs $echo_op""libTT""$echo_cl"
fi
if test "$echo_libs" = ""; then
  echo_libs="<none>"
else
  echo_libs="`echo $echo_libs`"
fi

echo_tt_hw=
for i in twin twin_tunnel twin_detunnel gtk x11 xml; do
  eval "echo_val=\${enable_tt_hw_$i}"
  if test "$echo_val" = y; then
    echo_tt_hw="$echo_tt_hw $i"
  elif test "$echo_val" = m; then
    echo_tt_hw="$echo_tt_hw $echo_op""$i""$echo_cl"
  fi
done
if test "$echo_tt_hw" = ""; then
  echo_tt_hw="<none>"
else
  echo_tt_hw="`echo $echo_tt_hw`"
fi

echo "
Configuration:

  Items in parenthesis (...) will be built as shared libraries/modules

  Compiler:                   ${CC}
  Compiler flags:             ${CC_FLAGS_OPT}
  Compiler features used:     ${echo_cc_feat}
  Shared libraries support:   ${echo_shlibs}
  Server features:            ${echo_feat}
  Server socket driver flags: ${echo_socket}
  Server HW drivers:          ${echo_hw}
  Server HW tty driver flags: ${echo_hw_tty}
  Libraries:                  ${echo_libs}
  TT library HW targets:      ${echo_tt_hw}
  Install path:               ${prefix}/bin, ${prefix}/lib/twin, ${prefix}/share/twin

to show/change other configuration settings, you can run one of:
'./configure <options>', 'scripts/Configure.sh <options>',
'make menuconfig' or 'make config'"

