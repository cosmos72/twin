SUBDIRS =
AM_CPPFLAGS           = -I$(top_srcdir)/include

LIBSTL = $(top_builddir)/libs/libstl/libstl.a

noinst_PROGRAMS       = twtest

twtest_SOURCES      = test_stl_vector.cpp test_main.cpp
twtest_LDADD        = $(LIBSTL)
