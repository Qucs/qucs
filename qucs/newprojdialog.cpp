/***************************************************************************
                          newprojdialog.cpp  -  description
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

#include "newprojdialog.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>

NewProjDialog::NewProjDialog(QWidget *parent, const char *name)
                             : QDialog(parent, name, true)
{
  setCaption("Create new project");

  QGridLayout *gbox = new QGridLayout(this,3,3,5,5);
  QLabel *Label1 = new QLabel("Project name:",this);
  gbox->addWidget(Label1,0,0);

  ProjName = new QLineEdit(this);
  ProjName->setMinimumWidth(250);
  gbox->addMultiCellWidget(ProjName,0,0,1,2);
  OpenProj = new QCheckBox("open new project",this);
  OpenProj->setChecked(true);
  gbox->addMultiCellWidget(OpenProj,1,1,1,2);
  
  ButtonOk = new QPushButton("Create",this);
  gbox->addWidget(ButtonOk,2,1);
  ButtonCancel = new QPushButton("Cancel",this);
  gbox->addWidget(ButtonCancel,2,2);

  connect(ButtonOk, SIGNAL(clicked()), SLOT(accept()));
  connect(ButtonCancel, SIGNAL(clicked()), SLOT(reject()));

  ButtonOk->setDefault(true);
  setFocusProxy(ProjName);
}

NewProjDialog::~NewProjDialog()
{
}
