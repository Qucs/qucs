# This spec file was generated using Kpp
# If you find any problems with this spec file please report
# the error to ian geiser <geiseri@msoe.edu>
Summary:   circuit simulator
Name:      qucs
Version:   0.1
Release:   0.1
Copyright: GPL
Vendor:    Michael Margraf <margraf@mwt.ee.tu-berlin.de>
Url:       

Packager:  Michael Margraf <margraf@mwt.ee.tu-berlin.de>
Group:     simulation
Source:    qucs-0.1.tar.gz
BuildRoot: 

%description
Qucs (Qt universal circuit simulator) is a simulation program for electronic circuits that (one day) should support all kinds of simulations (dc, ac, s-parameter, harmonic balance etc.). Qucs (speak: [kjuks] ) uses Qt (Trolltech) to provide a powerful graphical user interface.

%prep
%setup
CFLAGS="$RPM_OPT_FLAGS" CXXFLAGS="$RPM_OPT_FLAGS" ./configure \
                 \
                $LOCALFLAGS
%build
# Setup for parallel builds
numprocs=`egrep -c ^cpu[0-9]+ /proc/stat || :`
if [ "$numprocs" = "0" ]; then
  numprocs=1
fi

make -j$numprocs

%install
make install-strip DESTDIR=$RPM_BUILD_ROOT

cd $RPM_BUILD_ROOT
find . -type d | sed '1,2d;s,^\.,\%attr(-\,root\,root) \%dir ,' > $RPM_BUILD_DIR/file.list.qucs
find . -type f | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.qucs
find . -type l | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.qucs

%clean
rm -rf $RPM_BUILD_ROOT/*
rm -rf $RPM_BUILD_DIR/qucs
rm -rf ../file.list.qucs


%files -f ../file.list.qucs
