/***************************************************************************
                        id_dialog.cpp  -  description
                             -------------------
    begin                : Sat Oct 16 2004
    copyright            : (C) 2004 by Michael Margraf
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

#include "id_dialog.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qpushbutton.h>
#include <qvalidator.h>
#include <qlineedit.h>


ID_Dialog::ID_Dialog(QWidget *parent) : QDialog(parent)
{
  setCaption(tr("Edit Subcircuit Properties"));
  rx = new QRegExp("[A-Za-z][A-Za-z0-9_]{1,9}");
  Validator = new QRegExpValidator(*rx, this);

  v = new QVBoxLayout(this);
  v->setSpacing(5);
  v->setMargin(5);

  QHBox *h0 = new QHBox(this);
  h0->setSpacing(5);
  v->addWidget(h0);

  new QLabel(tr("Prefix:"), h0);
  Prefix = new QLineEdit(h0);
  Prefix->setValidator(Validator);
  Prefix->setText("SUB");


  QHBox *h3 = new QHBox(this);
  h3->setSpacing(5);
  v->addWidget(h3);

  QPushButton *ButtOK = new QPushButton(tr("OK"),h3);
  connect(ButtOK, SIGNAL(clicked()), SLOT(accept()));
  QPushButton *ButtCancel = new QPushButton(tr("Cancel"),h3);
  connect(ButtCancel, SIGNAL(clicked()), SLOT(reject()));
}

ID_Dialog::~ID_Dialog()
{
  delete v;
  delete rx;
  delete Validator;
}
