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

twdisplay_SOURCES     = alloc.c display.c dl_helper.c missing.c hw.c
twin_SOURCES          = wrapper.c
twin_server_SOURCES   = alloc.c builtin.c data.c dl.c dl_helper.c draw.c extensions/ext_query.c extreg.c hw.c hw_multi.c main.c methods.c missing.c printk.c remote.c resize.c scroller.c util.c 

librcparse_la_SOURCES = rcparse_tab.c rcparse_lex.c
libterm_la_SOURCES    = pty.c tterm.c tty.c
libsocket_la_SOURCES  = md5.c socket.c
libwm_la_SOURCES      = rcopt.c rcrun.c shm.c wm.c

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
