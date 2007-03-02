Summary: Your One Get Remaining Time library.
Name: See META file
Version: See META file
Release: See META file
License: Proprietary
Group: System Environment/Base
#URL: 
Packager: Christopher J. Morrone <morrone2@llnl.gov>
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

%description
A simple wrapper library that provides a unified get-remaining-time
interface for multiple parallel job scheduling systems.

%prep
%setup -q

%build
%ifos foo
#%ifos aix5.3 aix5.2 aix5.1 aix5.0 aix4.3
# Build all of the libraries twice: 32bit versions, and then 64bit versions
TOP="`pwd`"
TMP="$TOP/aix"
rm -rf "$TMP"
for bits in 64 32; do
	OBJECT_MODE=$bits
	export OBJECT_MODE
	%configure -C
	mkdir -p $TMP/orig/$bits
	DESTDIR=$TMP/orig/$bits make install
	make clean
done
# Now merge the 32bit and 64bit versions of the libraries together into
# one composite libyogrt.a library.
for subpackage in none slurm lcrm moab; do
	if [ ! -d $TMP/orig/32%{_libdir}/libyogrt/$subpackage ]; then
		continue
	fi
	mkdir -p $TMP/$subpackage/32
	cd $TMP/$subpackage/32
	ar -X32 x $TMP/orig/32%{_libdir}/libyogrt/$subpackage/libyogrt.a

	mkdir -p $TMP/$subpackage/64
	cd $TMP/$subpackage/64
	ar -X64 x $TMP/orig/64%{_libdir}/libyogrt/$subpackage/libyogrt.a

	cd $TMP/$subpackage
	ar -Xany cr libyogrt.a $TMP/$subpackage/*/*
	cd $TOP
done
rm -rf $TMP/orig
%else
%configure
make
%endif

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT
DESTDIR="$RPM_BUILD_ROOT" make install
%ifos foo
#%ifos aix5.3 aix5.2 aix5.1 aix5.0 aix4.3
if [ -d aix ] ; then
	for subpackage in none slurm lcrm moab; do
		if [ ! -d aix/$subpackage ] ; then continue; fi
		cp aix/$subpackage/libyogrt.a \
			"$RPM_BUILD_ROOT"%{_libdir}/libyogrt/$subpackage
	done
fi
%endif

%files
%defattr(-,root,root,-)
%doc
%{_includedir}/yogrt.h
%{_libdir}/*
%{_libdir}/libyogrt/*
%{_mandir}/*/*

%clean
rm -rf $RPM_BUILD_ROOT

%changelog
* Mon Feb 12 2007 Christopher J. Morrone <morrone@conon.llnl.gov> - 
- Initial build.

