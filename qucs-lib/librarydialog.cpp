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
#include "qucslib.h"

#include <qhbox.h>
#include <qvbox.h>
#include <qfile.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qscrollview.h>
#include <qradiobutton.h>
#include <qvbuttongroup.h>


LibraryDialog::LibraryDialog(QWidget *App_)
			: QDialog(App_, 0, TRUE, Qt::WDestructiveClose)
{
  setCaption(tr("Manage User Libraries"));

  Expr.setPattern("[\\w_]+");
  Validator = new QRegExpValidator(Expr, this);

  // ...........................................................
  QVBoxLayout *all = new QVBoxLayout(this);
  all->setMargin(5);
  all->setSpacing(6);

  Group = new QVButtonGroup(tr("Choose library:"), this);
  all->addWidget(Group);
  
  QScrollView *Dia_Scroll = new QScrollView(Group);
  Dia_Scroll->setMargin(5);
  QVBox *Dia_Box = new QVBox(Dia_Scroll->viewport());
  Dia_Scroll->addChild(Dia_Box);

  QHBox *h1 = new QHBox(this);
  all->addWidget(h1);
  theLabel = new QLabel(tr("New Name:"), h1);
  NameEdit = new QLineEdit(h1);
  NameEdit->setValidator(Validator);

  // ...........................................................
  QHBox *h2 = new QHBox(this);
  all->addWidget(h2);
  ButtDelete = new QPushButton(tr("Delete"), h2);
  connect(ButtDelete, SIGNAL(clicked()), SLOT(slotDelete()));
  ButtRename = new QPushButton(tr("Rename"), h2);
  connect(ButtRename, SIGNAL(clicked()), SLOT(slotRename()));
  ButtClose = new QPushButton(tr("Close"), h2);
  connect(ButtClose, SIGNAL(clicked()), SLOT(reject()));
  ButtClose->setDefault(true);

  // ...........................................................
  // insert all user libraries
  QStringList LibFiles = UserLibDir.entryList("*.lib", QDir::Files, QDir::Name);


  QStringList::iterator it;
  // inserts all project directories
  for(it = LibFiles.begin(); it != LibFiles.end(); it++)
    LibList.append(new QRadioButton((*it).left((*it).length()-4), Dia_Box));

  QColor theColor;
  if(LibList.isEmpty()) {
    ButtDelete->setEnabled(false);
    ButtRename->setEnabled(false);
    theColor =
       (new QLabel(tr("No user library!"), Dia_Box))->paletteBackgroundColor();
  }
  else
    theColor = LibList.current()->paletteBackgroundColor();
  Dia_Scroll->viewport()->setPaletteBackgroundColor(theColor);
}

LibraryDialog::~LibraryDialog()
{
  delete Validator;
}

// ---------------------------------------------------------------
void LibraryDialog::slotRename()
{
  if(NameEdit->text().isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("Please insert a new library name!"));
    return;
  }

  QRadioButton *p;
  for(p = LibList.first(); p != 0; p = LibList.next())
    if(p->isOn()) {
      QFile LibFile(UserLibDir.absFilePath(p->text() + ".lib"));
      if(!LibFile.open(IO_ReadWrite)) {
    QMessageBox::critical(this, tr("Error"), tr("Error: Cannot create library!"));
    return;
  }
  QTextStream Stream;
  Stream.setDevice(&LibFile);
  Stream << "<Qucs Library " PACKAGE_VERSION " \"" << NameEdit->text() << "\">\n\n";

  LibFile.close();
    }

  if(p == 0)
    QMessageBox::critical(this, tr("Error"), tr("Please choose a library!"));
}

// ---------------------------------------------------------------
void LibraryDialog::slotDelete()
{
  QRadioButton *p;
  for(p = LibList.first(); p != 0; p = LibList.next())
    if(p->isOn()) {
      QFile LibFile(UserLibDir.absFilePath(p->text() + ".lib"));
      if(!LibFile.remove()) {
        QMessageBox::critical(this, tr("Error"), tr("Cannot delete library \"%1\".").arg(p->text()));
        return;
      }
    }

  if(p == 0)
    QMessageBox::critical(this, tr("Error"), tr("Please choose a library!"));
}
