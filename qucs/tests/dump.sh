#!/bin/sh
# testing netlisting
# there are more tests in the test package. but not reachable from here.

# now this one seems obsolete.

do_test(){
echo "../main/qucs -l leg_sch -n -i ${examplesdir}/$1 -o $1.out" >&9
../main/qucs -l leg_sch -n -i ${examplesdir}/$1 -o $1.out 2>&9

diff -w ${srcdir}/$1.ref $1.out >&9

if [ $? -ne 0 ]; then
	echo $1 failed
	exit 1
fi
}

do_test LPF-Balun2.sch
do_test resonance.sch
