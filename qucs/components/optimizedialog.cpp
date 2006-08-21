/***************************************************************************
                            optimizedialog.cpp
                           --------------------
    begin                : Sun Jul 30 2006
    copyright            : (C) 2006 by Michael Margraf
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

#include "optimizedialog.h"
#include "opt_sim.h"
#include "schematic.h"

#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlistview.h>
#include <qtabwidget.h>
#include <qvalidator.h>
#include <qpushbutton.h>
#include <qmessagebox.h>


OptimizeDialog::OptimizeDialog(Optimize_Sim *c_, Schematic *d_)
			: QDialog(d_, 0, TRUE, Qt::WDestructiveClose)
{
  Comp = c_;
  Doc  = d_;
  changed = false;
  setCaption(tr("Edit Optimization Properties"));

  Expr.setPattern("[\\w_]+");
  Validator = new QRegExpValidator(Expr, this);
  numVal = new QDoubleValidator(this);

  all = new QVBoxLayout(this); // to provide the neccessary size
  QTabWidget *t = new QTabWidget(this);
  all->addWidget(t);

  // ...........................................................
  QWidget *Tab1 = new QWidget(t);
  QGridLayout *gp1 = new QGridLayout(Tab1,3,2,3,3);

  gp1->addWidget(new QLabel(tr("Name:"), Tab1), 0,0);
  NameEdit = new QLineEdit(Tab1);
  NameEdit->setValidator(Validator);
  gp1->addWidget(NameEdit,0,1);

  gp1->addWidget(new QLabel(tr("Simulation:"), Tab1), 1,0);
  SimEdit = new QComboBox(Tab1);
  SimEdit->setEditable(true);
  gp1->addWidget(SimEdit,1,1);


  t->addTab(Tab1, tr("General"));

  // ...........................................................
  QWidget *Tab2 = new QWidget(t);
  QGridLayout *gp2 = new QGridLayout(Tab2,5,3,3,3);

  VarList = new QListView(Tab2);
  VarList->addColumn(tr("Name"));
  VarList->addColumn(tr("active"));
  VarList->addColumn(tr("initial"));
  VarList->addColumn(tr("min"));
  VarList->addColumn(tr("max"));
  gp2->addMultiCellWidget(VarList,0,0,0,2);
  connect(VarList, SIGNAL(clicked(QListViewItem*)),
                   SLOT(slotEditVariable(QListViewItem*)));

  QHBox *VarLine = new QHBox(Tab2);
  VarLine->setSpacing(3);
  gp2->addMultiCellWidget(VarLine, 1,1,0,2);

  new QLabel(tr("Name:"), VarLine);
  VarNameEdit = new QLineEdit(VarLine);
  VarNameEdit->setValidator(Validator);
  connect(VarNameEdit, SIGNAL(textChanged(const QString&)),
          SLOT(slotChangeVarName(const QString&)));
  VarActiveCheck = new QCheckBox(tr("active"), VarLine);
  VarActiveCheck->setChecked(true);
  connect(VarActiveCheck, SIGNAL(toggled(bool)),
          SLOT(slotChangeVarActive(bool)));

  gp2->addWidget(new QLabel(tr("initial:"), Tab2), 2,0);
  gp2->addWidget(new QLabel(tr("min:"), Tab2), 2,1);
  gp2->addWidget(new QLabel(tr("max:"), Tab2), 2,2);
  VarInitEdit = new QLineEdit(Tab2);
  VarInitEdit->setValidator(numVal);
  gp2->addWidget(VarInitEdit,3,0);
  connect(VarInitEdit, SIGNAL(textChanged(const QString&)),
          SLOT(slotChangeVarInit(const QString&)));
  VarMinEdit = new QLineEdit(Tab2);
  VarMinEdit->setValidator(numVal);
  gp2->addWidget(VarMinEdit,3,1);
  connect(VarMinEdit, SIGNAL(textChanged(const QString&)),
          SLOT(slotChangeVarMin(const QString&)));
  VarMaxEdit = new QLineEdit(Tab2);
  VarMaxEdit->setValidator(numVal);
  gp2->addWidget(VarMaxEdit,3,2);
  connect(VarMaxEdit, SIGNAL(textChanged(const QString&)),
          SLOT(slotChangeVarMax(const QString&)));

  QHBox *VarButtons = new QHBox(Tab2);
  VarButtons->setSpacing(3);
  gp2->addMultiCellWidget(VarButtons, 4,4,0,2);

  VarButtons->setStretchFactor(new QWidget(VarButtons),5);
  QPushButton *AddVar_Butt = new QPushButton(tr("Add"), VarButtons);
  connect(AddVar_Butt, SIGNAL(clicked()), SLOT(slotAddVariable()));
  QPushButton *DelVar_Butt = new QPushButton(tr("Delete"), VarButtons);
  connect(DelVar_Butt, SIGNAL(clicked()), SLOT(slotDeleteVariable()));


  t->addTab(Tab2, tr("Variables"));

  // ...........................................................
  QWidget *Tab3 = new QWidget(t);
  QGridLayout *gp3 = new QGridLayout(Tab3,4,3,3,3);

  GoalList = new QListView(Tab3);
  GoalList->addColumn(tr("Name"));
  GoalList->addColumn(tr("Value"));
  GoalList->addColumn(tr("Type"));
  gp3->addMultiCellWidget(GoalList,0,0,0,2);
  connect(GoalList, SIGNAL(clicked(QListViewItem*)),
                    SLOT(slotEditGoal(QListViewItem*)));

  gp3->addWidget(new QLabel(tr("Name:"), Tab3), 1,0);
  GoalNameEdit = new QLineEdit(Tab3);
  GoalNameEdit->setValidator(Validator);
  gp3->addWidget(GoalNameEdit,1,1);
  connect(GoalNameEdit, SIGNAL(textChanged(const QString&)),
          SLOT(slotChangeGoalName(const QString&)));

  gp3->addWidget(new QLabel(tr("Value:"), Tab3), 2,0);
  GoalNumEdit = new QLineEdit(Tab3);
  GoalNumEdit->setValidator(numVal);
  gp3->addWidget(GoalNumEdit,2,1);
  connect(GoalNumEdit, SIGNAL(textChanged(const QString&)),
          SLOT(slotChangeGoalNum(const QString&)));

  GoalTypeCombo = new QComboBox(Tab3);
  GoalTypeCombo->insertItem(tr("minimum"));
  GoalTypeCombo->insertItem(tr("maximum"));
  GoalTypeCombo->insertItem(tr("less"));
  GoalTypeCombo->insertItem(tr("greater"));
  GoalTypeCombo->insertItem(tr("equal"));
  gp3->addWidget(GoalTypeCombo,2,2);
  connect(GoalTypeCombo, SIGNAL(activated(const QString&)),
          SLOT(slotChangeGoalType(const QString&)));

  QHBox *GoalButtons = new QHBox(Tab3);
  GoalButtons->setSpacing(3);
  gp3->addMultiCellWidget(GoalButtons, 3,3,0,2);

  GoalButtons->setStretchFactor(new QWidget(GoalButtons),5);
  QPushButton *AddGoal_Butt = new QPushButton(tr("Add"), GoalButtons);
  connect(AddGoal_Butt, SIGNAL(clicked()), SLOT(slotAddGoal()));
  QPushButton *DelGoal_Butt = new QPushButton(tr("Delete"), GoalButtons);
  connect(DelGoal_Butt, SIGNAL(clicked()), SLOT(slotDeleteGoal()));

  t->addTab(Tab3, tr("Goals"));

  // ...........................................................
  // buttons on the bottom of the dialog (independent of the TabWidget)
  QHBox *Butts = new QHBox(this);
  Butts->setSpacing(3);
  Butts->setMargin(3);
  all->addWidget(Butts);

  QPushButton *OkButt = new QPushButton(tr("OK"), Butts);
  connect(OkButt, SIGNAL(clicked()), SLOT(slotOK()));
  QPushButton *ApplyButt = new QPushButton(tr("Apply"), Butts);
  connect(ApplyButt, SIGNAL(clicked()), SLOT(slotApply()));
  QPushButton *CancelButt = new QPushButton(tr("Cancel"), Butts);
  connect(CancelButt, SIGNAL(clicked()), SLOT(slotCancel()));

  OkButt->setFocus();

  // ...........................................................

  Component *pc;
  for(pc=Doc->Components->first(); pc!=0; pc=Doc->Components->next())
    if(pc != Comp)
      if(pc->Model[0] == '.')
        SimEdit->insertItem(pc->Name);
  if(!Comp->Props.getLast()->Value.isEmpty())
    SimEdit->setCurrentText(Comp->Props.getLast()->Value);

  NameEdit->setText(Comp->Name);

  Property *pp;
  for(pp = Comp->Props.first(); pp != 0; pp = Comp->Props.next()) {
    if(pp->Name != "Var") break;
    new QListViewItem(VarList, pp->Value.section('|',0,0),
        pp->Value.section('|',1,1), pp->Value.section('|',2,2),
        pp->Value.section('|',3,3), pp->Value.section('|',4,4));
  }
  for( ; pp != 0; pp = Comp->Props.next()) {
    if(pp->Name != "Goal") break;
    new QListViewItem(GoalList, pp->Value.section('|',0,0),
        pp->Value.section('|',1,1), pp->Value.section('|',2,2));
  }

  resize(300, 200);
}

OptimizeDialog::~OptimizeDialog()
{
  delete all;
  delete numVal;
  delete Validator;
}

// -----------------------------------------------------------
void OptimizeDialog::slotEditVariable(QListViewItem *Item)
{
  VarNameEdit->blockSignals(true);

  if(Item == 0) {
    VarNameEdit->clear();
    VarActiveCheck->setChecked(true);
    VarInitEdit->clear();
    VarMinEdit->clear();
    VarMaxEdit->clear();
    VarNameEdit->blockSignals(false);
    return;
  }

  VarNameEdit->setText(Item->text(0));
  VarActiveCheck->setChecked(Item->text(1) == tr("yes"));
  VarInitEdit->setText(Item->text(2));
  VarMinEdit->setText(Item->text(3));
  VarMaxEdit->setText(Item->text(4));
  VarNameEdit->blockSignals(false);
}

// -----------------------------------------------------------
void OptimizeDialog::slotAddVariable()
{
  if(VarNameEdit->text().isEmpty() || VarInitEdit->text().isEmpty() ||
        VarMinEdit->text().isEmpty() || VarMaxEdit->text().isEmpty()) {
    QMessageBox::critical(this, tr("Error"),
       tr("Every text field must be non-empty!"));
    return;
  }

  QListViewItem *item;
  for(item = VarList->firstChild(); item != 0; item = item->itemBelow())
    if(item->text(0) == VarNameEdit->text()) {
      QMessageBox::critical(this, tr("Error"),
         tr("Variable \"%1\" already in list!").arg(VarNameEdit->text()));
      return;
    }


  new QListViewItem(VarList, VarNameEdit->text(),
      VarActiveCheck->isChecked() ? tr("yes") : tr("no"),
      VarInitEdit->text(), VarMinEdit->text(), VarMaxEdit->text());

  slotEditVariable(0);   // clear entry fields
  VarList->clearSelection();
}

// -----------------------------------------------------------
void OptimizeDialog::slotDeleteVariable()
{
  QListViewItem *next_item = 0;

  QListViewItem *Item = VarList->selectedItem();
  if(Item) {
    next_item = Item->itemBelow();
    if(next_item == 0) next_item = Item->itemAbove();
    VarList->takeItem(Item);  // remove from ListView
    delete Item;              // delete item
  }

  slotEditVariable(next_item);
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeVarActive(bool On)
{
  QListViewItem *Item = VarList->selectedItem();
  if(Item == 0) return;

  Item->setText(1, On ? tr("yes") : tr("no"));
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeVarName(const QString&)
{
  VarList->clearSelection();
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeVarInit(const QString& Text)
{
  QListViewItem *Item = VarList->selectedItem();
  if(Item == 0) return;

  Item->setText(2, Text);
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeVarMin(const QString& Text)
{
  QListViewItem *Item = VarList->selectedItem();
  if(Item == 0) return;

  Item->setText(3, Text);
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeVarMax(const QString& Text)
{
  QListViewItem *Item = VarList->selectedItem();
  if(Item == 0) return;

  Item->setText(4, Text);
}

// -----------------------------------------------------------
void OptimizeDialog::slotEditGoal(QListViewItem *Item)
{
  GoalNameEdit->blockSignals(true);

  if(Item == 0) {
    GoalNameEdit->clear();
    GoalTypeCombo->setCurrentItem(0);
    GoalNumEdit->clear();
    GoalNameEdit->blockSignals(false);
    return;
  }

  GoalNameEdit->setText(Item->text(0));
  GoalNumEdit->setText(Item->text(1));
  GoalTypeCombo->setCurrentText(Item->text(2));
  GoalNameEdit->blockSignals(false);
}

// -----------------------------------------------------------
void OptimizeDialog::slotAddGoal()
{
  if(GoalNameEdit->text().isEmpty() || GoalNumEdit->text().isEmpty()) {
    QMessageBox::critical(this, tr("Error"),
       tr("Every text field must be non-empty!"));
    return;
  }

  QListViewItem *item;
  for(item = GoalList->firstChild(); item != 0; item = item->itemBelow())
    if(item->text(0) == GoalNameEdit->text()) {
      QMessageBox::critical(this, tr("Error"),
         tr("Goal \"%1\" already in list!").arg(GoalNameEdit->text()));
      return;
    }


  new QListViewItem(GoalList, GoalNameEdit->text(),
      GoalNumEdit->text(), GoalTypeCombo->currentText());

  slotEditGoal(0);    // clear entry fields
  GoalList->clearSelection();
}

// -----------------------------------------------------------
void OptimizeDialog::slotDeleteGoal()
{
  QListViewItem *next_item = 0;

  QListViewItem *Item = GoalList->selectedItem();
  if(Item) {
    next_item = Item->itemBelow();
    if(next_item == 0) next_item = Item->itemAbove();
    GoalList->takeItem(Item); // remove from ListView
    delete Item;              // delete item
  }

  slotEditGoal(next_item);
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeGoalName(const QString&)
{
  GoalList->clearSelection();
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeGoalNum(const QString& Text)
{
  QListViewItem *Item = GoalList->selectedItem();
  if(Item == 0) return;

  Item->setText(1, Text);
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeGoalType(const QString& Text)
{
  QListViewItem *Item = GoalList->selectedItem();
  if(Item == 0) return;

  Item->setText(2, Text);
}

// -----------------------------------------------------------
void OptimizeDialog::slotOK()
{
  slotApply();
  slotCancel();
}

// -----------------------------------------------------------
void OptimizeDialog::slotApply()
{
  Component *pc;
  if(NameEdit->text().isEmpty())
    NameEdit->setText(Comp->Name);
  else
  if(NameEdit->text() != Comp->Name) {
    for(pc = Doc->Components->first(); pc!=0; pc = Doc->Components->next())
      if(pc->Name == NameEdit->text())
        break;  // found component with the same name ?
    if(pc)
      NameEdit->setText(Comp->Name);
    else {
      Comp->Name = NameEdit->text();
      changed = true;
    }
  }

  if(SimEdit->currentText() != Comp->Props.last()->Value)
    changed = true;
  Comp->Props.remove();   // remove last property


  QString Prop;
  QListViewItem *item;
  Property *pp = Comp->Props.first();
  // apply all the new property values in the ListView
  for(item = VarList->firstChild(); item != 0; item = item->itemBelow()) {
    Prop = item->text(0) + "|" + item->text(1) + "|" +
           item->text(2) + "|" + item->text(3) + "|" +
           item->text(4);

    if(pp) {
      if(pp->Name != "Var") {
        pp->Name = "Var";
        changed = true;
      }
      if(pp->Value != Prop) {
        pp->Value = Prop;
        changed = true;
      }
    }
    else {
      Comp->Props.append(new Property("Var", Prop, false, ""));
      changed = true;
    }
    pp = Comp->Props.next();
  }

  for(item = GoalList->firstChild(); item != 0; item = item->itemBelow()) {
    Prop = item->text(0) + "|" + item->text(1) + "|" +
           item->text(2);

    if(pp) {
      if(pp->Name != "Goal") {
        pp->Name = "Goal";
        changed = true;
      }
      if(pp->Value != Prop) {
        pp->Value = Prop;
        changed = true;
      }
    }
    else {
      Comp->Props.append(new Property("Goal", Prop, false, ""));
      changed = true;
    }
    pp = Comp->Props.next();
  }

  // if more properties than in ListView -> delete the rest
  if(pp) {
    pp = Comp->Props.prev();
    Comp->Props.last();
    while(pp != Comp->Props.current())
      Comp->Props.remove();
    changed = true;
  }

  Comp->Props.append(new Property("Sim", SimEdit->currentText(), false, ""));

  if(changed)
    Doc->viewport()->repaint();
}

// -------------------------------------------------------------------------
// Is called if the "Cancel"-button is pressed.
void OptimizeDialog::slotCancel()
{
  if(changed) done(1); // changed could have been done before
  else done(0);        // (by "Apply"-button)
}
