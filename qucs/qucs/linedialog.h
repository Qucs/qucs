/***************************************************************************
                          linedialog.h  -  description
                             -------------------
    begin                : Wed Nov 26 2003
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

#ifndef LINEDIALOG_H
#define LINEDIALOG_H

#include <qdialog.h>

#include <qregexp.h>
#include <qlineedit.h>
#include <qpushbutton.h>


/**
  *@author Michael Margraf
  */

class LineDialog : public QDialog  {
Q_OBJECT
public: 
	LineDialog(const QString& _Caption, QWidget *parent=0, const char *name=0);
	~LineDialog();

private slots:
  void slotSetColor();

public:
  QRegExp     Expr;
  QLineEdit   *LineWidth;
  QPushButton *ColorButt;
};

#endif
