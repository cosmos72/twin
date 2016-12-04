pkglib_LTLIBRARIES = 

if LIBHW_DISPLAY_la
  pkglib_LTLIBRARIES += libhw_display.la
endif
if LIBHW_X11_la
  pkglib_LTLIBRARIES += libhw_X11.la
endif
if LIBHW_GFX_la
  pkglib_LTLIBRARIES += libhw_gfx.la
endif
if LIBHW_GGI_la
  pkglib_LTLIBRARIES += libhw_ggi.la
endif
if LIBHW_TTY_la
  pkglib_LTLIBRARIES += libhw_tty.la
endif
if LIBHW_TWIN_la
  pkglib_LTLIBRARIES += libhw_twin.la
endif

AM_CPPFLAGS           = -I$(top_srcdir)/include -I$(top_srcdir)/server
libhw_X11_la_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/server $(X11_CPPFLAGS)
libhw_gfx_la_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/server $(X11_CPPFLAGS) -DPKG_DATADIR="\"$(pkgdatadir)\""

libhw_display_la_SOURCES = hw_display.c
libhw_X11_la_SOURCES  = hw_X11.c
libhw_gfx_la_SOURCES  = hw_gfx.c
libhw_ggi_la_SOURCES  = hw_ggi.c
libhw_tty_la_SOURCES  = hw_tty.c
libhw_twin_la_SOURCES = hw_twin.c

AM_LDFLAGS            = -export-dynamic -export-symbols-regex '^(Init|Quit)Module$$' -release $(PACKAGE_VERSION)
libhw_X11_la_LDFLAGS  = -export-dynamic -export-symbols-regex '^(Init|Quit)Module$$' -release $(PACKAGE_VERSION) $(X11_LDFLAGS)
libhw_gfx_la_LDFLAGS  = -export-dynamic -export-symbols-regex '^(Init|Quit)Module$$' -release $(PACKAGE_VERSION) $(X11_LDFLAGS)

libhw_display_la_LIBADD =
libhw_X11_la_LIBADD   = $(LIBTUTF) $(LIBX11)
libhw_gfx_la_LIBADD   = $(LIBTUTF) $(LIBX11) $(LIBXPM)
libhw_ggi_la_LIBADD   = $(LIBGGI)
libhw_tty_la_LIBADD   = $(LIBTUTF) $(LIBTERMCAP) $(LIBGPM)
libhw_twin_la_LIBADD  = $(LIBTUTF) $(LIBTW) $(LIBZ)
