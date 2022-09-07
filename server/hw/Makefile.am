pkglib_LTLIBRARIES =

if LIBHW_DISPLAY_la
  pkglib_LTLIBRARIES += libhw_display.la
endif
if LIBHW_X11_la
  pkglib_LTLIBRARIES += libhw_X11.la
endif
if LIBHW_XFT_la
  pkglib_LTLIBRARIES += libhw_xft.la
endif
if LIBHW_TTY_la
  pkglib_LTLIBRARIES += libhw_tty.la
endif
if LIBHW_TWIN_la
  pkglib_LTLIBRARIES += libhw_twin.la
endif

AM_CPPFLAGS           = -I$(top_srcdir)/include -I$(top_srcdir)/server
libhw_X11_la_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/server $(X11_CPPFLAGS)
libhw_xft_la_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/server $(X11_CPPFLAGS) $(XFT_CFLAGS)

libhw_display_la_SOURCES = hw_display.cpp
libhw_X11_la_SOURCES  = hw_X11.cpp
libhw_xft_la_SOURCES  = hw_xft.cpp
libhw_tty_la_SOURCES  = hw_tty.cpp
libhw_twin_la_SOURCES = hw_twin.cpp

AM_LDFLAGS            = -export-dynamic -export-symbols-regex '^(Init|Quit)Module$$' -release $(PACKAGE_VERSION)
libhw_X11_la_LDFLAGS  = -export-dynamic -export-symbols-regex '^(Init|Quit)Module$$' -release $(PACKAGE_VERSION) $(X11_LDFLAGS)
libhw_xft_la_LDFLAGS  = -export-dynamic -export-symbols-regex '^(Init|Quit)Module$$' -release $(PACKAGE_VERSION) $(X11_LDFLAGS)

libhw_display_la_LIBADD =
libhw_X11_la_LIBADD   = $(LIBTUTF) $(LIBTSTL) $(LIBX11)
libhw_xft_la_LIBADD   = $(LIBTUTF) $(LIBTSTL) $(LIBX11) $(XFT_LIBS) $(LIBXFT)
libhw_tty_la_LIBADD   = $(LIBTUTF) $(LIBTSTL) $(LIBTERMCAP) $(LIBGPM)
libhw_twin_la_LIBADD  = $(LIBTUTF) $(LIBTSTL) $(LIBTW) $(LIBZ)
