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
  m_Settings = new QSettings("qucs", "qucs");
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
