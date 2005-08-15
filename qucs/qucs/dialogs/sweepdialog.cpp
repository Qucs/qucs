/***************************************************************************
                               sweepdialog.cpp
                              -----------------
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

#include "sweepdialog.h"
#include "qucsdoc.h"
#include "../diagrams/graph.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qvalidator.h>
#include <qpushbutton.h>
#include <qspinbox.h>


/*class mySpinBox : public QSpinBox {
   Q_OBJECT
public:
  mySpinBox() {};
};*/


SweepDialog::SweepDialog(QucsDoc *d, Graph *g, QWidget *parent)
			: QDialog(parent, 0, TRUE, Qt::WDestructiveClose)
{
  setCaption(tr("Bias Points"));
  Doc = d;
  pGraph = g;

  Expr.setPattern("");  // no number should be inserted manually
  Validator = new QRegExpValidator(Expr, this);

  int i = 0;
  // ...........................................................
  QGridLayout *all = new QGridLayout(this, pGraph->cPointsX.count()+1,2,3,3);

  DataX *pD;
  QSpinBox *Box;
  for(pD = pGraph->cPointsX.first(); pD!=0; pD = pGraph->cPointsX.next()) {
    all->addWidget(new QLabel(pD->Var, this), i,0);

    Box = new QSpinBox(0, pD->count-1, 1, this);
    Box->setValidator(Validator);
    all->addWidget(Box, i++,1);
    connect(Box, SIGNAL(valueChanged(int)), SLOT(slotNewValue(int)));
    BoxList.append(Box);
  }

  // ...........................................................
  QPushButton *ButtClose = new QPushButton(tr("Close"), this);
  all->addMultiCellWidget(ButtClose, i,i, 0,1);
  connect(ButtClose, SIGNAL(clicked()), SLOT(accept()));
}

SweepDialog::~SweepDialog()
{
  delete pGraph;
  delete Validator;
}

// ---------------------------------------------------------------
void SweepDialog::slotNewValue(int)
{
//  DataX *pD = pGraph->cPointsX.first();
  
//  for(QSpinBox *pb = BoxList.first(); pb!=0; pb = BoxList.next()) {
//    *(pd->Points + pb->value());
//  }
}
