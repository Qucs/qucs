/***************************************************************************
                          qucshelp.h  -  description
                             -------------------
    begin                : Sun Jan 11 2004
    copyright            : (C) 2004 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCSHELP_H
#define QUCSHELP_H

#include <qdialog.h>
#include <qtextbrowser.h>
#include <qdir.h>
#include <qfont.h>
#include <qstring.h>

struct tQucsSettings {
  int x, y, dx, dy;    // position and size of main window
  QFont font;
  QString BitmapDir;
  QString LangDir;
  QString DocDir;
};

extern tQucsSettings QucsSettings;
extern QDir QucsHelpDir;

/**
  *@author Michael Margraf
  */

class QucsHelp : public QDialog  {
   Q_OBJECT
public:
  QucsHelp(const QString& page);
 ~QucsHelp();

private slots:
  void slotGotoIndex();
  void slotClose();

private:
  void closeEvent(QCloseEvent*);

  QTextBrowser *text;
};

#endif
