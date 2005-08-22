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
#include <qspinbox.h>

class QucsDoc;
class Graph;
class Node;
class QGridLayout;
class QRegExpValidator;


class mySpinBox : public QSpinBox {
   Q_OBJECT
public:
  mySpinBox(int, int, int, double*, QWidget*);

protected:
  QString mapValueToText(int);

private:
  double *Values;
};



class SweepDialog : public QDialog {
   Q_OBJECT
public:
  SweepDialog(QucsDoc*, QWidget *parent=0);
 ~SweepDialog();

private slots:
  void slotNewValue(int);

private:
  Graph* setBiasPoints();
  
  QGridLayout *all;   // the mother of all widgets
  QRegExp Expr;
  QRegExpValidator  *Validator;
  QPtrList<mySpinBox> BoxList;

  QucsDoc *Doc;
  Graph   *pGraph;
  QPtrList<Node> NodeList;
  QPtrList<double> ValueList;
};

#endif
