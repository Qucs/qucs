/***************************************************************************
                          componentdialog.cpp  -  description
                             -------------------
    begin                : Tue Sep 9 2003
    copyright            : (C) 2003 by Michael Margraf
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

#include "componentdialog.h"

#include "../qucsview.h"

#include <qlayout.h>
#include <qhbox.h>
#include <qmessagebox.h>
#include <qvalidator.h>
#include <qfiledialog.h>
#include <qprocess.h>


ComponentDialog::ComponentDialog(Component *c,
			QWidget *parent, const char *name )
			: QDialog(parent, name, TRUE, Qt::WDestructiveClose)
{
  resize(400, 250);
  setCaption(tr("Edit Component Properties"));

  QGridLayout *g = new QGridLayout(this,9,2,5,5);

  QHBox *h2 = new QHBox(this);
  h2->setSpacing(5);
  g->addMultiCellWidget(h2,8,8,0,1);
  connect(new QPushButton(tr("OK"),h2), SIGNAL(clicked()),
	  SLOT(slotButtOK()));
  connect(new QPushButton(tr("Apply"),h2), SIGNAL(clicked()),
	  SLOT(slotApplyInput()));
  connect(new QPushButton(tr("Cancel"),h2), SIGNAL(clicked()),
	  SLOT(slotButtCancel()));

  QLabel *label1 = new QLabel(this);
  g->addMultiCellWidget(label1,0,0,0,1);

  prop = new QListView(this);
  prop->setMinimumSize(200, 150);
  prop->addColumn(tr("Name"));
  prop->addColumn(tr("Value"));
  prop->addColumn(tr("display"));
  prop->addColumn(tr("Description"));
  prop->setSorting(-1);   // no sorting
  g->addMultiCellWidget(prop,1,7,0,0);

  Name = new QLabel(this);
  g->addWidget(Name,1,1);

  Expr.setPattern("[^\"=]+");  // valid expression for property input 'edit'
  QValidator *Validator = new QRegExpValidator(Expr, this);

  Description = new QLabel(this);
  g->addWidget(Description,2,1);

  // hide, because it only replaces 'Description' in some cases
  NameEdit = new QLineEdit(this);
  NameEdit->setShown(false);
  NameEdit->setValidator(Validator);
  g->addWidget(NameEdit,2,1);
  connect(NameEdit, SIGNAL(returnPressed()), SLOT(slotApplyPropName()));

  edit = new QLineEdit(this);
  edit->setMinimumWidth(150);
  g->addWidget(edit,3,1);
  edit->setValidator(Validator);
  connect(edit, SIGNAL(returnPressed()), SLOT(slotApplyProperty()));

  // hide, because it only replaces 'edit' in some cases
  ComboEdit = new QComboBox(false,this);
  ComboEdit->setShown(false);
  g->addWidget(ComboEdit,3,1);
  connect(ComboEdit, SIGNAL(activated(const QString&)),
	  SLOT(slotApplyChange(const QString&)));

  QHBox *h3 = new QHBox(this);
  g->addWidget(h3,4,1);
  h3->setStretchFactor(new QWidget(h3),5); // stretchable placeholder
  EditButt = new QPushButton(tr("Edit"),h3);
  EditButt->setEnabled(false);
  EditButt->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  connect(EditButt, SIGNAL(clicked()), SLOT(slotEditFile()));
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
  changed = transfered = false;

//  prop->clear();

  QString s;
  // insert all properties into the ListBox
  for(Property *p = c->Props.last(); p != 0; p = c->Props.prev()) {
    if(p->display) s = tr("yes");
    else s = tr("no");
    prop->insertItem(new QListViewItem(prop, p->Name, p->Value, s,
		     p->Description));
  }

  if(prop->childCount() > 0) {
    prop->setCurrentItem(prop->firstChild());
    slotSelectProperty(prop->firstChild());
  }

  connect(prop, SIGNAL(clicked(QListViewItem*)),
	  SLOT(slotSelectProperty(QListViewItem*)));
}

ComponentDialog::~ComponentDialog()
{
}

// -------------------------------------------------------------------------
// Is called if a property is selected. It transfers the values to the right
// side for editing.
void ComponentDialog::slotSelectProperty(QListViewItem *item)
{
  if(item == 0) return;
  item->setSelected(true);  // if called from elsewhere, this was not yet done

  if(item->text(2) == tr("yes")) disp->setChecked(true);
  else disp->setChecked(false);

  if(item->text(0) == "File") {
    EditButt->setEnabled(true);
    BrowseButt->setEnabled(true);
  }
  else {
    EditButt->setEnabled(false);
    BrowseButt->setEnabled(false);
  }

  int i;
  QString PropDesc = item->text(3);
  if(PropDesc.isEmpty()) {
    // show two line edit fields (name and value)
    ButtAdd->setEnabled(true);
    ButtRem->setEnabled(true);

    Name->setText("");
    NameEdit->setText(item->text(0));
    edit->setText(item->text(1));

    edit->setShown(true);
    NameEdit->setShown(true);
    Description->setShown(false);
    ComboEdit->setShown(false);

    NameEdit->setFocus();   // edit QLineEdit
  }
  else {
    ButtAdd->setEnabled(false);
    ButtRem->setEnabled(false);

    Name->setText(item->text(0));
    edit->setText(item->text(1));

    NameEdit->setShown(false);
    NameEdit->setText(item->text(0));  // perhaps used for adding properties
    Description->setShown(true);

    i = PropDesc.find('(');
    Description->setText(PropDesc.left(i));
    PropDesc = PropDesc.mid(i+1);
    PropDesc.remove(')');
    QStringList List = List.split(',',PropDesc);
    if(List.count() > 1) {    // ComboBox with value list or line edit ?
      ComboEdit->clear();
      ComboEdit->insertStringList(List);

      for(i=ComboEdit->count()-1; i>=0; i--)
       if(item->text(1) == ComboEdit->text(i)) {
         ComboEdit->setCurrentItem(i);
	 break;
       }
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
  prop->currentItem()->setText(1, Text);	// apply edit line
  changed = true;

  ComboEdit->setFocus();
  QListViewItem *item = prop->currentItem()->itemBelow();
  if(item == 0) return;

  prop->setSelected(item, true);
  slotSelectProperty(item);   // switch to the next property
}

// -------------------------------------------------------------------------
// Is called if the "RETURN"-button is pressed in the "edit" Widget.
void ComponentDialog::slotApplyProperty()
{
  QListViewItem *item = prop->currentItem();

  if(item->text(1) != edit->text()) {
    item->setText(1, edit->text());    // apply edit line
    changed = true;
  }
  if(NameEdit->isShown())	// also apply property name ?
    if(item->text(0) != NameEdit->text()) {
      item->setText(0, NameEdit->text());  // apply property name
      changed = true;
    }

  item = item->itemBelow();
  if(!item) {
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
  QListViewItem *item = prop->currentItem();
  if(item->text(0) != NameEdit->text()) {
    item->setText(0, NameEdit->text());  // apply property name
    changed = true;
  }
  edit->setFocus();   // cursor into "edit" widget
}

// -------------------------------------------------------------------------
// Is called if the checkbox is pressed (changed).
void ComponentDialog::slotApplyState(int State)
{
  QListViewItem *item = prop->currentItem();
  if(item == 0) return;

  QString ButtonState;
  if(State == QButton::On) ButtonState = tr("yes");
  else ButtonState = tr("no");

  if(item->text(2) != ButtonState) {
    item->setText(2, ButtonState);
    changed = true;
  }
}

// -------------------------------------------------------------------------
// Is called if the "OK"-button is pressed.
void ComponentDialog::slotButtOK()
{
  slotApplyInput();
  if(changed) done(1);
  else done(0);
}

// -------------------------------------------------------------------------
// Is called if the "Cancel"-button is pressed.
void ComponentDialog::slotButtCancel()
{
  if(transfered) done(1);	// changed could have been done before
  else done(0);			// (by "Apply"-button)
}

// -------------------------------------------------------------------------
// Is called, if the "Apply"-button is pressed.
void ComponentDialog::slotApplyInput()
{
  QListViewItem *item = prop->firstChild();
  if(item == 0) return;

  item = prop->currentItem();
  if(item->text(1) != edit->text()) {
    item->setText(1, edit->text());    // apply edit line
    changed = true;
  }
  if(NameEdit->isShown())	// also apply property name ?
    if(item->text(0) != NameEdit->text()) {
      item->setText(0, NameEdit->text());  // apply property name
      changed = true;
    }

  bool display;
  Comp->Props.clear();
  // apply all the new property values, i.e. rebuild the properties
  for(item = prop->firstChild(); item != 0; item = item->itemBelow()) {
    display = (item->text(2) == tr("yes"));
    Comp->Props.append(new
	Property(item->text(0), item->text(1), display, item->text(3)));
  }
  transfered = true;     // applied changed to the component itself

  if(changed) {
    Comp->recreate();   // to apply changes to the schematic symbol
    ((QucsView*)parent())->viewport()->repaint();
  }
}

// -------------------------------------------------------------------------
void ComponentDialog::slotBrowseFile()
{
  QString s = QFileDialog::getOpenFileName("", tr("All Files (*.*)"),
					this, "", tr("Select a file"));
  if(!s.isEmpty()) {
    edit->setText(s);
    changed = true;
  }
  prop->currentItem()->setText(1, s);
}

// -------------------------------------------------------------------------
void ComponentDialog::slotEditFile()
{
  QString com = QucsSettings.Editor + " " + edit->text();
  QProcess QucsEditor(QStringList::split(" ", com));
  if(!QucsEditor.start())
    QMessageBox::critical(this, tr("Error"), tr("Cannot start text editor!"));
}

// -------------------------------------------------------------------------
// Is called if the add button is pressed. This is only possible for some
// properties.
void ComponentDialog::slotButtAdd()
{
  QListViewItem *item = prop->selectedItem();
  if(item == 0) item = prop->lastItem();

  QString s = tr("no");
  if(disp->isChecked()) s = tr("yes");

  prop->setSelected(new QListViewItem(prop, item,
			NameEdit->text(), edit->text(), s), true);
  changed = true;
}

// -------------------------------------------------------------------------
// Is called if the remove button is pressed. This is only possible for
// some properties.
void ComponentDialog::slotButtRem()
{
  if(prop->childCount() < 2) return;  // the last property cannot be removed
  QListViewItem *item = prop->selectedItem();
  if(item == 0) return;

  QListViewItem *next_item = item->itemBelow();
  if(next_item == 0) next_item = item->itemAbove();
  prop->takeItem(item);     // remove from ListView
  delete item;              // delete item
  changed = true;

  slotSelectProperty(next_item);
}
