/*
 * test_vector.cpp - Miscellaneous unit tests for Qucs core library
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

#include "config.h"
#include "precision.h"
#include "qucs_typedefs.h"
#include "object.h"
#include "vector.h"
#include "math/complex.h"

//#include "testDefine.h"   // constants used on tests
#include "gtest/gtest.h"  // Google Test
using namespace qucs;

TEST (vector, constructor) {
  //std::cout << "can we print info"  << std::endl;
  vector vect;
  EXPECT_EQ(   0, vect.getSize());
  vector vt2(2);
  EXPECT_EQ(   0, vect.getSize());
}

TEST (vector, add) {
  nr_complex_t val(1.0);
  vector vect(1);
  vect.add(val);
  EXPECT_EQ(   2, vect.getSize());
}

TEST (vector, get) {
  nr_complex_t val0(0.0);
  nr_complex_t val1(1.0);
  vector vect(1);
  vect.add(val1);
  EXPECT_EQ(   2, vect.getSize());
  EXPECT_EQ(vect.get(0).real(), val0.real());
  EXPECT_EQ(vect.get(1).real(), val1.real());
}

TEST (vector, math) {
  nr_complex_t val(1.0);
  vector vect;
  vect.add(val);
  vect += 2.0; EXPECT_EQ(   3.0, vect.get(0).real());
  vect -= 1.0; EXPECT_EQ(   2.0, vect.get(0).real());
  vect /= 2.0; EXPECT_EQ(   1.0, vect.get(0).real());
  vect *= 3.8; EXPECT_EQ(   3.8, vect.get(0).real());
  vect.variance();
}

TEST (vector, variance) {
  vector vect;
  nr_complex_t val_1(1.0);
  nr_complex_t val_2(2.0);
  nr_complex_t val_3(3.0);
  vect.add(val_1);
  EXPECT_EQ(   0, vect.variance());
  vect.add(val_2);
  vect.add(val_3);
  EXPECT_EQ( 1.0, vect.variance());
}

TEST (vector, property) {
  vector vect;
  vect.addProperty("key_a", "A");
  vect.addProperty("key_b", "B");
  EXPECT_TRUE(                           vect.hasProperty("key_a"));
  EXPECT_STREQ("B"                     , vect.getPropertyString("key_b"));
  EXPECT_STREQ("key_b=\"B\"key_a=\"A\"", vect.propertyList());
}
// TODO more test for better coverage
