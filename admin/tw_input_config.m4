m4_define([TW_OPTIONS], [

dnl custom malloc/free
dnl AC_ARG_ENABLE(-alloc,dnl
dnl [  --enable--alloc[=yes|no]               enable custom malloc/free routines ],,
dnl enable__alloc=yes
dnl )dnl
enable__alloc=no

dnl hand-optimized assembler (if available for your platform)
AC_ARG_ENABLE(-asm,dnl
[  --enable--asm[=yes|no]                 enable hand-optimized assembler if supported ],,
enable__asm=yes
)dnl

dnl dlopen() and friends
AC_ARG_ENABLE(-dlopen,dnl
[  --enable--dlopen[=yes|no]              use dlopen() to load modules instead of libtool lt_dlopen()],,
enable__dlopen=yes
)dnl

dnl socket server
AC_ARG_ENABLE(socket,dnl
[  --enable-socket[=yes|no]               enable remote socket connections (libTw) ],,
enable_socket=yes
)dnl

dnl socket compression
AC_ARG_ENABLE(socket_gz,dnl
[  --enable-socket-gz[=yes|no]            enable gzip compression on sockets ],,
enable_socket_gz=yes
)dnl

dnl thread safe libTw
AC_ARG_ENABLE(socket_pthreads,dnl
[  --enable-socket-pthreads[=yes|no]      enable thread safe libraries (requires pthread) ],,
enable_socket_pthreads=yes
)dnl

dnl support alien architectures
AC_ARG_ENABLE(socket_alien,dnl
[  --enable-socket-alien[=yes|no]         enable support for non-native architectures ],,
enable_socket_alien=yes
)dnl

dnl builtin window manager
AC_ARG_ENABLE(wm,dnl
[  --enable-wm[=yes|no]                   enable the builtin WM (absolutely needed!) ],,
enable_wm=yes
)dnl

dnl ~/.twinrc parser
AC_ARG_ENABLE(wm_rc,dnl
[  --enable-wm-rc[=yes|no]                enable ~/.twinrc configuration parser ],,
enable_wm_rc=yes
)dnl

dnl use mmapped shared file for parser
AC_ARG_ENABLE(wm_rc_shmmap,dnl
[  --enable-wm-rc-shmmap[=yes|no]         enable mmapped shared file for parser ],,
enable_wm_rc_shmmap=yes
)dnl

dnl shrink memory when parser finished
AC_ARG_ENABLE(wm_rc_shrink,dnl
[  --enable-wm-rc-shrink[=yes|no]         enable shrinking memory when parser finished ],,
enable_wm_rc_shrink=yes
)dnl

dnl terminal emulator
AC_ARG_ENABLE(term,dnl
[  --enable-term[=yes|no]                 enable builtin terminal emulator ],,
enable_term=yes
)dnl

dnl use /dev/pts/* for terminal emulator
AC_ARG_ENABLE(term_devpts,dnl
[  --enable-term-devpts[=yes|no]          enable Unix98 /dev/pts/* pseudo-ttys ],,
enable_term_devpts=yes
)dnl

dnl store server messages in "Messages" window
AC_ARG_ENABLE(printk,dnl
[  --enable-printk[=yes|no]               enable logging messages in Messages window ],,
enable_printk=yes
)dnl

dnl tty drivers
AC_ARG_ENABLE(hw-tty,dnl
[  --enable-hw-tty[=yes|no]               enable tty drivers ],,
enable_hw_tty=yes
)dnl

dnl the Linux console driver
AC_ARG_ENABLE(hw-tty-linux,dnl
[  --enable-hw-tty-linux[=yes|no]         enable the Linux console driver (gpm mouse, /dev/vcsa)],,
enable_hw_tty_linux=yes
)dnl

dnl the twterm terminal driver
AC_ARG_ENABLE(hw-tty-twterm,dnl
[  --enable-hw-tty-twterm[=yes|no]        enable the twterm terminal driver ],,
enable_hw_tty_twterm=yes
)dnl

dnl the termcap terminal driver
AC_ARG_ENABLE(hw-tty-termcap,dnl
[  --enable-hw-tty-termcap[=yes|no]       enable the termcap/ncurses terminal driver ],,
enable_hw_tty_termcap=yes
)dnl

dnl the X11 driver
AC_ARG_ENABLE(hw-x11,dnl
[  --enable-hw-x11[=yes|no]               enable the X11 driver ],,
enable_hw_x11=yes
)dnl

dnl the gfx driver
AC_ARG_ENABLE(hw-gfx,dnl
[  --enable-hw-gfx[=yes|no]               enable the gfx (enhanced X11) driver ],,
enable_hw_gfx=yes
)dnl

dnl the twin driver
AC_ARG_ENABLE(hw-twin,dnl
[  --enable-hw-twin[=yes|no]              enable the twin nested driver ],,
enable_hw_twin=yes
)dnl

dnl the twdisplay driver
AC_ARG_ENABLE(hw-display,dnl
[  --enable-hw-display[=yes|no]           enable the twdisplay client driver ],,
enable_hw_display=yes
)dnl

dnl the ggi driver
AC_ARG_ENABLE(hw-ggi,dnl
[  --enable-hw-ggi[=yes|no]               enable the ggi driver (UNFINISHED) ],,
enable_hw_ggi=yes
)dnl

dnl server extensions
AC_ARG_ENABLE(ext,dnl
[  --enable-ext[=yes|no]                  enable server extensions ],,
enable_ext=yes
)dnl

enable_opt_shadows=yes
enable_opt_blink=no
enable_opt_cursor_always=no
enable_opt_menu_hide=no
enable_opt_menu_info=no
enable_opt_menu_relax=yes
enable_opt_screen_scroll=no

])
