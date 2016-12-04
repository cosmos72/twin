SUBDIRS = mapscrn

bin_PROGRAMS  = twattach twcat twclip twclutter twcuckoo twdialog twevent twfindtwin twlsmsgport twlsobj twsendmsg twsetroot twsysmon twterm twthreadtest
sbin_PROGRAMS = twdm

AM_CPPFLAGS = -I$(top_srcdir)/include

LIBTUTF = $(top_builddir)/libs/libTutf/libTutf.la
LIBTW = $(top_builddir)/libs/libTw/libTw.la

twattach_SOURCES     = attach.c
twcat_SOURCES        = cat.c
twclip_SOURCES       = clip.c
twclutter_SOURCES    = clutter.c
twcuckoo_SOURCES     = cuckoo.c
twdialog_SOURCES     = dialog.c
twdm_SOURCES         = dm.c
twevent_SOURCES      = event.c
twfindtwin_SOURCES   = findtwin.c
twlsmsgport_SOURCES  = lsmsgport.c
twlsobj_SOURCES      = lsobj.c
twsendmsg_SOURCES    = sendmsg.c
twsetroot_SOURCES    = setroot.c
twsysmon_SOURCES     = sysmon.c
twterm_SOURCES       = pty.c term.c
twthreadtest_SOURCES = threadtest.c

twattach_LDADD     = $(LIBTW)
twcat_LDADD        = $(LIBTW)
twclip_LDADD       = $(LIBTW)
twclutter_LDADD    = $(LIBTW)
twcuckoo_LDADD     = $(LIBTW)
twdialog_LDADD     = $(LIBTW)
twdm_LDADD         = $(LIBTW) $(LIBTUTF) $(LIBCRYPT)
twevent_LDADD      = $(LIBTW)
twfindtwin_LDADD   = $(LIBTW)
twlsmsgport_LDADD  = $(LIBTW)
twlsobj_LDADD      = $(LIBTW)
twsendmsg_LDADD    = $(LIBTW)
twsetroot_LDADD    = $(LIBTW)
twsysmon_LDADD     = $(LIBTW)
twterm_LDADD       = $(LIBTW) $(LIBTUTF)
twthreadtest_LDADD = $(LIBTW) $(LIBPTHREAD)

install-exec-hook:
	rm -f            $(DESTDIR)$(bindir)/twdetach
	$(LN_S) twattach $(DESTDIR)$(bindir)/twdetach

