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

#include "../qucsview.h"

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
#include <qvalidator.h>
#include <qcolordialog.h>


// standard colors: blue, red, magenta, green, cyan, yellow, black
// (white is only a dummy)
static const QRgb DefaultColors[]
          = {0x0000ff, 0xff0000, 0xff00ff, 0x00ff00, 0x00ffff, 0xffff00,
             0xffffff, 0x000000};


DiagramDialog::DiagramDialog(Diagram *d, const QString& _DataSet,
                             QWidget *parent, const char *name )
                    : QDialog(parent, name, TRUE, Qt::WDestructiveClose)
{
  Diag = d;
  defaultDataSet = _DataSet;
  setCaption(tr("Edit Diagram Properties"));
  changed = false;
  toTake = false;   // double-clicked variable be inserted into graph list ?
//  setFixedSize(QSize(400, 400));
//  setMinimumSize(QSize(400, 400));
  
  QVBoxLayout *all = new QVBoxLayout(this); // to provide neccessary size
  QTabWidget *t = new QTabWidget(this);
  all->addWidget(t);

  // ...........................................................
  QVBox *Tab1 = new QVBox(this);
  Tab1->setSpacing(5);

  QVButtonGroup *InputGroup = new QVButtonGroup(tr("Graph Input"), Tab1);
  GraphInput = new QLineEdit(InputGroup);
  connect(GraphInput, SIGNAL(textChanged(const QString&)),
		      SLOT(slotResetToTake(const QString&)));
  QHBox *Box2 = new QHBox(InputGroup);
  Box2->setSpacing(5);
  QLabel *l3 = new QLabel(tr("Color:"),Box2);
  ColorButt = new QPushButton("        ",Box2);
  connect(ColorButt, SIGNAL(clicked()), SLOT(slotSetColor()));
  QLabel *l4 = new QLabel(tr("       Thickness:"),Box2);
  Expr.setPattern("[0-9]{1}");  // valid expression for property input
  QValidator *Validator = new QRegExpValidator(Expr, this);
  GraphThick = new QLineEdit(Box2);
  GraphThick->setValidator(Validator);
  GraphThick->setMaximumWidth(20);
  GraphThick->setText("0");
  connect(GraphThick, SIGNAL(textChanged(const QString&)),
		      SLOT(slotSetThick(const QString&)));

  QHBox *Box1 = new QHBox(Tab1);
  Box1->setSpacing(5);

  QVButtonGroup *DataGroup = new QVButtonGroup(tr("Dataset"), Box1);
  ChooseData = new QComboBox(false, DataGroup);
  ChooseData->setMinimumWidth(200);
  connect(ChooseData, SIGNAL(activated(int)), SLOT(slotReadVars(int)));
  ChooseVars = new QListView(DataGroup);
  ChooseVars->addColumn(tr("Name"));
  ChooseVars->addColumn(tr("Status"));
  ChooseVars->addColumn(tr("Size"));
// QT 3.2
//  connect(Content, SIGNAL(doubleClicked(QListViewItem*, const QPoint &,int)),
//                   SLOT(slotTakeVar(QListViewItem*, const QPoint &,int)));
  connect(ChooseVars, SIGNAL(doubleClicked(QListViewItem*)),
		      SLOT(slotTakeVar(QListViewItem*)));


  QVButtonGroup *GraphGroup = new QVButtonGroup(tr("Graph"), Box1);
  GraphList = new QListBox(GraphGroup);
  connect(GraphList, SIGNAL(clicked(QListBoxItem*)),
		     SLOT(slotSelectGraph(QListBoxItem*)));
  QPushButton *NewButt = new QPushButton(tr("New Graph"), GraphGroup);
  connect(NewButt, SIGNAL(clicked()), SLOT(slotNewGraph()));
  QPushButton *DelButt = new QPushButton(tr("Delete Graph"), GraphGroup);
  connect(DelButt, SIGNAL(clicked()), SLOT(slotDeleteGraph()));

  t->addTab(Tab1, tr("Data"));

  // ...........................................................
  QWidget *Tab2 = new QWidget(t);
  QGridLayout *gp = new QGridLayout(Tab2,6,2,5,5);

  QLabel *l1 = new QLabel(tr("x-Axis Label:"), Tab2);
  gp->addWidget(l1,0,0);
  xLabel = new QLineEdit(Tab2);
  gp->addWidget(xLabel,0,1);

  QLabel *l2 = new QLabel(tr("y-Axis Label:"), Tab2);
  gp->addWidget(l2,1,0);
  yLabel = new QLineEdit(Tab2);
  gp->addWidget(yLabel,1,1);

  GridOn = new QCheckBox(tr("show Grid"), Tab2);
  gp->addMultiCellWidget(GridOn,2,2,0,1);

  t->addTab(Tab2, tr("Properties"));

  // ...........................................................
  QHBox *Butts = new QHBox(this);
  Butts->setSpacing(5);
  Butts->setMargin(5);
  all->addWidget(Butts);

  QPushButton *OkButt = new QPushButton(tr("OK"), Butts);
  connect(OkButt, SIGNAL(clicked()), SLOT(slotOK()));
  QPushButton *ApplyButt = new QPushButton(tr("Apply"), Butts);
  connect(ApplyButt, SIGNAL(clicked()), SLOT(slotApply()));
  QPushButton *CancelButt = new QPushButton(tr("Cancel"), Butts);
  connect(CancelButt, SIGNAL(clicked()), SLOT(reject()));
  QPushButton *HelpButt = new QPushButton(tr("Function Help"), Butts);
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
      // default dataset should be the current
      ChooseData->setCurrentItem(ChooseData->count()-1);
  }
  slotReadVars(0);  // put variables into the ListView

  // ...........................................................
  // put all graphs into the ListBox
  for(Graph *pg = Diag->Graphs.first(); pg != 0; pg = Diag->Graphs.next())
    GraphList->insertItem(pg->Var);
  ColorButt->setPaletteBackgroundColor
             (QColor(DefaultColors[GraphList->count()]));

  // ...........................................................
  // some differences between the different diagram types
  if(Diag->Name == "Tab") {
    ColorButt->setEnabled(false);
    GraphThick->setEnabled(false);
    l3->setEnabled(false);
    l4->setEnabled(false);
  }

  // ...........................................................
  // transfer the diagram properties to the dialog
  xLabel->setText(Diag->xLabel);
  yLabel->setText(Diag->yLabel);
  GridOn->setChecked(Diag->GridOn);
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
  QTextStream ReadWhole(&file);
  QString FileString = ReadWhole.read();   // read whole data file
  file.close();

  ChooseVars->clear();
  int i=0, j=0;
  i = FileString.find('<')+1;
  if(i > 0)
  do {
    j = FileString.find('>', i);
    Line = FileString.mid(i, j-i);
    i = FileString.find('<', j)+1;
    if(Line.left(3) == "dep") {
      tmp = Line.section(' ', 2);
      new QListViewItem(ChooseVars, Line.section(' ', 1, 1).remove('>'),
			"dep", tmp.remove('>'));
    }
    else if(Line.left(5) == "indep") {
      tmp = Line.section(' ', 2, 2);
      new QListViewItem(ChooseVars, Line.section(' ', 1, 1).remove('>'),
			"indep", tmp.remove('>'));
    }
  } while(i > 0);
}

