/***************************************************************************
                             importdialog.cpp
                            ------------------
    begin                : Fri Jun 23 2006
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

#include <qhbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qvgroupbox.h>
#include <qfiledialog.h>
#include <qpushbutton.h>
#include <qmessagebox.h>

#include "importdialog.h"
#include "main.h"
#include "qucs.h"


ImportDialog::ImportDialog(QWidget *parent)
		: QDialog(parent, 0, FALSE, Qt::WDestructiveClose)
{
  setCaption(tr("Import Data File..."));

  all = new QGridLayout(this, 4, 3, 5, 3);

  all->addWidget(new QLabel(tr("Import File:"), this), 0, 0);
  ImportEdit = new QLineEdit(this);
  all->addWidget(ImportEdit, 0, 1);
  QPushButton *BrowseButt = new QPushButton(tr("Browse"), this);
  all->addWidget(BrowseButt, 0, 2);
  connect(BrowseButt, SIGNAL(clicked()), SLOT(slotBrowse()));

  all->addWidget(new QLabel(tr("Output File:"), this), 1, 0);
  OutputEdit = new QLineEdit(this);
  all->addWidget(OutputEdit, 1, 1);

  QVGroupBox *Group1 = new QVGroupBox(tr("Messages"),this);
  all->addMultiCellWidget(Group1, 2,2, 0,2);

  MsgText = new QTextEdit(Group1);
  MsgText->setTextFormat(Qt::PlainText);
  MsgText->setReadOnly(true);
  MsgText->setWordWrap(QTextEdit::NoWrap);
  MsgText->setMinimumSize(250, 60);

  QHBox *Butts = new QHBox(this);
  all->addMultiCellWidget(Butts, 3,3, 0,2);

  Butts->setStretchFactor(new QWidget(Butts), 5); // stretchable placeholder
  
  ImportButt = new QPushButton(tr("Import"), Butts);
  connect(ImportButt, SIGNAL(clicked()), SLOT(slotImport()));
  AbortButt = new QPushButton(tr("Abort"), Butts);
  AbortButt->setDisabled(true);
  connect(AbortButt, SIGNAL(clicked()), SLOT(slotAbort()));
  CancelButt = new QPushButton(tr("Close"), Butts);
  connect(CancelButt, SIGNAL(clicked()), SLOT(reject()));
}

ImportDialog::~ImportDialog()
{
  if(Process.isRunning())  Process.kill();
  delete all;
}

// ------------------------------------------------------------------------
void ImportDialog::slotBrowse()
{
  QString s = QFileDialog::getOpenFileName(
     lastDir.isEmpty() ? QString(".") : lastDir,
     tr("All known")+" (*.s?p *.citi *.cit *.asc *.mdl *.vcd);;"+
     tr("Touchstone files")+" (*.s?p);;"+
     tr("CITI files")+" (*.citi *.cit);;"+
     tr("ZVR ASCII files")+" (*.asc);;"+
     tr("IC-CAP model files")+" (*.mdl);;"+
     tr("VCD files")+" (*.vcd);;"+
     tr("Any File")+" (*)",
     this, 0, tr("Enter a Data File Name"));

  if(!s.isEmpty()) {
    QFileInfo Info(s);
    lastDir = Info.dirPath(true);  // remember last directory
    ImportEdit->setText(s);

    if(OutputEdit->text().isEmpty())
      OutputEdit->setText(Info.baseName()+".dat");
  }
}

// ------------------------------------------------------------------------
void ImportDialog::slotImport()
{
  MsgText->clear();
  if (OutputEdit->text().isEmpty())
    return;

  ImportButt->setDisabled(true);
  AbortButt->setDisabled(false);

  QFile File(QucsWorkDir.filePath(OutputEdit->text()));
  if(File.exists())
    if(QMessageBox::information(this, tr("Info"),
          tr("Output file already exists!")+"\n"+tr("Overwrite it?"),
          tr("&Yes"), tr("&No"), 0,1,1))
      {
	ImportButt->setDisabled(false);
	AbortButt->setDisabled(true);
	return;
      }

  QFileInfo Info(ImportEdit->text());
  QString Suffix = Info.extension();
  QStringList CommandLine;
  CommandLine << QucsSettings.BinDir + "qucsconv" << "-if";
  
  if((Suffix == "citi") || (Suffix == "cit"))
    CommandLine << "citi";
  else if(Suffix == "vcd")
    CommandLine << "vcd";
  else if(Suffix == "asc")
    CommandLine << "zvr";
  else if(Suffix == "mdl")
    CommandLine << "mdl";
  else for(;;) {
    if(Suffix.at(0) == 's')
      if(Suffix.at(2) == 'p')
        if(Suffix.length() == 3)
          if(Suffix.at(1).isDigit()) {
            CommandLine << "touchstone";
            break;
          }

    MsgText->append(tr("ERROR: Unknown file format! Please check file name extension!"));
    return;
  }
  CommandLine << "-of" << "qucsdata" << "-i" << ImportEdit->text()
              << "-o" << QucsWorkDir.filePath(OutputEdit->text());

  Process.setArguments(CommandLine);
  Process.blockSignals(false);

  disconnect(&Process, 0, 0, 0);
  connect(&Process, SIGNAL(readyReadStderr()), SLOT(slotDisplayErr()));
  connect(&Process, SIGNAL(readyReadStdout()), SLOT(slotDisplayMsg()));
  connect(&Process, SIGNAL(processExited()), SLOT(slotProcessEnded()));

  if(!Process.start())
    MsgText->append(tr("ERROR: Cannot start converter!"));
}

// ------------------------------------------------------------------------
void ImportDialog::slotAbort()
{
  if(Process.isRunning())  Process.kill();
  AbortButt->setDisabled(true);
  ImportButt->setDisabled(false);
}

// ------------------------------------------------------------------------
// Is called when the process sends an output to stdout.
void ImportDialog::slotDisplayMsg()
{
  int par = MsgText->paragraphs();
  int idx = MsgText->paragraphLength(par-1);
  MsgText->setCursorPosition(par-1,idx);
  MsgText->insert(QString(Process.readStdout()));
}

// ------------------------------------------------------------------------
// Is called when the process sends an output to stderr.
void ImportDialog::slotDisplayErr()
{
  int par = MsgText->paragraphs();
  int idx = MsgText->paragraphLength(par-1);
  MsgText->setCursorPosition(par-1,idx);
  MsgText->insert(QString(Process.readStderr()));
}

// ------------------------------------------------------------------------
// Is called when the simulation process terminates.
void ImportDialog::slotProcessEnded()
{
  MsgText->append(" ");
  ImportButt->setDisabled(false);
  AbortButt->setDisabled(true);

  if(Process.normalExit() && (Process.exitStatus() == 0)) {
    MsgText->append(tr("Successfully imported file!"));
    OutputEdit->clear();

    disconnect(CancelButt, SIGNAL(clicked()), 0, 0);
    connect(CancelButt, SIGNAL(clicked()), SLOT(accept()));
  }
  else
    MsgText->append(tr("Converter ended with errors!"));
}
