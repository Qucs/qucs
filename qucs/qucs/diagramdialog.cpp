/***************************************************************************
                          diagramdialog.cpp  -  description
                             -------------------
    begin                : Sun Oct 5 2003
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

#include "diagramdialog.h"

#include <qvbox.h>
#include <qlayout.h>
#include <qvbuttongroup.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qlabel.h>
#include <qstringlist.h>
#include <qmessagebox.h>
#include <qptrlist.h>
#include <qpoint.h>


// standard colors: blue, red, magenta, green, cyan, yellow, black (white is only a dummy)
static const QRgb DefaultColors[]
          = {0x0000ff, 0xff0000, 0xff00ff, 0x00ff00, 0x00ffff, 0xffff00, 0xffffff, 0x000000};


DiagramDialog::DiagramDialog(Diagram *d, const QString& _DataSet, QWidget *parent, const char *name )
                                  : QDialog(parent, name, Qt::WDestructiveClose)
{
  Diag = d;
  defaultDataSet = _DataSet;
  setCaption("Edit Diagram Properties");
  changed = false;
//  setFixedSize(QSize(400, 400));
//  setMinimumSize(QSize(400, 400));
  
  QVBoxLayout *all = new QVBoxLayout(this); // to provide the neccessary size
  QTabWidget *t = new QTabWidget(this);
  all->addWidget(t);

  // ...........................................................
  QVBox *Tab1 = new QVBox(this);
  Tab1->setSpacing(5);

  QHBox *Box1 = new QHBox(Tab1);
  Box1->setSpacing(5);

  QVButtonGroup *DataGroup = new QVButtonGroup("Dataset", Box1);
  ChooseData = new QComboBox(false, DataGroup);
  ChooseData->setMinimumWidth(200);
  connect(ChooseData, SIGNAL(activated(int)), SLOT(slotReadVars(int)));
  ChooseVars = new QListView(DataGroup);
  ChooseVars->addColumn("Name");
  ChooseVars->addColumn("Status");
  ChooseVars->addColumn("Size");
// QT 3.2
//  connect(Content, SIGNAL(doubleClicked(QListViewItem*, const QPoint &,int)),
//                   SLOT(slotTakeVar(QListViewItem*, const QPoint &,int)));
  connect(ChooseVars, SIGNAL(doubleClicked(QListViewItem*)), SLOT(slotTakeVar(QListViewItem*)));


  QVButtonGroup *GraphGroup = new QVButtonGroup("Graph", Box1);
  GraphList = new QListBox(GraphGroup);
  connect(GraphList, SIGNAL(highlighted(int)), SLOT(slotSelectGraph(int)));
  QPushButton *NewButt = new QPushButton("New Graph", GraphGroup);
  connect(NewButt, SIGNAL(clicked()), SLOT(slotNewGraph()));
  QPushButton *DelButt = new QPushButton("Delete Graph", GraphGroup);
  connect(DelButt, SIGNAL(clicked()), SLOT(slotDeleteGraph()));
  QPushButton *AppButt = new QPushButton("Apply Graph Input", GraphGroup);
  connect(AppButt, SIGNAL(clicked()), SLOT(slotApplyGraphInput()));

  QVButtonGroup *InputGroup = new QVButtonGroup("Graph Input", Tab1);
  GraphInput = new QLineEdit(InputGroup);

  t->addTab(Tab1, "Data");

  // ...........................................................
  QWidget *Tab2 = new QWidget(t);
  QGridLayout *gp = new QGridLayout(Tab2,6,3,5,5);

  QLabel *l1 = new QLabel("x-Axis Label:", Tab2);
  gp->addWidget(l1,0,0);
  xLabel = new QLineEdit(Tab2);
  gp->addWidget(xLabel,0,1);

  QLabel *l2 = new QLabel("y-Axis Label:", Tab2);
  gp->addWidget(l2,1,0);
  yLabel = new QLineEdit(Tab2);
  gp->addWidget(yLabel,1,1);

  t->addTab(Tab2, "Properties");

  // ...........................................................
  QHBox *Butts = new QHBox(this);
  Butts->setSpacing(5);
  Butts->setMargin(5);
  all->addWidget(Butts);

  QPushButton *OkButt = new QPushButton("OK", Butts);
  connect(OkButt, SIGNAL(clicked()), SLOT(slotOK()));
  QPushButton *ApplyButt = new QPushButton("Apply", Butts);
  connect(ApplyButt, SIGNAL(clicked()), SLOT(slotApply()));
  QPushButton *CancelButt = new QPushButton("Cancel", Butts);
  connect(CancelButt, SIGNAL(clicked()), SLOT(reject()));
  QPushButton *HelpButt = new QPushButton("Function Help", Butts);
  connect(HelpButt, SIGNAL(clicked()), SLOT(slotFuncHelp()));

  OkButt->setDefault(true);


  // ...........................................................
  // put all data files into ComboBox
  QDir ProjDir(".");
  QStringList Elements = ProjDir.entryList("*.dat", QDir::Files, QDir::Name);
  QStringList::iterator it;
  for(it = Elements.begin(); it != Elements.end(); ++it) {
    ChooseData->insertItem((*it).left((*it).length()-4));
    if((*it) == defaultDataSet)
      ChooseData->setCurrentItem(ChooseData->count()-1);  // default dataset should be the current
  }
  slotReadVars(0);  // put variables into the ListView

  // ...........................................................
  // put all graphs into the ListBox
  for(Graph *ptr1 = Diag->Graphs.first(); ptr1 != 0; ptr1 = Diag->Graphs.next())
    GraphList->insertItem(ptr1->Line);
}

DiagramDialog::~DiagramDialog()
{
}

// --------------------------------------------------------------------------
void DiagramDialog::slotReadVars(int)
{
  QString DocName = ChooseData->currentText()+".dat";
  
  QFile file(DocName);
  if(!file.open(IO_ReadOnly)) {
    return;
  }

  QString Line, tmp;
  QTextStream stream(&file);

  ChooseVars->clear();
  while(!stream.atEnd()) {
    Line = stream.readLine();
    if(Line.left(4) == "<dep") {
      tmp = Line.section(' ', 2);
      new QListViewItem(ChooseVars, Line.section(' ', 1, 1), "dep", tmp.remove('>'));
    }
    else if(Line.left(6) == "<indep") {
      tmp = Line.section(' ', 2, 2);
      new QListViewItem(ChooseVars, Line.section(' ', 1, 1), "indep", tmp.remove('>'));
    }
  }
  
  file.close();
}

// --------------------------------------------------------------------------
// Inserts the double-clicked variable into the Graph Input Line at the cursor position.
void DiagramDialog::slotTakeVar(QListViewItem *Item)
{
  int     i = GraphInput->cursorPosition();
  QString s = GraphInput->text();
  GraphInput->setText(s.left(i)+Item->text(0)+s.right(s.length()-i));
}

// --------------------------------------------------------------------------
// Puts the text of the selected graph into the line edit.
void DiagramDialog::slotSelectGraph(int index)
{
  GraphInput->setText(GraphList->text(index));
}

// --------------------------------------------------------------------------
void DiagramDialog::slotApplyGraphInput()
{
  if(GraphInput->text().isEmpty()) return;
  if(GraphList->currentItem() == -1) {   // is item selected ?
    GraphList->insertItem(GraphInput->text());

    int z=0;
    Graph *g = Diag->Graphs.last();
    if(g != 0) {
      while(DefaultColors[z] != 0)  // find the next graph color
        if(g->Color == QColor(DefaultColors[z++])) break;
      if(DefaultColors[z] == 0) z = 0;
    }

    g = new Graph(GraphInput->text());   // create a new graph
    g->Color = QColor(DefaultColors[z]);
    Diag->Graphs.append(g);
  }
  else  // change the selected graph
    GraphList->changeItem(GraphInput->text(), GraphList->currentItem());

  GraphInput->setText("");  // erase input line
  changed = true;
}

// --------------------------------------------------------------------------
// Is called when the 'delelte graph' button is pressed.
void DiagramDialog::slotDeleteGraph()
{
  int i = GraphList->currentItem();
  if(i == -1) return;   // return, if no item selected

  GraphList->removeItem(i);
  Diag->Graphs.remove(i);
  changed = true;
}

// --------------------------------------------------------------------------
void DiagramDialog::slotNewGraph()
{
  if(GraphInput->text().isEmpty()) return;

  GraphList->insertItem(GraphInput->text());

  int z=0;
  Graph *g = Diag->Graphs.last();
  if(g != 0) {
    while(DefaultColors[z] != 0)  // find the next graph color
      if(g->Color == QColor(DefaultColors[z++])) break;
    if(DefaultColors[z] == 0) z = 0;
  }

  g = new Graph(GraphInput->text());   // create a new graph
  g->Color = QColor(DefaultColors[z]);
  Diag->Graphs.append(g);
  changed = true;
}

// --------------------------------------------------------------------------
void DiagramDialog::slotOK()
{
  slotApply();
  if(changed) done(1);
  else done(0);
}

// --------------------------------------------------------------------------
void DiagramDialog::slotApply()
{
  Diag->loadGraphData(defaultDataSet);
}

// --------------------------------------------------------------------------
void DiagramDialog::slotFuncHelp()
{
}
