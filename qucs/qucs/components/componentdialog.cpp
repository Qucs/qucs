/***************************************************************************
                          componentdialog.cpp  -  description
                             -------------------
    begin                : Tue Sep 9 2003
    copyright            : (C) 2003, 2004 by Michael Margraf
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
#include "qucs.h"
#include "qucsview.h"
#include "qucsdoc.h"

#include <qlayout.h>
#include <qhbox.h>
#include <qmessagebox.h>
#include <qvalidator.h>
#include <qfiledialog.h>


ComponentDialog::ComponentDialog(Component *c, QucsDoc *d, QWidget *parent)
			: QDialog(parent, 0, TRUE, Qt::WDestructiveClose)
{
  resize(400, 250);
  setCaption(tr("Edit Component Properties"));

  QGridLayout *g = new QGridLayout(this,10,2,5,5);

  QHBox *h2 = new QHBox(this);
  h2->setSpacing(5);
  g->addMultiCellWidget(h2,9,9,0,1);
  connect(new QPushButton(tr("OK"),h2), SIGNAL(clicked()),
	  SLOT(slotButtOK()));
  connect(new QPushButton(tr("Apply"),h2), SIGNAL(clicked()),
	  SLOT(slotApplyInput()));
  connect(new QPushButton(tr("Cancel"),h2), SIGNAL(clicked()),
	  SLOT(slotButtCancel()));

  QLabel *label1 = new QLabel(this);
  g->addMultiCellWidget(label1,0,0,0,1);

  QHBox *h5 = new QHBox(this);
  h5->setSpacing(5);
  g->addWidget(h5,1,0);
  QLabel *label2 = new QLabel(h5);
  label2->setText(tr("Name:"));
  CompNameEdit = new QLineEdit(h5);
  connect(CompNameEdit, SIGNAL(returnPressed()), SLOT(slotButtOK()));

  prop = new QListView(this);
  prop->setMinimumSize(200, 150);
  prop->addColumn(tr("Name"));
  prop->addColumn(tr("Value"));
  prop->addColumn(tr("display"));
  prop->addColumn(tr("Description"));
  prop->setSorting(-1);   // no sorting
  g->addMultiCellWidget(prop,2,8,0,0);

  Name = new QLabel(this);
  g->addWidget(Name,2,1);

  Expr.setPattern("[^\"=]+");  // valid expression for property input 'edit'
  QValidator *Validator = new QRegExpValidator(Expr, this);

  Description = new QLabel(this);
  g->addWidget(Description,3,1);

  // hide, because it only replaces 'Description' in some cases
  NameEdit = new QLineEdit(this);
  NameEdit->setShown(false);
  NameEdit->setValidator(Validator);
  g->addWidget(NameEdit,3,1);
  connect(NameEdit, SIGNAL(returnPressed()), SLOT(slotApplyPropName()));

  edit = new QLineEdit(this);
  edit->setMinimumWidth(150);
  g->addWidget(edit,4,1);
  edit->setValidator(Validator);
  connect(edit, SIGNAL(returnPressed()), SLOT(slotApplyProperty()));

  // hide, because it only replaces 'edit' in some cases
  ComboEdit = new QComboBox(false,this);
  ComboEdit->setShown(false);
  g->addWidget(ComboEdit,4,1);
  connect(ComboEdit, SIGNAL(activated(const QString&)),
	  SLOT(slotApplyChange(const QString&)));

  QHBox *h3 = new QHBox(this);
  g->addWidget(h3,5,1);
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
  g->addWidget(disp,6,1);
  connect(disp, SIGNAL(stateChanged(int)), SLOT(slotApplyState(int)));

  QVBoxLayout *v = new QVBoxLayout(); // stretchable placeholder
  v->addStretch(2);
  g->addLayout(v,7,1);

  QHBox *h4 = new QHBox(this);
  h4->setSpacing(5);
  g->addMultiCellWidget(h4,8,8,1,1);
  ButtAdd = new QPushButton(tr("Add"),h4);
  ButtRem = new QPushButton(tr("Remove"),h4);
  connect(ButtAdd, SIGNAL(clicked()), SLOT(slotButtAdd()));
  connect(ButtRem, SIGNAL(clicked()), SLOT(slotButtRem()));

  // ------------------------------------------------------------
  Comp  = c;
  Doc   = d;
  label1->setText(c->Description);
  CompNameEdit->setText(c->Name);
  changed = transfered = false;

  c->TextSize(tx_Dist, ty_Dist);
  int tmp = c->tx+tx_Dist - c->x1;
  if((tmp > 0) || (tmp < -6))  tx_Dist = 0;  // remember the text position
  tmp = c->ty+ty_Dist - c->y1;
  if((tmp > 0) || (tmp < -6))  ty_Dist = 0;

//  prop->clear();

  QString s;
  // insert all properties into the ListBox
  for(Property *p = c->Props.last(); p != 0; p = c->Props.prev()) {
    if(p->display) s = tr("yes");
    else s = tr("no");
    QString str = p->Description;
    correctDesc (str);
    prop->insertItem(new QListViewItem(prop, p->Name, p->Value, s, str));
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
// Used to correct the given property description to handle special combobox
// items.  If 'clst' is not NULL, then an appropriate ComboBox string list
// is returned.
void ComponentDialog::correctDesc(QString &desc, QStringList *clst)
{
  int b, e;
  QString str;
  QStringList List;
  b = desc.find('[');
  e = desc.findRev(']');
  if (e-b > 2) {
    str = desc.mid(b+1, e-b-1);
    List = List.split(',',str);
    desc = desc.left(b)+"["+List.join(",")+"]"+desc.mid(e+1);
    for (QStringList::Iterator it = List.begin(); it != List.end(); ++it )
      (*it).replace( QRegExp("[^a-zA-Z0-9_]"), "" );
  }
  if(clst != 0) *clst = List;
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
  else {  // show standard line edit (description and value)
    ButtAdd->setEnabled(false);
    ButtRem->setEnabled(false);

    Name->setText(item->text(0));
    edit->setText(item->text(1));

    NameEdit->setShown(false);
    NameEdit->setText(item->text(0));  // perhaps used for adding properties
    Description->setShown(true);

    // handle special combobox items
    QStringList List;
    correctDesc( PropDesc, &List );

    QString s = PropDesc;
    QFontMetrics  metrics(QucsSettings.font);   // get size of text
    while(metrics.width(s) > 270) {  // if description too long, cut it
      if (s.findRev(' ') != -1)
	s = s.left(s.findRev(' ', -1)) + "....";
      else
	s = s.left(s.length()-5) + "....";
    }
    Description->setText(s);

    if(List.count() >= 1) {    // ComboBox with value list or line edit ?
      ComboEdit->clear();
      ComboEdit->insertStringList(List);

      for(int i=ComboEdit->count()-1; i>=0; i--)
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

  if(ComboEdit->isShown())   // take text from ComboBox ?
    edit->setText(ComboEdit->currentText());

  if(item->text(1) != edit->text()) {
    item->setText(1, edit->text());    // apply edit line
    changed = true;
  }
  if(NameEdit->isShown())	// also apply property name ?
    if(item->text(0) != NameEdit->text()) {
//      if(NameEdit->text() == "Export")
//        item->setText(0, "Export_");   // name must not be "Export" !!!
//      else
      item->setText(0, NameEdit->text());  // apply property name
      changed = true;
    }

  item = item->itemBelow();
  if(!item) {
    slotButtOK();   // close dialog, if it was the last property
    return;
  }

  prop->setSelected(item, true);
  prop->ensureItemVisible(item);
  slotSelectProperty(item);   // switch to the next property
}

// -------------------------------------------------------------------------
// Is called if the "RETURN"-button is pressed in the "NameEdit" Widget.
void ComponentDialog::slotApplyPropName()
{
  QListViewItem *item = prop->currentItem();
  if(item->text(0) != NameEdit->text()) {
//    if(NameEdit->text() == "Export") {
//	item->setText(0, "Export_");   // name must not be "Export" !!!
//	NameEdit->setText("Export_");
//    }
//      else
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
  Component *pc;
  if(CompNameEdit->text().isEmpty())  CompNameEdit->setText(Comp->Name);
  else
  if(CompNameEdit->text() != Comp->Name) {
    for(pc = Doc->Comps->first(); pc!=0; pc = Doc->Comps->next())
      if(pc->Name == CompNameEdit->text())
        break;  // found component with the same name ?
    if(pc)  CompNameEdit->setText(Comp->Name);
    else {
      Comp->Name = CompNameEdit->text();
      changed = true;
    }
  }

  QListViewItem *item = prop->firstChild();
 if(item != 0) {

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
 }
  transfered = true;     // applied changes to the component itself

  if(changed) {
    int dx, dy;
    Comp->TextSize(dx, dy);
    if(tx_Dist != 0) {
      Comp->tx += tx_Dist-dx;
      tx_Dist = dx;
    }
    if(ty_Dist != 0) {
      Comp->ty += ty_Dist-dy;
      ty_Dist = dy;
    }

    QString tmp = Comp->Name;  // is sometimes changed by "recreate"
    Doc->Comps->setAutoDelete(false);
    Doc->deleteComp(Comp);
    Comp->recreate();   // to apply changes to the schematic symbol
    Doc->insertRawComponent(Comp);
    Doc->Comps->setAutoDelete(true);
    Comp->Name = tmp;
    ((QucsView*)parent())->viewport()->repaint();
  }
}

// -------------------------------------------------------------------------
void ComponentDialog::slotBrowseFile()
{
  QString s = QFileDialog::getOpenFileName(QucsWorkDir.path(),
					tr("All Files")+" (*.*)",
					this, "", tr("Select a file"));
  if(!s.isEmpty()) {
    // snip path if file in current directory
    QFileInfo file(s);
    if(QucsWorkDir.exists(file.fileName()) &&
       QucsWorkDir.absPath() == file.dirPath(true)) s = file.fileName();
    edit->setText(s);
    changed = true;
  }
  prop->currentItem()->setText(1, s);
}

// -------------------------------------------------------------------------
void ComponentDialog::slotEditFile()
{
  Doc->App->editFile(QucsWorkDir.filePath(edit->text()));
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
  if(prop->childCount() < 3) return;  // the last property cannot be removed
  QListViewItem *item = prop->selectedItem();
  if(item == 0) return;

  QListViewItem *next_item = item->itemBelow();
  if(next_item == 0) next_item = item->itemAbove();
  prop->takeItem(item);     // remove from ListView
  delete item;              // delete item
  changed = true;

  slotSelectProperty(next_item);
}
