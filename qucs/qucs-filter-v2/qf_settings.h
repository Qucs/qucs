/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

struct tQucsSettings {
  int x, y;      // position of main window
  QFont font;
  QString LangDir;
  QString BitmapDir;
  QString Language;
};

extern struct tQucsSettings QucsSettings;

bool loadSettings();
bool saveApplSettings(qf_box *);

