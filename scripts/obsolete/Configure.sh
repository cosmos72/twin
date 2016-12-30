#!/bin/sh
# Copyright (C) 2000 by Massimiliano Ghilardi
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
#

for ac_option
do
  case "$ac_option" in
  -*=*) ac_optarg=`echo "$ac_option" | sed 's/[-_a-zA-Z0-9]*=//'` ;;
  *) ac_optarg= ;;
  esac
  
  case "$ac_option" in

  -help | --help | --hel | --he )
    cat << EOF
Usage: scripts/Configure.sh [OPTIONS]

This script lets you manually configure twin with a command-line interface.

Options: [defaults in brackets after descriptions]
  --help                  print this message
  --show                  show current configuration
  --top-srcdir=DIR        set top source directory
  --top-builddir=DIR      set top build directory
  --disable-FEATURE       do not include FEATURE (same as --enable-FEATURE=no)
  --enable-FEATURE[=ARG]  include FEATURE [ARG=yes]
--enable options recognized:
  --enable--shlibs[=yes|no]              enable building shared libraries
  --enable--modules[=yes|no]             enable building shared modules
  --enable--unicode[=yes|no]             enable building unicode-aware code
  --enable--alloc[=yes|no]               enable custom malloc/free routines
  --enable--long-long[=yes|no]           enable C type 'long long' if supported
  --enable--asm[=yes|no]                 enable hand-optimized assembler if available
  --enable-socket[=yes|no]               enable remote socket connections (libTw)
  --enable-socket-gz[=yes|no]            enable gzip compression on sockets
  --enable-socket-pthreads[=yes|no]      enable thread safe libTw (requires pthread)
  --enable-socket-alien[=yes|no]         enable support for non-native architectures
EOF
    cat << EOF
  --enable-tt[=yes|no]                   enable Text Toolkit library (libTT)
  --enable-tt-hw-twin[=yes|no|mod]       enable libTT to use twin (libTw) as display driver
  --enable-tt-hw-twin-tunnel[=yes|mod|no]   enable libTT to use twin + server side libTT
  --enable-tt-hw-twin-detunnel[=yes|mod|no] enable libTT on twin server side
  --enable-tt-hw-gtk[=yes|no|mod]        enable libTT to use gtk as display driver
  --enable-tt-hw-x11[=yes|mod|no]        enable libTT to use X11 as display target
  --enable-tt-hw-xml[=yes|mod|no]        enable libTT to use xml files as display target
