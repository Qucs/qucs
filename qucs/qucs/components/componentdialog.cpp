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
  setCaption(tr("Edit Component Properties"));

  QGridLayout *g = new QGridLayout(this,9,2,5,5);

  QHBox *h2 = new QHBox(this);
  h2->setSpacing(5);
  g->addMultiCellWidget(h2,8,8,0,1);
  connect(new QPushButton(tr("OK"),h2), SIGNAL(clicked()), SLOT(slotButtOK()));
  connect(new QPushButton(tr("Apply"),h2), SIGNAL(clicked()), SLOT(slotApplyInput()));
  connect(new QPushButton(tr("Cancel"),h2), SIGNAL(clicked()), SLOT(close()));

  QLabel *label1 = new QLabel(this);
  g->addMultiCellWidget(label1,0,0,0,1);

  prop = new QListView(this);
  prop->setMinimumSize(200, 150);
//  prop->setMaximumSize(250, 500);
  prop->addColumn(tr("Name"));
  prop->addColumn(tr("Value"));
  prop->addColumn(tr("display"));
  prop->setSorting(-1);   // no sorting
  g->addMultiCellWidget(prop,1,7,0,0);

  Name = new QLabel(this);
  g->addWidget(Name,1,1);

  Expr.setPattern("[^\"=]+");  // valid expression for property input 'edit'
  QValidator *Validator = new QRegExpValidator(Expr, this);

  Description = new QLabel(this);
  g->addWidget(Description,2,1);
  NameEdit = new QLineEdit(this);
  NameEdit->setShown(false);   // hide, because it only replaces 'Description' in some cases
  NameEdit->setValidator(Validator);
  g->addWidget(NameEdit,2,1);
  connect(NameEdit, SIGNAL(returnPressed()), SLOT(slotApplyPropName()));

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
  BrowseButt = new QPushButton(tr("Browse"),h3);
  BrowseButt->setEnabled(false);
  BrowseButt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  connect(BrowseButt, SIGNAL(clicked()), SLOT(slotBrowseFile()));

  disp = new QCheckBox(tr("display in schematic"),this);
  g->addWidget(disp,5,1);
  connect(disp, SIGNAL(stateChanged(int)), SLOT(slotApplyState(int)));

  QVBoxLayout *v = new QVBoxLayout(); // stretchable placeholder
  v->addStretch(2);
  g->addLayout(v,6,1);

  QHBox *h4 = new QHBox(this);
  h4->setSpacing(5);
  g->addMultiCellWidget(h4,7,7,1,1);
  ButtAdd = new QPushButton(tr("Add"),h4);
  ButtRem = new QPushButton(tr("Remove"),h4);
  connect(ButtAdd, SIGNAL(clicked()), SLOT(slotButtAdd()));
  connect(ButtRem, SIGNAL(clicked()), SLOT(slotButtRem()));

  // ------------------------------------------------------------
  Comp = c;
  label1->setText(c->Description);
  changed = false;

