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
//#include <qsize.h>


SimMessage::SimMessage(QWidget *parent) : QDialog(parent)
{
  setCaption("Qucs Simulation Messages");
  
  QVBoxLayout *all = new QVBoxLayout(this);
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

}

SimMessage::~SimMessage()
{
}
