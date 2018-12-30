#!/bin/sh
# testing netlisting
# there are more tests in the test package. but not reachable from here.
# TODO: migrate all tests

do_test(){
echo "../main/qucs -n -i ${examplesdir}/$1 -o $1.out" >&9
../main/qucs -n -i ${examplesdir}/$1 -o $1.out 2>&9

diff ${srcdir}/$1.ref $1.out 2>&9

if [ $? -ne 0 ]; then
	echo $1 failed
	exit 1
fi
}

do_test resonance.sch
do_test LPF-Balun2.sch
