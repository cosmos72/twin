noinst_LTLIBRARIES = 

if LIBHW_GFX_la
  noinst_LTLIBRARIES += libhw_gfx.la
endif
if LIBHW_GGI_la
  noinst_LTLIBRARIES += libhw_ggi.la
endif
if LIBHW_TTY_la
  noinst_LTLIBRARIES += libhw_tty.la
endif
if LIBHW_TWIN_la
  noinst_LTLIBRARIES += libhw_twin.la
endif
if LIBHW_X11_la
  noinst_LTLIBRARIES += libhw_X11.la
endif

AM_CPPFLAGS           = -I$(top_srcdir)/include -I$(top_srcdir)/server
libhw_gfx_la_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/server $(X11_CPPFLAGS)
libhw_X11_la_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/server $(X11_CPPFLAGS)

libhw_gfx_la_SOURCES  = hw_gfx.c
libhw_ggi_la_SOURCES  = hw_ggi.c
libhw_tty_la_SOURCES  = hw_tty.c
libhw_twin_la_SOURCES = hw_twin.c
libhw_X11_la_SOURCES  = hw_X11.c

libhw_gfx_la_LDFLAGS  = -release $(PACKAGE_VERSION) -rpath $(pkglibdir)/modules $(X11_LDFLAGS)
libhw_ggi_la_LDFLAGS  = -release $(PACKAGE_VERSION) -rpath $(pkglibdir)/modules
libhw_tty_la_LDFLAGS  = -release $(PACKAGE_VERSION) -rpath $(pkglibdir)/modules
libhw_twin_la_LDFLAGS = -release $(PACKAGE_VERSION) -rpath $(pkglibdir)/modules
libhw_X11_la_LDFLAGS  = -release $(PACKAGE_VERSION) -rpath $(pkglibdir)/modules $(X11_LDFLAGS)

libhw_gfx_la_LIBADD   = $(LIBXPM) $(LIBX11)
libhw_ggi_la_LIBADD   = $(LIBGGI)
libhw_tty_la_LIBADD   = $(LIBGPM)
libhw_twin_la_LIBADD  = $(LIBTW) $(LIBTUTF) $(LIBZ)
libhw_X11_la_LIBADD   = $(LIBX11)