//  prop->clear();

  QString s;
  // insert all properties into the ListBox
  for(Property *p = c->Props.last(); p != 0; p = c->Props.prev()) {
    if(p->display) s = tr("yes");
    else s = tr("no");
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
// Is called if a property is selected. It transfers the values to the right side
// for editing.
void ComponentDialog::slotSelectProperty(QListViewItem *item)
{
  if(item == 0) return;
  item->setSelected(true);    // if called from another function, this was not yet done

  if(item->text(2) == tr("yes")) disp->setChecked(true);
  else disp->setChecked(false);

  Property *pp;
  for(pp = Comp->Props.first(); pp!=0; pp = Comp->Props.next())
    if(pp->Name == item->text(0)) break;

  if(pp->Name == "File") BrowseButt->setEnabled(true);
  else BrowseButt->setEnabled(false);

  if(pp->Description.isEmpty()) { ButtAdd->setEnabled(true); ButtRem->setEnabled(true); }
  else { ButtAdd->setEnabled(false); ButtRem->setEnabled(false); }

  if(pp->Description.isEmpty()) {
    Name->setText("");
    NameEdit->setText(item->text(0));
    edit->setText(item->text(1));

    NameEdit->setShown(true);
    Description->setShown(false);

    NameEdit->setFocus();   // edit QLineEdit
  }
  else {
    Name->setText(item->text(0));
    edit->setText(item->text(1));

    NameEdit->setShown(false);
    NameEdit->setText(item->text(0));    // only for uniformity (perhaps used for adding properties)
    Description->setShown(true);

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
//    edit->deselect();  // doesn't work ?!?
  }
}

// -------------------------------------------------------------------------
void ComponentDialog::slotApplyChange(const QString& Text)
{
  edit->setText(Text);
  slotApplyProperty();
  ComboEdit->setFocus();
}

// -------------------------------------------------------------------------
// Is called if the "RETURN"-button is pressed in the "edit" Widget.
void ComponentDialog::slotApplyProperty()
{
  prop->currentItem()->setText(1, edit->text());	// apply edit line
  if(NameEdit->isShown())
    prop->currentItem()->setText(0, NameEdit->text());	// also apply property name

  QListViewItem *item = prop->currentItem()->itemBelow();
  if(item == 0) {
    slotButtOK();   // close dialog, if it was the last property
    return;
  }

  prop->setSelected(item, true);
  slotSelectProperty(item);   // switch to the next property
}

// -------------------------------------------------------------------------
// Is called if the "RETURN"-button is pressed in the "NameEdit" Widget.
void ComponentDialog::slotApplyPropName()
{
  prop->currentItem()->setText(0, NameEdit->text());	// apply property name
  edit->setFocus();   // cursor into "edit" widget
}

// -------------------------------------------------------------------------
// Is called if the checkbox is pressed (changed).
void ComponentDialog::slotApplyState(int State)
{
  if(State == QButton::On) prop->currentItem()->setText(2, tr("yes"));
  else prop->currentItem()->setText(2, tr("no"));
}

// -------------------------------------------------------------------------
// Is called if the "OK"-button is pressed.
void ComponentDialog::slotButtOK()
{
  slotApplyInput();
//  close();
  if(changed) done(1);
  else done(0);
}

// -------------------------------------------------------------------------
// Is called, if the "Apply"-button is pressed.
void ComponentDialog::slotApplyInput()
{
  QListViewItem *item = prop->firstChild();
  if(item == 0) return;

  prop->currentItem()->setText(1, edit->text());	// apply edit line to current item
  if(NameEdit->isShown())
    prop->currentItem()->setText(0, NameEdit->text());	// also apply property name

  bool display;
  // take all the new property values
  for(Property *pp = Comp->Props.first(); pp!=0; pp = Comp->Props.next()) {
    if(pp->Value != item->text(1)) {
      pp->Value = item->text(1);
      changed = true;
    }
    if(pp->Description.isEmpty())
      if(pp->Name != item->text(0)) {
        pp->Name = item->text(0);
        changed = true;
      }

    display = (item->text(2) == tr("yes"));
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
  QString s = QFileDialog::getOpenFileName("",tr("All Files (*.*)"),this,"",tr("Select a file"));
  if(!s.isEmpty()) edit->setText(s);
  prop->currentItem()->setText(1, s);
}

// -------------------------------------------------------------------------
// Is called if the add button is pressed. This is only possible for some properties.
void ComponentDialog::slotButtAdd()
{
  QListViewItem *item = prop->selectedItem();
  if(item == 0) item = prop->lastItem();

  QString s = tr("no");
  if(disp->isChecked()) s = tr("yes");

  prop->setSelected(new QListViewItem(prop, item, NameEdit->text(), edit->text(), s), true);

  int z=1;
  for(QListViewItem *i = prop->firstChild(); i != 0; i = i->nextSibling()) {    // search selected property
    if(i == item) {
      // insert property in component memory at the correct position
      Comp->Props.insert(z, new Property(NameEdit->text(), edit->text(), disp->isChecked()));
      break;
    }
    z++;   // count the position
  }
  changed = true;
}

// -------------------------------------------------------------------------
// Is called if the remove button is pressed. This is only possible for some properties.
void ComponentDialog::slotButtRem()
{
  if(prop->childCount() < 2) return;     // the last property cannot be removed
  QListViewItem *item = prop->selectedItem();
  if(item == 0) return;

  Property *pp = Comp->Props.first();
  for(QListViewItem *i = prop->firstChild(); i != 0; i = i->nextSibling()) {    // search selected property
    if(i == item) {
      Comp->Props.remove();     // delete property in component memory
      break;
    }
    pp = Comp->Props.next();
  }

  QListViewItem *next_item = item->itemBelow();
  if(next_item == 0) next_item = item->itemAbove();
  prop->takeItem(item);     // remove from ListView
  delete item;              // delete item
  changed = true;

  slotSelectProperty(next_item);
}
