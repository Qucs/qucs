/*
 * test_strlist.cpp - Miscellaneous unit tests for Qucs core library
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

#include "strlist.h"

#include "testDefine.h"   // constants used on tests
#include "gtest/gtest.h"  // Google Test
using namespace qucs;

TEST (strlist, constructor) {
  //std::cout << "can we print info"  << std::endl;
  strlist sl;
  EXPECT_EQ(   0, sl.length());
}

TEST (strlist, add) {
  strlist sl;
  EXPECT_STREQ("", sl.toString());
  sl.add("abc");
  EXPECT_EQ(   1, sl.length());
  EXPECT_STREQ("abc", sl.toString());

  sl.add("def");
  EXPECT_EQ(   2, sl.length());
  EXPECT_STREQ("def abc", sl.toString());
}

TEST (strlist, contains) {
  strlist sl;
  sl.add("abc");
  EXPECT_EQ(   1, sl.length());
  EXPECT_STREQ("abc", sl.toString());

  sl.add("def");
  EXPECT_EQ(   2, sl.length());
  EXPECT_STREQ("def abc", sl.toString());

  EXPECT_EQ(   1, sl.contains("def"));
  EXPECT_EQ(   1, sl.contains("abc"));
  EXPECT_EQ(   0, sl.contains("x"));
}

TEST (strlist, index) {
  strlist sl;
  // strdup because of Warning: deprecated conversion from string constant to ‘char*’
  EXPECT_EQ(  -1, sl.index(strdup("def")));

  sl.add("abc");
  EXPECT_EQ(   1, sl.length());
  EXPECT_STREQ("abc", sl.toString());

  sl.add("def");
  EXPECT_EQ(   2, sl.length());
  EXPECT_STREQ("def abc", sl.toString());

  EXPECT_EQ(   0, sl.index(strdup("def")));
  EXPECT_EQ(   1, sl.index(strdup("abc")));
  EXPECT_EQ(  -1, sl.index(strdup("x")));
}

TEST (strlist, firstLast) {
  strlist sl;
  sl.add("abc");
  EXPECT_EQ(   1, sl.length());
  EXPECT_STREQ("abc", sl.toString());

  sl.add("def");
  EXPECT_EQ(   2, sl.length());
  EXPECT_STREQ("def abc", sl.toString());

  EXPECT_STREQ("def", sl.first());
  EXPECT_STREQ("abc", sl.last());
}

TEST (strlist, del) {
  strlist sl1, sl2;
  sl1.append("abc");
  sl1.append("def");
  sl1.append("ghi");

  sl2.add("def");
  sl1.del(&sl2);
  EXPECT_EQ(   2, sl1.length());
  EXPECT_STREQ("abc ghi", sl1.toString());
}

TEST (strlist, append) {
  strlist sl1, sl2;
  sl1.add("abc");
  sl1.add("def");
  sl1.append("ghi");
  EXPECT_STREQ("def abc ghi", sl1.toString());

  sl2.add("jkl");
  sl1.append(&sl2);
  EXPECT_STREQ("def abc ghi jkl", sl1.toString());
}

TEST (strlist, join) {
  strlist sl1, sl2, sl3;
  sl1.add("abc");
  sl1.append("def");

  sl2.add("jkl");

  strlist *ret = sl3.join(&sl1, &sl2);
  EXPECT_STREQ("abc def"    , sl1.toString());
  EXPECT_STREQ("jkl"        , sl2.toString());
  EXPECT_STREQ("abc def jkl", ret->toString());
}
TEST (strlistiterator, test) {
  strlist sl;
  sl.append("abc");
  sl.append("def");
  sl.append("ghi");
  strlistiterator it (sl);
  EXPECT_EQ(3, it.count());
  EXPECT_STREQ("abc", *it); ++it;
  EXPECT_STREQ("def", *it); ++it;
  EXPECT_STREQ("ghi", *it); ++it;
  EXPECT_EQ(NULL, *it);
  EXPECT_STREQ("abc", it.toFirst()); ++it;
  EXPECT_STREQ("def", it.current());
  EXPECT_STREQ("ghi", it.toLast());
  EXPECT_STREQ("abc", it.first());
  EXPECT_STREQ("ghi", it.last());
}
