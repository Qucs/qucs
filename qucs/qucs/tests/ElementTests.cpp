/*
 * ElementTests.cpp - Unit tests for the Element class
 *
 * Copyright (C) 2017, Guilherme Brondani Torri, <guitorri@gmail.com>
 *
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
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
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "element.h"

#include <QTest>

#include "ElementTests.h"
#include "../paintings/graphictext.h"

void ElementTests::testConstructor() {
    Element *e = new Element();
    // check defaults
    QCOMPARE(e->Type, isDummyElement);
    QCOMPARE(e->isSelected, false);
    QCOMPARE(e->cx, 0);
    QCOMPARE(e->cy, 0);
    QCOMPARE(e->x1, 0);
    QCOMPARE(e->y1, 0);
    QCOMPARE(e->x2, 0);
    QCOMPARE(e->y2, 0);
}

void ElementTests::test_GraphicText() {
  QSKIP("see https://bugreports.qt.io/browse/QTBUG-83156", SkipAll);
  GraphicText *c01 = new GraphicText();
  bool ret = c01->load(" 330 70 16 #000000 0 \"Peltz oscillator\"");
  QCOMPARE(ret, true); //<Text 330 70 16 #000000 0 "Peltz oscillator">
}

QTEST_MAIN(ElementTests)
