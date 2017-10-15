#!/bin/sh
# testing basic module loading and unloading.

QUCS=../main/qucs

if [ ! -x ${QUCS} ]; then
	echo ${QUCS} does not exist
   exit 1
fi

text=$( ${QUCS} -a ./helloworld -q | \
       tr '\n' '|' | tr '\r' '|' | \
       awk -F"|" '{print $1 $2 $3;}' )

if [ "$text" = "hellogoodbye" ]; then
	echo hellogoodbye passed; >&2
	echo hellogoodbye passed; >&9
else
	echo "unexpected: ->$text<-" >&2
	echo "unexpected: ->$text<-" >&9
	exit 1
fi

