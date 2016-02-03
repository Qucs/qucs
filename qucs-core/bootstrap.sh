#! /bin/sh
#
# bootstrap.sh
#
# Run this script in preparation for running configure and make.
#
# Copyright (C) 2013 Richard Crozier <richard.crozier@yahoo.co.uk.org>
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this package; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
# Boston, MA 02110-1301, USA.
#

here=`pwd`
cd `dirname $0`

if [ -d "./deps/adms" ]; then
# if present, run bootstrap on the adms subproject
  if [ -e "./deps/adms/bootstrap.sh" ]; then
    echo "running adms bootstrap.sh ..."
    ./deps/adms/bootstrap.sh "$@"
  elif [ -e "./deps/adms/autogen.sh" ]; then
    echo "running autogen.sh for the adms sources ..."
    ./deps/adms/autogen.sh "$@";
  else
    echo "Could not locate adms autogen script in ./adms, you may use configure with --disable-adms to use installed version"
    exit
  fi
else
  echo "No local adms source folder found (you may need to use the configure --diable-adms option to build with installed version)"
fi

echo "bootstrapping the qucs-core sources..."
echo -n "Creating aclocal.m4... "
${ACLOCAL:-aclocal} -I m4
echo "done."
echo -n "Creating config.h.in... "
autoheader
echo "done."
echo -n "Libtoolizing... "
case `uname` in
  *Darwin*) LIBTOOLIZE=glibtoolize ;;
  *)        LIBTOOLIZE=libtoolize ;;
esac
$LIBTOOLIZE
echo "done."
echo -n "Creating Makefile.in(s)... "
${AUTOMAKE:-automake} -a -f -c
echo "done."
echo -n "Creating configure... "
autoconf
echo "done."
