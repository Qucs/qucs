/***************************************************************************
                          diagramdialog.cpp  -  description
                             -------------------
    begin                : Sun Oct 5 2003
    copyright            : (C) 2003 by Michael Margraf
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

#include "diagramdialog.h"
#include "qucsview.h"
#include "qucs.h"

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
#include <qvalidator.h>


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
  Graphs.setAutoDelete(true);
  copyDiagramGraphs();   // make a copy of all graphs
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
  Expr.setPattern("[0-9]{1,2}");  // valid expression for property input
  QValidator *Validator = new QRegExpValidator(Expr, this);

  if(Diag->Name == "Tab") {
    Label1 = new QLabel(tr("Number Notation: "), Box2);
    PropertyBox = new QComboBox(Box2);
    PropertyBox->insertItem(tr("real/imaginary"));
    PropertyBox->insertItem(tr("magnitude/angle (degree)"));
    PropertyBox->insertItem(tr("magnitude/angle (radian)"));
    connect(PropertyBox, SIGNAL(activated(int)), SLOT(slotSetNumMode(int)));
    Box2->setStretchFactor(new QWidget(Box2), 5); // stretchable placeholder

    Label2 = new QLabel(tr("Precision:"), Box2);
    Property2 = new QLineEdit(Box2);
    Property2->setValidator(Validator);
    Property2->setMaximumWidth(25);
    Property2->setText("3");
  }
  else {
    Label1 = new QLabel(tr("Color:"),Box2);
    ColorButt = new QPushButton("        ",Box2);
    ColorButt->setEnabled(false);
    connect(ColorButt, SIGNAL(clicked()), SLOT(slotSetColor()));
    Box2->setStretchFactor(new QWidget(Box2), 5); // stretchable placeholder

    Label2 = new QLabel(tr("Thickness:"),Box2);
    Property2 = new QLineEdit(Box2);
    Property2->setValidator(Validator);
    Property2->setMaximumWidth(25);
    Property2->setText("0");

    QHBox *Box3 = new QHBox(InputGroup);
    Box3->setSpacing(5);
    Label3 = new QLabel(tr("Style:"),Box3);
    Label3->setEnabled(false);
    PropertyBox = new QComboBox(Box3);
    PropertyBox->insertItem(tr("solid line"));
    PropertyBox->insertItem(tr("dash line"));
    PropertyBox->insertItem(tr("dot line"));
    PropertyBox->insertItem(tr("long dash line"));
    connect(PropertyBox, SIGNAL(activated(int)),
			 SLOT(slotSetGraphStyle(int)));
    Box3->setStretchFactor(new QWidget(Box3), 5); // stretchable placeholder
  }
  connect(Property2, SIGNAL(textChanged(const QString&)),
			SLOT(slotSetProp2(const QString&)));

  Label1->setEnabled(false);
  PropertyBox->setEnabled(false);
  Label2->setEnabled(false);
  Property2->setEnabled(false);

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
    QGridLayout *gp = new QGridLayout(Tab2,8,2,5,5);

  if(Diag->Name != "Tab") {
    gp->addWidget(new QLabel(tr("x-Axis Label:"), Tab2), 0,0);
    xLabel = new QLineEdit(Tab2);
    gp->addWidget(xLabel,0,1);

    gp->addWidget(new QLabel(tr("y-Axis Label:"), Tab2), 1,0);
    yLabel = new QLineEdit(Tab2);
    gp->addWidget(yLabel,1,1);

    GridOn = new QCheckBox(tr("show Grid"), Tab2);
    connect(GridOn, SIGNAL(stateChanged(int)), SLOT(slotSetGridBox(int)));
    gp->addMultiCellWidget(GridOn,2,2,0,1);

    GridLabel1 = new QLabel(tr("Grid Color:"),Tab2);
    gp->addWidget(GridLabel1, 3,0);
    GridColorButt = new QPushButton("        ",Tab2);
    connect(GridColorButt, SIGNAL(clicked()), SLOT(slotSetGridColor()));
    gp->addWidget(GridColorButt, 3,1);

    GridLabel2 = new QLabel(tr("Grid Style: "), Tab2);
    gp->addWidget(GridLabel2, 4,0);
    GridStyleBox = new QComboBox(Tab2);
    GridStyleBox->insertItem(tr("solid line"));
    GridStyleBox->insertItem(tr("dash line"));
    GridStyleBox->insertItem(tr("dot line"));
    GridStyleBox->insertItem(tr("dash dot line"));
    GridStyleBox->insertItem(tr("dash dot dot line"));
    gp->addWidget(GridStyleBox, 4,1);

    // ...........................................................
    // transfer the diagram properties to the dialog
    xLabel->setText(Diag->xLabel);
    yLabel->setText(Diag->yLabel);
    GridOn->setChecked(Diag->GridOn);
    if(!Diag->GridOn) slotSetGridBox(QButton::Off);
    GridColorButt->setPaletteBackgroundColor(Diag->GridPen.color());
    GridStyleBox->setCurrentItem(Diag->GridPen.style()-1);

    if(Diag->Name == "Rect") {
      GridLogX = new QCheckBox(tr("logarithmical X Axis Grid"), Tab2);
      gp->addMultiCellWidget(GridLogX,5,5,0,1);

      GridLogY = new QCheckBox(tr("logarithmical Y Axis Grid"), Tab2);
      gp->addMultiCellWidget(GridLogY,6,6,0,1);

      // ...........................................................
      // transfer the diagram properties to the dialog
      GridLogX->setChecked(Diag->xlog);
      GridLogY->setChecked(Diag->ylog);
    }
  }

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
  connect(CancelButt, SIGNAL(clicked()), SLOT(slotCancel()));
//  QPushButton *HelpButt = new QPushButton(tr("Function Help"), Butts);
//  connect(HelpButt, SIGNAL(clicked()), SLOT(slotFuncHelp()));

  OkButt->setDefault(true);


  // ...........................................................
  // put all data files into ComboBox
  QDir ProjDir(QucsWorkDir);
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

  if(Diag->Name != "Tab")
    ColorButt->setPaletteBackgroundColor
	(QColor(DefaultColors[GraphList->count()]));
}

DiagramDialog::~DiagramDialog()
{
}

// --------------------------------------------------------------------------
void DiagramDialog::slotReadVars(int)
{
  QString DocName = ChooseData->currentText()+".dat";

  QFile file(QucsWorkDir.filePath(DocName));
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

  int     i  = GraphInput->cursorPosition();
  QString s  = GraphInput->text();
  QString s1 = Item->text(0);
  if(ChooseData->currentText() != defaultDataSet.section('.',0,0))
    s1 = ChooseData->currentText() + ":" + s1;

  GraphInput->setText(s.left(i) + s1 + s.right(s.length()-i));

  if(s.isEmpty()) {
    GraphList->insertItem(GraphInput->text());
    GraphList->setSelected(GraphList->count()-1,true);

    Graph *g = new Graph(GraphInput->text());   // create a new graph

    if(Diag->Name != "Tab") {
      g->Color = ColorButt->paletteBackgroundColor();
      g->Thick = Property2->text().toInt();
      ColorButt->setPaletteBackgroundColor(
		QColor(DefaultColors[GraphList->count()]));
      g->Style =  PropertyBox->currentItem();

     Label3->setEnabled(true);
     ColorButt->setEnabled(true);
    }
    else {
      g->Precision = Property2->text().toInt();
      g->numMode   = PropertyBox->currentItem();
    }

    Graphs.append(g);
    changed = true;
    toTake  = true;
  }

  GraphInput->blockSignals(false);

  Label1->setEnabled(true);
  PropertyBox->setEnabled(true);
  Label2->setEnabled(true);
  Property2->setEnabled(true);
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

  Graph *g = Graphs.at(index);
  if(Diag->Name != "Tab") {
    Property2->setText(QString::number(g->Thick));
    ColorButt->setPaletteBackgroundColor(g->Color);
    PropertyBox->setCurrentItem(g->Style);

    Label3->setEnabled(true);
    ColorButt->setEnabled(true);
  }
  else {
    Property2->setText(QString::number(g->Precision));
    PropertyBox->setCurrentItem(g->numMode);
  }
  toTake = false;

  Label1->setEnabled(true);
  PropertyBox->setEnabled(true);
  Label2->setEnabled(true);
  Property2->setEnabled(true);
}

// --------------------------------------------------------------------------
// Is called when the 'delelte graph' button is pressed.
void DiagramDialog::slotDeleteGraph()
{
  int i = GraphList->index(GraphList->selectedItem());
  if(i < 0) return;   // return, if no item selected

  GraphList->removeItem(i);
  Graphs.remove(i);

  GraphInput->setText("");  // erase input line and back to default values
  if(Diag->Name != "Tab") {
    ColorButt->setPaletteBackgroundColor(
		QColor(DefaultColors[GraphList->count()]));
    Property2->setText("0");

    Label3->setEnabled(false);
    ColorButt->setEnabled(false);
  }
  else  Property2->setText("3");
  PropertyBox->setCurrentItem(0);
  changed = true;
  toTake  = false;

  Label1->setEnabled(false);
  PropertyBox->setEnabled(false);
  Label2->setEnabled(false);
  Property2->setEnabled(false);
}

// --------------------------------------------------------------------------
void DiagramDialog::slotNewGraph()
{
  if(GraphInput->text().isEmpty()) return;

  GraphList->insertItem(GraphInput->text());

  Graph *g = new Graph(GraphInput->text());   // create a new graph
  if(Diag->Name != "Tab") {
    g->Color = ColorButt->paletteBackgroundColor();
    g->Thick = Property2->text().toInt();
    g->Style = PropertyBox->currentItem();
  }
  else {
    g->Precision = Property2->text().toInt();
    g->numMode   = PropertyBox->currentItem();
  }
  Graphs.append(g);
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
  if(Diag->Name != "Tab") {
    if(Diag->xLabel.isEmpty()) Diag->xLabel = ""; // can be not 0 and empty!
    if(xLabel->text().isEmpty()) xLabel->setText("");
    if(Diag->xLabel != xLabel->text()) {
      Diag->xLabel = xLabel->text();
      changed = true;
    }
    if(Diag->yLabel.isEmpty()) Diag->yLabel = ""; // can be not 0 and empty!
    if(yLabel->text().isEmpty()) yLabel->setText("");
    if(Diag->yLabel != yLabel->text()) {
      Diag->yLabel = yLabel->text();
      changed = true;
    }
    if(Diag->GridOn != GridOn->isChecked()) {
      Diag->GridOn = GridOn->isChecked();
      changed = true;
    }
    if(Diag->GridPen.color() != GridColorButt->paletteBackgroundColor()) {
      Diag->GridPen.setColor(GridColorButt->paletteBackgroundColor());
      changed = true;
    }
    if(Diag->GridPen.style()!=(Qt::PenStyle)(GridStyleBox->currentItem()+1)) {
      Diag->GridPen.setStyle((Qt::PenStyle)(GridStyleBox->currentItem()+1));
      changed = true;
    }
    if(Diag->Name == "Rect") {
      if(Diag->xlog != GridLogX->isChecked()) {
        Diag->xlog = GridLogX->isChecked();
        changed = true;
      }
      if(Diag->ylog != GridLogY->isChecked()) {
        Diag->ylog = GridLogY->isChecked();
        changed = true;
      }
    }
  }
  Diag->Graphs.clear();   // delete the graphs
  Graphs.setAutoDelete(false);
  for(Graph *pg = Graphs.first(); pg != 0; pg = Graphs.next())
    Diag->Graphs.append(pg);  // transfer the new graphs to diagram
  Graphs.clear();
  Graphs.setAutoDelete(true);

  Diag->loadGraphData(defaultDataSet);
  ((QucsView*)parent())->viewport()->repaint();
  copyDiagramGraphs();
}


// --------------------------------------------------------------------------
// Is called if "Cancel" button is pressed.
void DiagramDialog::slotCancel()
{
//  Diag->loadGraphData(defaultDataSet);
//  ((QucsView*)parent())->viewport()->repaint();
  reject();
}

// --------------------------------------------------------------------------
void DiagramDialog::slotSetColor()
{
  QColor c = QColorDialog::getColor(ColorButt->paletteBackgroundColor(),this);
  if(!c.isValid()) return;
  ColorButt->setPaletteBackgroundColor(c);

  int i = GraphList->index(GraphList->selectedItem());
  if(i < 0) return;   // return, if no item selected

  Graph *g = Graphs.at(i);
  g->Color = c;
  changed = true;
  toTake  = false;
}

// --------------------------------------------------------------------------
void DiagramDialog::slotSetGridColor()
{
  QColor c = QColorDialog::getColor(
			GridColorButt->paletteBackgroundColor(),this);
  if(!c.isValid()) return;
  GridColorButt->setPaletteBackgroundColor(c);
  changed = true;
}

// --------------------------------------------------------------------------
// Is set if the graph input line changes.
void DiagramDialog::slotResetToTake(const QString& s)
{
  int i = GraphList->index(GraphList->selectedItem());
  if(i < 0) return;   // return, if no item selected

  Graph *g = Graphs.at(i);
  g->Var = s;
  GraphList->changeItem(s, i);   // must done after the graph settings !!!
  changed = true;
  toTake  = false;
}

// --------------------------------------------------------------------------
// Is called if the user changes the graph thickness or the precision.
void DiagramDialog::slotSetProp2(const QString& s)
{
  int i = GraphList->index(GraphList->selectedItem());
  if(i < 0) return;   // return, if no item selected

  Graph *g = Graphs.at(i);
  if(Diag->Name == "Tab") g->Precision = s.toInt();
  else  g->Thick = s.toInt();
  changed = true;
  toTake  = false;
}

// --------------------------------------------------------------------------
// Is called if the user changes the number mode.
void DiagramDialog::slotSetNumMode(int Mode)
{
  int i = GraphList->index(GraphList->selectedItem());
  if(i < 0) return;   // return, if no item selected

  Graph *g = Graphs.at(i);
  g->numMode = Mode;
  changed = true;
  toTake  = false;
}

// --------------------------------------------------------------------------
// Is called when the "show grid" checkbox is changed.
void DiagramDialog::slotSetGridBox(int state)
{
  if(state == QButton::On) {
    GridColorButt->setEnabled(true);
    GridStyleBox->setEnabled(true);
    GridLabel1->setEnabled(true);
    GridLabel2->setEnabled(true);
  }
  else {
    GridColorButt->setEnabled(false);
    GridStyleBox->setEnabled(false);
    GridLabel1->setEnabled(false);
    GridLabel2->setEnabled(false);
  }
}

// --------------------------------------------------------------------------
// Is called if the user changes the graph style (combobox).
void DiagramDialog::slotSetGraphStyle(int style)
{
  int i = GraphList->index(GraphList->selectedItem());
  if(i < 0) return;   // return, if no item selected

  Graph *g = Graphs.at(i);
  g->Style = style;
  changed = true;
  toTake  = false;
}

// --------------------------------------------------------------------------
// Makes a copy of all graphs in the diagram.
void DiagramDialog::copyDiagramGraphs()
{
  for(Graph *pg = Diag->Graphs.first(); pg != 0; pg = Diag->Graphs.next())
    Graphs.append(pg->sameNewOne());
}
