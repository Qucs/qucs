/***************************************************************************
                               spicedialog.cpp
                              -----------------
    begin                : Tue May 3 2005
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

#include "spicedialog.h"
#include "spicefile.h"
#include "qucs.h"
#include "qucsview.h"
#include "qucsdoc.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qfiledialog.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qcheckbox.h>
#include <qprocess.h>


SpiceDialog::SpiceDialog(SpiceFile *c, QucsDoc *d, QWidget *parent)
			: QDialog(parent, 0, TRUE, Qt::WDestructiveClose)
{
  resize(400, 250);
  setCaption(tr("Edit Spice Component Properties"));
  Comp = c;
  Doc  = d;

  all = new QVBoxLayout(this); // to provide neccessary size
  QWidget *myParent = this;

  Expr.setPattern("[^\"=]+");  // valid expression for property 'edit' etc
  Validator = new QRegExpValidator(Expr, this);
  Expr.setPattern("[\\w_]+");  // valid expression for property 'NameEdit' etc
  ValRestrict = new QRegExpValidator(Expr, this);


  // ...........................................................
  QGridLayout *topGrid = new QGridLayout(0, 3,4,5,5);
  all->addLayout(topGrid);

  topGrid->addWidget(new QLabel(tr("Name:"), myParent), 0,0);
  CompNameEdit = new QLineEdit(myParent);
  CompNameEdit->setValidator(ValRestrict);
  topGrid->addWidget(CompNameEdit, 0,1);
  connect(CompNameEdit, SIGNAL(returnPressed()), SLOT(slotButtOK()));

  topGrid->addWidget(new QLabel(tr("File:"), myParent), 1,0);
  FileEdit = new QLineEdit(myParent);
  FileEdit->setValidator(ValRestrict);
  topGrid->addWidget(FileEdit, 1,1);
  connect(FileEdit, SIGNAL(returnPressed()), SLOT(slotButtOK()));

  ButtBrowse = new QPushButton(tr("Browse"), myParent);
  topGrid->addWidget(ButtBrowse, 1,2);
  connect(ButtBrowse, SIGNAL(clicked()), SLOT(slotButtBrowse()));

  ButtEdit = new QPushButton(tr("Edit"), myParent);
  topGrid->addWidget(ButtEdit, 1,3);
  connect(ButtEdit, SIGNAL(clicked()), SLOT(slotButtEdit()));

  FileCheck = new QCheckBox(tr("show file name in schematic"), myParent);
  topGrid->addMultiCellWidget(FileCheck, 2,2,1,3);


  // ...........................................................
  QGridLayout *midGrid = new QGridLayout(0, 2,3,5,5);
  all->addLayout(midGrid);

  midGrid->addWidget(new QLabel(tr("Spice net nodes:"), myParent), 0,0);
  NodesList = new QListBox(myParent);
  midGrid->addWidget(NodesList, 1,0);
  connect(NodesList, SIGNAL(doubleClicked(QListBoxItem*)),
	SLOT(slotAddPort(QListBoxItem*)));
  
  QVBox *v0 = new QVBox(myParent);
  v0->setSpacing(5);
  midGrid->addWidget(v0, 1,1);
  ButtAdd = new QPushButton(tr("Add >>"), v0);
  connect(ButtAdd, SIGNAL(clicked()), SLOT(slotButtAdd()));
  ButtRemove = new QPushButton(tr("<< Remove"), v0);
  connect(ButtRemove, SIGNAL(clicked()), SLOT(slotButtRemove()));
  v0->setStretchFactor(new QWidget(v0), 5); // stretchable placeholder

  midGrid->addWidget(new QLabel(tr("Component ports:"), myParent), 0,2);
  PortsList = new QListBox(myParent);
  midGrid->addWidget(PortsList, 1,2);
  connect(PortsList, SIGNAL(doubleClicked(QListBoxItem*)),
	SLOT(slotRemovePort(QListBoxItem*)));
  

  // ...........................................................
  QHBox *h0 = new QHBox(this);
  h0->setSpacing(5);
  all->addWidget(h0);
  connect(new QPushButton(tr("OK"),h0), SIGNAL(clicked()),
	  SLOT(slotButtOK()));
  connect(new QPushButton(tr("Apply"),h0), SIGNAL(clicked()),
	  SLOT(slotButtApply()));
  connect(new QPushButton(tr("Cancel"),h0), SIGNAL(clicked()),
	  SLOT(slotButtCancel()));

  // ------------------------------------------------------------
  CompNameEdit->setText(Comp->Name);
  changed = false;

  // insert all properties into the ListBox
  Property *pp = Comp->Props.first();
  FileEdit->setText(pp->Value);
  FileCheck->setChecked(pp->display);

  // load netlist nodes
  loadSpiceNetList(pp->Value);
  pp = Comp->Props.next();
  if(!pp->Value.isEmpty()) {
    PortsList->clear();
    PortsList->insertStringList(QStringList::split(',', pp->Value));

    QString tmp;
    QListBoxItem *pi;
    for(unsigned int i=0; i<PortsList->count(); i++) {
      tmp = PortsList->text(i);
      PortsList->changeItem(tmp.remove(0, 4), i);

      pi = NodesList->findItem(tmp, Qt::CaseSensitive | Qt::ExactMatch);
      if(pi) delete pi;
      else PortsList->removeItem(i--);
    }
  }
}

SpiceDialog::~SpiceDialog()
{
  delete all;
  delete Validator;
  delete ValRestrict;
}

// -------------------------------------------------------------------------
// Is called if the "OK"-button is pressed.
void SpiceDialog::slotButtOK()
{
  slotButtApply();
  if(changed) done(1);
  else done(0);
}

// -------------------------------------------------------------------------
// Is called if the "Cancel"-button is pressed.
void SpiceDialog::slotButtCancel()
{
  if(changed) done(1);	// changed could have been done before
  else done(0);		// (by "Apply"-button)
}

// -------------------------------------------------------------------------
// Is called, if the "Apply"-button is pressed.
void SpiceDialog::slotButtApply()
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


  // apply all the new property values
  Property *pp = Comp->Props.first();
  if(pp->Value != FileEdit->text()) {
    pp->Value = FileEdit->text();
    changed = true;
  }
  if(pp->display != FileCheck->isChecked()) {
    pp->display = FileCheck->isChecked();
    changed = true;
  }

  QString tmp;
  for(unsigned int i=0; i<PortsList->count(); i++) {
    if(!tmp.isEmpty())  tmp += ',';
    tmp += "_net" + PortsList->text(i);   // chosen ports
  }
  pp = Comp->Props.next();
  if(pp->Value != tmp) {
    pp->Value = tmp;
    changed = true;
  }


  if(changed) {
    tmp = Comp->Name;    // is sometimes changed by "recreate"
    Doc->Comps->setAutoDelete(false);
    Doc->deleteComp(Comp);
    Comp->recreate(); // to apply changes to the schematic symbol
    Doc->insertRawComponent(Comp);
    Doc->Comps->setAutoDelete(true);
    Comp->Name = tmp;
    ((QucsView*)parent())->viewport()->repaint();
  }
}

// -------------------------------------------------------------------------
void SpiceDialog::slotButtBrowse()
{
  static QString lastDir;  // to remember last directory and file
  QString s = QFileDialog::getOpenFileName(
		lastDir.isEmpty() ? QString(".") : lastDir,
		tr("All Files")+" (*.*)", this, "", tr("Select a file"));
  if(s.isEmpty()) return;

  lastDir = s;   // remember last directory and file

  // snip path if file in current directory
  QFileInfo FileInfo(s);
  if(QucsWorkDir.exists(FileInfo.fileName()) &&
     QucsWorkDir.absPath() == FileInfo.dirPath(true)) s = FileInfo.fileName();
  FileEdit->setText(s);

  loadSpiceNetList(s);
}

// -------------------------------------------------------------------------
bool SpiceDialog::loadSpiceNetList(const QString& s)
{
  if(s.isEmpty()) return false;

  // first call Qucsconv ............
  QProcess *QucsConv = new QProcess(this);
  QucsConv->addArgument(QucsSettings.BinDir + "qucsconv");
  QucsConv->addArgument("-if");
  QucsConv->addArgument("spice");
  QucsConv->addArgument("-of");
  QucsConv->addArgument("qucs");
  QucsConv->addArgument("-i");
  QucsConv->addArgument(s);
  if(!QucsConv->start()) {
    QMessageBox::critical(this, tr("Error"),
                 tr("Cannot execute")+" "+QucsSettings.BinDir + "qucsconv.");
    return false;
  }

  QMessageBox *MBox = new QMessageBox(tr("Info"), tr("Converting ..."),
               QMessageBox::NoIcon, QMessageBox::Abort,
               QMessageBox::NoButton, QMessageBox::NoButton, this, 0, true,
	       Qt::WStyle_DialogBorder |  Qt::WDestructiveClose);
  connect(QucsConv, SIGNAL(processExited()), MBox, SLOT(close()));
  MBox->exec();
  if(QucsConv->isRunning())  QucsConv->kill();


  // then read converted netlist .......
  QString Line;
  Comp->withSim = false;
  NodesList->clear();
  PortsList->clear();
  for(;;) {
    Line = QucsConv->readLineStdout();
    if(Line == QString::null) break;
    Line = Line.stripWhiteSpace();
    if(Line.at(0) == '.') {
      Comp->withSim = true;
      continue;
    }

    if(Line == "### TOPLEVEL NODELIST BEGIN") {
      for(;;) {
	Line = QucsConv->readLineStdout();
	if(Line == QString::null) break;
	if(Line == "### TOPLEVEL NODELIST END") break;
	if(Line.left(2) != "# ") continue;
	Line.remove(0, 2);

	if(Line.left(4) == "_net")
	  NodesList->insertItem(Line.remove(0, 4));
      }
    }
    else if(Line == "### SPICE OUTPUT NODELIST BEGIN") {
      for(;;) {
	Line = QucsConv->readLineStdout();
	if(Line == QString::null) break;
	if(Line == "### SPICE OUTPUT NODELIST END") break;
	if(Line.left(2) != "# ") continue;
	Line.remove(0, 2);

	if(Line.left(4) == "_net")
	  PortsList->insertItem(Line.remove(0, 4));
      }
    }
  }

  delete QucsConv;
  return true;
}

// -------------------------------------------------------------------------
void SpiceDialog::slotButtEdit()
{
  Doc->App->Acts.editFile(QucsWorkDir.filePath(FileEdit->text()));
}

// -------------------------------------------------------------------------
// Is called if the add button is pressed.
void SpiceDialog::slotButtAdd()
{
  int i = NodesList->currentItem();
  if(i < 0) return;
  PortsList->insertItem(NodesList->currentText());
  NodesList->removeItem(i);
}

// -------------------------------------------------------------------------
// Is called if the remove button is pressed.
void SpiceDialog::slotButtRemove()
{
  int i = PortsList->currentItem();
  if(i < 0) return;
  NodesList->insertItem(PortsList->currentText());
  PortsList->removeItem(i);
}

// -------------------------------------------------------------------------
// Is called when double-click on NodesList-Box
void SpiceDialog::slotAddPort(QListBoxItem *Item)
{
  if(Item) slotButtAdd();
}

// -------------------------------------------------------------------------
// Is called when double-click on PortsList-Box
void SpiceDialog::slotRemovePort(QListBoxItem *Item)
{
  if(Item) slotButtRemove();
}
