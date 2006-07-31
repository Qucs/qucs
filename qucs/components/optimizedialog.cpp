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
#include <qlistview.h>
#include <qtabwidget.h>
#include <qvalidator.h>
#include <qpushbutton.h>


OptimizeDialog::OptimizeDialog(Optimize_Sim *c_, Schematic *d_)
			: QDialog(d_, 0, TRUE, Qt::WDestructiveClose)
{
  Comp = c_;
  Doc  = d_;
  setCaption(tr("Edit Optimization Properties"));

  Expr.setPattern("[\\w_]+");
  Validator = new QRegExpValidator(Expr, this);

  all = new QVBoxLayout(this); // to provide the neccessary size
  QTabWidget *t = new QTabWidget(this);
  all->addWidget(t);

  // ...........................................................
  QWidget *Tab1 = new QWidget(t);
  QGridLayout *gp1 = new QGridLayout(Tab1,2,2,3,3);

  gp1->addWidget(new QLabel(tr("Name:"), Tab1), 0,0);
  NameEdit = new QLineEdit(Tab1);
  gp1->addWidget(NameEdit,0,1);


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

  gp2->addWidget(new QLabel(tr("Name:"), Tab2), 1,0);
  VarNameEdit = new QLineEdit(Tab2);
  gp2->addWidget(VarNameEdit,1,1);
  VarActiveCheck = new QCheckBox(tr("active"), Tab2);
  gp2->addWidget(VarActiveCheck,1,2);

  gp2->addWidget(new QLabel(tr("initial:"), Tab2), 2,0);
  gp2->addWidget(new QLabel(tr("min:"), Tab2), 2,1);
  gp2->addWidget(new QLabel(tr("max:"), Tab2), 2,2);
  VarInitEdit = new QLineEdit(Tab2);
  gp2->addWidget(VarInitEdit,3,0);
  VarMinEdit = new QLineEdit(Tab2);
  gp2->addWidget(VarMinEdit,3,1);
  VarMaxEdit = new QLineEdit(Tab2);
  gp2->addWidget(VarMaxEdit,3,2);

  QPushButton *AddVar_Butt = new QPushButton(tr("Add"), Tab2);
  gp2->addWidget(AddVar_Butt,4,1);
  connect(AddVar_Butt, SIGNAL(clicked()), SLOT(slotAddVariable()));
  QPushButton *DelVar_Butt = new QPushButton(tr("Delete"), Tab2);
  gp2->addWidget(DelVar_Butt,4,2);
  connect(DelVar_Butt, SIGNAL(clicked()), SLOT(slotDeleteVariable()));


  t->addTab(Tab2, tr("Variables"));

  // ...........................................................
  QWidget *Tab3 = new QWidget(t);
  QGridLayout *gp3 = new QGridLayout(Tab3,5,2,3,3);

  GoalList = new QListView(Tab3);
  GoalList->addColumn(tr("Name"));
  GoalList->addColumn(tr("Goal"));
  gp3->addMultiCellWidget(GoalList,0,0,0,1);
  connect(GoalList, SIGNAL(clicked(QListViewItem*)),
                    SLOT(slotEditGoal(QListViewItem*)));

  QPushButton *AddGoal_Butt = new QPushButton(tr("Set"), Tab3);
  gp3->addWidget(AddGoal_Butt,1,1);
  connect(AddGoal_Butt, SIGNAL(clicked()), SLOT(slotAddGoal()));
  QPushButton *DelGoal_Butt = new QPushButton(tr("Remove"), Tab3);
  gp3->addWidget(DelGoal_Butt,2,1);
  connect(DelGoal_Butt, SIGNAL(clicked()), SLOT(slotDeleteGoal()));

//  gp3->setRowStretch(4,5);
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
  connect(CancelButt, SIGNAL(clicked()), SLOT(reject()));

  OkButt->setDefault(true);

  // ...........................................................

  resize(300, 200);
}

OptimizeDialog::~OptimizeDialog()
{
  delete all;
  delete Validator;
}

// -----------------------------------------------------------
void OptimizeDialog::slotEditVariable(QListViewItem*)
{
}

// -----------------------------------------------------------
void OptimizeDialog::slotEditGoal(QListViewItem*)
{
}

// -----------------------------------------------------------
void OptimizeDialog::slotAddVariable()
{
}

// -----------------------------------------------------------
void OptimizeDialog::slotDeleteVariable()
{
}

