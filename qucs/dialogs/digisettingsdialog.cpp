/***************************************************************************
                           digisettingsdialog.cpp
                          ------------------------
    begin                : Sat Apr 01 2006
    copyright            : (C) 2006 by Michael Margraf
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

#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qvgroupbox.h>
#include <qstring.h>
#include <qstringlist.h>

#include "digisettingsdialog.h"
#include "textdoc.h"
#include "main.h"


DigiSettingsDialog::DigiSettingsDialog(TextDoc *Doc_)
                  : QDialog(Doc_, 0, true, Qt::WDestructiveClose)
{
  Doc = Doc_;
  setCaption(tr("Document Settings"));

  Expr.setPattern("[0-9][0-9a-zA-Z ]+"); // valid expression for LineEdit
  Validator = new QRegExpValidator(Expr, this);

  QVBoxLayout *all = new QVBoxLayout(this);
  all->setMargin(5);

  QVGroupBox *setGroup = new QVGroupBox(tr("Digital Simulation Settings"), this);
  all->addWidget(setGroup);

  QButtonGroup *toggleGroup = new QButtonGroup();
  simRadio = new QRadioButton(tr("Simulation"), setGroup);
  simRadio->setChecked(Doc->simulation);

  QHBox *hb1 = new QHBox(setGroup);
  hb1->setSpacing(5);
  TimeLabel = new QLabel(tr("Duration of Simulation:"), hb1);
  Doc->loadSimulationTime(SimTime);
  TimeEdit = new QLineEdit(hb1);
  TimeEdit->setValidator(Validator);
  TimeEdit->setText(SimTime);

  QRadioButton *comRadio = new QRadioButton(tr("Precompile Module"), setGroup);
  toggleGroup->insert(simRadio);
  toggleGroup->insert(comRadio);
  connect(toggleGroup, SIGNAL(clicked(int)), SLOT(slotChangeMode(int)));

  QHBox *hb3 = new QHBox(setGroup);
  hb3->setSpacing(5);
  NameLabel = new QLabel(tr("Library Name:"), hb3);
  NameEdit = new QLineEdit(hb3);
  NameEdit->setText(Doc->Library);

  setGroup->addSpace(15);
  QHBox *hb2 = new QHBox(setGroup);
  hb2->setSpacing(5);
  LibLabel = new QLabel(tr("Libraries:"), hb2);
  LibEdit = new QLineEdit(hb2);
  LibEdit->setText(Doc->Libraries);

  all->addSpacing(5);
  all->addStretch();
  QHBox *Buttons = new QHBox(this);
  all->addWidget(Buttons);
  QPushButton *ButtonOk = new QPushButton(tr("Ok"), Buttons);
  QPushButton *ButtonCancel = new QPushButton(tr("Cancel"), Buttons);
  connect(ButtonOk, SIGNAL(clicked()), SLOT(slotOk()));
  connect(ButtonCancel, SIGNAL(clicked()), SLOT(reject()));

  simRadio->setChecked(Doc->simulation);
  comRadio->setChecked(!Doc->simulation);
  slotChangeMode(!Doc->simulation);

  ButtonOk->setDefault(true);
  setFocusProxy(TimeEdit);
  TimeEdit->setFocus();
}

DigiSettingsDialog::~DigiSettingsDialog()
{
  delete Validator;
}

void DigiSettingsDialog::slotOk()
{
  bool changed = false;
  if(SimTime != TimeEdit->text()) {
    QString s = TimeEdit->text();
    if(!VHDL_Time(s, tr("Document Settings"))) {
      QMessageBox::critical(this, tr("Error"), s.mid(1));
      reject();
      return;
    } else {
      Doc->SimTime = s;
      changed = true;
    }
  }
  if(Doc->Libraries != LibEdit->text()) {
    QStringList lst = QStringList::split(' ',LibEdit->text());
    Doc->Libraries = lst.join(" ");
    changed = true;
  }
  if(Doc->simulation != simRadio->isChecked()) {
    Doc->simulation = simRadio->isChecked();
    changed = true;
  }
  if(Doc->Libraries != NameEdit->text()) {
    QString lib = NameEdit->text().stripWhiteSpace();
    Doc->Library = lib;
    changed = true;
  }

  if(changed) {
    Doc->SetChanged = true;
    Doc->slotSetChanged();
  }
  accept();
}

void DigiSettingsDialog::slotChangeMode(int idx)
{
  switch(idx) {
  case 0:
    TimeEdit->setEnabled(true);
    TimeEdit->setFocus();
    TimeLabel->setEnabled(true);
    NameEdit->setEnabled(false);
    NameLabel->setEnabled(false);
    break;
  case 1:
    TimeEdit->setEnabled(false);
    TimeLabel->setEnabled(false);
    NameEdit->setEnabled(true);
    NameLabel->setEnabled(true);
    break;
  }
}
