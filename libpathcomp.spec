#
# spec file for package libpathcomp
#
# Copyright (c) 2016 Edward Baudrez <edward.baudrez@gmail.com>
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.
#

Name: libpathcomp
Version: 0.2
Release: 1
%define lib_name libpathcomp
%define so_version 1
Summary: C library for composing and working with structured pathnames
URL: http://github.com/ebaudrez/libpathcomp
License: GPL-2.0+
BuildRoot: %{_tmppath}/%{name}-%{version}-build
Source: %{name}-%{version}.tar.gz
%description
Libpathcomp is a C library for composing and working with structured pathnames.
With it, you can build pathnames like

    /opt/data/N6/N6_8903.dat

from a template specified in a config file, and a few lines of C code.
Libpathcomp has the following features:

* It takes its information from a configuration file. No recompilation
  required when the configuration file changes.
* Settings from the configuration file can be overridden at run time.
* Constructed pathnames are allocated dynamically, and are automatically of
  the right size. Static or fixed-size buffers are not needed.
* It supports functions written in Lua, to construct arbitrarily complex
  pathnames.
* It supports 'variations' of pathnames, like multiple possibilities for a
  file extension, multiple root directories (sort of like a search path), etc.
* It has a few convenience functions to make it easy to work with the
  constructed pathnames, e.g., checking for existence, creating the containing
  directory, finding the first (or all) of a set of files.

%package -n %lib_name%so_version
Summary: C library for composing and working with structured pathnames - shared library
Group: System/Libraries

%description -n %lib_name%so_version
Libpathcomp is a C library for composing and working with structured pathnames.
With it, you can build pathnames like

    /opt/data/N6/N6_8903.dat

from a template specified in a config file, and a few lines of C code.
Libpathcomp has the following features:

* It takes its information from a configuration file. No recompilation
  required when the configuration file changes.
* Settings from the configuration file can be overridden at run time.
* Constructed pathnames are allocated dynamically, and are automatically of
  the right size. Static or fixed-size buffers are not needed.
* It supports functions written in Lua, to construct arbitrarily complex
  pathnames.
* It supports 'variations' of pathnames, like multiple possibilities for a
  file extension, multiple root directories (sort of like a search path), etc.
* It has a few convenience functions to make it easy to work with the
  constructed pathnames, e.g., checking for existence, creating the containing
  directory, finding the first (or all) of a set of files.

This package contains the shared library.

%package devel
Summary: C library for composing and working with structured pathnames - header files
Group: Development/Libraries/C and C++
BuildRequires: pkg-config
Requires: %lib_name%so_version = %{version}
%if 0%{?suse_version}
Recommends: %{name}-util = %{version}
%endif

%description devel
Libpathcomp is a C library for composing and working with structured pathnames.
With it, you can build pathnames like

    /opt/data/N6/N6_8903.dat

from a template specified in a config file, and a few lines of C code.
Libpathcomp has the following features:

* It takes its information from a configuration file. No recompilation
  required when the configuration file changes.
* Settings from the configuration file can be overridden at run time.
* Constructed pathnames are allocated dynamically, and are automatically of
  the right size. Static or fixed-size buffers are not needed.
* It supports functions written in Lua, to construct arbitrarily complex
  pathnames.
* It supports 'variations' of pathnames, like multiple possibilities for a
  file extension, multiple root directories (sort of like a search path), etc.
* It has a few convenience functions to make it easy to work with the
  constructed pathnames, e.g., checking for existence, creating the containing
  directory, finding the first (or all) of a set of files.

This package contains the header files and other files necessary for developing
new projects.

%package util
Summary: C library for composing and working with structured pathnames - utility
Group: Productivity/File utilities
Requires: %lib_name%so_version = %{version}

%description util
Libpathcomp is a C library for composing and working with structured pathnames.
With it, you can build pathnames like

    /opt/data/N6/N6_8903.dat

from a template specified in a config file, and a few lines of C code.
Libpathcomp has the following features:

* It takes its information from a configuration file. No recompilation
  required when the configuration file changes.
* Settings from the configuration file can be overridden at run time.
* Constructed pathnames are allocated dynamically, and are automatically of
  the right size. Static or fixed-size buffers are not needed.
* It supports functions written in Lua, to construct arbitrarily complex
  pathnames.
* It supports 'variations' of pathnames, like multiple possibilities for a
  file extension, multiple root directories (sort of like a search path), etc.
* It has a few convenience functions to make it easy to work with the
  constructed pathnames, e.g., checking for existence, creating the containing
  directory, finding the first (or all) of a set of files.

This package contains the standalone utility.

%prep
%setup -q

%build
%configure --disable-static --with-pic
make %{?_smp_mflags}

%install
%make_install
rm -vf %{buildroot}%{_libdir}/*.la

%check
make check %{?_smp_mflags} || (cat ./test/test-suite.log && false)

%post -n %lib_name%so_version
/sbin/ldconfig

%postun -n %lib_name%so_version
/sbin/ldconfig

%files -n %lib_name%so_version
%defattr(-, root, root)
%{_libdir}/libpathcomp.so.1.0.0
%{_libdir}/libpathcomp.so.1

%files devel
%defattr(-, root, root)
%doc COPYING README.md
%{_includedir}/pathcomp.h
%{_includedir}/pathcomp/log.h
%{_libdir}/libpathcomp.so
%{_libdir}/pkgconfig/libpathcomp.pc

%files util
%defattr(-, root, root)
%{_bindir}/pathcomp

%changelog
* Tue Oct 4 2016 Edward Baudrez <edward.baudrez@gmail.com>
- initial release
