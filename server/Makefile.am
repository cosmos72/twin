SUBDIRS = hw

bin_PROGRAMS       = twdisplay twin twin_server
bin_SCRIPTS        = twstart
pkglib_LTLIBRARIES =

if LIBRCPARSE_la
  pkglib_LTLIBRARIES += librcparse.la
endif
if LIBSOCKET_la
  pkglib_LTLIBRARIES += libsocket.la
endif
if LIBTERM_la
  pkglib_LTLIBRARIES += libterm.la
endif

AM_CPPFLAGS           = -I$(top_srcdir)/include $(LTDLINCL) -DPLUGINDIR="\"$(pkglibdir)\"" -DCONFDIR="\"$(sysconfdir)/twin\""
twin_CPPFLAGS         = -I$(top_srcdir)/include $(LTDLINCL) -DBINDIR="\"$(bindir)\""

twdisplay_SOURCES     = alloc.cpp display.cpp dl_helper.cpp log.cpp missing.cpp hw.cpp
twin_SOURCES          = wrapper.c
twin_server_SOURCES   = alloc.cpp builtin.cpp data.cpp dl.cpp dl_helper.cpp draw.cpp \
                        extreg.cpp hw.cpp hw_multi.cpp log.cpp main.cpp methods.cpp missing.cpp \
                        obj/all.cpp obj/display_hw.cpp obj/gadget.cpp obj/group.cpp \
                        obj/id.cpp obj/menu.cpp obj/menuitem.cpp obj/module.cpp obj/msg.cpp obj/msgport.cpp \
                        obj/mutex.cpp obj/obj.cpp obj/row.cpp obj/screen.cpp obj/widget.cpp obj/window.cpp \
                        printk.cpp privilege.cpp rcopt.cpp rcrun.cpp remote.cpp resize.cpp shm.cpp scroller.cpp \
                        util.cpp wm.cpp

librcparse_la_SOURCES = rcparse_tab.cpp rcparse_lex.cpp
libterm_la_SOURCES    = pty.cpp tterm.cpp tty.cpp
libsocket_la_SOURCES  = md5.cpp socket.cpp

twdisplay_LDFLAGS     = -export-dynamic $(LDFLAGS_BIN_EXPORT_DYNAMIC)
twin_server_LDFLAGS   = -export-dynamic $(LDFLAGS_BIN_EXPORT_DYNAMIC)

librcparse_la_LDFLAGS = -export-dynamic -export-symbols-regex '^(Init|Quit)Module$$' -release $(PACKAGE_VERSION)
libsocket_la_LDFLAGS  = -export-dynamic -export-symbols-regex '^(Init|Quit)Module$$' -release $(PACKAGE_VERSION)
libterm_la_LDFLAGS    = -export-dynamic -export-symbols-regex '^(Init|Quit)Module$$' -release $(PACKAGE_VERSION)

twdisplay_LDADD       = $(LIBTW) $(LIBTUTF) $(LIBDL) $(LIBTSTL)
twin_server_LDADD     =          $(LIBTUTF) $(LIBDL) $(LIBTSTL)

librcparse_la_LIBADD  = $(LIBTUTF) $(LIBTSTL)
libsocket_la_LIBADD   = $(LIBTUTF) $(LIBTSTL) $(LIBSOCK) $(LIBZ)
libterm_la_LIBADD     = $(LIBTUTF) $(LIBTSTL)
