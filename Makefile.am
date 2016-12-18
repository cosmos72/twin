SUBDIRS = docs include libs clients themes server

ACLOCAL_AMFLAGS = -I m4

pkglibexecdir = $(pkglibdir)

pkgdata_DATA       = BUGS COPYING COPYING.LIB Changelog.txt INSTALL README README.porting twin-current.lsm
pkglibexec_SCRIPTS = system.twenvrc.sh system.twinrc 

install-exec-hook:
	@if echo "$(host_triplet)" | $(GREP) linux >/dev/null 2>&1 ; then \
	  CMD_LDCONFIG="ldconfig" ; \
	elif echo "$(host_triplet)" | $(GREP) freebsd >/dev/null 2>&1 ; then \
	  CMD_LDCONFIG="ldconfig -R" ; \
	else \
	  CMD_LDCONFIG="" ; \
	fi; \
	if test "x$$CMD_LDCONFIG" != x ; then \
	  CMD_ERR1=""; \
	  CMD_ERR2=""; \
	  CMD_ERR3=""; \
	  if id 2>/dev/null | $(EGREP) "^uid=0" >/dev/null 2>&1 ; then \
	    true; \
	  else \
	    CMD_ERR1="installing with non-root user \"`whoami 2>/dev/null`\""; \
	  fi; \
	  if test "x$(DESTDIR)" != x ; then \
	    CMD_ERR2="installing to non-null DESTDIR = \"$(DESTDIR)\""; \
	  fi; \
	  if test "x$(host_triplet)" != "x$(build_triplet)" ; then \
	    CMD_ERR3="cross-compile detected, host system \"$(host_triplet)\" != build system \"$(build_triplet)\""; \
	  fi; \
	  if test "x$$CMD_ERR1$$CMD_ERR2$$CMD_ERR3" = x ; then \
	    echo " $$CMD_LDCONFIG" ; \
	    $$CMD_LDCONFIG ; \
	  else \
	    echo "[33;1m==================================================================="; \
	    echo "=========================== WARNING ==============================="; \
	    echo "==================================================================="; \
	    echo "===                                                             ==="; \
	    echo "===    cannot run required command \"$$CMD_LDCONFIG\" now. reasons:     ==="; \
	    echo "===                                                             ==="; \
	    if test "x$$CMD_ERR1" != x ; then \
	      echo "===   * $$CMD_ERR1"; \
	    fi; \
	    if test "x$$CMD_ERR2" != x ; then \
	      echo "===   * $$CMD_ERR2"; \
	    fi; \
	    if test "x$$CMD_ERR3" != x ; then \
	      echo "===   * $$CMD_ERR3"; \
	    fi; \
	    echo "===                                                             ==="; \
	    echo "=== remember to run \"$$CMD_LDCONFIG\" with root privileges (or reboot) ==="; \
	    echo "===       after you install twin into its final location,       ==="; \
	    echo "===    otherwise you may receive \"library not found\" errors     ==="; \
	    echo "===                                                             ==="; \
	    echo "==================================================================="; \
	    echo "==================================================================="; \
	    echo "===================================================================[0m"; \
          fi; \
	fi
