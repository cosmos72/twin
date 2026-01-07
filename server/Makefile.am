SUBDIRS = hw

bin_PROGRAMS       = twdisplay twin twin_server twin_server_static
bin_SCRIPTS        = twstart
pkglib_LTLIBRARIES =
noinst_LIBRARIES   = libtwin_static.a

AM_CPPFLAGS           = -I$(top_srcdir)/include $(LTDLINCL) -DPLUGINDIR="\"$(pkglibdir)\"" -DCONFDIR="\"$(sysconfdir)/twin\""
twin_CPPFLAGS         = -I$(top_srcdir)/include $(LTDLINCL) -DBINDIR="\"$(bindir)\""

twdisplay_SOURCES     = alloc.cpp display.cpp dl_helper.cpp hw.cpp log.cpp missing.cpp palette.cpp
twin_SOURCES          = wrapper.c
twin_server_common_SOURCES = alloc.cpp builtin.cpp data.cpp draw.cpp extreg.cpp \
                        hw.cpp hw_multi.cpp log.cpp main.cpp methods.cpp missing.cpp palette.cpp \
                        obj/all.cpp obj/display.cpp obj/gadget.cpp obj/group.cpp \
                        obj/id.cpp obj/menu.cpp obj/menuitem.cpp obj/module.cpp obj/msg.cpp obj/msgport.cpp \
                        obj/mutex.cpp obj/obj.cpp obj/row.cpp obj/screen.cpp obj/widget.cpp obj/window.cpp \
                        printk.cpp privilege.cpp pty.cpp rcopt.cpp rcrun.cpp remote.cpp resize.cpp shm.cpp scroller.cpp \
                        tterm.cpp tty.cpp util.cpp wm.cpp

librcparse_la_SOURCES = rcparse_tab.cpp rcparse_lex.cpp
libsocket_la_SOURCES  = md5.cpp socket.cpp

libtwin_static_a_SOURCES   = dl_static.cpp

twin_server_SOURCES        = $(twin_server_common_SOURCES) dl_dynamic.cpp dl_helper.cpp
twin_server_static_SOURCES = $(twin_server_common_SOURCES)

twdisplay_LDFLAGS          = -export-dynamic $(LDFLAGS_BIN_EXPORT_DYNAMIC)
twin_server_LDFLAGS        = -export-dynamic $(LDFLAGS_BIN_EXPORT_DYNAMIC)

# if you want to link against static system libraries (provided that they are installed) also add -Wl,-static
twin_server_static_LDFLAGS = -static

librcparse_la_LDFLAGS      = -export-dynamic -export-symbols-regex '^(Init|Quit)Module$$' -release $(PACKAGE_VERSION)
libsocket_la_LDFLAGS       = -export-dynamic -export-symbols-regex '^(Init|Quit)Module$$' -release $(PACKAGE_VERSION)

twdisplay_LDADD            = $(LIBTW)                        $(LIBTUTF) $(LIBDL) $(LIBTSTL)
twin_server_LDADD          =                                 $(LIBTUTF) $(LIBDL) $(LIBTSTL)
twin_server_static_LDADD   = libtwin_static.a hw/libhw_all.a $(LIBTUTF) $(LIBDL) $(LIBTSTL)

librcparse_la_LIBADD       = $(LIBTUTF) $(LIBTSTL)
libsocket_la_LIBADD        = $(LIBTUTF) $(LIBTSTL) $(LIBSOCK) $(LIBZ)

libtwin_static_a_CPPFLAGS  =

if LIBRCPARSE_la
  pkglib_LTLIBRARIES         += librcparse.la
  libtwin_static_a_CPPFLAGS  += -DTWIN_RCPARSE_STATIC
  libtwin_static_a_SOURCES   += rcparse_tab.cpp rcparse_lex.cpp
endif
if LIBSOCKET_la
  pkglib_LTLIBRARIES         += libsocket.la
  libtwin_static_a_CPPFLAGS  += -DTWIN_SOCKET_STATIC
  libtwin_static_a_SOURCES   += md5.cpp socket.cpp
  twin_server_static_LDADD   += $(LIBSOCK) $(LIBZ)
endif
if LIBHW_TTY_la
  twin_server_static_LDADD   += $(LIBTERMCAP) $(LIBGPM)
endif
if LIBHW_TWIN_la
  twin_server_static_LDADD   += $(LIBTW) $(LIBZ)
endif
if LIBHW_X11_la
  twin_server_static_LDADD   += $(LIBX11)
endif
if LIBHW_XFT_la
  twin_server_static_LDADD   += $(LIBXFT) $(XFT_LIBS) $(LIBX11)
endif
