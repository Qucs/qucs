/***************************************************************************
                          simmessage.h  -  description
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

#ifndef SIMMESSAGE_H
#define SIMMESSAGE_H

#include <qdialog.h>

#include <qtextedit.h>
#include <qprocess.h>
#include <qpushbutton.h>
#include <qprogressbar.h>


/**
  *@author Michael Margraf
  */

class SimMessage : public QDialog  {
   Q_OBJECT
public:
  SimMessage(const QString& DataDpl=0, QWidget *parent=0);
  ~SimMessage();

  bool startProcess(const QStringList&);
  void errorSimEnded();

private:
  void reject();

signals:
  void SimulationEnded(int, SimMessage*);
  void displayDataPage(QString);

public slots:
  void slotDisplayMsg();
  void slotDisplayErr();
  void slotSimEnded();
  void slotClose();
  void slotDisplayButton();

public:
  QProcess     SimProcess;
  QTextEdit    *ProgText, *ErrText;
  bool         wasLF;   // linefeed for "ProgText"
  QPushButton  *Display, *Abort;
  QString      DataDisplay;
  QProgressBar *SimProgress;
  QString      ProgressText;
};

#endif
