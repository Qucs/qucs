#!/bin/bash
OS=$1 ;
echo "----------------------------------------$2---------------------------" ;
find . -name "checklibraries.*" ;
find . -name "displaydialog.*" ;
find . -type f -exec grep -H "displaydialog" {} \; ;
find . -type f -exec grep -H "checklibraries" {} \; ;
echo "####################ls -la qucs/qucs-lib ;##################################################" ;
ls -la qucs/qucs-lib ;
echo "########################ls -la qucs-0.0.20/_build/qucs/qucs-lib ;###########################"
ls -la qucs-0.0.20/_build/qucs/qucs-lib ;
echo "----------------------------------------$2---------------------------" ;