// ------------------------------------------------------------------------
// Inserts the double-clicked variable into the Graph Input Line at the
// cursor position. If the Graph Input is empty, then the variable is
// also inserted as graph.
void DiagramDialog::slotTakeVar(QListViewItem *Item)
{
 GraphInput->blockSignals(true);
  if(toTake) GraphInput->setText("");

  int     i = GraphInput->cursorPosition();
  QString s = GraphInput->text();
  GraphInput->setText(s.left(i)+Item->text(0)+s.right(s.length()-i));

  if(s.isEmpty()) {
    GraphList->insertItem(GraphInput->text());
    GraphList->setSelected(GraphList->count()-1,true);

    Graph *g = new Graph(GraphInput->text());   // create a new graph
    g->Color = ColorButt->paletteBackgroundColor();
    g->Thick = GraphThick->text().toInt();
    Diag->Graphs.append(g);

    ColorButt->setPaletteBackgroundColor(
		QColor(DefaultColors[GraphList->count()]));
    changed = true;
    toTake  = true;
  }

  GraphInput->blockSignals(false);
}

// --------------------------------------------------------------------------
// Puts the text of the selected graph into the line edit.
void DiagramDialog::slotSelectGraph(QListBoxItem *item)
{
  if(item == 0) {
    GraphList->clearSelection();
    return;
  }

  int index = GraphList->index(item);
  GraphInput->blockSignals(true);
  GraphInput->setText(GraphList->text(index));
  GraphInput->blockSignals(false);

  Graph *g = Diag->Graphs.at(index);
  GraphThick->setText(QString::number(g->Thick));
  ColorButt->setPaletteBackgroundColor(g->Color);
  toTake = false;
}

