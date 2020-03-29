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

#include "qucs.h"
#include "components/ac_sim.h"
#include "components/equation.h"
#include "components/component.h"
#include "ComponentTests.h"

#include <QTest>
#include <QFont>

void ComponentTests::testConstructor() {
    Component c;
    QCOMPARE(c.Type, isAnalogComponent);
    QCOMPARE(c.isSelected, false);
    QCOMPARE(c.cx, 0);
    QCOMPARE(c.cy, 0);

    GateComponent g;
    QCOMPARE(g.Type, isComponent);
    QCOMPARE(g.isSelected, false);
    QCOMPARE(g.cx, 0);
    QCOMPARE(g.cy, 0);
}

void ComponentTests::test_AC_Sim(){
  AC_Sim ac_sim; //*ac_sim = new AC_Sim();
  QCOMPARE(ac_sim.name(), QString("AC"));
  QCOMPARE(ac_sim.description(), tr("ac simulation")); //i18n ?
  QCOMPARE(ac_sim.Texts.size(), 1);
  QCOMPARE(ac_sim.Texts.at(0)->s.length() > 0, true);
  QCOMPARE(ac_sim.Texts.at(0)->s, QString(tr("ac simulation"))); //i18n ?
  QCOMPARE(ac_sim.Props .count(), (uint)5);
  QCOMPARE(ac_sim.Lines .count(), 0);
  QCOMPARE(ac_sim.Arcs  .count(), 0);
  QCOMPARE(ac_sim.Rects .count(), 0);
  QCOMPARE(ac_sim.Ellips.count(), 0);
  QCOMPARE(ac_sim.Ports .count(), 0);
}

void ComponentTests::test_Equation(){
  QSKIP("see https://bugreports.qt.io/browse/QTBUG-83156", SkipAll);
  Equation eqn;
  QCOMPARE(eqn.name(), QString("AC"));
  QCOMPARE(eqn.description(), tr("equation")); //i18n ?
  QCOMPARE(eqn.Props .count(), (uint)2);
}



QTEST_MAIN(ComponentTests)
