/***************************************************************************
                          messagebox.cpp  -  description
                             -------------------
    begin                : Sat Oct 25 2003
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

#include "messagebox.h"

#include <qlayout.h>
#include <qhbox.h>
#include <qpushbutton.h>
#include <qlabel.h>


MessageBox::MessageBox(const QString& Caption, const QString& Text, QWidget *parent, const char *name)
                                  : QDialog(parent, name, Qt::WDestructiveClose)
{
  setCaption(Caption);

  QVBoxLayout *all = new QVBoxLayout(this);
  all->setSpacing(5);
  QLabel *l   = new QLabel(Text, this);
  l->setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
  l->setMargin(5);
  all->addWidget(l);

  QHBox  *Butts = new QHBox(this);
//  Butts->setSpacing(5);
  all->addWidget(Butts);
  QPushButton *Save = new QPushButton("Save", Butts);
  connect(Save, SIGNAL(clicked()), SLOT(slotSave()));
  QPushButton *SaveAll = new QPushButton("Save all", Butts);
  connect(SaveAll, SIGNAL(clicked()), SLOT(slotSaveAll()));
  QPushButton *Discard = new QPushButton("Discard", Butts);
  connect(Discard, SIGNAL(clicked()), SLOT(slotDiscard()));
  QPushButton *DiscardAll = new QPushButton("DiscardAll", Butts);
  connect(DiscardAll, SIGNAL(clicked()), SLOT(slotDiscardAll()));
  QPushButton *Cancel = new QPushButton("Cancel", Butts);
  connect(Cancel, SIGNAL(clicked()), SLOT(slotCancel()));
}

MessageBox::~MessageBox()
{
}

void MessageBox::slotSave()
{
  done(1);
}

void MessageBox::slotSaveAll()
{
  done(2);
}

void MessageBox::slotDiscard()
{
  done(3);
}

void MessageBox::slotDiscardAll()
{
  done(4);
}

void MessageBox::slotCancel()
{
  done(5);
}
