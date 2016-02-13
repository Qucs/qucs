#!/bin/sh
# testing basic module loading and unloading.

newline='
'

text=$(./qucs -a ./helloworld -q)

if [ "$text" = "hello${newline}goodbye" ]; then
	echo "test pass"
else
	echo wrong $text
	exit 1
fi

./qucs -a ./does_n_otexist

if [ $? -eq 0 ]; then
	echo did not notice. really bad;
	exit 1;
fi
