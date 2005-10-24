/***************************************************************************
                               changedialog.cpp
                              ------------------
    begin                : Fri Jul 22 2005
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

#include "changedialog.h"
#include "node.h"
#include "qucsdoc.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qvalidator.h>
#include <qpushbutton.h>
#include <qscrollview.h>
#include <qvbox.h>
#include <qcheckbox.h>
#include <qmessagebox.h>


ChangeDialog::ChangeDialog(QucsDoc *d, QWidget *parent)
			: QDialog(parent, 0, TRUE, Qt::WDestructiveClose)
{
  setCaption(tr("Change Component Properties"));
  Doc = d;

  Expr.setPattern("[^\"=]+");  // valid expression for property value
  Validator = new QRegExpValidator(Expr, this);
  Expr.setPattern("[\\w_]+");  // valid expression for property name
  ValRestrict = new QRegExpValidator(Expr, this);


  // ...........................................................
  all = new QGridLayout(this, 5,2,3,3);

  all->addWidget(new QLabel(tr("Components:"), this), 0,0);
  CompTypeEdit = new QComboBox(this);
  CompTypeEdit->insertItem(tr("all components"));
  CompTypeEdit->insertItem(tr("resistors"));
  CompTypeEdit->insertItem(tr("capacitors"));
  CompTypeEdit->insertItem(tr("inductors"));
  CompTypeEdit->insertItem(tr("transistors"));
  all->addWidget(CompTypeEdit, 0,1);

  all->addWidget(new QLabel(tr("Component Names:"), this), 1,0);
  CompNameEdit = new QLineEdit(this);
  CompNameEdit->setValidator(Validator);
  CompNameEdit->setText("*");
  all->addWidget(CompNameEdit, 1,1);
//  connect(CompNameEdit, SIGNAL(returnPressed()), SLOT(slotButtReplace()));

  all->addWidget(new QLabel(tr("Property Name:"), this), 2,0);
  PropNameEdit = new QComboBox(this);
  PropNameEdit->setEditable(true);
  PropNameEdit->setValidator(ValRestrict);
  PropNameEdit->insertItem("Temp");
  PropNameEdit->insertItem("Subst");
  PropNameEdit->insertItem("Model");
  all->addWidget(PropNameEdit, 2,1);
//  connect(PropNameEdit, SIGNAL(activated(int)), SLOT(slotButtReplace()));

  all->addWidget(new QLabel(tr("New Value:"), this), 3,0);
  NewValueEdit = new QLineEdit(this);
  NewValueEdit->setValidator(Validator);
  NewValueEdit->setText("-273.15");
  all->addWidget(NewValueEdit, 3,1);
//  connect(NewValueEdit, SIGNAL(returnPressed()), SLOT(slotButtReplace()));

  // ...........................................................
  QHBox *h0 = new QHBox(this);
  h0->setSpacing(5);
  all->addMultiCellWidget(h0, 4,4, 0,1);
  connect(new QPushButton(tr("Replace"),h0), SIGNAL(clicked()),
	  SLOT(slotButtReplace()));
  connect(new QPushButton(tr("Cancel"),h0), SIGNAL(clicked()),
	  SLOT(reject()));
}

ChangeDialog::~ChangeDialog()
{
  delete all;
  delete Validator;
  delete ValRestrict;
}

// -----------------------------------------------------------------------
// Returns "true" if the component model matches the user selection
// in "CompTypeEdit".
bool ChangeDialog::matches(const QString& CompModel)
{
  switch(CompTypeEdit->currentItem()) {
    case 0: return true;
    case 1: if(CompModel == "R") return true;
            return false;
    case 2: if(CompModel == "C") return true;
            return false;
    case 3: if(CompModel == "L") return true;
            return false;
    case 4: if(CompModel == "BJT") return true;
            if(CompModel == "_BJT") return true;
            if(CompModel == "JFET") return true;
            if(CompModel == "MOSFET") return true;
            if(CompModel == "_MOSFET") return true;
            return false;
  }

  return false;
}

// -----------------------------------------------------------------------
// Is called if the "Replace"-button is pressed.
void ChangeDialog::slotButtReplace()
{
  Expr.setWildcard(true);  // switch into wildcard mode
  Expr.setPattern(CompNameEdit->text());
/*  if(!Expr.isValid()) {
    QMessageBox::critical(this, tr("Error"),
	  tr("Regular expression for component name is invalid."));
    return;
  }*/


  // create dialog showing all found components
  QDialog *Dia = new QDialog(this);
  Dia->setCaption(tr("Found Components"));
  QVBoxLayout *Dia_All = new QVBoxLayout(Dia);
  Dia_All->setSpacing(3);
  QScrollView *Dia_Scroll = new QScrollView(Dia);
  Dia_Scroll->setMargin(5);
  Dia_All->addWidget(Dia_Scroll);
  QVBox *Dia_Box = new QVBox(Dia_Scroll->viewport());
  Dia_Scroll->addChild(Dia_Box);
  QLabel *Dia_Label = new QLabel(tr("Change properties of\n")
                               + tr("these components ?"), Dia);
  Dia_All->addWidget(Dia_Label);
  
  QHBox *Dia_h = new QHBox(Dia);
  Dia_h->setSpacing(5);
  Dia_All->addWidget(Dia_h);
  connect(new QPushButton(tr("Yes"), Dia_h), SIGNAL(clicked()),
	  Dia, SLOT(accept()));
  connect(new QPushButton(tr("Cancel"), Dia_h), SIGNAL(clicked()),
	  Dia, SLOT(reject()));


  QPtrList<QCheckBox> pList;
  Component * pc;
  // search through all components
  for(pc = Doc->Comps->first(); pc!=0; pc = Doc->Comps->next()) {
    if(matches(pc->Model)) {
      if(Expr.search(pc->Name) >= 0)
        for(Property *pp = pc->Props.first(); pp!=0; pp = pc->Props.next())
          if(pp->Name == PropNameEdit->currentText()) {
            pList.append(new QCheckBox(pc->Name, Dia_Box));
            pList.current()->setChecked(true);
            break;
          }
    }
  }
  
  if(!pList.isEmpty())
    Dia_Scroll->viewport()->setPaletteBackgroundColor(
        pList.current()->paletteBackgroundColor());
  Dia->resize(50, 300);


  // show user all components found
  int Result = Dia->exec();
  if(Result != QDialog::Accepted) return;


  bool changed = false;
  // change property values
  pList.first();
  for(pc = Doc->Comps->first(); pc!=0; pc = Doc->Comps->next()) {
    if(matches(pc->Model)) {
      if(Expr.search(pc->Name) >= 0)
        for(Property *pp = pc->Props.first(); pp!=0; pp = pc->Props.next())
          if(pp->Name == PropNameEdit->currentText())
            if(pList.current()->isChecked()) {
              pp->Value = NewValueEdit->text();
              pList.next();
              changed = true;
              break;
            }
            else pList.next();
    }
  }

  delete Dia_All;
  delete Dia;
  if(changed) accept();
  else reject();
}