EOF
    cat << EOF
  --enable-wm[=yes|no|mod]               enable the builtin WM (absolutely needed!)
  --enable-wm-rc[=yes|no|mod]            enable ~/.twinrc configuration parser
  --enable-wm-rc-shmmap[=yes|no]         enable mmapped shared file for parser
  --enable-wm-rc-shrink[=yes|no]         enable shrinking memory when parser finished
  --enable-term[=yes|no|mod]             enable builtin terminal emulator
  --enable-term-devpts[=yes|no]          enable Unix98 /dev/pts/* pseudo-ttys
  --enable-printk[=yes|no]               enable logging messages in Messages window
EOF
    cat << EOF
  --enable-hw-tty[=yes|no|mod]           enable tty drivers
  --enable-hw-tty-linux[=yes|no]         enable the Linux console driver
  --enable-hw-tty-twterm[=yes|no]        enable the twterm terminal driver
  --enable-hw-tty-termcap[=yes|no]       enable the termcap terminal driver
  --enable-hw-x11[=yes|no|mod]           enable the X11 driver
  --enable-hw-twin[=yes|no|mod]          enable the twin native driver
  --enable-hw-display[=yes|no|mod]       enable the twdisplay client driver
  --enable-hw-ggi[=yes|no|mod]           enable the ggi driver
EOF
    cat << EOF
  --enable-ext[=yes|no]                  enable server extensions
  --enable-ext-tt[=yes|mod|no]           enable the libTT server extensions
EOF
    cat << EOF
  --enable-opt-shadows[=yes|no]          enable shadows
  --enable-opt-blink[=yes|no]            enable blink/high background
  --enable-opt-alwayscursor[=yes|no]     enable cursor always visible
  --enable-opt-hidemenu[=yes|no]         enable hidden menu
  --enable-opt-menuinfo[=yes|no]         enable menu information line
  --enable-opt-menurelax[=yes|no]        enable arrows to open/close submenus
  --enable-opt-edgescroll[=yes|no]       enable screen scrolling
EOF
    exit 0 ;;
    
  -disable-* | --disable-* )
    ;;
  
  -enable-* | --enable-* )
    ;;
    
  --show | --sho | --sh )
    ;;
  
  -top-srcdir=* | --top-srcdir=* )
    T="$ac_optarg"
    ;;
   
  -top-builddir=* | --top-builddir=* )
    B="$ac_optarg"
    ;;
    
  *)
    echo "scripts/Configure.sh: unknown option $ac_option" 1>&2
    echo "       try 'scripts/Configure.sh --help' for usage summary" 2>&1
    exit 1;;
  esac
done

if [ "$#" = 0 ]; then
  echo "try 'scripts/Configure.sh --help' for usage summary" 2>&1
  exit 1
fi


#
# read current configuration
#
if [ ! -f "$T"conf/conf.default ]; then
  echo "scripts/Configure.sh: fatal: \"$Tconf/conf.default\" does not exist!"
  echo "        maybe you forgot to use --top-srcdir=DIR and --top-builddir=DIR ?"
  exit 1
fi

. "$T"conf/conf.default
if [ -f "$B"conf/conf.current ]; then
  . "$B"conf/conf.current
  if [ -f "$B"conf/conf.auto -a "$B"conf/conf.auto -nt "$B"conf/conf.current ]; then
    . "$B"conf/conf.auto
  fi
elif [ -f "$B"conf/conf.auto ]; then
  . "$B"conf/conf.auto
fi
if [ ! -f "$B"conf/conf.auto ]; then
  echo "scripts/Configure.sh: warning: missing conf/conf.auto: 'configure' will overwrite your changes!" 1>&2
fi

ac_cf_list=
#
# set enable_* variables from current configuration
#
for ac_CONF in `cat "$T"conf/conf.list`; do
  ac_feature=enable_`echo $ac_CONF | cut -d_ -f2- | tr 'A-Z-' 'a-z_'`
  ac_cf_list="$ac_cf_list $ac_feature"
  eval "$ac_feature=$`echo $ac_CONF`"
done

#
# process command line options
#
for ac_option
do
  case "$ac_option" in
  -*=*) ac_optarg=`echo "$ac_option" | sed 's/[-_a-zA-Z0-9]*=//'` ;;
  *) ac_optarg= ;;
  esac
  
  case "$ac_option" in
  
  -disable-* | --disable-*)
    ac_feature=`echo $ac_option|sed -e 's/-*disable-//'`
    # Reject names that are not valid shell variable names.
    if test -n "`echo $ac_feature| sed 's/[-a-zA-Z0-9_]//g'`"; then
      echo "scripts/Configure.sh: error: $ac_feature: invalid feature name" 1>&2
      exit 1
    fi
    ac_feature=`echo $ac_feature| sed 's/-/_/g'`
    eval "enable_${ac_feature}=no" ;;

  --show | --sho | --sh )
    ac_opt_show=yes ;;
    
  -enable-* | --enable-*)
    ac_feature=`echo $ac_option | sed -e 's/-*enable-//' -e 's/=.*//'`
    # Reject names that are not valid shell variable names.
    if test -n "`echo $ac_feature | sed 's/[-_a-zA-Z0-9]//g'`"; then
      echo "scripts/Configure.sh: error: $ac_feature: invalid feature name" 1>&2
      exit 1
    fi
    ac_feature=`echo $ac_feature | sed 's/-/_/g'`
    case "$ac_option" in
      *=*) ;;
      *) ac_optarg=yes ;;
    esac
    eval "enable_${ac_feature}='$ac_optarg'" ;;

  -help | --help | --hel | --he)
    ;;

  -top-* | --top-* )
   ;;

  *)
    echo "scripts/Configure.sh: error: $ac_option: invalid option; use --help to show usage" 1>&2;
    exit 1 ;;

  esac
done


#
# prepare conf/conf.auto
#

exec 3>"$B"conf/conf.auto

echo "#" 1>&3
echo "# This configuration was created by 'scripts/Configure.sh [OPTIONS]'." 1>&3
echo "# Don't edit it, use one of 'make config', 'make menuconfig', " 1>&3
echo "# 'scripts/Configure.sh [OPTIONS]' or './configure [OPTIONS]' instead." 1>&3
echo "#" 1>&3

if test "$ac_opt_show"; then
  echo "#" 1>&2
  echo "# scripts/Configure.sh: current configuration is:" 1>&2
  echo "#" 1>&2
fi

for ac_feature in $ac_cf_list; do
  eval "ac_cf_val=$`echo $ac_feature`"
  case "$ac_cf_val" in
    y | yes ) ac_cf_val=y ; ac_cf_val_long=yes ;;
    n | no  ) ac_cf_val=n ; ac_cf_val_long=no  ;;
    m | mod ) ac_cf_val=m ; ac_cf_val_long=mod ;;
    * )
      # unset / undetectable. set to empty.
      ac_cf_val= ; ac_cf_val_long= ;;
  esac
  if test "$ac_cf_val"; then
    ac_CONF=CONF_`echo $ac_feature | cut -d_ -f2- | sed 'y%abcdefghijklmnopqrstuvwxyz%ABCDEFGHIJKLMNOPQRSTUVWXYZ%'`
    #echo $ac_feature, $ac_cf_val_long
    #echo $ac_CONF, $ac_cf_val
    echo "$ac_CONF=$ac_cf_val" 1>&3
    if test "$ac_opt_show"; then
      echo "  --`echo $ac_feature | sed 's/_/-/g'`=$ac_cf_val_long"
    fi
  fi
done

if test -z "$ac_opt_show"; then
  echo "#" 1>&2
  echo "# scripts/Configure.sh: conf/conf.auto successfully updated." 1>&2
  echo "#" 1>&2
fi

