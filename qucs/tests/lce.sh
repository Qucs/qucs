#!/bin/bash

../main/qucs -a ../qucs/sim/.libs/verilog.so --list-entries > entries.out

diff ${srcdir}/entries.ref entries.out
