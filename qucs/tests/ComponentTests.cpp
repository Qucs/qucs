/*
 * ComponentTests.cpp - Unit tests for the Component class
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

#include "components/component.h"
#include "ComponentTests.h"

#include <QTest>
#include <QDebug>

class testComponent : public Component{
public:
	testComponent() : Component() {
	}
};

void ComponentTests::testConstructor() {
    Component *c = new testComponent();
    //QCOMPARE(c->isSelected(), false);
    QCOMPARE(c->cx_(), 0);
    QCOMPARE(c->cy_(), 0);
}

QTEST_MAIN(ComponentTests)
// int main(){
// 	ComponentTests t;
// 	t.testConstructor();
// }
