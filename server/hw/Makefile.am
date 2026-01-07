pkglib_LTLIBRARIES    =
noinst_LIBRARIES      = libhw_all.a

AM_CPPFLAGS           = -I$(top_srcdir)/include -I$(top_srcdir)/server
libhw_all_a_CPPFLAGS  = $(AM_CPPFLAGS)
libhw_x11_la_CPPFLAGS = $(AM_CPPFLAGS) $(X11_CPPFLAGS)
libhw_xft_la_CPPFLAGS = $(AM_CPPFLAGS) $(X11_CPPFLAGS) $(XFT_CFLAGS)

libhw_all_a_SOURCES   = hw_all.cpp
libhw_display_la_SOURCES = hw_display.cpp
libhw_tty_la_SOURCES  = hw_tty.cpp
libhw_twin_la_SOURCES = hw_twin.cpp
libhw_x11_la_SOURCES  = hw_x11.cpp
libhw_xft_la_SOURCES  = hw_xft.cpp

AM_LDFLAGS            = -export-dynamic -export-symbols-regex '^(Init|Quit)Module$$' -release $(PACKAGE_VERSION)
libhw_x11_la_LDFLAGS  = $(AM_LDFLAGS) $(X11_LDFLAGS)
libhw_xft_la_LDFLAGS  = $(AM_LDFLAGS) $(X11_LDFLAGS)

libhw_all_a_LIBADD      = $(LIBTUTF) $(LIBTSTL)
libhw_display_la_LIBADD =
libhw_x11_la_LIBADD     = $(LIBTUTF) $(LIBTSTL) $(LIBX11)
libhw_xft_la_LIBADD     = $(LIBTUTF) $(LIBTSTL) $(LIBXFT) $(XFT_LIBS) $(LIBX11)
libhw_tty_la_LIBADD     = $(LIBTUTF) $(LIBTSTL) $(LIBTERMCAP) $(LIBGPM)
libhw_twin_la_LIBADD    = $(LIBTUTF) $(LIBTSTL) $(LIBTW) $(LIBZ)

if LIBHW_DISPLAY_la
  pkglib_LTLIBRARIES   += libhw_display.la
  libhw_all_a_SOURCES  += hw_display.cpp
endif
if LIBHW_TTY_la
  pkglib_LTLIBRARIES  += libhw_tty.la
  libhw_all_a_CPPFLAGS += -DTWIN_HW_TTY_STATIC
  libhw_all_a_SOURCES += hw_tty.cpp
endif
if LIBHW_TWIN_la
  pkglib_LTLIBRARIES  += libhw_twin.la
  libhw_all_a_CPPFLAGS += -DTWIN_HW_TWIN_STATIC
  libhw_all_a_SOURCES += hw_twin.cpp
endif

if LIBHW_X11_la
  pkglib_LTLIBRARIES   += libhw_x11.la
  libhw_all_a_CPPFLAGS += -DTWIN_HW_X11_STATIC $(X11_CPPFLAGS)
  libhw_all_a_SOURCES  += hw_x11.cpp
endif
if LIBHW_XFT_la
  pkglib_LTLIBRARIES   += libhw_xft.la
  libhw_all_a_CPPFLAGS += -DTWIN_HW_XFT_STATIC $(X11_CPPFLAGS) $(XFT_CFLAGS)
  libhw_all_a_SOURCES  += hw_xft.cpp
endif
