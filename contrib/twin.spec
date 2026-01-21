# Twin is a windowing environment inside a text display.
#
# Copyright (C) 1999-2020 by Massimiliano Ghilardi
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see
# <https://www.gnu.org/licenses/>.
#

Name:       twin
Version:    0.9.1+git
Release:    %{?dist}%{!?dist:%{_vendor}1}
License:    GPL-2.0-or-later AND LGPL-2.0-or-later
Group:      User Interface/Twin
Summary:    Textmode WINdow environment
URL:        https://github.com/cosmos72/twin/
Source0:    %name-%version.tar.gz
BuildRoot:  %{_tmppath}/%{name}-%{version}-root

%define libgpm_dev_pkg %(rpm -q --whatprovides %{_libdir}/libgpm.so | sed 's/-[0-9]\\+\\.[^ ]*//')
%define libltdl_pkg %(rpm -q --whatprovides %{_libdir}/libltdl.so.7 | sed 's/-[0-9]\\+\\.[^ ]*//')
%define libltdl_dev_pkg %(rpm -q --whatprovides %{_libdir}/libltdl.so | sed 's/-[0-9]\\+\\.[^ ]*//')

BuildRequires:  gcc-c++
BuildRequires:  gpm
BuildRequires:  %libgpm_dev_pkg
BuildRequires:  %libltdl_pkg
BuildRequires:  %libltdl_dev_pkg
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
Summary:    Textmode WINdow environment - developer's files
Group:      Development/Libraries
Requires:   %{name}

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
%global optflags -O2 -g -ffat-lto-objects -fPIE
%{?optflags_lto:%global optflags_lto %optflags_lto -ffat-lto-objects}

%configure
./configure --prefix=%{_prefix} --libdir=%{_libdir} --enable-ltdl-install

%build
sed -i 's|^hardcode_libdir_flag_spec=.*|hardcode_libdir_flag_spec=""|g' libtool
sed -i 's|^runpath_var=LD_RUN_PATH|runpath_var=DIE_RPATH_DIE|g' libtool
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
%doc COPYING COPYING.LIB
%doc BUGS Changelog.txt
%doc INSTALL README README.porting twin-current.lsm
%doc docs/Compatibility docs/libtw.txt docs/ltrace.conf
%doc docs/Philosophy docs/Tutorial
%{_libdir}/libtstl.so.*
%{_libdir}/libtutf.so.*
%{_libdir}/libtw.so.*
%{_libdir}/%{name}
%{_datadir}/%{name}
%{_sysconfdir}/%{name}/twinrc
%{_sysconfdir}/%{name}/twenvrc.sh
%{_bindir}/tw*
%{_sbindir}/tw*

%files devel
%defattr(-, root, root)
%{_includedir}/Tutf
%{_includedir}/Tw
%{_libdir}/libtstl.a
%{_libdir}/libtstl.so
%{_libdir}/libtutf.a
%{_libdir}/libtutf.so
%{_libdir}/libtw.a
%{_libdir}/libtw.so

%changelog
* Wed Mar 26 2025 Vladimir Mišev <vladimir.mishev@gmail.com> 0.9.1+git-%{?dist}%{!?dist:%{_vendor}1}
- Updated as per Fedora guidelines; removed obsolete tags
- Updated header comment so it has proper Copyright and GPL-2
- Added macros to find pkg names for needed libs and then satisfy BuildRequires
- Added in description  x86_64, and credit to the Author
- Added in prep download (git clone) and then tar into RPM/SOURCES
- Added in setup optflags and dealing with lto
- Added configure and moved there ./configure and added ltdl install
- Added in build sed to deal with rpaths in libtool before make
- Removed from doc nonexistent docs/Configure and
  changed macro for twinrc and twenvrc.sh
- Added changelog

* Mon Oct 2 2022 Oron Peled <oron@actcom.co.il> 0.5.0-2
- Original twin.spec
