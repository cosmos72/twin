SUBDIRS = docs include libs clients themes server

ACLOCAL_AMFLAGS = -I m4

pkglibexecdir = $(pkglibdir)

pkgdata_DATA       = BUGS COPYING COPYING.LIB Changelog.txt INSTALL README README.porting twin-current.lsm
pkglibexec_SCRIPTS = system.twenvrc.sh system.twinrc 

install-exec-hook:
	@case "`uname 2>/dev/null`" in \
	  Linux)   CMD_LDCONFIG="ldconfig" ;; \
	  FreeBSD) CMD_LDCONFIG="ldconfig -R" ;; \
	esac; \
	echo "CMD_LDCONFIG = $$CMD_LDCONFIG"; \
	if test "x$(DESTDIR)" = x ; then \
	  if id 2>/dev/null | $(EGREP) "^uid=0" >/dev/null 2>&1 ; then \
	    echo "$$CMD_LDCONFIG" ; \
	    $$CMD_LDCONFIG ; \
	    CMD_LDCONFIG="" ; \
	  fi; \
	fi; \
	if test "x$$CMD_LDCONFIG" != x ; then \
	  echo "=================================================================="; \
	  echo "========================== WARNING ==============================="; \
	  echo "=================================================================="; \
	  echo "===                                                            ==="; \
	  echo "===                 cannot run \"$$CMD_LDCONFIG\" now                  ==="; \
	  echo "===                                                            ==="; \
	  echo "===          remember to run it with root proviledges          ==="; \
	  echo "===       when you install twin into its final location        ==="; \
	  echo "===     otherwise you may receive 'library not found' errors   ==="; \
	  echo "===                                                            ==="; \
	  echo "=================================================================="; \
	  echo "=================================================================="; \
	  echo "=================================================================="; \
	fi;
