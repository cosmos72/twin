SUBDIRS = extensions HW

noinst_LTLIBRARIES = librcparse.la libsocket.la libterm.la libwm.la
bin_PROGRAMS       = twdisplay twin twin_real

AM_CPPFLAGS        = -I$(top_srcdir)/include $(LTDLINCL)

#LIBTUTF = $(top_builddir)/libs/libTutf/libTutf.la
#LIBTW = $(top_builddir)/libs/libTw/libTw.la

librcparse_la_SOURCES = rcparse_tab.c rcparse_lex.c
libterm_la_SOURCES    = pty.c tterm.c tty.c
libsocket_la_SOURCES  = md5.c socket.c
libwm_la_SOURCES      = rcopt.c rcrun.c shm.c wm.c

twdisplay_SOURCES  = alloc.c display.c missing.c hw.c
twin_SOURCES       = wrapper.c
twin_real_SOURCES  = alloc.c builtin.c data.c dl.c draw.c extensions/ext_query.c extreg.c hw.c hw_multi.c main.c methods.c missing.c printk.c remote.c resize.c scroller.c util.c 

twdisplay_LDADD    = $(LIBTW) $(LIBTUTF) $(LIBLTDL) $(LIBDL)
twin_real_LDADD    =          $(LIBTUTF) $(LIBLTDL) $(LIBDL)
