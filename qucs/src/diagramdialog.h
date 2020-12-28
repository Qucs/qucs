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


class DiagramDialog : public SchematicDialog {
	Q_OBJECT // does not work. StaticMetaobject...??
public:
  DiagramDialog(QucsDoc* d);
  ~DiagramDialog();

  bool loadVarData(const QString&);
  void copyDiagramGraphs();

private: // SchematicDialog
  void attach(ElementGraphics* gfx) override;

private slots:
  void slotReadVars(int);
  void slotTakeVar(QTableWidgetItem *item);
//  void slotSelectGraph(int index);
  void slotSelectGraph(QListWidgetItem*);
  void slotNewGraph();
  void slotDeleteGraph();
  void slotButtOK();
  void slotButtApply();
  void slotButtCancel();
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

  Diagram *Diag{nullptr};
  QString defaultDataSet;
  QString Var2;
  int loc;
  QString Name;

  QRegExp Expr;
  QDoubleValidator *ValDouble{nullptr};
  QIntValidator    *ValInteger{nullptr};
  QRegExpValidator *Validator{nullptr};

  QComboBox *ChooseData{nullptr};
  QTableWidget *ChooseVars{nullptr};
  QListWidget  *GraphList{nullptr};

  QVBoxLayout *all{nullptr};   // the mother of all widgets
  QLineEdit   *GraphInput{nullptr}, *Property2{nullptr}, *xLabel{nullptr}, *ylLabel{nullptr}, *yrLabel{nullptr};
  QCheckBox   *GridOn{nullptr}, *GridLogX{nullptr}, *GridLogY{nullptr}, *GridLogZ{nullptr};
  QCheckBox   *manualX{nullptr}, *manualY{nullptr}, *manualZ{nullptr}, *hideInvisible{nullptr};
  QCheckBox   *inputV{nullptr}, *inputI{nullptr}, *inputP{nullptr}, *inputZ{nullptr};
  QLineEdit   *startX{nullptr}, *stepX{nullptr}, *stopX{nullptr};
  QLineEdit   *startY{nullptr}, *stepY{nullptr}, *stopY{nullptr};
  QLineEdit   *startZ{nullptr}, *stepZ{nullptr}, *stopZ{nullptr};
  QLineEdit   *rotationX{nullptr}, *rotationY{nullptr}, *rotationZ{nullptr};
  QLineEdit   *freq{nullptr};
  QLabel      *GridLabel1{nullptr}, *GridLabel2{nullptr}, *Label1{nullptr}, *Label2{nullptr}, *Label3{nullptr}, *Label4{nullptr};
  QComboBox   *PropertyBox{nullptr}, *GridStyleBox{nullptr}, *yAxisBox{nullptr};
  QPushButton *ColorButt{nullptr}, *GridColorButt{nullptr};
  QSlider     *SliderRotX{nullptr}, *SliderRotY{nullptr}, *SliderRotZ{nullptr};
  Cross3D     *DiagCross{nullptr};
  bool changed, transfer, toTake;
  Q3PtrList<Graph>  Graphs;
};

#endif
