/***************************************************************************
                          messagebox.h  -  description
                             -------------------
    begin                : Sat Oct 25 2003
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

#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <qdialog.h>

/**
  *@author Michael Margraf
  */

class MessageBox : public QDialog  {
Q_OBJECT
public: 
	MessageBox(const QString& Caption, const QString& Text, QWidget *parent=0, const char *name=0);
	~MessageBox();

private slots:
  void slotSave();
  void slotSaveAll();
  void slotDiscard();
  void slotDiscardAll();
  void slotCancel();
};

#endif
