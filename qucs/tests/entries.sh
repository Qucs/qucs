#!/bin/bash

mkdir -p out
../main/qucs -a ../plugins/verilog/.libs/verilog --list-entries > out/entries.out 2>&9

diff ${srcdir}/ref/entries.out out/entries.out >&9
