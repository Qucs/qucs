/***************************************************************************
                          qucsedit.h  -  description
                             -------------------
    begin                : Mon Nov 17 2003
    copyright            : (C) 2003 by Michael Margraf
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

#ifndef QUCSEDIT_H
#define QUCSEDIT_H

#include <qdialog.h>
#include <qtextedit.h>

/**
  *@author Michael Margraf
  */

class QucsEdit : public QDialog  {
   Q_OBJECT
public:
  QucsEdit(const QString&, bool readOnly=false);
 ~QucsEdit();

private slots:
  void slotAbout();
  void slotLoad();
  void slotSave();
  void slotQuit();

private:
  void closeEvent(QCloseEvent*);
  bool loadFile(const QString&);
  bool closeFile();

  QString FileName;
  QTextEdit *text;
};

#endif
