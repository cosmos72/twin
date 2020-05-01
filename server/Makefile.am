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
if LIBWM_la
  pkglib_LTLIBRARIES += libwm.la
endif

AM_CPPFLAGS           = -I$(top_srcdir)/include $(LTDLINCL) -DPKG_LIBDIR="\"$(pkglibdir)\""
twin_CPPFLAGS         = -I$(top_srcdir)/include $(LTDLINCL) -DBINDIR="\"$(bindir)\""

twdisplay_SOURCES     = alloc.cpp display.cpp dl_helper.cpp missing.cpp hw.cpp
twin_SOURCES          = wrapper.c
twin_server_SOURCES   = alloc.cpp builtin.cpp data.cpp dl.cpp dl_helper.cpp draw.cpp extensions/ext_query.cpp extreg.cpp \
                        hw.cpp hw_multi.cpp main.cpp methods.cpp missing.cpp \
                        obj/id.cpp obj/obj.cpp obj/widget.cpp obj/all.cpp \
                        printk.cpp privilege.cpp remote.cpp resize.cpp scroller.cpp util.cpp

librcparse_la_SOURCES = rcparse_tab.cpp rcparse_lex.cpp
libterm_la_SOURCES    = pty.cpp tterm.cpp tty.cpp
libsocket_la_SOURCES  = md5.cpp socket.cpp
libwm_la_SOURCES      = rcopt.cpp rcrun.cpp shm.cpp wm.cpp

twdisplay_LDFLAGS     = -export-dynamic $(LDFLAGS_BIN_EXPORT_DYNAMIC)
twin_server_LDFLAGS   = -export-dynamic $(LDFLAGS_BIN_EXPORT_DYNAMIC)

librcparse_la_LDFLAGS = -export-dynamic -export-symbols-regex '^(Init|Quit)Module$$' -release $(PACKAGE_VERSION)
libsocket_la_LDFLAGS  = -export-dynamic -export-symbols-regex '^(Init|Quit)Module$$' -release $(PACKAGE_VERSION)
libterm_la_LDFLAGS    = -export-dynamic -export-symbols-regex '^(Init|Quit)Module$$' -release $(PACKAGE_VERSION)
# libwm exports some symbols needed by librcparse, do not constrain them with -export-symbols-regex
libwm_la_LDFLAGS      = -export-dynamic                                              -release $(PACKAGE_VERSION)

twdisplay_LDADD       = $(LIBTW) $(LIBTUTF) $(LIBDL)
twin_server_LDADD     =          $(LIBTUTF) $(LIBDL)

libsocket_la_LIBADD   = $(LIBSOCK) $(LIBZ)
