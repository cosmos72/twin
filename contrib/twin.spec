#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
#
# Original Author: Massimiliano Ghilardi <https://github.com/cosmos72>

Name:     twin
Version:  0.9.0+git
Release:  %{?dist}%{!?_dist:%{_vendor}1}
License:  GPL-2.0-or-later AND LGPL-2.0-or-later
Group:    User Interface/Twin
Summary:  Textmode WINdow environment
URL:      https://github.com/cosmos72/twin/
Source0:	 %name-%version.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-root

BuildRequires:  gcc-c++
BuildRequires:  gpm
BuildRequires:  gpm-devel
BuildRequires:  libltdl7
%define dependency_fallback libltdl7-devel
%if 0%{?pkgconfig(libltdl):1}
%define dependency pkgconfig(libltdl)
%else
%define dependency %{dependency_fallback}
%endif
BuildRequires:  %{dependency}
BuildRequires:  pkgconfig(ncurses)
BuildRequires:  pkgconfig(xft)
BuildRequires:  pkgconfig(xpm)
BuildRequires:  pkgconfig(zlib)

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

Currently, twin is tested on Linux (i386, x86_64, PowerPC, Alpha, Sparc),
FreeBSD and SunOS.

This package was created based on the work of:
Massimiliano Ghilardi (https://github.com/cosmos72)


%package devel
Summary:	Textmode WINdow environment - developer's files
Group:		Development/Libraries
Requires:	%{name}

%description devel
This package supplements '%{name}'.
It contains header files and static libraries for %{name}
developers

%prep
%(
cd %{_topdir}/SOURCES/
git clone https://github.com/cosmos72/twin  %name-%version
tar -czf %name-%version.tar.gz %name-%version
rm -rf %name-%version
cd $OLDPWD
)

%setup
%{?optflags_lto:%global optflags_lto %optflags_lto -ffat-lto-objects}

%configure
./configure --prefix=%{_prefix} --libdir=%{_libdir} --enable-ltdl-install 
sed -i 's|^hardcode_libdir_flag_spec=.*|hardcode_libdir_flag_spec=""|g' libtool
sed -i 's|^runpath_var=LD_RUN_PATH|runpath_var=DIE_RPATH_DIE|g' libtool

%build
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
%doc docs/Tutorial docs/libtw.txt docs/ltrace.conf
%doc docs/Compatibility docs/Philosophy
%doc Changelog.txt README BUGS INSTALL README.porting
%doc COPYING COPYING.LIB twin-current.lsm
%{_libdir}/libtutf.so.*
%{_libdir}/libtw.so.*
%{_libdir}/libtstl.so.*
%{_libdir}/%{name}
%{_datadir}/%{name}
%config(noreplace) %{_sysconfdir}/%{name}/twinrc
%config(noreplace) %{_sysconfdir}/%{name}/twenvrc.sh
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
%{_libdir}/libtstl.a
%{_libdir}/libtstl.so
