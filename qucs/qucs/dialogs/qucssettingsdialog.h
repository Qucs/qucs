/***************************************************************************
                      qucssettingsdialog.h  -  description
                             -------------------
    begin                : Sun May 23 2004
    copyright            : (C) 2003 by Michael Margraf
    email                : margraf@mwt.ee.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCSSETTINGSDIALOG_H
#define QUCSSETTINGSDIALOG_H

#include "qucs.h"

#include <qdialog.h>
#include <qpushbutton.h>
#include <qfont.h>

/**
  *@author Michael Margraf
  */

class QucsSettingsDialog : public QDialog  {
   Q_OBJECT
public:
  QucsSettingsDialog(QucsApp *parent=0, const char *name=0);
  ~QucsSettingsDialog();

private slots:
  void slotOK();
  void slotApply();
  void slotFontDialog();
  void slotBGColorDialog();
  void slotDefaultValues();

public:
  QucsApp *App;

  QFont    Font;
  QPushButton *FontButton, *BGColorButton;
};

#endif
