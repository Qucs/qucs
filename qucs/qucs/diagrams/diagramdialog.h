/***************************************************************************
                          diagramdialog.h  -  description
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

#ifndef DIAGRAMDIALOG_H
#define DIAGRAMDIALOG_H

#include "diagram.h"

#include <qdialog.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qlineedit.h>
#include <qdir.h>
#include <qpoint.h>
#include <qregexp.h>
#include <qcheckbox.h>
#include <qlabel.h>


/**
  *@author Michael Margraf
  */

class DiagramDialog : public QDialog  {
Q_OBJECT
public: 
  DiagramDialog(Diagram *d, const QString& _DataSet, QWidget *parent=0,
		const char *name=0);
  ~DiagramDialog();

  bool loadVarData(const QString&);

public slots:
  void slotReadVars(int);
  void slotTakeVar(QListViewItem*);
//  void slotSelectGraph(int index);
  void slotSelectGraph(QListBoxItem*);
  void slotNewGraph();
  void slotDeleteGraph();
  void slotOK();
  void slotApply();
  void slotFuncHelp();
  void slotSetColor();
  void slotSetGridColor();
  void slotResetToTake(const QString&);
  void slotSetProp2(const QString&);
  void slotSetNumMode(int);
  void slotSetGridBox(int);
  void slotSetGraphStyle(int);

public:
  Diagram *Diag;
  QString defaultDataSet;

  QComboBox *ChooseData;
  QListView *ChooseVars;
  QListBox  *GraphList;

  QRegExp     Expr;
  QLineEdit   *GraphInput, *Property2, *xLabel, *yLabel;
  QCheckBox   *GridOn;
  QLabel      *GridLabel1, *GridLabel2;
  QComboBox   *PropertyBox, *GridStyleBox;
  QPushButton *ColorButt, *GridColorButt;
  bool changed, toTake;
};

#endif
