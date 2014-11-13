/***************************************************************************
                                 settings.h
                                ------------
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
 * \file setting.h
 * \provide a singleton interface to aceess setting
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_ value

#include <QSettings>
#include <QVariant>

class Settings {
public:
  static Settings *instance();
  static Settings *init();
  void load();

  QVariant get(const QString &section, const QString &key);
  bool set(const QString &section, const QString &key, const QVariant &value);
  bool testSet(const QString &section, const QString &key, const QVariant &value);

  void setDefault();
private:
  Settings();

  static Settings *m_Instance;
  QSettings *m_Settings;
};


#define SETTINGS (Settings::instance())

#endif
