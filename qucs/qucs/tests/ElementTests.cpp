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
class testElement : public Element{
public:
	testElement() : Element() {
		ElemSelected = false; // BUG
	}
};

void ElementTests::testConstructor() {
	 Element *e = new testElement();
    // check defaults
//    QCOMPARE(int( e->elemType() ), isDummyElement);
    QCOMPARE(e->isSelected(), false);
    QCOMPARE(e->cx, 0);
    QCOMPARE(e->cy, 0);
    QCOMPARE(e->x1, 0);
    QCOMPARE(e->y1, 0);
    QCOMPARE(e->x2, 0);
    QCOMPARE(e->y2, 0);
}

QTEST_MAIN(ElementTests)
