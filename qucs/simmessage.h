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

/**
  *@author Michael Margraf
  */

class SimMessage : public QDialog  {
public: 
	SimMessage(QWidget *parent=0);
	~SimMessage();

//private:
  QTextEdit *ProgText, *ErrText;

};

#endif
