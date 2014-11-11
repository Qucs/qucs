/***************************************************************************
                                 settings.cpp
                                --------------
    begin                : Mon Nov 10 2014
    copyright            : (C) 2014 by yodalee
    email                : lc85301@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/*!
 * \file setting.cpp
 * \provide a singleton interface to aceess setting
 */

#include "setting.h"

#include <QFont>
#include <QColor>

Settings *Settings::m_Instance = NULL;

Settings *
Settings::instance()
{
  return m_Instance;
}

Settings *
Settings::init()
{
  m_Instance = new Settings();
  return m_Instance;
}

void
Settings::load()
{
  m_Settings = new QSettings("qucs", "qucs-new");
}

Settings::Settings()
{
}

QVariant
Settings::get(const QString &section, const QString &key)
{
  m_Settings->beginGroup(section);
  QVariant value = m_Settings->value(key);
  m_Settings->endGroup();
  return value;
}

bool
Settings::set(const QString &section, const QString &key, const QVariant &value)
{
  bool rtn = false;
  m_Settings->beginGroup(section);
  if (m_Settings->value(key) != value) {
    m_Settings->setValue(key, value);
    rtn = true;
  }
  m_Settings->endGroup();
  m_Settings->sync();
  return rtn;
}

bool
Settings::testSet(const QString &section, const QString &key, const QVariant &value)
{
  m_Settings->beginGroup(section);
  if (!m_Settings->contains(key)) {
    m_Settings->endGroup();
    return set(section, key, value);
  }
  m_Settings->endGroup();
  return false;
}

// function to apply default settings
void
Settings::setDefault()
{
  //general
  testSet("general", "font", QFont("Helvetica", 12));
  testSet("general", "largeFontSize", 16.0);
  testSet("general", "maxUndo", 20);
  testSet("general", "Editor", "qucs");

  // color
  testSet("color", "BGColor",   QColor(255, 250, 225));
  testSet("color", "Comment",   Qt::gray);
  testSet("color", "String",    Qt::red);
  testSet("color", "Integer",   Qt::blue);
  testSet("color", "Real",      Qt::darkMagenta);
  testSet("color", "Character", Qt::magenta);
  testSet("color", "Type",      Qt::darkRed);
  testSet("color", "Attribute", Qt::darkCyan);
  testSet("color", "Directive", Qt::darkCyan);
  testSet("color", "Task",      Qt::darkRed);

  //bool
  testSet("bool", "NodeWiring", false);
  testSet("bool", "IgnoreFutureVersion", false);
  testSet("bool", "DrawInAntiAliasing" , false);
}