// -----------------------------------------------------------
void OptimizeDialog::slotAddGoal()
{
/*  QListViewItem *Item = List_Suffix->selectedItem();
  if(Item) {
    Item->setText(0, Input_Suffix->text());
    Item->setText(1, Input_Program->text());
    return;
  }


  for(Item = List_Suffix->firstChild(); Item!=0; Item = Item->itemBelow())
    if(Item->text(0) == Input_Suffix->text()) {
      QMessageBox::critical(this, tr("Error"),
			tr("This suffix is already registered!"));
      return;
    }

  List_Suffix->ensureItemVisible(
      new QListViewItem(List_Suffix, List_Suffix->lastItem(),
          Input_Suffix->text(), Input_Program->text()));
  Input_Suffix->setFocus();
  Input_Suffix->setText("");
  Input_Program->setText("");*/
}

// -----------------------------------------------------------
void OptimizeDialog::slotDeleteGoal()
{
/*  QListViewItem *Item = List_Suffix->selectedItem();
  if(Item == 0) return;

  List_Suffix->takeItem(Item);   // remove from ListView
  delete Item;

  Input_Suffix->setText("");
  Input_Program->setText("");*/
}

// -----------------------------------------------------------
void OptimizeDialog::slotOK()
{
  slotApply();
  accept();
}

// -----------------------------------------------------------
void OptimizeDialog::slotApply()
{
/*  bool changed = false;

  if(QucsSettings.BGColor != BGColorButton->paletteBackgroundColor()) {
    QucsSettings.BGColor = BGColorButton->paletteBackgroundColor();

    int No=0;
    QWidget *w;
    while((w=App->DocumentTab->page(No++)) != 0)
      if(w->inherits("QTextEdit"))
        ((TextDoc*)w)->viewport()->setPaletteBackgroundColor(
					QucsSettings.BGColor);
      else
        ((Schematic*)w)->viewport()->setPaletteBackgroundColor(
					QucsSettings.BGColor);
    changed = true;
  }

  if(savingFont != Font) {
    savingFont = Font;
    changed = true;
  }

  QucsSettings.Language =
      LanguageCombo->currentText().section('(',1,1).remove(')');

  if(QucsSettings.VHDL_Comment != ColorComment->paletteForegroundColor()) {
    QucsSettings.VHDL_Comment = ColorComment->paletteForegroundColor();
    changed = true;
  }
  if(QucsSettings.VHDL_String != ColorString->paletteForegroundColor()) {
    QucsSettings.VHDL_String = ColorString->paletteForegroundColor();
    changed = true;
  }
  if(QucsSettings.VHDL_Integer != ColorInteger->paletteForegroundColor()) {
    QucsSettings.VHDL_Integer = ColorInteger->paletteForegroundColor();
    changed = true;
  }
  if(QucsSettings.VHDL_Real != ColorReal->paletteForegroundColor()) {
    QucsSettings.VHDL_Real = ColorReal->paletteForegroundColor();
    changed = true;
  }
  if(QucsSettings.VHDL_Character != ColorCharacter->paletteForegroundColor()) {
    QucsSettings.VHDL_Character = ColorCharacter->paletteForegroundColor();
    changed = true;
  }
  if(QucsSettings.VHDL_Types != ColorDataType->paletteForegroundColor()) {
    QucsSettings.VHDL_Types = ColorDataType->paletteForegroundColor();
    changed = true;
  }
  if(QucsSettings.VHDL_Attributes != ColorAttributes->paletteForegroundColor()) {
    QucsSettings.VHDL_Attributes = ColorAttributes->paletteForegroundColor();
    changed = true;
  }

  bool ok;
  if(QucsSettings.maxUndo != undoNumEdit->text().toUInt(&ok)) {
    QucsSettings.maxUndo = undoNumEdit->text().toInt(&ok);
    changed = true;
  }
  if(QucsSettings.Editor != editorEdit->text()) {
    QucsSettings.Editor = editorEdit->text();
    changed = true;
  }

  QListViewItem *Item;
  QucsSettings.FileTypes.clear();
  for(Item = List_Suffix->firstChild(); Item!=0; Item = Item->itemBelow())
    QucsSettings.FileTypes.append(Item->text(0)+"/"+Item->text(1));


  saveApplSettings(App);  // also sets the small and large font
  if(changed)
    App->repaint();*/
}
