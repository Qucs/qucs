/***************************************************************************
                               helpdialog.cpp
                             ------------------
    begin                : Fri Mar 04 2005
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

#include "helpdialog.h"

#include <qlayout.h>
#include <qhbox.h>
#include <qpushbutton.h>
#include <qtextedit.h>


HelpDialog::HelpDialog(QWidget *parent)
                     : QDialog(parent, 0, false, Qt::WDestructiveClose)
{
  setCaption("QucsFilter Help");


  // --------  set help text into dialog  ------------
  QString s(tr("QucsFilter is a filter synthesis program.\n")+
	    tr("To create a filter, simply enter all\n")+
	    tr("parameters and press the big button at the\n")+
	    tr("bottom of the main window. Immediatly, the\n")+
	    tr("schematic of the filter is calculated and\n")+
	    tr("put into the clipboard. Now go to Qucs,\n")+
	    tr("open an empty schematic and press\n")+
	    tr("CTRL-V (paste from clipboard). The filter\n")+
	    tr("schematic can now be inserted and\n")+
	    tr(" simulated. Have lots of fun!"));


  // --------  create dialog widgets  ------------
  resize(250, 230);

  vLayout = new QVBoxLayout(this);

  Text = new QTextEdit(s, QString::null, this);
  Text->setTextFormat(Qt::PlainText);
  Text->setReadOnly(true);
  Text->setWordWrap(QTextEdit::NoWrap);
  Text->setMinimumSize(200,200);
  vLayout->addWidget(Text);

  QHBox *h = new QHBox(this);
  vLayout->addWidget(h);

  h->setStretchFactor(new QWidget(h),5); // stretchable placeholder

  QPushButton *ButtonClose = new QPushButton(tr("Close"), h);
  connect(ButtonClose, SIGNAL(clicked()), SLOT(slotClose()));
  ButtonClose->setFocus();

  h->setStretchFactor(new QWidget(h),5); // stretchable placeholder
}

HelpDialog::~HelpDialog()
{
  delete vLayout;
}

// ************************************************************
void HelpDialog::slotClose()
{
  accept();
}
