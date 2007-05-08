/***************************************************************************
                              librarydialog.cpp
                             -------------------
    begin                : Sun Jun 04 2006
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

#include "librarydialog.h"
#include "qucs.h"
#include "main.h"
#include "schematic.h"

#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qcheckbox.h>
#include <qlistview.h>
#include <qvalidator.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qscrollview.h>
#include <qvbuttongroup.h>

extern QStringList StringList;

LibraryDialog::LibraryDialog(QucsApp *App_, QListViewItem *SchematicList)
			: QDialog(App_, 0, TRUE, Qt::WDestructiveClose)
{
  App = App_;
  setCaption(tr("Create Library"));

  Expr.setPattern("[\\w_]+");
  Validator = new QRegExpValidator(Expr, this);

  // ...........................................................
  all = new QVBoxLayout(this);
  all->setMargin(5);
  all->setSpacing(6);

  QHBox *h1 = new QHBox(this);
  all->addWidget(h1);
  theLabel = new QLabel(tr("Library Name:"), h1);
  NameEdit = new QLineEdit(h1);
  NameEdit->setValidator(Validator);

  Group = new QVButtonGroup(tr("Choose subcircuits:"), this);
  all->addWidget(Group);
  
  QScrollView *Dia_Scroll = new QScrollView(Group);
  Dia_Scroll->setMargin(5);
  QVBox *Dia_Box = new QVBox(Dia_Scroll->viewport());
  Dia_Scroll->addChild(Dia_Box);

  ErrText = new QTextEdit(this);
  ErrText->setHidden(true);
  ErrText->setTextFormat(Qt::PlainText);
  ErrText->setWordWrap(QTextEdit::NoWrap);
  all->addWidget(ErrText);
  
  // ...........................................................
  QHBox *h2 = new QHBox(this);
  all->addWidget(h2);
  ButtCancel = new QPushButton(tr("Cancel"), h2);
  connect(ButtCancel, SIGNAL(clicked()), SLOT(reject()));
  ButtCreate = new QPushButton(tr("Next >>"), h2);
  connect(ButtCreate, SIGNAL(clicked()), SLOT(slotCreate()));
  ButtCreate->setDefault(true);

  // ...........................................................
  // insert all subcircuits of current project
  QListViewItem *p = SchematicList->firstChild();
  while(p) {
    if(p->parent() == 0)
      break;
    if(!p->text(1).isEmpty())
      BoxList.append(new QCheckBox(p->text(0), Dia_Box));
    p = p->nextSibling();
  }

  QColor theColor;
  if(BoxList.isEmpty()) {
    ButtCreate->setEnabled(false);
    theColor =
       (new QLabel(tr("No subcircuits!"), Dia_Box))->paletteBackgroundColor();
  }
  else
    theColor = BoxList.current()->paletteBackgroundColor();
  Dia_Scroll->viewport()->setPaletteBackgroundColor(theColor);
}

LibraryDialog::~LibraryDialog()
{
  delete all;
  delete Validator;
}

// ---------------------------------------------------------------
void LibraryDialog::slotCreate()
{
  if(NameEdit->text().isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("Please insert a library name!"));
    return;
  }

  int count=0;
  QCheckBox *p;
  for(p = BoxList.first(); p != 0; p = BoxList.next())
    if(p->isChecked())
      count++;

  if(count < 1) {
    QMessageBox::critical(this, tr("Error"), tr("Please choose at least one subcircuit!"));
    return;
  }


  QDir LibDir(QucsHomeDir);
  if(!LibDir.cd("user_lib")) { // user library directory exists ?
    if(!LibDir.mkdir("user_lib")) { // no, then create it
      QMessageBox::warning(this, tr("Warning"),
                   tr("Cannot create user library directory !"));
      return;
    }
    LibDir.cd("user_lib");
  }


  LibFile.setName(QucsSettings.LibDir + NameEdit->text() + ".lib");
  if(LibFile.exists()) {
    QMessageBox::critical(this, tr("Error"), tr("A system library with this name already exists!"));
    return;
  }

  LibFile.setName(LibDir.absFilePath(NameEdit->text()) + ".lib");
  if(LibFile.exists()) {
    QMessageBox::critical(this, tr("Error"), tr("A library with this name already exists!"));
    return;
  }

  Group->setHidden(true);
  ErrText->setHidden(false);
  NameEdit->setHidden(true);
  disconnect(ButtCreate, SIGNAL(clicked()), 0, 0);
  connect(ButtCreate, SIGNAL(clicked()), SLOT(slotNext()));

  for(p = BoxList.first(); p != 0; p = BoxList.next())
    if(p->isChecked())
      break;
  theLabel->setText(tr("Enter description for \"%1\":").arg(p->text()));

  for(p = BoxList.next(); p != 0; p = BoxList.next())
    if(p->isChecked())
      break;
  if(p == 0)
    ButtCreate->setText(tr("Create"));
}

// ---------------------------------------------------------------
void LibraryDialog::slotNext()
{
  Descriptions.append(ErrText->text());
  ErrText->clear();

  QCheckBox *p = BoxList.current();
  if(p) {
    theLabel->setText(tr("Enter description for \"%1\":").arg(p->text()));
    for(p = BoxList.next(); p != 0; p = BoxList.next())
      if(p->isChecked())
        break;
    if(p == 0)
      ButtCreate->setText(tr("Create"));
    return;
  }

  theLabel->setShown(false);
  ErrText->setReadOnly(true);
  ButtCancel->setEnabled(false);
  ButtCreate->setText(tr("Close"));
  disconnect(ButtCreate, SIGNAL(clicked()), 0, 0);
  connect(ButtCreate, SIGNAL(clicked()), SLOT(accept()));

  if(!LibFile.open(IO_WriteOnly)) {
    ErrText->append(tr("Error: Cannot create library!"));
    return;
  }
  QTextStream Stream;
  Stream.setDevice(&LibFile);
  Stream << "<Qucs Library " PACKAGE_VERSION " \"" << NameEdit->text() << "\">\n\n";


  int countInit = 0;
  bool Success = true, ret;
  QStringList Collect;
  QStringList::Iterator it = Descriptions.begin();
  QString tmp;
  QTextStream ts(&tmp, IO_WriteOnly);
  for(p = BoxList.first(); p != 0; p = BoxList.next())
    if(p->isChecked()) {
      Stream << "<Component " + p->text().section('.',0,0) + ">\n"
             << "  <Description>\n"
             << *(it++)
             << "\n  </Description>\n";

      Schematic *Doc = new Schematic(0, QucsWorkDir.filePath(p->text()));
      if(!Doc->loadDocument()) {  // load document if possible
        delete Doc;
        ErrText->append(tr("Error: Cannot load subcircuit \"%1\".").arg(p->text()));
        break;
      }
      Doc->DocName = NameEdit->text() + "_" + p->text();
      Success = false;

      // save analog model
      tmp = "";
      countInit = 0;
      Collect.clear();
      StringList.clear();
      ret = Doc->createSubNetlist(&ts, countInit, Collect, ErrText, -1);
      if(ret) {
	Stream << "  <Model>";
	Stream << tmp;
	Stream << "  </Model>\n";
	Success = true;
      } else {
	ErrText->insert("\n");
      }

      // save verilog model
      tmp = "";
      countInit = 0;
      Collect.clear();
      StringList.clear();
      Doc->isVerilog = true;
      ret = Doc->createSubNetlist(&ts, countInit, Collect, ErrText, 0);
      if(ret) {
	Stream << "  <VerilogModel>";
	Stream << tmp;
	Stream << "  </VerilogModel>\n";
	Success = true;
      } else {
	ErrText->insert("\n");
      }

      // save vhdl model
      tmp = "";
      countInit = 0;
      Collect.clear();
      StringList.clear();
      Doc->isVerilog = false;
      ret = Doc->createSubNetlist(&ts, countInit, Collect, ErrText, 0);
      if(ret) {
	Stream << "  <VHDLModel>";
	Stream << tmp;
	Stream << "  </VHDLModel>\n";
	Success = true;
      } else {
	ErrText->insert("\n");
      }

      Stream << "  <Symbol>\n";
      Doc->createSubcircuitSymbol();
      Painting *pp;
      for(pp = Doc->SymbolPaints.first(); pp != 0; pp = Doc->SymbolPaints.next())
        Stream << "    <" << pp->save() << ">\n";

      Stream << "  </Symbol>\n"
             << "</Component>\n\n";

      delete Doc;
      if(!Success) break;
    }

  LibFile.close();
  if(!Success) {
    LibFile.remove();
    return;
  }

  ErrText->append(tr("Successfully created library."));
}
