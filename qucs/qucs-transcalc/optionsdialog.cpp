/***************************************************************************
                              optionsdialog.cpp
                             --------------------
    begin                : Sun Apr 03 2005
    copyright            : (C) 2005 by Stefan Jahn
    email                : stefan@lkcc.org
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

#include <qlayout.h>
#include <qhbox.h>
#include <qhgroupbox.h>
#include <qpushbutton.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qcombobox.h>

#include "optionsdialog.h"
#include "qucstrans.h"

extern struct TransUnit TransUnits[];

OptionsDialog::OptionsDialog(QWidget *parent)
                     : QDialog(parent, 0, false, Qt::WDestructiveClose)
{
  setCaption("Qucs Transcalc "+tr("Options"));

  // --------  create dialog widgets  ------------
  vLayout = new QVBoxLayout(this);
  vLayout->setMargin(3);
  vLayout->setSpacing(3);

  QHGroupBox * h = new QHGroupBox(tr("Units"), this);
  vLayout->addWidget(h);
  QVBox * l = new QVBox(h);
  l->setSpacing(3);
  QLabel * lfr = new QLabel(tr("Frequency"),l);
  lfr->setAlignment (Qt::AlignRight);
  QLabel * lle = new QLabel(tr("Length"),l);
  lle->setAlignment (Qt::AlignRight);
  QLabel * lre = new QLabel(tr("Resistance"),l);
  lre->setAlignment (Qt::AlignRight);
  QLabel * lan = new QLabel(tr("Angle"),l);
  lan->setAlignment (Qt::AlignRight);
  QVBox * r = new QVBox(h);
  r->setSpacing(3);
  for (int j = 0; j < 4; j++) {
    units[j] = new QComboBox(r);
    for (int i = 0; TransUnits[j].units[i] != NULL; i++)
      units[j]->insertItem (TransUnits[j].units[i]);
  }
  units[0]->setCurrentItem (QucsSettings.freq_unit);
  units[1]->setCurrentItem (QucsSettings.length_unit);
  units[2]->setCurrentItem (QucsSettings.res_unit);
  units[3]->setCurrentItem (QucsSettings.ang_unit);
  
  QHBox * h2 = new QHBox(this);
  vLayout->addWidget(h2);

  QPushButton *ButtonSave = new QPushButton(tr("Save as Default"), h2);
  connect(ButtonSave, SIGNAL(clicked()), SLOT(slotSave()));

  QPushButton *ButtonClose = new QPushButton(tr("Dismiss"), h2);
  connect(ButtonClose, SIGNAL(clicked()), SLOT(slotClose()));
  ButtonClose->setFocus();
}

OptionsDialog::~OptionsDialog()
{
  delete vLayout;
}

void OptionsDialog::slotClose()
{
  accept();
}

void OptionsDialog::slotSave()
{
  QucsSettings.freq_unit = units[0]->currentItem();
  QucsSettings.length_unit = units[1]->currentItem();
  QucsSettings.res_unit = units[2]->currentItem();
  QucsSettings.ang_unit = units[3]->currentItem();
  accept();
}
