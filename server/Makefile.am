SUBDIRS = extensions HW

bin_PROGRAMS = twin_real
twin_real_SOURCES = alloc.c builtin.c data.c display.c dl.c draw.c extreg.c hw.c hw_multi.c main.c md5.c methods.c missing.c printk.c pty.c rcopt.c rcparse_lex.c rcparse_tab.c rcrun.c remote.c resize.c scroller.c shm.c socket.c tterm.c tty.c util.c wm.c

AM_CPPFLAGS = -I$(top_srcdir)/include

LIBZ = @LIBZ@
LIBGPM = @LIBGPM@
LIBGGI = @LIBGGI@
LIBPTHREAD = @LIBPTHREAD@
LIBTERMCAP = @LIBTERMCAP@