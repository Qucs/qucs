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


LibraryDialog::LibraryDialog(QucsApp *App_, QListViewItem *SchematicList)
			: QDialog(App_, 0, TRUE, Qt::WDestructiveClose)
{
  App = App_;
  setCaption(tr("Create Library"));

  Expr.setPattern("[\\w_]+");
  Validator = new QRegExpValidator(Expr, this);

  // ...........................................................
  QVBoxLayout *all = new QVBoxLayout(this);
  all->setMargin(5);
  all->setSpacing(6);

  QHBox *h1 = new QHBox(this);
  all->addWidget(h1);
  new QLabel(tr("Library Name:"), h1);
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
  ErrText->setReadOnly(true);
  ErrText->setWordWrap(QTextEdit::NoWrap);
  all->addWidget(ErrText);
  
  // ...........................................................
  QHBox *h2 = new QHBox(this);
  all->addWidget(h2);
  ButtCreate = new QPushButton(tr("Create"), h2);
  connect(ButtCreate, SIGNAL(clicked()), SLOT(slotCreate()));
  ButtCancel = new QPushButton(tr("Cancel"), h2);
  connect(ButtCancel, SIGNAL(clicked()), SLOT(reject()));

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


  QFile LibFile(QucsSettings.LibDir + NameEdit->text() + ".lib");
  if(LibFile.exists()) {
    QMessageBox::critical(this, tr("Error"), tr("A system library with this name already exists!"));
    return;
  }

  LibFile.setName(LibDir.absFilePath(NameEdit->text()) + ".lib");
  if(LibFile.exists()) {
    QMessageBox::critical(this, tr("Error"), tr("A library with this name already exists!"));
    return;
  }


  // ==> point of no return -> dialog can only be closed now
  Group->setHidden(true);
  ErrText->setHidden(false);
  ButtCreate->setEnabled(false);
  ButtCancel->setText(tr("Close"));

  if(!LibFile.open(IO_WriteOnly)) {
    ErrText->insert(tr("Error: Cannot create library!"));
    return;
  }
  QTextStream Stream;
  Stream.setDevice(&LibFile);
  Stream << "<Qucs Library " PACKAGE_VERSION " \"" << NameEdit->text() << "\">\n\n";


  int countInit = 0;
  bool Success = true;
  QStringList Collect;
  for(p = BoxList.first(); p != 0; p = BoxList.next())
    if(p->isChecked()) {
      Stream << "<Component " + p->text().section('.',0,0) + ">\n"
             << "  <Description>\n"
             << "  </Description>\n"
             << "  <Model>";

      Schematic *Doc = new Schematic(0, QucsWorkDir.filePath(p->text()));
      if(!Doc->loadDocument()) {  // load document if possible
        delete Doc;
        ErrText->insert(tr("Error: Cannot load subcircuit \"%1\".").arg(p->text()));
        break;
      }
      Doc->DocName = NameEdit->text() + "_" + p->text();
      Success = Doc->createSubNetlist(&Stream, countInit, Collect, ErrText, -1);
    
      Stream << "  </Model>\n"
             << "  <Symbol>\n";

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

  ErrText->insert(tr("\nSuccessfully created library."));
}
