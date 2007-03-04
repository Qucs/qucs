#!/bin/sh

(cd ../.. && ./autodsp.sh -v -6)
rm -f *.ds? *.tgz *~
mv `find ../.. -name "*.ds?"` .
tar cvhozf vcgen-qucs.tgz *.dsp *.dsw *.sh *.bat config.h.*

