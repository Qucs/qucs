/*
 * test_equation.cpp - Miscellaneous unit tests for Qucs core library
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

#if HAVE_CONFIG_H
# include <config.h>
#endif
#include "math/complex.h"
#include "property.h"
#include "qucs_typedefs.h"
#include "object.h"
#include "vector.h"
#include "variable.h"
#include "equation.h"

#include "testDefine.h"   // constants used on tests
#include "gtest/gtest.h"  // Google Test
using namespace qucs;
using namespace eqn;

//---------------------node--------------------
TEST (equation, node) {
  //std::cout << "can we print info"  << std::endl;
  node nde;
  nde.setInstance("abc");
  EXPECT_STREQ(nde.getInstance(), "abc");
}

//---------------------constant--------------------
TEST (equation, constant) {
  constant cnst(TAG_STRING);
  cnst.s = strdup("abc");
  cnst.dataref = true; //do free const.s
  EXPECT_STREQ(cnst.toString(), "'abc'");
}
//---------------------reference--------------------
TEST (equation, reference){
  reference ref;
  ref.ref = new node(CONSTANT);
  ref.n = strdup("efg");
  EXPECT_STREQ(ref.toString(), "efg");
}
//---------------------assignment--------------------
TEST (equation, assignment){
  assignment assg;
  assg.body = new node();
  assg.body->txt = strdup("bdy");
  assg.body->setType(CONSTANT);
  assg.result = strdup("hij");
  EXPECT_STREQ(assg.toString(), "hij = bdy");
}
//---------------------application--------------------
TEST (equation, application){
  application app;
  constant *n1 = new constant(TAG_DOUBLE); n1->d = 1;
  constant *n2 = new constant(TAG_DOUBLE); n2->d = 2;
  n1->append(n2);
  app.n = strdup("+"); //"+";
  app.args = n1;
  app.nargs = 2;
  EXPECT_STREQ(app.toString(), "(1+2)");
}
//---------------------checker--------------------
TEST (equation, checker){
  constant *cnst = new constant(TAG_DOUBLE);
  cnst->d = 1.1;

  assignment *assg = new assignment();
  assg->body = cnst;
  assg->result = strdup("rslt");

  checker chkr;
  chkr.addEquation(assg);
  EXPECT_EQ(  0, chkr.check()); // 0 errors
}
//---------------------solver--------------------
TEST (equation, solver){
  constant *cnst = new constant(TAG_DOUBLE);
  cnst->d = 1.1;

  assignment *assg = new assignment();
  assg->body = cnst;
  assg->result = strdup("rslt");

  checker chkr;
  chkr.addEquation(assg);
  solver slvr(&chkr);
  slvr.evaluate();
}