// --------------------------------------------------------------------------
// Is called when the 'delelte graph' button is pressed.
void DiagramDialog::slotDeleteGraph()
{
  int i = GraphList->index(GraphList->selectedItem());
  if(i < 0) return;   // return, if no item selected

  GraphList->removeItem(i);
  Diag->Graphs.remove(i);

  GraphInput->setText("");  // erase input line and back to default values
  ColorButt->setPaletteBackgroundColor(
		QColor(DefaultColors[GraphList->count()]));
  GraphThick->setText("1");
  changed = true;
  toTake  = false;
}

// --------------------------------------------------------------------------
void DiagramDialog::slotNewGraph()
{
  if(GraphInput->text().isEmpty()) return;

  GraphList->insertItem(GraphInput->text());

  Graph *g = new Graph(GraphInput->text());   // create a new graph
  g->Color = ColorButt->paletteBackgroundColor();
  g->Thick = GraphThick->text().toInt();
  Diag->Graphs.append(g);
  changed = true;
  toTake  = false;
}

// --------------------------------------------------------------------------
// Is called if "Ok" button is pressed.
void DiagramDialog::slotOK()
{
  slotApply();
  if(changed) done(QDialog::Accepted);
  else done(QDialog::Rejected);
}

// --------------------------------------------------------------------------
// Is called if "Apply" button is pressed.
void DiagramDialog::slotApply()
{
  if(Diag->xLabel.isEmpty()) Diag->xLabel = ""; // can be non-Null and empty!
  if(xLabel->text().isEmpty()) xLabel->setText("");
  if(Diag->xLabel != xLabel->text()) {
    Diag->xLabel = xLabel->text();
    changed = true;
  }
  if(Diag->yLabel.isEmpty()) Diag->yLabel = ""; // can be non-Null and empty!
  if(yLabel->text().isEmpty()) yLabel->setText("");
  if(Diag->yLabel != yLabel->text()) {
    Diag->yLabel = yLabel->text();
    changed = true;
  }
  if(Diag->GridOn != GridOn->isChecked()) {
    Diag->GridOn = GridOn->isChecked();
    changed = true;
  }

  Diag->loadGraphData(defaultDataSet);
  ((QucsView*)parent())->viewport()->repaint();
}

// --------------------------------------------------------------------------
void DiagramDialog::slotFuncHelp()
{
}

// --------------------------------------------------------------------------
void DiagramDialog::slotSetColor()
{
  QColor c = QColorDialog::getColor(ColorButt->paletteBackgroundColor(),this);
  if(!c.isValid()) return;
  ColorButt->setPaletteBackgroundColor(c);

  int i = GraphList->index(GraphList->selectedItem());
  if(i < 0) return;   // return, if no item selected

  Graph *g = Diag->Graphs.at(i);
  g->Color = c;
  changed = true;
  toTake  = false;
}

// --------------------------------------------------------------------------
// Is set if the graph input line changes.
void DiagramDialog::slotResetToTake(const QString& s)
{
  int i = GraphList->index(GraphList->selectedItem());
  if(i < 0) return;   // return, if no item selected

  Graph *g = Diag->Graphs.at(i);
  g->Var = s;
  GraphList->changeItem(s, i);   // must done after the graph settings !!!
  changed = true;
  toTake  = false;
}

// --------------------------------------------------------------------------
// Is called if the user changes the graph thickness.
void DiagramDialog::slotSetThick(const QString& s)
{
  int i = GraphList->index(GraphList->selectedItem());
  if(i < 0) return;   // return, if no item selected

  Graph *g = Diag->Graphs.at(i);
  g->Thick = s.toInt();
  changed = true;
  toTake  = false;
}
