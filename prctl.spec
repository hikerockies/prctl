%define version 1.6

Summary: Utility to perform process operations
Name: prctl
Version: %{version}
Release: 1
Copyright: GPL
Group: Applications/System
Source: ftp://linux.hpl.hp.com/pub/linux-ia64/prctl-%{version}.tar.gz
Buildroot: /var/tmp/%{name}-root
ExclusiveOS: linux 

%description
The prctl utility allows a user to control certain process behavior in the
runtime environment. This utility works on Linux 2.4 and higher kernels only.

%prep
%setup

%build
./configure
make

%clean
rm -rf $RPM_BUILD_ROOT

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/usr/local/man
make install prefix=$RPM_BUILD_ROOT/usr/local

%post

%preun

%files
%doc
/usr/local/bin/prctl
/usr/local/man/man1/prctl.1

%changelog
* Wed Feb 26 2014	Khalid Aziz (khalid.aziz@oracle.com)
- Improved messages and suppressed unnecessary error messages
- Added support for PR_MCE_KILL

* Thu May 18 2006	Khalid Aziz (khalid_aziz@hp.com)
- Updated man page location in Makefile.in
- Better error message in case kernel does not support prctl operations.

* Wed Mar  5 2003	Khalid Aziz (khalid_aziz@hp.com)
- Added "always-signal" option to --unaligned for ia64.

* Wed Oct 17 2001	Khalid Aziz (khalid_aziz@hp.com)
- Added option to set floating point emulation behavior.

* Mon Jun  4 2001	Khalid Aziz (khalid_aziz@hp.com)
- Bug fix: Did not recognize "--unaligned=default" as a valid 
  command.

* Fri Jan 12 2001	Khalid Aziz (khalid_aziz@hp.com)
- Removed post-install warning from spec file. Added the removed
  warning to NOTES section of man page.

* Thu Nov  9 2000	Khalid Aziz (khalid_aziz@hp.com)
- Bug fix: If invoked as "prctl --unaligned=signal ./foo -b -a 
  -r bletch", options -b, -a and -r are interpreted as options to 
  prctl as opposed to options to foo.

* Thu Sep 21 2000	Khalid Aziz (khalid_aziz@hp.com)
- Initial release of prctl with support for unaligned memory access
  behavior control.
