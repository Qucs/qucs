/***************************************************************************
                          simmessage.cpp  -  description
                             -------------------
    begin                : Sat Sep 6 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : margraf@mwt.ee.tu-berlin.de
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
#include <qhbox.h>
#include <qtimer.h>


SimMessage::SimMessage(QWidget *parent) : QDialog(parent)
{
  setCaption("Qucs Simulation Messages");
  
  QVBoxLayout *all = new QVBoxLayout(this);
  all->setSpacing(5);
  QVGroupBox *Group1 = new QVGroupBox("Progress:",this);
  all->addWidget(Group1);

  ProgText = new QTextEdit(Group1);
  ProgText->setTextFormat(Qt::PlainText);
  ProgText->setReadOnly(true);
  ProgText->setMinimumSize(400,80);

  
  QVGroupBox *Group2 = new QVGroupBox("Errors and Warnings:",this);
  all->addWidget(Group2);

  ErrText = new QTextEdit(Group2);
  ErrText->setTextFormat(Qt::PlainText);
  ErrText->setReadOnly(true);
  ErrText->setMinimumSize(400,80);

  QHBox *Butts = new QHBox(this);
  all->addWidget(Butts);

  Display = new QPushButton("Goto display page", Butts);
  Display->setDisabled(true);
  connect(Display,SIGNAL(clicked()),SLOT(slotDisplayButton()));

  Abort = new QPushButton("Abort simulation", Butts);
  connect(Abort,SIGNAL(clicked()),SLOT(slotClose()));

  // ........................................................
  connect(&SimProcess, SIGNAL(readyReadStdout()), SLOT(slotDisplayMsg()));
  connect(&SimProcess, SIGNAL(readyReadStderr()), SLOT(slotDisplayErr()));
  connect(&SimProcess, SIGNAL(processExited()), SLOT(slotSimEnded()));
}

SimMessage::~SimMessage()
{
}

// ------------------------------------------------------------------------
bool SimMessage::startProcess(const QStringList& commands)
{
  Abort->setText("Abort simulation");
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
  ProgText->insert(QString(SimProcess.readStdout()));
}

// ------------------------------------------------------------------------
// Is called when the process sends an output to stderr.
void SimMessage::slotDisplayErr()
{
  ErrText->insert(QString(SimProcess.readStderr()));
}

// ------------------------------------------------------------------------
void SimMessage::slotSimEnded()
{
  Abort->setText("Close window");
  Display->setDisabled(false);

  emit SimulationEnded(SimProcess.exitStatus());
}

// ------------------------------------------------------------------------
void SimMessage::slotClose()
{
  SimProcess.blockSignals(true);  // no 'processexited' signal, is set back in 'startProcess'
  if(SimProcess.isRunning()) {
    SimProcess.tryTerminate();
    QTimer::singleShot(2000,&SimProcess,SLOT(kill()));
  }

  close();
}

// ------------------------------------------------------------------------
void SimMessage::slotDisplayButton()
{
  emit displayDataPage();
  close();
}
