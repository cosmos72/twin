SUBDIRS = HW

bin_PROGRAMS       = twdisplay twin twin_real
noinst_LTLIBRARIES = 

if LIBRCPARSE_la
  noinst_LTLIBRARIES += librcparse.la
endif
if LIBSOCKET_la
  noinst_LTLIBRARIES += libsocket.la
endif
if LIBTERM_la
  noinst_LTLIBRARIES += libterm.la
endif
if LIBWM_la
  noinst_LTLIBRARIES += libwm.la
endif

AM_CPPFLAGS           = -I$(top_srcdir)/include $(LTDLINCL)
data_CPPFLAGS         = -I$(top_srcdir)/include $(LTDLINCL) -DPACKAGE_LIBDIR=$(pkglibdir)
display_CPPFLAGS      = -I$(top_srcdir)/include $(LTDLINCL) -DPACKAGE_LIBDIR=$(pkglibdir)

twdisplay_SOURCES     = alloc.c display.c missing.c hw.c
twin_SOURCES          = wrapper.c
twin_real_SOURCES     = alloc.c builtin.c data.c dl.c draw.c extensions/ext_query.c extreg.c hw.c hw_multi.c main.c methods.c missing.c printk.c remote.c resize.c scroller.c util.c 

librcparse_la_SOURCES = rcparse_tab.c rcparse_lex.c
libterm_la_SOURCES    = pty.c tterm.c tty.c
libsocket_la_SOURCES  = md5.c socket.c
libwm_la_SOURCES      = rcopt.c rcrun.c shm.c wm.c

twdisplay_LDADD       = $(LIBTW) $(LIBTUTF) $(LIBLTDL) $(LIBDL)
twin_real_LDADD       =          $(LIBTUTF) $(LIBLTDL) $(LIBDL)

librcparse_la_LDFLAGS = -release $(PACKAGE_VERSION) -rpath $(pkglibdir)/modules
libsocket_la_LDFLAGS  = -release $(PACKAGE_VERSION) -rpath $(pkglibdir)/modules
libterm_la_LDFLAGS    = -release $(PACKAGE_VERSION) -rpath $(pkglibdir)/modules
libwm_la_LDFLAGS      = -release $(PACKAGE_VERSION) -rpath $(pkglibdir)/modules

libsocket_la_LIBADD   = $(LIBSOCK) $(LIBZ)
