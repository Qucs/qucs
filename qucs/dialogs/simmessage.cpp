/***************************************************************************
                          simmessage.cpp  -  description
                             -------------------
    begin                : Sat Sep 6 2003
    copyright            : (C) 2003 by Michael Margraf
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

#include "simmessage.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qvgroupbox.h>
#include <qhgroupbox.h>
#include <qhbox.h>
#include <qtimer.h>
#include <qpushbutton.h>
#include <qprogressbar.h>
#include <qtextedit.h>


SimMessage::SimMessage(const QString& DataDpl, QWidget *parent)
		: QDialog(parent, 0, FALSE, Qt::WDestructiveClose)
{
  DataDisplay = DataDpl;
  setCaption(tr("Qucs Simulation Messages"));

  all = new QVBoxLayout(this);
  all->setSpacing(5);
  all->setMargin(5);
  QVGroupBox *Group1 = new QVGroupBox(tr("Progress:"),this);
  all->addWidget(Group1);

  ProgText = new QTextEdit(Group1);
  ProgText->setTextFormat(Qt::PlainText);
  ProgText->setReadOnly(true);
  ProgText->setWordWrap(QTextEdit::NoWrap);
  ProgText->setMinimumSize(400,80);
  wasLF = false;

  QHGroupBox *HGroup = new QHGroupBox(this);
  HGroup->setInsideMargin(5);
  HGroup->setInsideSpacing(5);
  all->addWidget(HGroup);
  new QLabel(tr("Progress:"), HGroup);
  SimProgress = new QProgressBar(HGroup);

  QVGroupBox *Group2 = new QVGroupBox(tr("Errors and Warnings:"),this);
  all->addWidget(Group2);

  ErrText = new QTextEdit(Group2);
  ErrText->setTextFormat(Qt::PlainText);
  ErrText->setReadOnly(true);
  ErrText->setWordWrap(QTextEdit::NoWrap);
  ErrText->setMinimumSize(400,80);

  QHBox *Butts = new QHBox(this);
  all->addWidget(Butts);

  Display = new QPushButton(tr("Goto display page"), Butts);
  Display->setDisabled(true);
  connect(Display,SIGNAL(clicked()),SLOT(slotDisplayButton()));

  Abort = new QPushButton(tr("Abort simulation"), Butts);
  connect(Abort,SIGNAL(clicked()),SLOT(reject()));

  // ........................................................
  connect(&SimProcess, SIGNAL(readyReadStdout()), SLOT(slotDisplayMsg()));
  connect(&SimProcess, SIGNAL(readyReadStderr()), SLOT(slotDisplayErr()));
  connect(&SimProcess, SIGNAL(processExited()), SLOT(slotSimEnded()));
}

SimMessage::~SimMessage()
{
  if(SimProcess.isRunning())  SimProcess.kill();
  delete all;
}

// ------------------------------------------------------------------------
bool SimMessage::startProcess(const QStringList& commands)
{
  Abort->setText(tr("Abort simulation"));
  Display->setDisabled(true);

  SimProcess.blockSignals(false);
  if(SimProcess.isRunning()) return false;

  SimProcess.setArguments(commands);
  if(!SimProcess.start()) return false;

  return true;
}

// ------------------------------------------------------------------------
// Is called when the process sends an output to stdout.
void SimMessage::slotDisplayMsg()
{
  int i, Para;
  QString s = QString(SimProcess.readStdout());
  //qDebug(s);
  while((i = s.find('\r')) >= 0) {
    if(wasLF) {
      ProgressText += s.left(i-1);
    }
    else {
      int k = s.findRev('\n',i-s.length());
      if (k > 0) {
	ProgText->insert(s.left(k));
	s = s.mid(k+1);
	i = s.find('\r');
      }
      Para = ProgText->paragraphs()-1;
      ProgressText = ProgText->text(Para) + s.left(i-1);
      ProgText->removeParagraph(Para);  // remove last text line
    }
    s = s.mid(i+1);
    Para = ProgressText.length()-11;
    Para = 10*int(ProgressText.at(Para).latin1()-'0') +
	      int(ProgressText.at(Para+1).latin1()-'0');
    if(Para < 0)  Para += 160;
    SimProgress->setProgress(Para, 100);
    ProgressText = "";
    wasLF = true;
  }
  if(s.length() < 1)  return;

  if(wasLF) {
    if(s.find('\n') >= 0) {
      ProgText->insert("\n"+s);
      wasLF = false;
    }
  }
  else  ProgText->insert(s);
}

// ------------------------------------------------------------------------
// Is called when the process sends an output to stderr.
void SimMessage::slotDisplayErr()
{
  ErrText->append(QString(SimProcess.readStderr()));
}

// ------------------------------------------------------------------------
// Is called when the simulation process terminates.
void SimMessage::slotSimEnded()
{
  Abort->setText(tr("Close window"));
  Display->setDisabled(false);

  int stat = (!SimProcess.normalExit()) ? -1 : SimProcess.exitStatus();
  emit SimulationEnded(stat, this);
}

// ------------------------------------------------------------------------
// Is called when the simulation ended with errors before starting simulator
// process.
void SimMessage::errorSimEnded()
{
  Abort->setText(tr("Close window"));
  Display->setDisabled(false);

  emit SimulationEnded(1, this);
}

// ------------------------------------------------------------------------
// To call accept(), which is protected, from the outside.
void SimMessage::slotClose()
{
  accept();
}

// ------------------------------------------------------------------------
void SimMessage::slotDisplayButton()
{
  emit displayDataPage(DataDisplay);
  accept();
}
