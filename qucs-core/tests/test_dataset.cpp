/*
 * test_dataset.cpp - Miscellaneous unit tests for Qucs core library
 *
 * Author Szymon Blachuta
 * Copyright (C) 2014, 2015, 2020 Qucs Team
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */
#include <iostream>
#include <stdio.h>

#if HAVE_CONFIG_H
# include <config.h>
#endif
#include "qucs_typedefs.h"
#include "object.h"
#include "strlist.h"
#include "vector.h"
#include "dataset.h"
#include "math/complex.h"

#include "testDefine.h"   // constants used on tests
#include "gtest/gtest.h"  // Google Test
using namespace qucs;

void doTest(const char *dtaSet, nr_complex_t *cVal=NULL); //proto
TEST (dataset, load_simple) {
  const char * qData =
  "<Qucs Dataset 0.0.20>\n"
  "<indep time 2\n>"
  "  +0.00000000000000000000e+00\n"
  "  +1.00000000000000000000e+00\n"
  "</indep>\n"
  "<dep Pr1.It time>\n"
  "  +0.12300000000000000000e+00\n"
  "  +1.33920000000000000000e-12\n"
  "</dep>\n";
  doTest(qData);
}
TEST (dataset, load_ctrlRctrlN) {
  const char * qData =
  "<Qucs Dataset 0.0.20>\r\n"
  "<indep time 2\n>"
  "  +0.00000000000000000000e+00\r\n"
  "  +1.00000000000000000000e+00\n"
  "</indep>\n"
  "<dep Pr1.It time>\n"
  "  +0.12300000000000000000e+00\n"
  "  +1.33920000000000000000e-12\r\n"
  "</dep>\n";
  doTest(qData);
}
TEST (dataset, load_comment) {
  const char * qData =
  "# comment line\n"
  "<Qucs Dataset 0.0.20>\n"
  "<indep time 2>\n"
  "    \n" //empty line
  "  +0.00000000000000000000e+00\n"
  "  +1.00000000000000000000e+00\n"
  "</indep>\n"
  "<dep Pr1.It time>\n"
  "  +0.12300000000000000000e+00\n"
  "  +1.33920000000000000000e-12\n"
  "</dep>\n";
  doTest(qData);
}
TEST (dataset, load_continuation) { // \ at end of line
  const char * qData =
  "<Qucs Dataset 0.0.20>\n"
  "<indep\ttime\t2> 0.0 1.0</indep>\n" // tabs
  "<dep Pr1.It time> 0.123 +1.3392e-12\\\n</dep>\n";
  doTest(qData);
}
TEST (dataset, load_complex) {
  const char * qData =
  "<Qucs Dataset 0.0.20>\n"
  "<indep\ttime\t2> 0.0 1.0</indep>\n" // tabs
  "<dep Pr1.It time> 0.123 +25e-12-j12e33\\\n</dep>\n";
  nr_complex_t cv(25e-12, -12e33);
  doTest(qData, &cv);
}
void doTest(const char *dtaSet, nr_complex_t * cVal){
  const char *FNAME = "test_dataset_tmp.dat";
  FILE *fo = fopen(FNAME, "wb");
  fputs(dtaSet, fo);
  fclose(fo);

  dataset dta;
  vector *dVect = new vector();
  dta.addDependency(dVect);
  vector *vect1 = new vector();
  dta.addVariable(vect1);
  dataset *result = dta.load(FNAME);

  nr_complex_t rt;
  EXPECT_EQ(1, result->countDependencies());
  EXPECT_EQ(1, result->countVariables());

  EXPECT_STREQ("time"  , result->getDependencies()->getName());
  rt = result->getDependencies()->get(0);
  EXPECT_EQ(0.0     , rt.real());
  rt = result->getDependencies()->get(1);
  EXPECT_EQ(1.0     , rt.real());

  EXPECT_STREQ("Pr1.It", result->getVariables()->getName());
  rt = result->getVariables()->get(0);
  EXPECT_EQ(0.123     , rt.real());
  rt = result->getVariables()->get(1);
  if (cVal==NULL)
    EXPECT_EQ(1.3392e-12, rt.real());
  else{
    EXPECT_EQ(cVal->real(), rt.real());
    EXPECT_EQ(cVal->imag(), rt.imag());
  }
  remove(FNAME);
}
// see scan_dataset.lpp
//  WS       [ \t\n\r]
//  IDENT1   [a-zA-Z_][a-zA-Z0-9_]*
//  IDENT2   [a-zA-Z_][a-zA-Z0-9_\[\],]*
//  IDENT3   [a-zA-Z0-9_][a-zA-Z0-9_]*
//  IDENT    {IDENT1}|{IDENT2}
//  PIDENT   {IDENT1}|{IDENT2}|{IDENT3}
//  SIMPLEID {IDENT}
//  POSTID   "."{PIDENT}
//  ID       {SIMPLEID}{POSTID}*
//  DIGIT    [0-9]
//  EXPONENT [Ee][+-]?{DIGIT}+                                # e-21
//  RINT     [+-]?{DIGIT}+                                    # -73
//  IINT     [+-]?[ij]{1}{DIGIT}+                             # +i36
//  RFLOAT1  [+-]?{DIGIT}+{EXPONENT}                          # +25e-12
//  RFLOAT2  [+-]?{DIGIT}*"."{DIGIT}+({EXPONENT})?            #
//  IFLOAT1  [+-]?[ij]{1}{DIGIT}+{EXPONENT}                   # -j12e33
//  IFLOAT2  [+-]?[ij]{1}{DIGIT}*"."{DIGIT}+({EXPONENT})?     #
//  CREAL    ({RFLOAT1}|{RFLOAT2}|{RINT})                     # +25e-12
//  CIMAG    ({IFLOAT1}|{IFLOAT2}|{IINT})                     # -j12e12
//  COMPLEX  {CREAL}{CIMAG}                                   # +25e-12-j12e33
//  SPACE    [ \t]
//  VERSION  "<Qucs Dataset "{DIGIT}+"."{DIGIT}+"."{DIGIT}+">"
//  DBEGIN   "dep"
//  IBEGIN   "indep"
//  DEND     "/dep"
//  IEND     "/indep"
//
//  <INITIAL>{VERSION}        { return Version;  }
//  <DESCRIPTION>{DBEGIN}     { return DepBegin;  }
//  <DESCRIPTION>{IBEGIN}     { return IndepBegin; }
//  <DESCRIPTION>{DEND}       { return DepEnd; }
//  <DESCRIPTION>{IEND}       { return IndepEnd; }
//  <INITIAL,DESCRIPTION>{ID} { return Identifier;  }
//  <INITIAL>{CREAL}          { return REAL; }
//  <INITIAL>{CIMAG}          { return IMAG; }
//  <INITIAL>{COMPLEX}        { return COMPLEX; }
//  <DESCRIPTION>{RINT}       { return Integer;  }
//  <INITIAL>"<"              { return '<'; }
//  <DESCRIPTION>">"          { return '>'; }
//  <INITIAL>\r?\n            { return Eol; }
//  <*>{SPACE}|\\\r?\n        /* skip spaces and the trailing '\' */
//  <INITIAL>"#"              { BEGIN(COMMENT); }
//  <INITIAL,DESCRIPTION>.    { return InvalidCharacter; }
//  <COMMENT>.                { /* skip any character in here */ }
//  <COMMENT>\r?\n            { BEGIN(INITIAL); /* skipping ends here */ }
