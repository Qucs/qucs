/***************************************************************************
                              sweepdialog.h
                             ---------------
    begin                : Sat Aug 13 2005
    copyright            : (C) 2005 by Michael Margraf
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

#ifndef SWEEPDIALOG_H
#define SWEEPDIALOG_H

#include <qdialog.h>
#include <qregexp.h>
#include <qptrlist.h>

class QucsDoc;
class Graph;
class QGridLayout;
class QRegExpValidator;
class QSpinBox;


class SweepDialog : public QDialog {
   Q_OBJECT
public:
  SweepDialog(QucsDoc*, Graph*, QWidget *parent=0);
 ~SweepDialog();

private slots:
  void slotNewValue(int);

private:
  QGridLayout *all;   // the mother of all widgets
  QRegExpValidator  *Validator;
  QRegExp     Expr;
  QPtrList<QSpinBox> BoxList;

  QucsDoc *Doc;
  Graph   *pGraph;
};

#endif
