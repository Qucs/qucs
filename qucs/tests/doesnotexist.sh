#!/bin/sh
# load a nonexistant module

./qucs -a ./does_n_otexist
ret=$?

if [ $ret -eq 0 ]; then
	echo "did not notice nonexistant module. really bad"; >&9
	exit 1;
fi
