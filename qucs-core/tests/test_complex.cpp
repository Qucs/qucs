/*
 * test_complex.cpp - Miscellaneous unit tests for Qucs core library
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
#include "math/complex.h"

#include "testDefine.h"   // constants used on tests
#include "gtest/gtest.h"  // Google Test
using namespace qucs;

TEST (complex, constructor) {
  //std::cout << "can we print info"  << std::endl;
  nr_complex_t val(1.0);
  EXPECT_EQ(  1.0, val.real());
}
TEST (complex, math) {
  nr_complex_t val(1.0);
  val += 2.0; EXPECT_EQ(  3.0, val.real());
  val -= 1.0; EXPECT_EQ(  2.0, val.real());
  val *= 4.5; EXPECT_EQ(  9.0, val.real());
  val /= 2.0; EXPECT_EQ(  4.5, val.real());

  EXPECT_EQ  (   0.0, qucs::acos(1.0));
  EXPECT_NEAR(1.0472, qucs::acos(0.5), 0.0001);

  EXPECT_EQ  (   0.0, qucs::asin(0.0));
  EXPECT_NEAR(1.5708  , qucs::asin(1.0), 0.0001);
  EXPECT_NEAR(0.523599, qucs::asin(0.5), 0.0001);
}
