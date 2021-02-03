/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
	                            2018, 2020 Felix Salfelder / QUCS team
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
#include "qt_compat.h"

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
class DataChooser;


class DiagramDialog : public SchematicDialog {
	Q_OBJECT // does not work. StaticMetaobject...??
public:
	DiagramDialog(QucsDoc* d);
	~DiagramDialog();

	bool loadVarData(const QString&);
	void copyDiagramGraphs();

private: // SchematicDialog
  virtual void attach(ElementGraphics* gfx) override;

private slots:
  virtual void slotReadVars(int);
  virtual void slotTakeVar(QTableWidgetItem *item);
//virtual   void slotSelectGraph(int index);
  virtual void slotSelectGraph(QListWidgetItem*);
  virtual void slotNewGraph();
  virtual void slotDeleteGraph();
  virtual void slotButtOK();
  virtual void slotButtApply();
  virtual void slotButtCancel();
  virtual void slotSetColor();
  virtual void slotSetGridColor();
  virtual void slotResetToTake(const QString&);
  virtual void slotSetProp2(const QString&);
  virtual void slotSetNumMode(int);
  virtual void slotSetGridBox(int);
  virtual void slotSetGraphStyle(int);
  virtual void slotSetYAxis(int);
  virtual void slotManualX(int);
  virtual void slotManualY(int);
  virtual void slotManualZ(int);
  virtual void slotChangeTab(QWidget*);

  virtual void slotNewRotX(int);
  virtual void slotNewRotY(int);
  virtual void slotNewRotZ(int);
  virtual void slotEditRotX(const QString&);
  virtual void slotEditRotY(const QString&);
  virtual void slotEditRotZ(const QString&);
  /* RELATED TO WAVEAC AND PHASOR DIAGRAMS
  void PhasorvalV(int);
  void PhasorvalI(int);
  void PhasorvalP(int);
  void PhasorvalZ(int);
  void addvar(QString);
  void remvar(QString);
  bool testvar(QString);*/

protected slots:
  virtual void reject();

protected:
  virtual void SelectGraph(Graph*);

  Diagram *Diag;
  QString defaultDataSet;
  QString Var2;
  int loc;
  QString Name;

  QRegExp Expr;
  QDoubleValidator *ValDouble;
  QIntValidator    *ValInteger;
  QRegExpValidator *Validator;

  DataChooser *ChooseData;
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
