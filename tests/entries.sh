#!/bin/bash

../main/qucs -a ../plugins/.libs/verilog --list-entries > entries.out 2>&9

diff ${srcdir}/entries.ref entries.out >&9
