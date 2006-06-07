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

  QLabel *Label1 = new QLabel(tr("Duration of Simulation:"), this);
  all->addWidget(Label1);

  Doc->loadSimulationTime(SimTime);
  TimeEdit = new QLineEdit(this);
  TimeEdit->setValidator(Validator);
  TimeEdit->setText(SimTime);
  all->addWidget(TimeEdit);
/*
  QLabel *Label2 = new QLabel(
          tr("Note: This value is stored in the datadisplay."), all);
*/
  QHBox *Buttons = new QHBox(this);
  all->addWidget(Buttons);
  QPushButton *ButtonOk = new QPushButton(tr("Ok"), Buttons);
  QPushButton *ButtonCancel = new QPushButton(tr("Cancel"), Buttons);
  connect(ButtonOk, SIGNAL(clicked()), SLOT(slotOk()));
  connect(ButtonCancel, SIGNAL(clicked()), SLOT(reject()));

  ButtonOk->setDefault(true);
  setFocusProxy(TimeEdit);
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

  if(changed)
    Doc->slotSetChanged();
  accept();
}
