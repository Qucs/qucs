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
#include "node.h"
#include "qucs.h"
#include "main.h"
#include "qucsview.h"
#include "../diagrams/graph.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qvalidator.h>
#include <qpushbutton.h>


mySpinBox::mySpinBox(int Min, int Max, int Step, double *Val, QWidget *Parent)
          : QSpinBox(Min, Max, Step, Parent)
{
  Values = Val;
}

QString mySpinBox::mapValueToText(int Val)
{
  return QString::number(*(Values+Val));
}




SweepDialog::SweepDialog(QucsDoc *d, QWidget *parent)
			: QDialog(parent, 0, TRUE, Qt::WDestructiveClose)
{
  Doc = d;

  Expr.setPattern("");  // no number should be inserted manually
  Validator = new QRegExpValidator(Expr, this);

  pGraph = setBiasPoints();
  // if simulation has no sweeps, terminate dialog before showing it
  if(pGraph->cPointsX.count() == 0) {
    reject();
    return;
  }
  if(pGraph->cPointsX.count() <= 1)
    if(pGraph->cPointsX.getFirst()->count <= 1) {
      reject();
      return;
    }


  setCaption(tr("Bias Points"));

  int i = 0;
  // ...........................................................
  QGridLayout *all = new QGridLayout(this, pGraph->cPointsX.count()+1,2,3,3);

  DataX *pD;
  mySpinBox *Box;
  for(pD = pGraph->cPointsX.first(); pD!=0; pD = pGraph->cPointsX.next()) {
    all->addWidget(new QLabel(pD->Var, this), i,0);

    Box = new mySpinBox(0, pD->count-1, 1, pD->Points, this);
    Box->setValidator(Validator);
    Box->setValue(0);  // call "mapValueToText()"
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

  for(double *p = ValueList.first(); p!=0; p = ValueList.next())
    delete p;
}

// ---------------------------------------------------------------
void SweepDialog::slotNewValue(int)
{
  DataX *pD = pGraph->cPointsX.first();

  int Factor = 1, Index = 0;
  for(mySpinBox *pb = BoxList.first(); pb!=0; pb = BoxList.next()) {
    Index  += pb->value() * Factor;
    Factor *= pD->count;
  }
  Index *= 2;  // because of complex values

  double *p = ValueList.first();
  for(Node *pn = NodeList.first(); pn!=0; pn = NodeList.next()) {
    pn->Name = num2str(*(p+Index));
    if(pn->x1 & 0x10)
      pn->Name += "A";
    else
      pn->Name += "V";
    p = ValueList.next();
  }

  QucsMain->view->viewport()->update();
}

// ---------------------------------------------------
Graph* SweepDialog::setBiasPoints()
{
  // When this function is entered, a simulation was performed.
  // Thus, the node names are still in "node->Name".

  bool hasNoComp;
  Graph *pg = new Graph("");
  Diagram *Diag = new Diagram();

  Node *pn;
  Element *pe;
  // create DC voltage for all nodes
  for(pn = Doc->Nodes->first(); pn != 0; pn = Doc->Nodes->next()) {
    if(pn->Name.isEmpty()) continue;

    pn->x1 = 0;
    if(pn->Connections.count() < 2) {
      pn->Name = "";  // no text at open nodes
      continue;
    }
    else {
      hasNoComp = true;
      for(pe = pn->Connections.first(); pe!=0; pe = pn->Connections.next())
        if(pe->Type == isWire) {
          if( ((Wire*)pe)->isHorizontal() )  pn->x1 |= 2;
        }
        else {
          if( ((Component*)pe)->Model == "GND" ) {
            hasNoComp = true;   // no text at ground symbol
            break;
          }

          if(pn->cx < pe->cx)  pn->x1 |= 1;  // to the right is no room
          hasNoComp = false;
        }
      if(hasNoComp) {  // text only were a component is connected
        pn->Name = "";
        continue;
      }
    }

    pg->Var = pn->Name + ".V";
    if(Diag->loadVarData(Doc->DataSet, pg)) {
      pn->Name = num2str(*(pg->cPointsY)) + "V";
      NodeList.append(pn);             // remember node ...
      ValueList.append(pg->cPointsY);  // ... and all of its values
      pg->cPointsY = 0;   // do not delete it next time !
    }
    else
      pn->Name = "0V";


    for(pe = pn->Connections.first(); pe!=0; pe = pn->Connections.next())
      if(pe->Type == isWire) {
        if( ((Wire*)pe)->Port1 != pn )  // no text at next node
          ((Wire*)pe)->Port1->Name = "";
        else  ((Wire*)pe)->Port2->Name = "";
      }
  }


  // create DC current through each probe
  for(Component *pc = Doc->Comps->first(); pc != 0; pc = Doc->Comps->next())
    if(pc->Model == "IProbe") {
      pn = pc->Ports.first()->Connection;
      if(!pn->Name.isEmpty())   // preserve node voltage ?
        pn = pc->Ports.next()->Connection;

      pn->x1 = 0x10;   // mark current
      pg->Var = pc->Name + ".I";
      if(Diag->loadVarData(Doc->DataSet, pg)) {
        pn->Name = num2str(*(pg->cPointsY)) + "A";
        NodeList.append(pn);             // remember node ...
        ValueList.append(pg->cPointsY);  // ... and all of its values
        pg->cPointsY = 0;   // do not delete it next time !
      }
      else
        pn->Name = "0A";

      for(pe = pn->Connections.first(); pe!=0; pe = pn->Connections.next())
        if(pe->Type == isWire) {
          if( ((Wire*)pe)->isHorizontal() )  pn->x1 |= 2;
        }
        else {
          if(pn->cx < pe->cx)  pn->x1 |= 1;  // to the right is no room
        }
    }


  Doc->showBias = 1;
  delete Diag;
  return pg;
}
