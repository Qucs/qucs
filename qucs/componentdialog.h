/***************************************************************************
                          componentdialog.h  -  description
                             -------------------
    begin                : Tue Sep 9 2003
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

#ifndef COMPONENTDIALOG_H
#define COMPONENTDIALOG_H

#include "component.h"

#include <qlabel.h>
#include <qdialog.h>
#include <qlistview.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qregexp.h>


/**
  *@author Michael Margraf
  */

class ComponentDialog : public QDialog {
   Q_OBJECT
public: 
	ComponentDialog(Component *c, QWidget *parent=0, const char *name=0);
	~ComponentDialog();

private slots:
  void slotApplyChange(const QString& Text);
  void slotButtOK();
  void slotSelectProperty(QListViewItem *item);
  void slotApplyInput();
  void slotApplyState(int State);
  
private:
  QRegExp   Expr;
  QListView *prop;
  QLineEdit *edit;
  QLabel    *Name;
  QCheckBox *disp;
  QLabel    *Description;
  Component *Comp;
  bool      changed;
};

#endif
