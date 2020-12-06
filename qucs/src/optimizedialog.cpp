/***************************************************************************
    copyright            : (C) 2006 by Michael Margraf
                               2016 Qucs Team
                               2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qucs.h"
#include "components/optimizedialog.h"
#include "components/opt_sim.h"
#include "schematic_doc.h"
#include "property.h"

#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QTableWidget>
#include <QTabWidget>
#include <QValidator>
#include <QPushButton>
#include <QMessageBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QApplication>
#include <QClipboard>
#include <QAction>
#include <QMenu>
#include <QInputDialog>


OptimizeDialog::OptimizeDialog(QucsDoc* d) : SchematicDialog(d)
{
  setWindowTitle(tr("Edit Optimization Properties"));

}

void OptimizeDialog::attach(Object*)
{
	incomplete();
#if 0 // BUG: it's a Simulation
  Comp = prechecked_cast<Optimize_Sim*>(sim);
  assert(Comp);
  Doc = nullptr; //??? sim->parent?
  changed = false;
  numPrec = 3;

  Expr.setPattern("[\\w_]+");
  Validator = new QRegExpValidator(Expr, this);
  numVal = new QDoubleValidator(this);
  intVal = new QIntValidator(this);

  all = new QVBoxLayout(); // to provide the neccessary size
  QTabWidget *t = new QTabWidget();
  all->addWidget(t);
  this->setLayout(all);

  // ...........................................................
  QWidget *Tab1 = new QWidget(t);
  QGridLayout *gp1 = new QGridLayout();
  Tab1->setLayout(gp1);

  NameEdit = new QLineEdit();
  NameEdit->setValidator(Validator);
  SimEdit = new QComboBox(Tab1);
  SimEdit->setEditable(true);

  gp1->addWidget(new QLabel(tr("Name:"), Tab1), 0, 0);
  gp1->addWidget(NameEdit, 0, 1);
  gp1->addWidget(new QLabel(tr("Simulation:"), Tab1), 1, 0);
  gp1->addWidget(SimEdit, 1, 1);

  setTabOrder(NameEdit, SimEdit);

  t->addTab(Tab1, tr("General"));

  // ...........................................................
  QWidget *Tab2 = new QWidget(t);
  QGridLayout *gp2 = new QGridLayout();
  Tab2->setLayout(gp2);

  MethodCombo = new QComboBox();
  MethodCombo->insertItems(-1, QString("DE/best/1/exp;"
				       "DE/rand/1/exp;"
				       "DE/rand-to-best/1/exp;"
				       "DE/best/2/exp;"
				       "DE/rand/1/exp;"
				       "DE/best/1/bin;"
				       "DE/rand/1/bin;"
				       "DE/rand-to-best/1/bin;"
				       "DE/best/2/bin;"
				       "DE/rand/2/bin").split(";"));

  gp2->addWidget(new QLabel(tr("Method:")), 0,0);
  gp2->addWidget(MethodCombo,0,1);

  IterEdit = new QLineEdit();
  IterEdit->setValidator(intVal);

  gp2->addWidget(new QLabel(tr("Maximum number of iterations:")), 1,0);
  gp2->addWidget(IterEdit,1,1);

  RefreshEdit = new QLineEdit();
  RefreshEdit->setValidator(intVal);

  gp2->addWidget(new QLabel(tr("Output refresh cycle:")), 2,0);
  gp2->addWidget(RefreshEdit,2,1);

  ParentsEdit = new QLineEdit();
  ParentsEdit->setValidator(intVal);

  gp2->addWidget(new QLabel(tr("Number of parents:")), 3,0);
  gp2->addWidget(ParentsEdit,3,1);

  ConstEdit = new QLineEdit();
  ConstEdit->setValidator(new QDoubleValidator(0.0,2.0,20,ConstEdit));

  gp2->addWidget(new QLabel(tr("Constant F:")), 4,0);
  gp2->addWidget(ConstEdit,4,1);

  CrossEdit = new QLineEdit();
  CrossEdit->setValidator(new QDoubleValidator(0.0,1.0,20,CrossEdit));

  gp2->addWidget(new QLabel(tr("Crossing over factor:")), 5,0);
  gp2->addWidget(CrossEdit,5,1);

  SeedEdit = new QLineEdit();
  SeedEdit->setValidator(numVal);

  gp2->addWidget(new QLabel(tr("Pseudo random number seed:")), 6,0);
  gp2->addWidget(SeedEdit,6,1);

  CostVarEdit = new QLineEdit();
  CostVarEdit->setValidator(numVal);

  gp2->addWidget(new QLabel(tr("Minimum cost variance:")), 7,0);
  gp2->addWidget(CostVarEdit,7,1);

  CostObjEdit = new QLineEdit();
  CostObjEdit->setValidator(numVal);

  gp2->addWidget(new QLabel(tr("Cost objectives:")), 8,0);
  gp2->addWidget(CostObjEdit,8,1);

  CostConEdit = new QLineEdit();
  CostConEdit->setValidator(numVal);

  gp2->addWidget(new QLabel(tr("Cost constraints:")), 9,0);
  gp2->addWidget(CostConEdit,9,1);

  t->addTab(Tab2, tr("Algorithm"));

  // ...........................................................
  QWidget *Tab3 = new QWidget(t);
  QGridLayout *gp3 = new QGridLayout();
  Tab3->setLayout(gp3);

  VarTable = new QTableWidget();
  VarTable->horizontalHeader()->setStretchLastSection(true);
  VarTable->verticalHeader()->hide();
  VarTable->setColumnCount(6);
  VarTable->setHorizontalHeaderLabels(
      QStringList() << tr("Name") << tr("active") << tr("initial") << tr("min") << tr("max") << tr("Type"));
  VarTable->setSortingEnabled(false);
  VarTable->setSelectionBehavior(QAbstractItemView::SelectRows);
#if QT_VERSION < 0x050000
  VarTable->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  VarTable->horizontalHeader()->setClickable(false); // no action when clicking on the header 
#endif

  // right-click on the table header to open the context menu
  VarTable->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(VarTable->horizontalHeader(), SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slotSetPrecision(const QPoint&)));

  connect(VarTable, SIGNAL(currentCellChanged(int, int, int, int)),
      SLOT(slotEditVariable()));
  gp3->addWidget(VarTable, 0, 0, 1, 3);

  VarNameEdit = new QLineEdit();
  VarNameEdit->setValidator(Validator);
  connect(VarNameEdit, SIGNAL(textChanged(const QString&)),
          SLOT(slotChangeVarName(const QString&)));
  VarActiveCheck = new QCheckBox(tr("active"));
  VarActiveCheck->setChecked(true);
  connect(VarActiveCheck, SIGNAL(toggled(bool)),
          SLOT(slotChangeVarActive(bool)));

  QHBoxLayout *VarLine = new QHBoxLayout();
  VarLine->setSpacing(3);
  VarLine->addWidget(new QLabel(tr("Name:")));
  VarLine->addWidget(VarNameEdit);
  VarLine->addWidget(VarActiveCheck);
  gp3->addLayout(VarLine, 1, 0, 1, 3);

  VarInitEdit = new QLineEdit();
  VarInitEdit->setValidator(numVal);
  connect(VarInitEdit, SIGNAL(textChanged(const QString&)),
          SLOT(slotChangeVarInit(const QString&)));
  VarMinEdit = new QLineEdit();
  VarMinEdit->setValidator(numVal);
  connect(VarMinEdit, SIGNAL(textChanged(const QString&)),
          SLOT(slotChangeVarMin(const QString&)));
  VarMaxEdit = new QLineEdit(Tab3);
  VarMaxEdit->setValidator(numVal);
  connect(VarMaxEdit, SIGNAL(textChanged(const QString&)),
          SLOT(slotChangeVarMax(const QString&)));

  gp3->addWidget(new QLabel(tr("initial:")), 2,0);
  gp3->addWidget(new QLabel(tr("min:")), 2,1);
  gp3->addWidget(new QLabel(tr("max:")), 2,2);
  gp3->addWidget(VarInitEdit,3,0);
  gp3->addWidget(VarMinEdit,3,1);
  gp3->addWidget(VarMaxEdit,3,2);

  setTabOrder(VarActiveCheck, VarInitEdit);
  setTabOrder(VarInitEdit, VarMinEdit);
  setTabOrder(VarMinEdit, VarMaxEdit);

  VarTypeCombo = new QComboBox();
  VarTypeCombo->insertItem(0, tr("linear double"));
  VarTypeCombo->insertItem(1, tr("logarithmic double"));
  VarTypeCombo->insertItem(2, tr("linear integer"));
  VarTypeCombo->insertItem(3, tr("logarithmic integer"));
  VarTypeCombo->insertItem(4, tr("E3 series"));
  VarTypeCombo->insertItem(5, tr("E6 series"));
  VarTypeCombo->insertItem(6, tr("E12 series"));
  VarTypeCombo->insertItem(7, tr("E24 series"));
  VarTypeCombo->insertItem(8, tr("E48 series"));
  VarTypeCombo->insertItem(9, tr("E96 series"));
  VarTypeCombo->insertItem(10, tr("E192 series"));
  connect(VarTypeCombo, SIGNAL(activated(const QString&)),
          SLOT(slotChangeVarType(const QString&)));

  QPushButton *AddVar_Butt = new QPushButton(tr("Add"));
  connect(AddVar_Butt, SIGNAL(clicked()), SLOT(slotAddVariable()));
  QPushButton *DelVar_Butt = new QPushButton(tr("Delete"));
  connect(DelVar_Butt, SIGNAL(clicked()), SLOT(slotDeleteVariable()));

  QHBoxLayout *VarButtons = new QHBoxLayout();
  VarButtons->setSpacing(3);
  VarButtons->addWidget(new QLabel(tr("Type:")));
  VarButtons->addWidget(VarTypeCombo);
  VarButtons->addStretch();
  VarButtons->addWidget(AddVar_Butt);
  VarButtons->addWidget(DelVar_Butt);
  gp3->addLayout(VarButtons, 4, 0, 1, 3);

  // add horizontal line
  QFrame *line = new QFrame(this);
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken); 
  gp3->addWidget(line, 5, 0, 1, -1); // fill the entire width
  QPushButton *CreateEqn_Butt = new QPushButton(tr("Copy current values to equation"));
  connect(CreateEqn_Butt, SIGNAL(clicked()), SLOT(slotCreateEqn()));
  gp3->addWidget(CreateEqn_Butt,6,0);

  t->addTab(Tab3, tr("Variables"));

  // ...........................................................
  QWidget *Tab4 = new QWidget(t);
  QGridLayout *gp4 = new QGridLayout();
  Tab4->setLayout(gp4);

  GoalTable = new QTableWidget();
  GoalTable->horizontalHeader()->setStretchLastSection(true);
  GoalTable->verticalHeader()->hide();
  GoalTable->setColumnCount(3);
  GoalTable->setHorizontalHeaderLabels(
      QStringList() << tr("Name") << tr("Type") << tr("Value"));
  GoalTable->setSortingEnabled(false);
  GoalTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  connect(GoalTable, SIGNAL(currentCellChanged(int, int, int, int)),
                    SLOT(slotEditGoal()));
  gp4->addWidget(GoalTable, 0, 0, 1, 3);

  GoalNameEdit = new QLineEdit();
  GoalNameEdit->setValidator(Validator);
  connect(GoalNameEdit, SIGNAL(textChanged(const QString&)),
          SLOT(slotChangeGoalName(const QString&)));

  gp4->addWidget(new QLabel(tr("Name:")), 1,0);
  gp4->addWidget(GoalNameEdit,1,1);

  GoalNumEdit = new QLineEdit();
  GoalNumEdit->setValidator(numVal);
  connect(GoalNumEdit, SIGNAL(textChanged(const QString&)),
          SLOT(slotChangeGoalNum(const QString&)));

  gp4->addWidget(new QLabel(tr("Value:")), 2,0);
  gp4->addWidget(GoalNumEdit,2,1);

  GoalTypeCombo = new QComboBox();
  GoalTypeCombo->insertItem(0, tr("minimize"));
  GoalTypeCombo->insertItem(1, tr("maximize"));
  GoalTypeCombo->insertItem(2, tr("less"));
  GoalTypeCombo->insertItem(3, tr("greater"));
  GoalTypeCombo->insertItem(4, tr("equal"));
  GoalTypeCombo->insertItem(5, tr("monitor"));
  connect(GoalTypeCombo, SIGNAL(activated(const QString&)),
          SLOT(slotChangeGoalType(const QString&)));
  gp4->addWidget(GoalTypeCombo,2,2);

  QPushButton *AddGoal_Butt = new QPushButton(tr("Add"));
  connect(AddGoal_Butt, SIGNAL(clicked()), SLOT(slotAddGoal()));
  QPushButton *DelGoal_Butt = new QPushButton(tr("Delete"));
  connect(DelGoal_Butt, SIGNAL(clicked()), SLOT(slotDeleteGoal()));

  QHBoxLayout *GoalButtons = new QHBoxLayout();
  GoalButtons->setSpacing(3);
  GoalButtons->addStretch();
  GoalButtons->addWidget(AddGoal_Butt);
  GoalButtons->addWidget(DelGoal_Butt);
  gp4->addLayout(GoalButtons, 3, 0, 1, 3);

  t->addTab(Tab4, tr("Goals"));

  // ...........................................................
  // buttons on the bottom of the dialog (independent of the TabWidget)
  QHBoxLayout *Butts = new QHBoxLayout();
  Butts->setSpacing(3);
  Butts->setMargin(3);

  QPushButton *OkButt = new QPushButton(tr("OK"));
  connect(OkButt, SIGNAL(clicked()), SLOT(slotOK()));
  QPushButton *ApplyButt = new QPushButton(tr("Apply"));
  connect(ApplyButt, SIGNAL(clicked()), SLOT(slotApply()));
  QPushButton *CancelButt = new QPushButton(tr("Cancel"));
  connect(CancelButt, SIGNAL(clicked()), SLOT(slotCancel()));

  Butts->addStretch();
  Butts->addWidget(OkButt);
  Butts->addWidget(ApplyButt);
  Butts->addWidget(CancelButt);
  all->addLayout(Butts);

  OkButt->setFocus();

  // ...........................................................

  Component *pc;
  for(auto pc:Doc->components()){
    incomplete();
    // BUG: this is not a component.

    // if(pc != Comp)
    //   if(pc->Model[0] == '.' && pc->Model != ".Opt")
    //     SimEdit->insertItem(SimEdit->count(), pc->Name);
  }

  Property *pp;
  pp = Comp->Props.at(0);
  if(!pp->Value.isEmpty()) {
    // set selected simulation in combo box to the currently used one
    int i = SimEdit->findText(pp->Value);
    if (i != -1) // current simulation is in the available simulations list (normal case)
      SimEdit->setCurrentIndex(i);
    else  // current simulation not in the available simulations list
      SimEdit->setEditText(pp->Value);
  }

  pp = Comp->Props.at(1);
  if(!pp->Value.isEmpty()) {
    MethodCombo->setCurrentIndex(pp->Value.section('|',0,0).toInt()-1);
    IterEdit->setText(pp->Value.section('|',1,1));
    RefreshEdit->setText(pp->Value.section('|',2,2));
    ParentsEdit->setText(pp->Value.section('|',3,3));
    ConstEdit->setText(pp->Value.section('|',4,4));
    CrossEdit->setText(pp->Value.section('|',5,5));
    SeedEdit->setText(pp->Value.section('|',6,6));
    CostVarEdit->setText(pp->Value.section('|',7,7));
    CostObjEdit->setText(pp->Value.section('|',8,8));
    CostConEdit->setText(pp->Value.section('|',9,9));
  }

  NameEdit->setText(Comp->name());

  QTableWidgetItem *item;
  for(pp = Comp->Props.at(2); pp != 0; pp = Comp->Props.next()) {
    if(pp->Name == "Var") { // BUG
      QStringList ValueSplit = pp->Value.split("|");
      int row = VarTable->rowCount();
      VarTable->insertRow(row);
      // Name
      item = new QTableWidgetItem(ValueSplit.at(0));
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      VarTable->setItem(row, 0, item);
      // active
      item = new QTableWidgetItem((ValueSplit.at(1) == "yes")? tr("yes") : tr("no"));
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      VarTable->setItem(row, 1, item);
      // initial
      item = new QTableWidgetItem(QString::number(ValueSplit.at(2).toDouble(), 'g', numPrec));
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      VarTable->setItem(row, 2, item);
      // min
      item = new QTableWidgetItem(ValueSplit.at(3));
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      VarTable->setItem(row, 3, item);
      // max
      item = new QTableWidgetItem(ValueSplit.at(4));
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      VarTable->setItem(row, 4, new QTableWidgetItem(ValueSplit.at(4)));
      // Type
      QString typeStr;
      if (ValueSplit.at(5) == "LIN_DOUBLE") {
        typeStr = tr("linear double");
      } else if (ValueSplit.at(5) == "LOG_DOUBLE") {
        typeStr = tr("logarithmic double");
      } else if (ValueSplit.at(5) == "LIN_INT") {
        typeStr = tr("linear integer");
      } else if (ValueSplit.at(5) == "E3") {
        typeStr = tr("E3 series");
      } else if (ValueSplit.at(5) == "E6") {
        typeStr = tr("E6 series");
      } else if (ValueSplit.at(5) == "E12") {
        typeStr = tr("E12 series");
      } else if (ValueSplit.at(5) == "E24") {
        typeStr = tr("E24 series");
      } else if (ValueSplit.at(5) == "E48") {
        typeStr = tr("E48 series");
      } else if (ValueSplit.at(5) == "E96") {
        typeStr = tr("E96 series");
      } else if (ValueSplit.at(5) == "E192") {
        typeStr = tr("E192 series");
      } else {
        typeStr = tr("logarithmic integer");
      }
      item = new QTableWidgetItem(typeStr);
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      VarTable->setItem(row, 5, item);
    }
    if(pp->Name == "Goal") { // BUG
      QStringList GoalSplit = pp->Value.split("|");
      int row = GoalTable->rowCount();
      GoalTable->insertRow(row);

      item = new QTableWidgetItem(GoalSplit.at(0));
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      GoalTable->setItem(row, 0, item);
      QString typeStr;
      if (GoalSplit.at(1) == "MIN") {
        typeStr = tr("minimize");
      } else if (GoalSplit.at(1) == "MAX") {
        typeStr = tr("maximize");
      } else if (GoalSplit.at(1) == "LE") {
        typeStr = tr("less");
      } else if (GoalSplit.at(1) == "GE") {
        typeStr = tr("greater");
      } else if (GoalSplit.at(1) == "EQ") {
        typeStr = tr("equal");
      } else {
        typeStr = tr("monitor");
      }
      item = new QTableWidgetItem(typeStr);
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      GoalTable->setItem(row, 1, item);
      item = new QTableWidgetItem(GoalSplit.at(2));
      item->setFlags(item->flags() & ~Qt::ItemIsEditable);
      GoalTable->setItem(row, 2, item);
    }
  }

  resize(300, 250);
#endif
}

OptimizeDialog::~OptimizeDialog()
{
  delete all;
  delete numVal;
  delete intVal;
  delete Validator;
}

// -----------------------------------------------------------
void OptimizeDialog::slotEditVariable()
{
  VarNameEdit->blockSignals(true);

  int row = VarTable->currentRow();
  if (row < 0 || row >= VarTable->rowCount()) {
    VarNameEdit->clear();
    VarActiveCheck->setChecked(true);
    VarInitEdit->clear();
    VarMinEdit->clear();
    VarMaxEdit->clear();
    VarTypeCombo->setCurrentIndex(0);

    VarNameEdit->blockSignals(false);
    return;
  }

  VarNameEdit->setText(VarTable->item(row, 0)->text());
  VarActiveCheck->setChecked(VarTable->item(row, 1)->text() == tr("yes"));
  VarInitEdit->setText(VarTable->item(row, 2)->text());
  VarMinEdit->setText(VarTable->item(row, 3)->text());
  VarMaxEdit->setText(VarTable->item(row, 4)->text());
  VarTypeCombo->setCurrentIndex(VarTypeCombo->findText(VarTable->item(row, 5)->text()));

  VarNameEdit->blockSignals(false);
}

// -----------------------------------------------------------
void OptimizeDialog::slotAddVariable()
{
  if(VarNameEdit->text().isEmpty() || VarInitEdit->text().isEmpty() ||
      VarMinEdit->text().isEmpty() || VarMaxEdit->text().isEmpty()) {
    QMessageBox::critical(this, tr("Error"),
       tr("Every text field must be non-empty!"));
    return;
  }

  int row;
  for (row = 0; row < VarTable->rowCount(); ++row) {
    if (VarNameEdit->text() == VarTable->item(row, 0)->text()) {
      QMessageBox::critical(this, tr("Error"), 
          tr("Variable \"%1\" aleardy in list!").arg(VarNameEdit->text()));
      return;
    }
  }

  QTableWidgetItem *item;
  row = VarTable->rowCount();
  VarTable->insertRow(row);
  item = new QTableWidgetItem(VarNameEdit->text());
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  VarTable->setItem(row, 0, item);
  item = new QTableWidgetItem(VarActiveCheck->isChecked() ? tr("yes") : tr("no"));
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  VarTable->setItem(row, 1, item);
  item = new QTableWidgetItem(VarInitEdit->text());
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  VarTable->setItem(row, 2, item);
  item = new QTableWidgetItem(VarMinEdit->text());
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  VarTable->setItem(row, 3, item);
  item = new QTableWidgetItem(VarMaxEdit->text());
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  VarTable->setItem(row, 4, item);
  item = new QTableWidgetItem(VarTypeCombo->currentText());
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  VarTable->setItem(row, 5, item);

  VarTable->setCurrentCell(row, 0);
  slotEditVariable();   // clear entry fields
  VarTable->clearSelection();
}

// -----------------------------------------------------------
void OptimizeDialog::slotDeleteVariable()
{
  int selectedrow = VarTable->currentRow();
  VarTable->removeRow(selectedrow);
  int nextRow = (selectedrow == VarTable->rowCount())? selectedrow-1 : selectedrow;
  VarTable->setCurrentCell(nextRow, 0);
  slotEditVariable();
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeVarActive(bool On)
{
  int selectedrow = VarTable->currentRow();
  QTableWidgetItem *item = VarTable->item(selectedrow, 1);
  if (item) {
    item->setText( On ? tr("yes") : tr("no") );
  }
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeVarName(const QString&)
{
  VarTable->clearSelection();
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeVarInit(const QString& Text)
{
  int selectedrow = VarTable->currentRow();
  QTableWidgetItem *item = VarTable->item(selectedrow, 2);
  if (item) {
    item->setText(Text);
  }
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeVarMin(const QString& Text)
{
  int selectedrow = VarTable->currentRow();
  QTableWidgetItem *item = VarTable->item(selectedrow, 3);
  if (item) {
    item->setText(Text);
  }
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeVarMax(const QString& Text)
{
  int selectedrow = VarTable->currentRow();
  QTableWidgetItem *item = VarTable->item(selectedrow, 4);
  if (item) {
    item->setText(Text);
  }
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeVarType(const QString& Text)
{
  int selectedrow = VarTable->currentRow();
  QTableWidgetItem *item = VarTable->item(selectedrow, 5);
  if (item) {
    item->setText(Text);
  }
}

// -----------------------------------------------------------
void OptimizeDialog::slotEditGoal()
{
  GoalNameEdit->blockSignals(true);

  int row = GoalTable->currentRow();
  if(row < 0 || row >= GoalTable->rowCount()) {
    GoalNameEdit->clear();
    GoalTypeCombo->setCurrentIndex(0);
    GoalNumEdit->clear();
    GoalNameEdit->blockSignals(false);
    return;
  }

  GoalNameEdit->setText(GoalTable->item(row, 0)->text());
  GoalTypeCombo->setCurrentIndex(GoalTypeCombo->findText(GoalTable->item(row, 1)->text()));
  GoalNumEdit->setText(GoalTable->item(row, 2)->text());

  GoalNameEdit->blockSignals(false);
}

// -----------------------------------------------------------
void OptimizeDialog::slotAddGoal()
{
  if(GoalNameEdit->text().isEmpty() || GoalNumEdit->text().isEmpty()) {
    QMessageBox::critical(this, tr("Error"),
       tr("Every text field must be non-empty!"));
    return;
  }

  int row;
  for (row = 0; row < GoalTable->rowCount(); ++row) {
    if (GoalNameEdit->text() == GoalTable->item(row, 0)->text()) {
      QMessageBox::critical(this, tr("Error"),
         tr("Goal \"%1\" already in list!").arg(GoalNameEdit->text()));
      return;
    }
  }

  QTableWidgetItem *item;
  row = GoalTable->rowCount();
  GoalTable->insertRow(row);
  item = new QTableWidgetItem(GoalNameEdit->text());
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  GoalTable->setItem(row, 0, item);
  item = new QTableWidgetItem(GoalTypeCombo->currentText());
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  GoalTable->setItem(row, 1, item);
  item = new QTableWidgetItem(GoalNumEdit->text());
  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  GoalTable->setItem(row, 2, item);

  GoalTable->setCurrentCell(row, 0);
  slotEditGoal();    // clear entry fields
  GoalTable->clearSelection();
}

// -----------------------------------------------------------
void OptimizeDialog::slotDeleteGoal()
{
  int selectedrow = GoalTable->currentRow();
  GoalTable->removeRow(selectedrow);
  int nextRow = (selectedrow == GoalTable->rowCount())? selectedrow-1 : selectedrow;
  GoalTable->setCurrentCell(nextRow, 0);
  slotEditVariable();
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeGoalName(const QString&)
{
  GoalTable->clearSelection();
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeGoalType(const QString& Text)
{
  int selectedrow = GoalTable->currentRow();
  QTableWidgetItem *item = GoalTable->item(selectedrow, 1);
  if (item) {
    item->setText(Text);
  }
}

// -----------------------------------------------------------
void OptimizeDialog::slotChangeGoalNum(const QString& Text)
{
  int selectedrow = GoalTable->currentRow();
  QTableWidgetItem *item = GoalTable->item(selectedrow, 2);
  if (item) {
    item->setText(Text);
  }
}

// -----------------------------------------------------------
void OptimizeDialog::slotOK()
{
  slotApply();
  slotCancel();
}

// -----------------------------------------------------------
void OptimizeDialog::slotApply()
{
  if(NameEdit->text().isEmpty())
    NameEdit->setText(QString::fromStdString(Comp->label()));
  else
  if(NameEdit->text().toStdString() != Comp->label()) {
    auto pc = Doc->find_component(NameEdit->text());
    if(pc){
      NameEdit->setText(QString::fromStdString(Comp->label()));
	 } else {
		 incomplete();
      // Comp->obsolete_name_override_hack(NameEdit->text());
      changed = true;
    }
  }

  QString Prop;
  if(SimEdit->currentText() != Comp->Props.at(0)->Value) {
    Comp->Props.at(0)->Value = SimEdit->currentText();
    changed = true;
  }
  Prop = QString::number(MethodCombo->currentIndex()+1) + "|" +
    IterEdit->text() + "|" +
    RefreshEdit->text() + "|" +
    ParentsEdit->text() + "|" +
    ConstEdit->text() + "|" +
    CrossEdit->text() + "|" +
    SeedEdit->text() + "|" +
    CostVarEdit->text() + "|" +
    CostObjEdit->text() + "|" +
    CostConEdit->text();
  if(Prop != Comp->Props.at(1)->Value) {
    Comp->Props.at(1)->Value = Prop;
    changed = true;
  }

  Property *pp = Comp->Props.at(2);
  int row;
  // apply all the new property values in the TableWidget
  for (row = 0; row < VarTable->rowCount(); ++row) {
    QStringList propList;
    propList << VarTable->item(row, 0)->text();
    propList << ((VarTable->item(row, 1)->text() == tr("yes"))? "yes" : "no");
    propList << VarTable->item(row, 2)->text();
    propList << VarTable->item(row, 3)->text();
    propList << VarTable->item(row, 4)->text();
    QString typeStr = VarTable->item(row, 5)->text();
    if (typeStr == tr("linear double")) {
      propList << "LIN_DOUBLE";
    } else if (typeStr == tr("logarithmic double")) {
      propList << "LOG_DOUBLE";
    } else if (typeStr == tr("linear integer")) {
      propList << "LIN_INT";
    } else if (typeStr == tr("E3 series")) {
      propList << "E3";
    } else if (typeStr == tr("E6 series")) {
      propList << "E6";
    } else if (typeStr == tr("E12 series")) {
      propList << "E12";
    } else if (typeStr == tr("E24 series")) {
      propList << "E24";
    } else if (typeStr == tr("E48 series")) {
      propList << "E48";
    } else if (typeStr == tr("E96 series")) {
      propList << "E96";
    } else if (typeStr == tr("E192 series")) {      
      propList << "E192";
    } else {
      propList << "LOG_INT";
    }
    Prop = propList.join("|");

    if(pp) {
      if(pp->Name != "Var") {
        pp->Name = "Var";
        changed = true;
      }
      if(pp->Value != Prop) {
        pp->Value = Prop;
        changed = true;
      }
    }
    else {
      Comp->Props.append(new Property("Var", Prop, false, ""));
      changed = true;
    }
    pp = Comp->Props.next();
  }

  for (row = 0; row < GoalTable->rowCount(); ++row) {
    QStringList propList;
    propList << GoalTable->item(row, 0)->text();
    QString typeStr = GoalTable->item(row, 1)->text();
    if (typeStr == tr("minimize")) {
      propList << "MIN";
    } else if (typeStr == tr("maximize")) {
      propList << "MAX";
    } else if (typeStr == tr("less")) {
      propList << "LE";
    } else if (typeStr == tr("greater")) {
      propList << "GE";
    } else if (typeStr == tr("equal")) {
      propList << "EQ";
    } else {
      propList << "MON";
    }
    propList << GoalTable->item(row, 2)->text();
    Prop = propList.join("|");

    if(pp) {
      if(pp->Name != "Goal") {
        pp->Name = "Goal";
        changed = true;
      }
      if(pp->Value != Prop) {
        pp->Value = Prop;
        changed = true;
      }
    }
    else {
      Comp->Props.append(new Property("Goal", Prop, false, ""));
      changed = true;
    }
    pp = Comp->Props.next();
  }

  // if more properties than in ListView -> delete the rest
  if(pp) {
    pp = Comp->Props.prev();
    Comp->Props.last();
    while(pp != Comp->Props.current())
      Comp->Props.remove();
    changed = true;
  }

  if(changed)
    Doc->viewport()->repaint();
}

// -------------------------------------------------------------------------
// Is called if the "Cancel"-button is pressed.
void OptimizeDialog::slotCancel()
{
  if(changed) done(QDialog::Accepted); // changed could have been done before
  else done(QDialog::Rejected);        // (by "Apply"-button)
}


void OptimizeDialog::slotCreateEqn()
{
  QString s = "<Qucs Schematic " "PACKAGE_VERSION" ">\n"
              "<Components>\n"
              //<Model Name ShowName cx cy tx ty mirroredX rotate
              "<Eqn OptValues 1 0 0 -28 15 0 0 ";

 Property *pp;
 for(pp = Comp->Props.at(2); pp != 0; pp = Comp->Props.next()) {
   if(pp->Name == "Var") { // property is an optimization variable
      QStringList ValueSplit = pp->Value.split("|");
      // "Name" = "initial (current) value"
      s += "\"" + ValueSplit.at(0) + "=" + ValueSplit.at(2) + "\" 1 ";
   }
 }

 s += QString("\"yes\" 0>\n" // Export yes, no display
              "</Components>\n"
	      "<Wires>\n"
	      "</Wires>\n"
	      "<Diagrams>\n"
	      "</Diagrams>\n"
	      "<Paintings>\n"
	      "</Paintings>\n");

 QApplication::clipboard()->setText(s, QClipboard::Clipboard);
 // uncomment to have the dialog close and the Equation pasted...
 //QucsMain->slotEditPaste(true);
 //accept();
}

void OptimizeDialog::slotSetPrecision(const QPoint& pos)
{
  QPoint globalPos = VarTable->horizontalHeader()->mapToGlobal(pos);
  int column = VarTable->horizontalHeader()->logicalIndexAt(pos);

  // change precision is supported only for the 'initial' column
  if (column != 2) return;

  QMenu cMenu;
  cMenu.addAction("Set precision");

  QAction* selectedItem = cMenu.exec(globalPos);
  if (selectedItem) {
    bool ok;
    int i = QInputDialog::getInt(this, tr("Set precision"),
                                 tr("Precision:"), 2, 1, 16, 1, &ok);
    if (!ok) return;
    numPrec = i;
    // update the shown values according to the new precision
    int row = 0;
    Property *pp;
    QTableWidgetItem *item;
    for(pp = Comp->Props.at(2); pp != 0; pp = Comp->Props.next()) {
      if(pp->Name == "Var") {
	QStringList ValueSplit = pp->Value.split("|");
	// 'initial' column
	item = VarTable->item(row++, 2);
	item->setText(QString::number(ValueSplit.at(2).toDouble(), 'g', numPrec));
      }
    }
  }
}

