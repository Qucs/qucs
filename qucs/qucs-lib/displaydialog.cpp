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
#include <qvgroupbox.h>


DisplayDialog::DisplayDialog(QWidget *parent)
                     : QDialog(parent, 0, false, Qt::WDestructiveClose)
{
  vLayout = new QVBoxLayout(this);
  vLayout->setMargin(3);

  QVGroupBox *Analog = new QVGroupBox (tr("Analogue"), this);
  Text = new QTextEdit(Analog);
  Text->setTextFormat(Qt::PlainText);
  Text->setReadOnly(true);
  Text->setMinimumSize(200, 80);
  vLayout->addWidget(Analog);

  QVGroupBox *VHDL = new QVGroupBox (tr("VHDL"), this);
  VHDLText = new QTextEdit(VHDL);
  VHDLText->setTextFormat(Qt::PlainText);
  VHDLText->setReadOnly(true);
  VHDLText->setMinimumSize(200, 80);
  vLayout->addWidget(VHDL);

  QVGroupBox *Verilog = new QVGroupBox (tr("Verilog"), this);
  VerilogText = new QTextEdit(Verilog);
  VerilogText->setTextFormat(Qt::PlainText);
  VerilogText->setReadOnly(true);
  VerilogText->setMinimumSize(200, 80);
  vLayout->addWidget(Verilog);

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
