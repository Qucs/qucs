/***************************************************************************
                          newprojdialog.cpp  -  description
                             -------------------
    begin                : Sun Aug 24 2003
    copyright            : (C) 2003 by Michael Margraf
                           (C) 2016 Qucs Team
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

#include "newprojdialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QGridLayout>


NewProjDialog::NewProjDialog(QWidget *parent)
  : QDialog(parent)
{
  setWindowTitle(tr("Create new project"));

  gbox = new QGridLayout(this);
  QLabel *Label1 = new QLabel(tr("Project name:"));
  gbox->addWidget(Label1,0,0);

  ProjName = new QLineEdit(this);
  ProjName->setMinimumWidth(250);
  connect(ProjName, SIGNAL(textChanged(const QString&)), SLOT(slotTextChanged(const QString&)));
  gbox->addWidget(ProjName, 0, 1, 1, 2);
  OpenProj = new QCheckBox(tr("open new project"));
  OpenProj->setChecked(true);
  gbox->addWidget(OpenProj, 1, 1, 1, 2);

  ButtonOk = new QPushButton(tr("Create"));
  gbox->addWidget(ButtonOk,2,1);
  ButtonOk->setEnabled(false);
  ButtonCancel = new QPushButton(tr("Cancel"));
  gbox->addWidget(ButtonCancel,2,2);

  connect(ButtonOk, SIGNAL(clicked()), SLOT(accept()));
  connect(ButtonCancel, SIGNAL(clicked()), SLOT(reject()));

  ButtonOk->setDefault(true);
  setFocusProxy(ProjName);
}

void NewProjDialog::slotTextChanged(const QString &text){
  /* avoid creating project with empty name */
  if (text.isEmpty()) {
    ButtonOk->setEnabled(false);
  } else {
    ButtonOk->setEnabled(true);
  }
}

NewProjDialog::~NewProjDialog()
{
  delete gbox;
}
