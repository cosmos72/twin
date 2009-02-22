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
Vendor:		Massimiliano Ghilardi <max@linuz.sns.it>
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

%package libTT
Summary:	Text Toolkit library for twin - Textmode WINdow environment
Group:		User Interface/Twin
Requires:	%{name}
Provides:	%{name}-libTT

%description libTT
This package supplements '%{name}'.
It contains libTT, a Text Toolkit library for %{name}


%package libTT-devel
Summary:	Text Toolkit library for twin - developer's files
Group:		Development/Libraries
Requires:	%{name} %{name}-libTT

%description libTT-devel
This package supplements 'libTT'.
It contains header files and static libraries for libTT

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
%doc docs/Configure docs/Tutorial docs/libTw.txt docs/ltrace.conf
%doc docs/Compatibility docs/libTT-design.txt docs/Philosophy
%doc COPYING COPYING.LIB Changelog.txt README BUGS INSTALL README.porting
%doc twin-current.lsm
%{_libdir}/libTutf.so.*
%{_libdir}/libTw.so.*
%{_libdir}/twin
%{_datadir}/twin
%config %{_libdir}/twin/.twinrc
%config %{_libdir}/twin/.twenvrc.sh
%config %{_libdir}/twin/system.twinrc
%config %{_libdir}/twin/system.twenvrc.sh
%{_bindir}/tw*
%{_sbindir}/tw*

%files devel
%defattr(-, root, root)
%{_includedir}/Tutf
%{_includedir}/Tw
%{_libdir}/libTutf.a
%{_libdir}/libTutf.so
%{_libdir}/libTw.a
%{_libdir}/libTw.so

%files libTT
%{_libdir}/TT
%{_libdir}/libTT.so.*
%{_bindir}/tt*

%files libTT-devel
%{_includedir}/TT
%{_libdir}/libTT.a
%{_libdir}/libTT.so

%clean
[ "$RPM_BUILD_ROOT" != "/" ] && [ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT

