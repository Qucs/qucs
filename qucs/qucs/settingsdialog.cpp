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


SettingsDialog::SettingsDialog(QucsDoc *d, QWidget *parent, const char *name)
                                : QDialog(parent, name, Qt::WDestructiveClose)
{
  Doc = d;
  setCaption("Edit File Properties");

  QVBoxLayout *all = new QVBoxLayout(this); // to provide the neccessary size
  QTabWidget *t = new QTabWidget(this);
  all->addWidget(t);

  // ...........................................................
  QWidget *Tab1 = new QWidget(t);
  QGridLayout *gp = new QGridLayout(Tab1,3,2,5,5);

  QLabel *l1 = new QLabel("Data Display:", Tab1);
  gp->addWidget(l1,0,0);
  Input_DataDisplay = new QLineEdit(Tab1);
  gp->addWidget(Input_DataDisplay,0,1);

  QLabel *l2 = new QLabel("Data Set:", Tab1);
  gp->addWidget(l2,1,0);
  Input_DataSet = new QLineEdit(Tab1);
  gp->addWidget(Input_DataSet,1,1);

  Check_OpenDpl = new QCheckBox("open data display after simulation",Tab1);
  gp->addMultiCellWidget(Check_OpenDpl,2,2,0,1);
  
  t->addTab(Tab1, "Simulation");

  // ...........................................................
  QWidget *Tab2 = new QWidget(t);
//  QGridLayout *gp = new QGridLayout(Tab2,6,3,5,5);

  t->addTab(Tab2, "Grid");

  // ...........................................................
  // buttons on the bottom of the dialog (independent of the TabWidget)
  QHBox *Butts = new QHBox(this);
  Butts->setSpacing(5);
  Butts->setMargin(5);
  all->addWidget(Butts);

  QPushButton *OkButt = new QPushButton("OK", Butts);
  connect(OkButt, SIGNAL(clicked()), SLOT(slotOK()));
  QPushButton *ApplyButt = new QPushButton("Apply", Butts);
  connect(ApplyButt, SIGNAL(clicked()), SLOT(slotApply()));
  QPushButton *CancelButt = new QPushButton("Cancel", Butts);
  connect(CancelButt, SIGNAL(clicked()), SLOT(reject()));

  OkButt->setDefault(true);

  // ...........................................................
  // fill the fields with the QucsDoc-Properties

  Input_DataSet->setText(Doc->DataSet);
  Input_DataDisplay->setText(Doc->DataDisplay);
  Check_OpenDpl->setChecked(Doc->SimOpenDpl);
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

  if(changed) Doc->setChanged(true);
}
