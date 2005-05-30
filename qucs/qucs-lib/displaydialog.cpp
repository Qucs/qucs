/***************************************************************************
                               displaydialog.cpp
                              -------------------
    begin                : Sat May 28 2005
    copyright            : (C) 2005 by Michael Margraf
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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "displaydialog.h"

#include <qlayout.h>
#include <qhbox.h>
#include <qpushbutton.h>
#include <qtextedit.h>


DisplayDialog::DisplayDialog(QWidget *parent)
                     : QDialog(parent, 0, false, Qt::WDestructiveClose)
{
  vLayout = new QVBoxLayout(this);

  Text = new QTextEdit(this);
  Text->setTextFormat(Qt::PlainText);
  Text->setReadOnly(true);
  Text->setMinimumSize(200, 100);
  vLayout->addWidget(Text);

  QHBox *h = new QHBox(this);
  vLayout->addWidget(h);

  h->setStretchFactor(new QWidget(h),5); // stretchable placeholder

  QPushButton *ButtonClose = new QPushButton(tr("Close"), h);
  connect(ButtonClose, SIGNAL(clicked()), SLOT(slotClose()));
  ButtonClose->setFocus();

  h->setStretchFactor(new QWidget(h),5); // stretchable placeholder
}

DisplayDialog::~DisplayDialog()
{
  delete vLayout;
}

// ************************************************************
void DisplayDialog::slotClose()
{
  accept();
}
