/***************************************************************************
                          settingsdialog.cpp  -  description
                             -------------------
    begin                : Mon Oct 20 2003
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

#include "settingsdialog.h"

#include <qwidget.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qlayout.h>
#include <qvalidator.h>


SettingsDialog::SettingsDialog(QucsDoc *d, QWidget *parent, const char *name)
                         : QDialog(parent, name, TRUE, Qt::WDestructiveClose)
{
  Doc = d;
  setCaption(tr("Edit File Properties"));

  QVBoxLayout *all = new QVBoxLayout(this); // to provide the neccessary size
  QTabWidget *t = new QTabWidget(this);
  all->addWidget(t);

  // ...........................................................
  QWidget *Tab1 = new QWidget(t);
  QGridLayout *gp = new QGridLayout(Tab1,3,2,5,5);

  QLabel *l1 = new QLabel(tr("Data Display:"), Tab1);
  gp->addWidget(l1,0,0);
  Input_DataDisplay = new QLineEdit(Tab1);
  gp->addWidget(Input_DataDisplay,0,1);

  QLabel *l2 = new QLabel(tr("Data Set:"), Tab1);
  gp->addWidget(l2,1,0);
  Input_DataSet = new QLineEdit(Tab1);
  gp->addWidget(Input_DataSet,1,1);

  Check_OpenDpl = new QCheckBox(tr("open data display after simulation"),Tab1);
  gp->addMultiCellWidget(Check_OpenDpl,2,2,0,1);

  t->addTab(Tab1, tr("Simulation"));

  // ...........................................................
  QWidget *Tab2 = new QWidget(t);
  QGridLayout *gp2 = new QGridLayout(Tab2,3,2,5,5);
  Check_GridOn = new QCheckBox(tr("show Grid"),Tab2);

  Expr.setPattern("[0-9]{1,3}");  // valid expression for property input
  QValidator *Validator = new QRegExpValidator(Expr, this);

  QLabel *l3 = new QLabel(tr("horizontal Grid:"), Tab2);
  gp2->addWidget(l3,1,0);
  Input_GridX = new QLineEdit(Tab2);
  Input_GridX->setValidator(Validator);
//  Input_GridX->setInputMask("000");   // for Qt 3.2
  gp2->addWidget(Input_GridX,1,1);

  QLabel *l4 = new QLabel(tr("vertical Grid:"), Tab2);
  gp2->addWidget(l4,2,0);
  Input_GridY = new QLineEdit(Tab2);
  Input_GridY->setValidator(Validator);
//  Input_GridY->setInputMask("000");   // for Qt 3.2
  gp2->addWidget(Input_GridY,2,1);

  t->addTab(Tab2, tr("Grid"));

  // ...........................................................
  // buttons on the bottom of the dialog (independent of the TabWidget)
  QHBox *Butts = new QHBox(this);
  Butts->setSpacing(5);
  Butts->setMargin(5);
  all->addWidget(Butts);

  QPushButton *OkButt = new QPushButton(tr("OK"), Butts);
  connect(OkButt, SIGNAL(clicked()), SLOT(slotOK()));
  QPushButton *ApplyButt = new QPushButton(tr("Apply"), Butts);
  connect(ApplyButt, SIGNAL(clicked()), SLOT(slotApply()));
  QPushButton *CancelButt = new QPushButton(tr("Cancel"), Butts);
  connect(CancelButt, SIGNAL(clicked()), SLOT(reject()));

  OkButt->setDefault(true);

  // ...........................................................
  // fill the fields with the QucsDoc-Properties

  Input_DataSet->setText(Doc->DataSet);
  Input_DataDisplay->setText(Doc->DataDisplay);
  Check_OpenDpl->setChecked(Doc->SimOpenDpl);
  Check_GridOn->setChecked(Doc->GridOn);
  Input_GridX->setText(QString::number(Doc->GridX));
  Input_GridY->setText(QString::number(Doc->GridY));
}

SettingsDialog::~SettingsDialog()
{
}

// -----------------------------------------------------------
void SettingsDialog::slotOK()
{
  slotApply();
  reject();
}

// -----------------------------------------------------------
void SettingsDialog::slotApply()
{
  bool changed = false;
  
  if(Doc->DataSet != Input_DataSet->text()) {
    Doc->DataSet = Input_DataSet->text();
    changed = true;
  }

  if(Doc->DataDisplay != Input_DataDisplay->text()) {
    Doc->DataDisplay = Input_DataDisplay->text();
    changed = true;
  }

  if(Doc->SimOpenDpl != Check_OpenDpl->isChecked()) {
    Doc->SimOpenDpl = Check_OpenDpl->isChecked();
    changed = true;
  }

  if(Doc->GridOn != Check_GridOn->isChecked()) {
    Doc->GridOn = Check_GridOn->isChecked();
    changed = true;
  }

  if(Doc->GridX != Input_GridX->text()) {
    Doc->GridX = Input_GridX->text().toInt();
    changed = true;
  }

  if(Doc->GridY != Input_GridY->text()) {
    Doc->GridY = Input_GridY->text().toInt();
    changed = true;
  }

  if(changed) Doc->setChanged(true);
}
