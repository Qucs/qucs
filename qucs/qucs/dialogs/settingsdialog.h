/***************************************************************************
                          settingsdialog.h  -  description
                             -------------------
    begin                : Mon Oct 20 2003
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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <qdialog.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qregexp.h>

class QucsDoc;

/**
  *@author Michael Margraf
  */

class SettingsDialog : public QDialog  {
   Q_OBJECT
public:
	SettingsDialog(QucsDoc *d, QWidget *parent=0, const char *name=0);
	~SettingsDialog();

private slots:
  void slotOK();
  void slotApply();

public:
  QucsDoc   *Doc;

  QLineEdit *Input_DataSet, *Input_DataDisplay;
  QLineEdit *Input_GridX, *Input_GridY;
  QCheckBox *Check_OpenDpl, *Check_GridOn;
};

#endif
