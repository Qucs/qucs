# !/bin/sh

here=`pwd`
cd `dirname $0`

echo "bootstrapping the ADMS sources..."
echo "Creating aclocal.m4... "
aclocal -I m4
echo "Creating headers... "
autoheader
echo "Creating configure... "
autoconf
echo "Libtoolizing... "
case `uname` in
  *Darwin*) LIBTOOLIZE=glibtoolize ;;
  *)        LIBTOOLIZE=libtoolize ;;
esac
$LIBTOOLIZE
echo "Creating Makefiles... "
automake --copy --add-missing --foreign
echo "done."

