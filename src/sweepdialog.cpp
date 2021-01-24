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
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "sweepdialog.h"
#include "schematic_doc.h"
#include "qucs_app.h"

#include <QGridLayout>
#include "graph.h"
#include "misc.h"

#include <QLabel>
#include <QLineEdit>
#include <QValidator>
#include <QPushButton>
#include <QDebug>

// SpinBoxes are used to show the calculated bias points at the given set of sweep points
mySpinBox::mySpinBox(int Min, int Max, int Step, double *Val, QWidget *Parent)
          : QSpinBox(Parent)
{ untested();
  setMinimum(Min);
  setMaximum(Max);
  setSingleStep(Step); 
  Values = Val;
  ValueSize = Max;
  //editor()->
	//	setReadOnly(true);
}


#include <iostream>
using namespace std;
QString mySpinBox::textFromValue(int Val) const
{ untested();
  if (Values == NULL) return "";

  qDebug() << "Values + Val" << *(Values+Val) << endl;
  return QString::number(*(Values+Val));
}

QValidator::State mySpinBox::validate ( QString & text, int & pos ) const
{ untested();
  if(pos>ValueSize)return QValidator::Invalid; 
  if(QString::number(*(Values+pos))==text)
  return QValidator::Acceptable;
  else return QValidator::Invalid;
}

///
/// \brief SweepDialog::SweepDialog
/// \param Doc_
/// This dialog is launched when the Calculate DC bias is requested
/// for a project which contains a sweep.
SweepDialog::SweepDialog(SchematicDoc *Doc_)
			: QDialog(Doc_)
{ untested();
#if 0
	incomplete(); // simOutput?

  Doc = Doc_;

  pGraph = setBiasPoints();
  // if simulation has no sweeps, terminate dialog before showing it
  if(!pGraph->numAxes()) { untested();
    reject();
    return;
  }else if(pGraph->numAxes() > 1) {
  }else if(pGraph->axis(0)->count <= 1) { untested();
	  reject();
	  return;
  }else{
  }

  setWindowTitle(tr("Bias Points"));

  // ...........................................................
  QGridLayout *all = new QGridLayout(this);//, pGraph->cPointsX.count()+2,2,3,3);
  all->setMargin(5);
  all->setSpacing(5);
  all->setColumnStretch(1,5);

  DataX const *pD;
  mySpinBox *Box;
  
  for(unsigned ii=0; (pD=pGraph->axis(ii)); ++ii) { untested();
    all->addWidget(new QLabel(pD->Var, this), 0, 0);
    //cout<<"count: "<<pD->count-1<<", points: "<<*pD->Points<<endl;
    //works only for linear:
    /*double Min = pD->Points[0];
    double Max = pD->Points[pD->count-1];
    double Step = (Max-Min)/(pD->count);
    cout<<"Min: "<<Min<<", Max: "<<Max<<", Step: "<<Step<<endl;
    Box = new mySpinBox(Min, Max, Step, pD->Points, this);*/
    Box = new mySpinBox(0, pD->count-1, 1, pD->Points, this);
    Box->setValue(0);  
    all->addWidget(Box, 0, 1);
    connect(Box, SIGNAL(valueChanged(int)), SLOT(slotNewValue(int)));
    BoxList.append(Box);
  }

  // ...........................................................
  all->setRowStretch(0,5);
  QPushButton *ButtClose = new QPushButton(tr("Close"), this);
  all->addWidget(ButtClose, 1, 0, 1, 5);
  connect(ButtClose, SIGNAL(clicked()), SLOT(accept()));
  show();
#endif
}

SweepDialog::~SweepDialog()
{ untested();
  delete pGraph;

  while(!ValueList.isEmpty()) { untested();
    delete ValueList.takeFirst();
  }
}

// ---------------------------------------------------------------
void SweepDialog::slotNewValue(int)
{ untested();
#if 0
  DataX const*pD = pGraph->axis(0);

  int Factor = 1, Index = 0;
  QList<mySpinBox *>::const_iterator it;
  for(it = BoxList.constBegin(); it != BoxList.constEnd(); it++) { untested();
    Index  += (*it)->value() * Factor;
    Factor *= pD->count;
  }
  Index *= 2;  // because of complex values

  QList<Node *>::iterator node_it;
  QList<double *>::const_iterator value_it = ValueList.begin();
  for(node_it = NodeList.begin(); node_it != NodeList.end(); node_it++) { untested();
//    qDebug() << "SweepDialog::slotNewValue:(*node_it)->Name:" << (*node_it)->name();

	 incomplete();
	 // BUG: a voltage tag is not a name.
//    (*node_it)->setName(misc::num2str(*((*value_it)+Index))
//                        + QString(((*node_it)->x1_() & 0x10)? "A" : "V"));
    value_it++;
  }

  Doc->viewport()->update();
#endif
}

