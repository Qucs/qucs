/***************************************************************************
                          componentdialog.cpp  -  description
                             -------------------
    begin                : Tue Sep 9 2003
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

#include "componentdialog.h"

#include <qlayout.h>
#include <qhbox.h>
#include <qpushbutton.h>


ComponentDialog::ComponentDialog(Component *c, QWidget *parent, const char *name )
                                  : QDialog(parent, name, Qt::WDestructiveClose)
{
  setCaption("Edit Component Properties");

  QGridLayout *g = new QGridLayout(this,6,2,5,5);

  QLabel *label1 = new QLabel(this);
  g->addMultiCellWidget(label1,0,0,0,1);

  prop = new QListBox(this);
  prop->setMinimumSize(200, 150);
  g->addMultiCellWidget(prop,1,4,0,0);

  Name = new QLabel(this);
//  Name->setText("Name");
  g->addWidget(Name,1,1);
  
  edit = new QLineEdit(this);
  edit->setMinimumWidth(150);
  g->addWidget(edit,2,1);
  connect(edit, SIGNAL(returnPressed()), SLOT(slotApplyInput()));

  disp = new QCheckBox("display in schematic",this);
  g->addWidget(disp,3,1);
  
  QVBoxLayout *v = new QVBoxLayout(); // stretchable placeholder
  v->addStretch(1);
  g->addLayout(v,4,1);

  QHBox *h2 = new QHBox(this);
  h2->setSpacing(5);
  g->addMultiCellWidget(h2,5,5,0,1);

  connect(new QPushButton("OK",h2), SIGNAL(clicked()), SLOT(slotButtOK()));
  connect(new QPushButton("Apply",h2), SIGNAL(clicked()), SLOT(slotButtApply()));
  connect(new QPushButton("Cancel",h2), SIGNAL(clicked()), SLOT(close()));
  
  // ------------------------------------------------------------
  Comp = c;
  label1->setText(c->Description);

//  prop->clear();
  
  // insert all properties into the ListBox
  for(Property *p = c->Props.first(); p != 0; p = c->Props.next())
    prop->insertItem(p->Name+"="+p->Value);

  prop->setCurrentItem(0);
  slotSelectProperty(0);

  connect(prop, SIGNAL(highlighted(int)), SLOT(slotSelectProperty(int)));
}                                           

ComponentDialog::~ComponentDialog()
{
}

void ComponentDialog::slotSelectProperty(int n)
{
  QString s = prop->text(n);  //item->text();
  int m = s.findRev("=");
  edit->setText(s.right(s.length()-m-1));
  s = s.left(m);
  Name->setText(s);
  
  Property *p = Comp->Props.at(n);
  disp->setChecked(p->display);
}

void ComponentDialog::slotButtOK()
{
  close();
}

void ComponentDialog::slotButtApply()
{
  slotApplyInput();
}

/*void ComponentDialog::slotButtCancel()
{
}*/

void ComponentDialog::slotApplyInput()
{
  Property *p = Comp->Props.at(prop->currentItem());
  p->Value   = edit->text();
  p->display = disp->isChecked();
  prop->changeItem(p->Name+"="+p->Value, prop->currentItem());  // new value into ListBox
  prop->repaint();
}
