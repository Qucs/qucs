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
#include <qmessagebox.h>
#include <qvalidator.h>
#include <qfiledialog.h>


ComponentDialog::ComponentDialog(Component *c, QWidget *parent, const char *name )
                                  : QDialog(parent, name, Qt::WDestructiveClose)
{
  setCaption("Edit Component Properties");

  QGridLayout *g = new QGridLayout(this,8,2,5,5);

  QHBox *h2 = new QHBox(this);
  h2->setSpacing(5);
  g->addMultiCellWidget(h2,7,7,0,1);
  connect(new QPushButton("OK",h2), SIGNAL(clicked()), SLOT(slotButtOK()));
  connect(new QPushButton("Apply",h2), SIGNAL(clicked()), SLOT(slotApplyInput()));
  connect(new QPushButton("Cancel",h2), SIGNAL(clicked()), SLOT(close()));

  QLabel *label1 = new QLabel(this);
  g->addMultiCellWidget(label1,0,0,0,1);

  prop = new QListView(this);
  prop->setMinimumSize(200, 150);
//  prop->setMaximumSize(250, 500);
  prop->addColumn("Name");
  prop->addColumn("Value");
  prop->addColumn("display");
  prop->setSorting(-1);   // no sorting
  g->addMultiCellWidget(prop,1,6,0,0);

  Name = new QLabel(this);
  g->addWidget(Name,1,1);
  
  Description = new QLabel(this);
  g->addWidget(Description,2,1);

  Expr.setPattern("[^\"]+");  // valid expression for property input 'edit'
  QValidator *Validator = new QRegExpValidator(Expr, this);

  edit = new QLineEdit(this);
  edit->setMinimumWidth(150);
  g->addWidget(edit,3,1);
  edit->setValidator(Validator);
  connect(edit, SIGNAL(returnPressed()), SLOT(slotApplyProperty()));

  ComboEdit = new QComboBox(false,this);
  ComboEdit->setShown(false);   // hide, because it only replaces 'edit' in some cases
  g->addWidget(ComboEdit,3,1);
  connect(ComboEdit, SIGNAL(activated(const QString&)), SLOT(slotApplyChange(const QString&)));

  QHBox *h3 = new QHBox(this);
  g->addWidget(h3,4,1);
  QWidget *h = new QWidget(h3); // stretchable placeholder
  h3->setStretchFactor(h,5);
  BrowseButt = new QPushButton("Browse",h3);
  BrowseButt->setEnabled(false);
  BrowseButt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  connect(BrowseButt, SIGNAL(clicked()), SLOT(slotBrowseFile()));

  disp = new QCheckBox("display in schematic",this);
  g->addWidget(disp,5,1);
  connect(disp, SIGNAL(stateChanged(int)), SLOT(slotApplyState(int)));
  
  QVBoxLayout *v = new QVBoxLayout(); // stretchable placeholder
  v->addStretch(2);
  g->addLayout(v,6,1);
  
  // ------------------------------------------------------------
  Comp = c;
  label1->setText(c->Description);
  changed = false;

//  prop->clear();
  
  QString s;
  // insert all properties into the ListBox
  for(Property *p = c->Props.last(); p != 0; p = c->Props.prev()) {
    if(p->display) s = "yes";
    else s = "no";
    prop->insertItem(new QListViewItem(prop,p->Name,p->Value,s));
  }

  if(prop->childCount() > 0) {
    prop->setCurrentItem(prop->firstChild());
    slotSelectProperty(prop->firstChild());
  }

  connect(prop, SIGNAL(clicked(QListViewItem*)), SLOT(slotSelectProperty(QListViewItem*)));
}                                           

ComponentDialog::~ComponentDialog()
{
}

// -------------------------------------------------------------------------
void ComponentDialog::slotSelectProperty(QListViewItem *item)
{
  if(item == 0) return;
  Name->setText(item->text(0));
  edit->setText(item->text(1));

  if(item->text(2) == "yes") disp->setChecked(true);
  else disp->setChecked(false);

  Property *pp;
  for(pp = Comp->Props.first(); pp!=0; pp = Comp->Props.next())
    if(pp->Name == item->text(0)) break;

  if(pp->Name == "File") BrowseButt->setEnabled(true);
  else BrowseButt->setEnabled(false);

  QStringList List = List.split('|',pp->Description);
  Description->setText(List.first());
  if(List.count() > 1) {    // should property values been choosen or been entered ?
    List.remove(List.begin());
    ComboEdit->clear();
    ComboEdit->insertStringList(List);
    edit->setShown(false);
    ComboEdit->setShown(true);
  }
  else {
    edit->setShown(true);
    ComboEdit->setShown(false);
  }

  edit->setFocus();   // edit QLineEdit
//  edit->deselect();  // doesn't work ?!?
}

// -------------------------------------------------------------------------
void ComponentDialog::slotApplyChange(const QString& Text)
{
  edit->setText(Text);
  slotApplyProperty();
  ComboEdit->setFocus();
}

// -------------------------------------------------------------------------
void ComponentDialog::slotApplyProperty()
{
  prop->currentItem()->setText(1, edit->text());

  QListViewItem *item = prop->currentItem()->itemBelow();
  if(item == 0) {
    slotButtOK();   // close dialog, if it was the last property
    return;
  }

  prop->setSelected(item, true);
  slotSelectProperty(item);   // switch to the next property
}

// -------------------------------------------------------------------------
void ComponentDialog::slotApplyState(int State)
{
  if(State == QButton::On) prop->currentItem()->setText(2, "yes");
  else prop->currentItem()->setText(2, "no");
}

// -------------------------------------------------------------------------
void ComponentDialog::slotButtOK()
{
  slotApplyInput();
//  close();
  if(changed) done(1);
  else done(0);
}

// -------------------------------------------------------------------------
void ComponentDialog::slotApplyInput()
{
  QListViewItem *item = prop->firstChild();
  if(item == 0) return;

  bool display;
  // take all the new property values
  for(Property *pp = Comp->Props.first(); pp!=0; pp = Comp->Props.next()) {
    if(pp->Value != item->text(1)) {
      pp->Value = item->text(1);
      changed = true;
    }

    display = (item->text(2) == "yes");
    if(pp->display != display) {
      pp->display = display;
      changed = true;
    }
    
    item = item->itemBelow();   // next item
  }
}

// -------------------------------------------------------------------------
void ComponentDialog::slotBrowseFile()
{
  QString s = QFileDialog::getOpenFileName("","All Files (*.*)",this,"","Select a file");
  if(!s.isEmpty()) edit->setText(s);
  prop->currentItem()->setText(1, s);
}
