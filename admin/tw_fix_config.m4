m4_define([TW_FIX_CONFIG], [

if test "$ac_cv_func_mmap_fixed_mapped" = no; then
  enable_rcparse_shmmap=no
fi

if test "$ac_cv_header_dlfcn_h" = no -o "$ac_cv_search_dlopen" = no; then
  enable__dlopen=no
fi

if test "$ac_cv_search_socket" = no; then
  AC_MSG_WARN(missing socket(): disabling socket connections (libtw)!)
  enable_socket=no
fi

if test "$ac_cv_search_connect" = no; then
  AC_MSG_WARN(missing connect(): disabling socket connections (libtw)!)
  enable_socket=no
fi

if test "$ac_cv_search_gethostbyname" = no; then
  AC_MSG_WARN(missing gethostbyname(): libtw will only accept numerical IP addresses!)
fi

if test "$ac_cv_search_pthread_create" = no -o "$ac_cv_header_pthread_h" = no; then
  enable_socket_pthreads=no
fi

if test "$ac_cv_search_deflate" = no -o "$ac_cv_header_zlib_h" = no; then
  enable_socket_gz=no
fi

if test "$ac_cv_func_grantpt" = no -o "$ac_cv_func_unlockpt" = no -o "$ac_cv_func_ptsname" = no; then
  enable_term_devpts=no
fi

if test "$ac_cv_search_Gpm_Open" = no -o "$ac_cv_header_gpm_h" = no; then
  enable_hw_tty_linux=no
fi

if test "$ac_cv_header_linux_kd_h" = no -o "$ac_cv_header_linux_keyboard_h" = no -o "$ac_cv_header_linux_vt_h" = no; then
  enable_hw_tty_lrawkbd=no
fi

if test "$ac_cv_search_tgetent" = no; then
  enable_hw_tty_termcap=no
fi

if test "$have_x" != yes; then
  enable_hw_x11=no
fi

if test "$have_x" != yes -o "$ac_cv_header_X11_Xft_Xft_h" != yes -o "$ac_cv_header_fontconfig_fontconfig_h" != yes -o "$ac_cv_search_XftFontOpen" = no; then
  enable_hw_xft=no
fi

])
