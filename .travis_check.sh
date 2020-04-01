#!/bin/bash
OS=$1 ;
echo "----------------------------------------$2---------------------------" ;
find . -name "checklibraries.*" ;
find . -name "displaydialog.*" ;
find . -type f -exec grep -H "displaydialog" {} \; ;
echo "####################" ;
ls -la qucs/qucs-lib ;
ls -la _build/qucs/qucs-lib ;
echo "----------------------------------------$2---------------------------" ;
