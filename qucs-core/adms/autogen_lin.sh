#! /bin/sh
#
# autogen_lin.sh
#
# Run this script to re-generate all maintainer-generated files.
#
# Copyright (C) 2003 Stefan Jahn <stefan@lkcc.org>
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this package; see the file COPYING. If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
# Boston, MA 02110-1301, USA.
#

here=`pwd`
cd `dirname $0`

rm -rf auxconf
mkdir auxconf
echo -n "Creating aclocal.m4... "
aclocal
echo "done."
echo -n "Creating config.h.in... "
autoheader
echo "done."
echo -n "Libtoolizing... "
case `uname` in
  *Darwin*) LIBTOOLIZE=glibtoolize ;;
  *)        LIBTOOLIZE=libtoolize ;;
esac
$LIBTOOLIZE --copy --force --ltdl -c
echo "done."
echo -n "Creating Makefile.in(s)... "
automake --copy -Wall --add-missing
echo "done."
echo -n "Creating configure... "
autoconf
echo "done."

# from README
# rm -rf auxconf && autoheader && mkdir auxconf && aclocal && libtoolize --force --ltdl -c
# touch ChangeLog && automake -a -c && autoconf
# ./configure --enable-maintainer-mode

#
# run configure
#
CMD="./configure --enable-maintainer-mode"
echo "Running $CMD $@ ..."
$CMD "$@"
#
# prepare make
#
echo "Running make clean"
make clean

echo
echo "Continue with:"
echo "make"
echo "make install"