// ---------------------------------------------------
Graph* SweepDialog::setBiasPoints()
{ untested();
  // When this function is entered, a simulation was performed.
  // Thus, the node names are still in "node->name()".

  qDebug() << "SweepDialog::setBiasPoints()";

//  bool hasNoComp;
  Graph *pg = new Graph(NULL, ""); // HACK!
  QFileInfo Info(Doc->docName());
  QString DataSet = Info.path() + QDir::separator() + Doc->DataSet;

  // Note 1:
  // Invalidate it so that "Graph::loadDatFile()" does not check for the previously loaded time.
  // This is a current hack as "Graph::loadDatFile()" does not support multi-node data loading
  // from the simulation results without refreshing (changing) or invalidating the timestamp.

  NodeList.clear();
  ValueList.clear();

  // create DC voltage for all nodes
#if 0 // use nets instead!
  for(auto pn : Doc->nodes()){ untested();
    if(pn->name().isEmpty()) continue;

    pn->reset_something();
    if(pn->connectionsCount() < 2) { untested();
      // pn->Name = "";  // no text at open nodes
		//                  // too late. d'uh
      continue; // BUG.
    }
    else { untested();
      hasNoComp = true;
      for(auto i : pn->connections()){ untested();
        pe = i;
        if(auto w=wire(pe)) { untested();
          if( w->isHorizontal() ) { untested();
				 pn->set_something(2);
			 }
        }else { untested();
          if( ((Component*)pe)->obsolete_model_hack() == "GND" ) { // BUG
            hasNoComp = true;   // no text at ground symbol
            break;
          }

		  	 // to the right is no room
          if(pn->cx_() < pe->cx_()){ untested();
			  	 pn->set_something(1);
			 }
          hasNoComp = false;
        }
		}
      if(hasNoComp) {  // text only were a component is connected
			incomplete(); // pn->Name = "";
        continue;
      }
    }

    pg->Var = pn->name() + ".V";
    pg->lastLoaded = QDateTime(); // Note 1 at the start of this function
    if(pg->loadDatFile(DataSet) == 2) { untested();
		 incomplete(); // node names are not mutable.
      // pn->name() = misc::num2str(*(pg->cPointsY)) + "V";
      NodeList.append(pn);             // remember node ...
      ValueList.append(pg->cPointsY);  // ... and all of its values
      pg->cPointsY = 0;   // do not delete it next time !
    }else{ untested();
		 incomplete();
    //  pn->Name = "0V";
	 }


    for(auto i : pn->connections()){ untested();
		 pe = i;
      if(pe->Type == isWire) { untested();
#if 0
        if( ((Wire*)pe)->portValue(0) != pn )  // no text at next node
          ((Wire*)pe)->portValue(0)->setName("");
        else  ((Wire*)pe)->portValue(1)->setName("");
#endif
      }
	 }
  }
#endif


  // create DC current through each probe
#if 0
  Node* pn;
  for(auto pc : Doc->components()){ untested();
    if(pc->obsolete_model_hack() == "IProbe") { // BUG.
      pn = pc->Ports.first()->Connection;
      if(!pn->name().isEmpty())   // preserve node voltage ?
        pn = pc->Ports.at(1)->Connection;

      pn->reset_something();
      pn->set_something(0x10);   // mark current
      pg->Var = pc->name() + ".I";
      pg->lastLoaded = QDateTime(); // Note 1 at the start of this function
      if(pg->loadDatFile(DataSet) == 2) { untested();
        pn->setName( misc::num2str(*(pg->cPointsY)) + "A");
        NodeList.append(pn);             // remember node ...
        ValueList.append(pg->cPointsY);  // ... and all of its values
        pg->cPointsY = 0;   // do not delete it next time !
      } else{ untested();
        pn->setName("0A");
		}

      for(auto i: pn->connections()){ untested();
			pe=i;
        if(pe->Type == isWire) { untested();
          if( ((Wire*)pe)->isHorizontal() ){ untested();
             pn->set_something(2);
          }else{ untested();
          }
        }
        else { untested();
          if(pn->cx_() < pe->cx_()) { untested();
            // to the right is no room
             pn->set_something(1);
          }
        }
		}
    }
  }
#endif


  Doc->showBias = 1;

  return pg;
}
