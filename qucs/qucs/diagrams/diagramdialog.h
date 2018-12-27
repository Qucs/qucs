/***************************************************************************
                              diagramdialog.h
                             -----------------
    begin                : Sun Oct 5 2003
    copyright            : (C) 2003 by Michael Margraf
	                            2018 Felix Salfelder / QUCS team
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DIAGRAMDIALOG_H
#define DIAGRAMDIALOG_H
#include "diagram.h"
#include "node.h"
#include "schematic_dialog.h"

#include <QDialog>
#include <QRegExp>
#include <Q3PtrList>

class QVBoxLayout;
class Cross3D;
class QLabel;
class QLineEdit;
class QCheckBox;
class QComboBox;
class QDoubleValidator;
class QIntValidator;
class QRegExpValidator;
class QSlider;
class QTableWidgetItem;
class QListWidgetItem;
class QTableWidget;
class QListWidget;


class DiagramDialog : public SchematicDialog {
	// Q_OBJECT // does not work. StaticMetaobject...
public:
  DiagramDialog();
  ~DiagramDialog();


  bool loadVarData(const QString&);
  void copyDiagramGraphs();
  void attach(Object* d);

private slots:
  void slotReadVars(int);
  void slotTakeVar(QTableWidgetItem *item);
//  void slotSelectGraph(int index);
  void slotSelectGraph(QListWidgetItem*);
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

  void slotNewRotX(int);
  void slotNewRotY(int);
  void slotNewRotZ(int);
  void slotEditRotX(const QString&);
  void slotEditRotY(const QString&);
  void slotEditRotZ(const QString&);
  /* RELATED TO WAVEAC AND PHASOR DIAGRAMS
  void PhasorvalV(int);
  void PhasorvalI(int);
  void PhasorvalP(int);
  void PhasorvalZ(int);
  void addvar(QString);
  void remvar(QString);
  bool testvar(QString);*/

protected slots:
    void reject();

private:
  void SelectGraph(Graph*);

  Diagram *Diag;
  QString defaultDataSet;
  QString Var2;
  int loc;
  QString Name;

  QRegExp Expr;
  QDoubleValidator *ValDouble;
  QIntValidator    *ValInteger;
  QRegExpValidator *Validator;

  QComboBox *ChooseData;
  QTableWidget *ChooseVars;
  QListWidget  *GraphList;

  QVBoxLayout *all;   // the mother of all widgets
  QLineEdit   *GraphInput, *Property2, *xLabel, *ylLabel, *yrLabel;
  QCheckBox   *GridOn, *GridLogX, *GridLogY, *GridLogZ;
  QCheckBox   *manualX, *manualY, *manualZ, *hideInvisible;
  QCheckBox   *inputV, *inputI, *inputP, *inputZ;
  QLineEdit   *startX, *stepX, *stopX;
  QLineEdit   *startY, *stepY, *stopY;
  QLineEdit   *startZ, *stepZ, *stopZ;
  QLineEdit   *rotationX, *rotationY, *rotationZ;
  QLineEdit   *freq;
  QLabel      *GridLabel1, *GridLabel2, *Label1, *Label2, *Label3, *Label4;
  QComboBox   *PropertyBox, *GridStyleBox, *yAxisBox;
  QPushButton *ColorButt, *GridColorButt;
  QSlider     *SliderRotX, *SliderRotY, *SliderRotZ;
  Cross3D     *DiagCross;
  bool changed, transfer, toTake;
  Q3PtrList<Graph>  Graphs;
};

#endif
