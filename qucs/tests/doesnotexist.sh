#!/bin/sh
# load a nonexistent module

./qucs -a ./does_n_otexist
ret=$?

if [ $ret -eq 0 ]; then
	echo "did not notice nonexistent module. really bad"; >&9
	exit 1;
fi
