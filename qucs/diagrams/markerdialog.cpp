/***************************************************************************
                          markerdialog.cpp  -  description
                             -------------------
    begin                : Wed April 21 2004
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

#include "markerdialog.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>


MarkerDialog::MarkerDialog(Marker *pm_, QWidget *parent, const char *name)
                     : QDialog(parent, name, FALSE, Qt::WDestructiveClose)
{
  setCaption(tr("Edit Marker Properties"));
  pMarker = pm_;

  QGridLayout *g = new QGridLayout(this, 2, 2, 5, 5);

  g->addWidget(new QLabel(tr("Precision: "), this), 0, 0);
  Precision = new QLineEdit(this);
  Precision->setText(QString::number(pMarker->Precision));
  g->addWidget(Precision, 0, 1);

  // first => activated by pressing RETURN
  QPushButton *ButtOK = new QPushButton(tr("OK"),this);
  connect(ButtOK, SIGNAL(clicked()), SLOT(slotAcceptValues()));
  g->addWidget(ButtOK, 1, 0);

  QPushButton *ButtCancel = new QPushButton(tr("Cancel"),this);
  connect(ButtCancel, SIGNAL(clicked()), SLOT(reject()));
  g->addWidget(ButtCancel, 1, 1);
}

MarkerDialog::~MarkerDialog()
{
}

// ----------------------------------------------------------
void MarkerDialog::slotAcceptValues()
{
  bool changed = false;
  int tmp = Precision->text().toInt();
  if(tmp != pMarker->Precision) {
    pMarker->Precision = tmp;
    changed = true;
  }

  if(changed) {
    pMarker->createText();
    done(2);
  }
  else done(1);
}
