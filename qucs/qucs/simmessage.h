/***************************************************************************
                          simmessage.h  -  description
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

#ifndef SIMMESSAGE_H
#define SIMMESSAGE_H

#include <qdialog.h>

#include <qtextedit.h>
#include <qprocess.h>
#include <qpushbutton.h>


/**
  *@author Michael Margraf
  */

class SimMessage : public QDialog  {
   Q_OBJECT
public: 
	SimMessage(QWidget *parent=0);
	~SimMessage();

  bool startProcess(const QStringList& commands);

signals:
  void SimulationEnded();

public slots:
  void slotDisplayMsg();
  void slotDisplayErr();
  void slotSimEnded();
  void slotClose();
  
public:
  QProcess   SimProcess;
  QTextEdit *ProgText, *ErrText;
  QPushButton *Abort;

};

#endif
