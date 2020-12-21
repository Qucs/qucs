#!/bin/sh
# testing basic module loading and unloading.

QUCS=../main/qucs
refdir=$srcdir/ref

if [ ! -x ${QUCS} ]; then
	echo ${QUCS} does not exist >&9
   exit 1
fi

mkdir -p out

${QUCS} -a ./helloworld -q | \
       tr '\n' '|' | tr '\r' '|' | \
       awk -F"|" '{print $1 $2 $3;}' > out/helloworld.sh.out

diff out/helloworld.sh.out $refdir/helloworld.sh.out
