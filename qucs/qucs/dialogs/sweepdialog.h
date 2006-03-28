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

#include "node.h"

class Graph;
class Schematic;
class QGridLayout;


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
  SweepDialog(Schematic*);
 ~SweepDialog();

private slots:
  void slotNewValue(int);

private:
  Graph* setBiasPoints();

  QGridLayout *all;   // the mother of all widgets
  QPtrList<mySpinBox> BoxList;

  Graph *pGraph;
  Schematic *Doc;
  QPtrList<Node> NodeList;
  QPtrList<double> ValueList;
};

#endif
