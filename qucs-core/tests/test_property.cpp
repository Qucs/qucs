/*
 * test_property.cpp - Miscellaneous unit tests for Qucs core library
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

TEST (property, constructor) {
  //std::cout << "can we print info"  << std::endl;
  property p;
  EXPECT_EQ(   0, strlen(p.getString()));
  EXPECT_EQ(   0, strlen(p.getReference()));
  EXPECT_EQ(   0, p.getInteger());
  EXPECT_EQ(   0, p.getDouble());
  EXPECT_EQ(NULL, p.getVector());
}
TEST (property, strng) {
  property p;   p.set("abc");
  EXPECT_STREQ("abc", p.getString());

  constant *cnst = new constant(TAG_STRING);   cnst->s = strdup("hij");
  variable *var  = new variable("var_01");     var->setConstant(cnst);
  p.set(var);
  EXPECT_STREQ("hij", p.getString());
}
TEST (property, dbl) {
  property p;   p.set(1.23);
  EXPECT_DOUBLE_EQ(1.23, p.getDouble());

  constant *cnst = new constant(TAG_DOUBLE);   cnst->d = 123.45;
  variable *var  = new variable("var_01");     var->setConstant(cnst);
  p.set(var);
  EXPECT_EQ(123.45, p.getDouble());
}

TEST (property, intgr) {
  property p0;   p0.set(123);
  EXPECT_EQ(123, p0.getInteger());

  property p1;   p1.set(123.45);
  EXPECT_EQ(123, p1.getInteger()); //floor

  constant *cnst = new constant(TAG_DOUBLE);   cnst->d = 89.0123;
  variable *var  = new variable("var_01");     var->setConstant(cnst);
  p0.set(var);
  EXPECT_EQ(89, p0.getInteger()); //floor
}

TEST (property, ref) {
  property p;     p.set("abc");
  EXPECT_STREQ("abc", p.getReference());
  p.set(new variable("efgh"));
  EXPECT_STREQ("efgh", p.getReference());
}

TEST (property, toString) {
  variable *var = new variable("var_01");
  EXPECT_TRUE(var->getType()==VAR_UNKNOWN);

  constant *cnst = new constant(TAG_CHAR);
  cnst->chr = 'a';
  var->setConstant(cnst);
  EXPECT_TRUE(var->getType()==VAR_CONSTANT);
  EXPECT_STREQ(var->getName(), "var_01");
  EXPECT_EQ(cnst, var->getConstant());

  property prop_ref;
  prop_ref.set(var);
  EXPECT_STREQ("var_01", prop_ref.toString().c_str());
  EXPECT_STREQ("var_01", prop_ref.getReference());
  //EXPECT_TRUE(prop->getVector()!=NULL); TODO getVector is buggy but never used

  property prop_int;
  prop_int.set(123);
  EXPECT_EQ   (123, prop_int.getInteger());
  EXPECT_STREQ("123.000000", prop_int.toString().c_str());

  property prop_dbl;
  prop_dbl.set(123.45);
  EXPECT_EQ   (123.45  , prop_dbl.getDouble());
  EXPECT_STREQ("123.450000", prop_dbl.toString().c_str());

  property prop_str;
  prop_str.set("abc");
  EXPECT_STREQ("abc", prop_str.getString());
  EXPECT_STREQ("abc", prop_str.toString().c_str());
}

TEST (properties, setGet) {
  property p_str;   p_str.set("abc");
  property p_int;   p_int.set(123);
  property p_dbl;   p_dbl.set(123.45);

  properties prps;
  prps.insert( {"ky01", p_str} );
  prps.insert( {"ky02", p_int} );
  prps["ky03"] = p_dbl;
  EXPECT_STREQ( "abc", prps["ky01"].getString()  );
  EXPECT_EQ   (   123, prps["ky02"].getInteger()  );
  EXPECT_EQ   (123.45, prps["ky03"].getDouble()  );
}
