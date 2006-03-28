/***************************************************************************
                              settingsdialog.h
                             ------------------
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

class Schematic;
class QLineEdit;
class QCheckBox;
class QVBoxLayout;
class QRegExpValidator;


class SettingsDialog : public QDialog  {
   Q_OBJECT
public:
  SettingsDialog(Schematic*);
 ~SettingsDialog();

private slots:
  void slotOK();
  void slotApply();

public:
  Schematic *Doc;

  QLineEdit *Input_DataSet, *Input_DataDisplay;
  QLineEdit *Input_GridX, *Input_GridY;
  QCheckBox *Check_OpenDpl, *Check_GridOn;

  QVBoxLayout *all;
  QRegExpValidator *valExpr;
};

#endif
