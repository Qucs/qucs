#!/bin/sh
# testing netlisting
# there are more tests in the test package. but not reachable from here.
# TODO: migrate all tests

../qucs/qucs -n -i ${examplesdir}/resonance.sch -o resonance.sch.out
