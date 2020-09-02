#!/bin/bash

../main/qucs -a ../qucs/sim/.libs/verilog.so --list-entries > entries.out 2>&9

diff ${srcdir}/entries.ref entries.out >&9
