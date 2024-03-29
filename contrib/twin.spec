#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
#
Name:		twin
Version:	0.5.0
Release:	2
Vendor:		Massimiliano Ghilardi <https://github.com/cosmos72>
Copyright:	GPL
Group:		User Interface/Twin
Packager:	Oron Peled <oron@actcom.co.il>
Summary:	Textmode WINdow environment
Source:		%name-%version.tar.gz
URL:		http://sourceforge.net/projects/twin
BuildRoot:	%{_tmppath}/%{name}-%{version}-root
Provides:	%{name}

%description
Twin is a windowing environment with mouse support, window manager,
terminal emulator and networked clients, all inside a text display.

It supports a variety of displays:
* plain text terminals (any termcap/ncurses compatible terminal,
  Linux console, twin's own terminal emulator);
* X11, where it can be used as a multi-window xterm;
* itself (you can display a twin on another twin);
* twdisplay, a general network-transparent display client, used
  to attach/detach more displays on-the-fly.

Currently, twin is tested on Linux (i386, PowerPC, Alpha, Sparc),
FreeBSD and SunOS.

%package devel
Summary:	Textmode WINdow environment - developer's files
Group:		Development/Libraries
Requires:	%{name}

%description devel
This package supplements '%{name}'.
It contains header files and static libraries for %{name}
developers

%prep
%setup

%build
./configure --prefix=%{_prefix}
make

%install
[ "$RPM_BUILD_ROOT" != "/" ] && [ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT
mkdir $RPM_BUILD_ROOT
%makeinstall

%post
ldconfig

%postun
ldconfig

%files
%defattr(-, root, root)
%doc %{_mandir}/man1/twin.1*
%doc docs/Configure docs/Tutorial docs/libtw.txt docs/ltrace.conf
%doc docs/Compatibility docs/Philosophy
%doc COPYING COPYING.LIB Changelog.txt README BUGS INSTALL README.porting
%doc twin-current.lsm
%{_libdir}/libtutf.so.*
%{_libdir}/libtw.so.*
%{_libdir}/twin
%{_datadir}/twin
%config %{_sysconfdir}/twin/twinrc
%config %{_sysconfdir}/twin/twenvrc.sh
%{_bindir}/tw*
%{_sbindir}/tw*

%files devel
%defattr(-, root, root)
%{_includedir}/Tutf
%{_includedir}/Tw
%{_libdir}/libtutf.a
%{_libdir}/libtutf.so
%{_libdir}/libtw.a
%{_libdir}/libtw.so

%clean
[ "$RPM_BUILD_ROOT" != "/" ] && [ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT
