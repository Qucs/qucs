/***************************************************************************
                          helpdialog.h  -  description
                             -------------------
    begin                : Sun Jan 11 2004
    copyright            : (C) 2004 by Michael Margraf
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

#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <qdialog.h>
#include <qtextbrowser.h>


/**
  *@author Michael Margraf
  */

class HelpDialog : public QDialog  {
   Q_OBJECT
public: 
	HelpDialog(const QString& page, QWidget *parent=0, const char *name=0);
	~HelpDialog();

private slots:
  void slotGotoIndex();
  void slotClose();

private:
  QTextBrowser *text;
};

#endif
