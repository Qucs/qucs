/***************************************************************************
                          graphictextdialog.h  -  description
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

#ifndef GRAPHICTEXTDIALOG_H
#define GRAPHICTEXTDIALOG_H

#include <qdialog.h>

#include <qregexp.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtextedit.h>


/**
  *@author Michael Margraf
  */

class GraphicTextDialog : public QDialog  {
Q_OBJECT
public: 
	GraphicTextDialog(QWidget *parent=0, const char *name=0);
	~GraphicTextDialog();

private slots:
  void slotSetColor();

public:
  QRegExp     Expr;
  QLineEdit   *TextSize;
  QPushButton *ColorButt;
  QTextEdit   *text;
};

#endif
