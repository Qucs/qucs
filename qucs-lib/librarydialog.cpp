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
  Dia_Box = new QVBox(Dia_Scroll->viewport());
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

  toggleGroup = new QVButtonGroup();  // only to handle exclusive toggling

  previousLib = 0;
  QStringList::iterator it;
  // inserts all project directories
  for(it = LibFiles.begin(); it != LibFiles.end(); it++)
    toggleGroup->insert(new QRadioButton((*it).left((*it).length()-4), Dia_Box));

  QColor theColor;
  QButton *rButton = toggleGroup->find(0);
  if(rButton)
    theColor = rButton->paletteBackgroundColor();
  else {
    ButtDelete->setEnabled(false);
    ButtRename->setEnabled(false);
    theColor =
       (new QLabel(tr("No user library!"), Dia_Box))->paletteBackgroundColor();
  }
  Dia_Scroll->viewport()->setPaletteBackgroundColor(theColor);
}

LibraryDialog::~LibraryDialog()
{
  delete Validator;
}

// ---------------------------------------------------------------
// Renames selected user library.
void LibraryDialog::slotRename()
{
  if(NameEdit->text().isEmpty()) {
    QMessageBox::critical(this, tr("Error"), tr("Please insert a new library name!"));
    return;
  }

  QRadioButton *rButton = (QRadioButton*)toggleGroup->selected();
  if(rButton == 0) {
    QMessageBox::critical(this, tr("Error"), tr("Please choose a library!"));
    return;
  }

  QFile NewLibFile(QucsSettings.LibDir + NameEdit->text() + ".lib");
  if(NewLibFile.exists()) {
    QMessageBox::critical(this, tr("Error"), tr("A system library with this name already exists!"));
    return;
  }

  NewLibFile.setName(UserLibDir.absFilePath(NameEdit->text() + ".lib"));
  if(NewLibFile.exists()) {
    QMessageBox::critical(this, tr("Error"), tr("A library with this name already exists!"));
    return;
  }

  QFile LibFile(UserLibDir.absFilePath(rButton->text() + ".lib"));
  if(!LibFile.open(IO_ReadOnly)) {
    QMessageBox::critical(this, tr("Error"), tr("Cannot open library!"));
    return;
  }

  QByteArray FileContent = LibFile.readAll();
//  LibFile.close();

  // rename library name within file
  char *p, *Name;
  char *Config = FileContent.data();
  for(;;) {
    p = strstr(Config, "<Qucs Library ");
    if(p == 0) break;
    Name = strstr(p, " \"");
    if(Name == 0) break;
    Name += 2;
    p = strstr(Name, "\">");
    if(p == 0) break;

    if(!NewLibFile.open(IO_WriteOnly)) {
      QMessageBox::critical(this, tr("Error"), tr("No permission to modify library!"));
      return;
    }
    int count = 0;
    count += NewLibFile.writeBlock(Config, Name-Config);
    count += NewLibFile.writeBlock(NameEdit->text().latin1(), NameEdit->text().length());
    count += NewLibFile.writeBlock(p, FileContent.count() - (p-Config) );
    NewLibFile.close();
    count -= FileContent.count() + NameEdit->text().length() - (p-Name);
    if(count != 0) {
      QMessageBox::critical(this, tr("Error"), tr("Writing new library not successful!"));
      return;
    }

    if(!LibFile.remove()) {
      QMessageBox::critical(this, tr("Error"), tr("Cannot delete old library."));
      toggleGroup->insert(new QRadioButton(NameEdit->text(), Dia_Box));
      NameEdit->clear();
      return;
    }

    rButton->setText(NameEdit->text());
    NameEdit->clear();
    return;
  }

  QMessageBox::critical(this, tr("Error"), tr("Library file is corrupt!"));
}

// ---------------------------------------------------------------
// Deletes the selected user library.
void LibraryDialog::slotDelete()
{
  QRadioButton *rButton = (QRadioButton*)toggleGroup->selected();
  if(rButton == 0) {
    QMessageBox::critical(this, tr("Error"), tr("Please choose a library!"));
    return;
  }

  QFile LibFile(UserLibDir.absFilePath(rButton->text() + ".lib"));
  if(!LibFile.remove()) {
    QMessageBox::critical(this, tr("Error"),
                 tr("No permission to delete library \"%1\".").arg(rButton->text()));
    return;
  }
  delete rButton;
}
