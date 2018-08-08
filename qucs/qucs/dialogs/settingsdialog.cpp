/***************************************************************************
                             settingsdialog.cpp
                            --------------------
    begin                : Mon Oct 20 2003
    copyright            : (C) 2003, 2004 by Michael Margraf
                           (C) 2016 Qucs Team
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
#include "settingsdialog.h"

#include "node.h"
#include "qucs.h"
#include "mnemo.h"
#include "schematic.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QLayout>
#include <QRegExp>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QTabWidget>
#include <QValidator>
#include <QPushButton>
#include <QFileSystemModel>
#include <QTreeView>
#include <QHeaderView>
#include <QModelIndex>
#include <QDebug>

SettingsDialog::SettingsDialog(Schematic *Doc_)
    : QDialog(Doc_) 
{
    Doc = Doc_;
    setWindowTitle(tr("Edit File Properties"));

    all = new QVBoxLayout(this);
    QTabWidget *t = new QTabWidget();
    all->addWidget(t);

    // ...........................................................
    QWidget *Tab1 = new QWidget(t);
    QGridLayout *gp = new QGridLayout(Tab1);
    Tab1->setLayout(gp);

    QLabel *l2 = new QLabel(tr("Data Set:"), Tab1);
    gp->addWidget(l2,0,0);
    Input_DataSet = new QLineEdit(Tab1);
    gp->addWidget(Input_DataSet,0,1,1,1);
    QPushButton *DataSetBrowseButt = new QPushButton(tr("Browse"));
    gp->addWidget(DataSetBrowseButt, 0, 2, 1, 1);
    connect(DataSetBrowseButt, SIGNAL(clicked()), SLOT(slotDataSetBrowse()));

    QLabel *l1 = new QLabel(tr("Data Display:"));
    gp->addWidget(l1,1,0);
    Input_DataDisplay = new QLineEdit(Tab1);
    gp->addWidget(Input_DataDisplay,1,1,1,1);
    QPushButton *DataDisplayBrowseButt = new QPushButton(tr("Browse"));
    gp->addWidget(DataDisplayBrowseButt, 1, 2, 1, 1);
    connect(DataDisplayBrowseButt, SIGNAL(clicked()), SLOT(slotDataDisplayBrowse()));
    Check_OpenDpl = new QCheckBox(tr("open data display after simulation"),
                                  Tab1);
    gp->addWidget(Check_OpenDpl,2,0,1,2);

    QLabel *l20 = new QLabel(tr("Octave Script:"), Tab1);
    gp->addWidget(l20,3,0);
    Input_Script = new QLineEdit(Tab1);
    gp->addWidget(Input_Script,3,1,1,1);
    QPushButton *ScriptBrowseButt = new QPushButton(tr("Browse"));
    gp->addWidget(ScriptBrowseButt, 3, 2, 1, 1);
    connect(ScriptBrowseButt, SIGNAL(clicked()), SLOT(slotScriptBrowse()));

    Check_RunScript = new QCheckBox(tr("run script after simulation"),
                                    Tab1);
    gp->addWidget(Check_RunScript,4,0,1,2);

    t->addTab(Tab1, tr("Simulation"));

    // ...........................................................
    QWidget *Tab2 = new QWidget(t);
    QGridLayout *gp2 = new QGridLayout(Tab2);
    Check_GridOn = new QCheckBox(tr("show Grid"), Tab2);
    gp2->addWidget(Check_GridOn,0,0,1,1);

    valExpr = new QRegExpValidator(QRegExp("[1-9]\\d{0,2}"), this);

    QLabel *l3 = new QLabel(tr("horizontal Grid:"), Tab2);
    gp2->addWidget(l3,1,0);
    Input_GridX = new QLineEdit(Tab2);
    Input_GridX->setValidator(valExpr);
    gp2->addWidget(Input_GridX,1,1,1,1);

    QLabel *l4 = new QLabel(tr("vertical Grid:"), Tab2);
    gp2->addWidget(l4,2,0);
    Input_GridY = new QLineEdit(Tab2);
    Input_GridY->setValidator(valExpr);
    gp2->addWidget(Input_GridY,2,1,1,1);

    t->addTab(Tab2, tr("Grid"));

    // ...........................................................
    QWidget *Tab3 = new QWidget(t);
    QGridLayout *gp3 = new QGridLayout(Tab3);
    Combo_Frame = new QComboBox(Tab3);
    Combo_Frame->addItem(tr("no Frame"));
    Combo_Frame->addItem(tr("DIN A5 landscape"));
    Combo_Frame->addItem(tr("DIN A5 portrait"));
    Combo_Frame->addItem(tr("DIN A4 landscape"));
    Combo_Frame->addItem(tr("DIN A4 portrait"));
    Combo_Frame->addItem(tr("DIN A3 landscape"));
    Combo_Frame->addItem(tr("DIN A3 portrait"));
    Combo_Frame->addItem(tr("Letter landscape"));
    Combo_Frame->addItem(tr("Letter portrait"));
    gp3->addWidget(Combo_Frame,0,0,1,2);

    Input_Frame0 = new QTextEdit(Tab3);
    Input_Frame0->setWordWrapMode(QTextOption::NoWrap);
    gp3->addWidget(Input_Frame0,1,0,2,2);

    Input_Frame1 = new QLineEdit(Tab3);
    gp3->addWidget(Input_Frame1,3,0,1,2);

    Input_Frame2 = new QLineEdit(Tab3);
    gp3->addWidget(Input_Frame2,4,0);
    Input_Frame3 = new QLineEdit(Tab3);
    gp3->addWidget(Input_Frame3,4,1);

    t->addTab(Tab3, tr("Frame"));

    // ...........................................................
    // buttons on the bottom of the dialog (independent of the TabWidget)
    QHBoxLayout *Butts = new QHBoxLayout();
    Butts->setSpacing(5);
    Butts->setMargin(5);
    all->addLayout(Butts);

    QPushButton *OkButt = new QPushButton(tr("OK"));
    Butts->addWidget(OkButt);
    connect(OkButt, SIGNAL(clicked()), SLOT(slotOK()));
    QPushButton *ApplyButt = new QPushButton(tr("Apply"));
    Butts->addWidget(ApplyButt);
    connect(ApplyButt, SIGNAL(clicked()), SLOT(slotApply()));
    QPushButton *CancelButt = new QPushButton(tr("Cancel"));
    Butts->addWidget(CancelButt);
    connect(CancelButt, SIGNAL(clicked()), SLOT(reject()));

    OkButt->setDefault(true);

    // ...........................................................
    // fill the fields with the QucsDoc-Properties

    Input_DataSet->setText(Doc->DataSet);
    Input_DataDisplay->setText(Doc->DataDisplay);
    Input_Script->setText(Doc->Script);
    Check_OpenDpl->setChecked(Doc->SimOpenDpl);
    Check_RunScript->setChecked(Doc->SimRunScript);
    Check_GridOn->setChecked(Doc->GridOn);
    Input_GridX->setText(QString::number(Doc->GridX));
    Input_GridY->setText(QString::number(Doc->GridY));
    Combo_Frame->setCurrentIndex(Doc->showFrame());

    QString Text_;
    decode_String(Text_ = Doc->frameText0());
    Input_Frame0->setText(Text_);
    decode_String(Text_ = Doc->frameText1());
    Input_Frame1->setText(Text_);
    decode_String(Text_ = Doc->frameText2());
    Input_Frame2->setText(Text_);
    decode_String(Text_ = Doc->frameText3());
    Input_Frame3->setText(Text_);

    resize(250, 200);
}

SettingsDialog::~SettingsDialog()
{
    delete all;
    delete valExpr;
}

void SettingsDialog::slotDataSetBrowse() {
  AuxFilesDialog *d = new AuxFilesDialog(this, "*.dat");
  if(d->exec() != QDialog::Accepted) return;

  Input_DataSet->setText(d->fileName);
}

void SettingsDialog::slotDataDisplayBrowse() {
  AuxFilesDialog *d = new AuxFilesDialog(this, "*.dpl");
  if(d->exec() != QDialog::Accepted) return;

  Input_DataDisplay->setText(d->fileName);
}

void SettingsDialog::slotScriptBrowse() {
  AuxFilesDialog *d = new AuxFilesDialog(this, "*.m");
  if(d->exec() != QDialog::Accepted) return;

  Input_Script->setText(d->fileName);
}

// -----------------------------------------------------------
// Close the dialog, applying any settings
void SettingsDialog::slotOK()
{
    slotApply();
    accept();
}

// -----------------------------------------------------------
// Applies the settings chosen in the dialog
void SettingsDialog::slotApply()
{
    bool changed = false;

    if(Doc->DataSet != Input_DataSet->text())
    {
        Doc->DataSet = Input_DataSet->text();
        changed = true;
    }

    if(Doc->DataDisplay != Input_DataDisplay->text())
    {
        Doc->DataDisplay = Input_DataDisplay->text();
        changed = true;
    }

    if(Doc->Script != Input_Script->text())
    {
        Doc->Script = Input_Script->text();
        changed = true;
    }

    if(Doc->SimOpenDpl != Check_OpenDpl->isChecked())
    {
        Doc->SimOpenDpl = Check_OpenDpl->isChecked();
        changed = true;
    }

    if(Doc->SimRunScript != Check_RunScript->isChecked())
    {
        Doc->SimRunScript = Check_RunScript->isChecked();
        changed = true;
    }

    if(Doc->GridOn != Check_GridOn->isChecked())
    {
        Doc->GridOn = Check_GridOn->isChecked();
        changed = true;
    }

    if(Doc->GridX != Input_GridX->text().toInt())
    {
        Doc->GridX = Input_GridX->text().toInt();
        changed = true;
    }

    if(Doc->GridY != Input_GridY->text().toInt())
    {
        Doc->GridY = Input_GridY->text().toInt();
        changed = true;
    }

	 Doc->setFrameType(Combo_Frame->currentIndex());

    QString t;

	 // TODO: C++ also has for loops...
    encode_String(Input_Frame0->toPlainText(), t);
    Doc->setFrameText(0, t);

    encode_String(Input_Frame1->text(), t);
    Doc->setFrameText(1, t);

    encode_String(Input_Frame2->text(), t);
    Doc->setFrameText(2, t);

    encode_String(Input_Frame3->text(), t);
    Doc->setFrameText(3, t);

    if(changed)
    {
        Doc->setChanged(true); //BUG keep track of changes, where they are
    }
	 Doc->viewport()->repaint(); // BUG: train wreck
}

AuxFilesDialog::AuxFilesDialog(QWidget *parent, const QString &filter) :QDialog(parent)
{
  fileName = QString();
  model = new QFileSystemModel();
  model->setFilter(QDir::Files);
  model->setRootPath(QucsSettings.QucsWorkDir.absolutePath());

  QStringList filtersList;
  filtersList << filter;
  model->setNameFilters(filtersList);
  // hide items not passing the filter, not just disable them
  model->setNameFilterDisables(false);

  tree = new QTreeView();
  tree->setModel(model);
  tree->sortByColumn(0, Qt::AscendingOrder);
  tree->setSortingEnabled(true);
  tree->setRootIndex(model->index(QucsSettings.QucsWorkDir.absolutePath()));

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(tree);
  tree->show();

  //tree->header()->setStretchLastSection(false);
  //tree->resizeColumnToContents(0);
  tree->header()->setResizeMode(QHeaderView::ResizeToContents);
  //tree->header()->setResizeMode(0, QHeaderView::Stretch); 
  connect(tree, SIGNAL(doubleClicked(const QModelIndex &)), SLOT(slotDoubleClick(const QModelIndex &)));

  setWindowTitle("Choose a file");

  // buttons at the bottom of the dialog
  QHBoxLayout *Btns = new QHBoxLayout();
  Btns->setSpacing(5);
  Btns->setMargin(5);
  layout->addLayout(Btns);
  
  Btns->addStretch();
  QPushButton *OkButt = new QPushButton(tr("Select"));
  Btns->addWidget(OkButt);
  connect(OkButt, SIGNAL(clicked()), SLOT(slotSelect()));
  QPushButton *CancelButt = new QPushButton(tr("Cancel"));
  Btns->addWidget(CancelButt);
  connect(CancelButt, SIGNAL(clicked()), SLOT(reject()));
  
  OkButt->setDefault(true);

  resize(600, 300);
}

void AuxFilesDialog::slotDoubleClick(const QModelIndex &index)
{
  // get the file name of the item
  fileName = model->fileInfo(index).fileName();
  accept();
}

void AuxFilesDialog::slotSelect(void)
{
  slotDoubleClick(tree->currentIndex());
}
