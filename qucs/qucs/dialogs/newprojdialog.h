/***************************************************************************
                          newprojdialog.h  -  description
                             -------------------
    begin                : Sun Aug 24 2003
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

#ifndef NEWPROJDIALOG_H
#define NEWPROJDIALOG_H

#include <qdialog.h>
#include <qcheckbox.h>
#include <qlineedit.h>

//class QLineEdit;
//class QCheckBox;
class QPushButton;

/**
  *@author Michael Margraf
  */

class NewProjDialog : public QDialog  {
public: 
	NewProjDialog(QWidget *parent=0, const char *name=0);
	~NewProjDialog();

  QLineEdit   *ProjName;
  QCheckBox   *OpenProj;

private:
  QPushButton *ButtonOk, *ButtonCancel;
};

#endif
