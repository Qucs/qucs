#!/bin/sh
# load a nonexistant module

QUCS=../main/qucs

if [ ! -x ${QUCS} ]; then
	echo ${QUCS} does not exist
   exit 1
fi

${QUCS} -a ./does_n_otexist
ret=$?

if [ $ret -eq 0 ]; then
	echo "did not notice nonexistant module. really bad"; >&9
	exit 1;
fi
