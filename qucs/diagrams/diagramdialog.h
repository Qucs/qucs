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
#include <qregexp.h>

class QLabel;
class QListBox;
class QLineEdit;
class QCheckBox;
class QListView;
class QListViewItem;
class QComboBox;
class QListBoxItem;
class QVBoxLayout;
class QDoubleValidator;
class QRegExpValidator;

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
  void copyDiagramGraphs();

public slots:
  void slotReadVars(int);
  void slotTakeVar(QListViewItem*);
//  void slotSelectGraph(int index);
  void slotSelectGraph(QListBoxItem*);
  void slotNewGraph();
  void slotDeleteGraph();
  void slotOK();
  void slotApply();
  void slotCancel();
  void slotSetColor();
  void slotSetGridColor();
  void slotResetToTake(const QString&);
  void slotSetProp2(const QString&);
  void slotSetNumMode(int);
  void slotSetGridBox(int);
  void slotSetGraphStyle(int);
  void slotSetYAxis(int);
  void slotManualX(int);
  void slotManualY(int);
  void slotManualZ(int);
  void slotChangeTab(QWidget*);

public:
  Diagram *Diag;
  QString defaultDataSet;

  QDoubleValidator *ValDouble;
  QRegExpValidator *Validator;

  QComboBox *ChooseData;
  QListView *ChooseVars;
  QListBox  *GraphList;

  QRegExp     Expr;
  QVBoxLayout *all;   // the mother of all widgets
  QLineEdit   *GraphInput, *Property2, *xLabel, *ylLabel, *yrLabel;
  QCheckBox   *GridOn, *GridLogX, *GridLogY, *GridLogZ;
  QCheckBox   *manualX, *manualY, *manualZ;
  QLineEdit   *startX, *stepX, *stopX;
  QLineEdit   *startY, *stepY, *stopY;
  QLineEdit   *startZ, *stepZ, *stopZ;
  QLineEdit   *rotationY, *rotationZ;
  QLabel      *GridLabel1, *GridLabel2, *Label1, *Label2, *Label3, *Label4;
  QComboBox   *PropertyBox, *GridStyleBox, *yAxisBox;
  QPushButton *ColorButt, *GridColorButt;
  bool changed, toTake;
  QPtrList<Graph>  Graphs;
};

#endif
