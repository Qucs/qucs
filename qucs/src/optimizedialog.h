/***************************************************************************
                            optimizedialog.h
                           ------------------
    begin                : Sun Jul 30 2006
    copyright            : (C) 2006 by Michael Margraf
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

#ifndef OPTIMIZEDIALOG_H
#define OPTIMIZEDIALOG_H

#include <QDialog>
#include <QRegExp>
#include "schematic_dialog.h"

class SchematicDoc;
class Optimize_Sim;
class QTableWidget;
class QCheckBox;
class QComboBox;
class QLineEdit;
class QVBoxLayout;
class QRegExpValidator;
class QDoubleValidator;
class QIntValidator;


class OptimizeDialog : public SchematicDialog  {
// Q_OBJECT does not work
public:
  OptimizeDialog(QucsDoc* d);
 ~OptimizeDialog();

 void attach(Object* sim);
private slots:
  void slotOK();
  void slotApply();
  void slotCancel();
  void slotAddVariable();
  void slotDeleteVariable();
  void slotAddGoal();
  void slotDeleteGoal();
  void slotEditGoal();
  void slotEditVariable();
  void slotChangeVarName(const QString&);
  void slotChangeVarActive(bool);
  void slotChangeVarInit(const QString&);
  void slotChangeVarMin(const QString&);
  void slotChangeVarMax(const QString&);
  void slotChangeVarType(const QString&);
  void slotChangeGoalName(const QString&);
  void slotChangeGoalNum(const QString&);
  void slotChangeGoalType(const QString&);
  void slotCreateEqn();
  void slotSetPrecision(const QPoint&);

private:
  void createASCOFiles();

public:
  Optimize_Sim *Comp{nullptr};
  SchematicDoc *Doc{nullptr};
  bool changed;
  int numPrec;

  QVBoxLayout *all{nullptr};
  QLineEdit *NameEdit{nullptr}, *VarNameEdit{nullptr},
            *VarInitEdit{nullptr}, *VarMinEdit{nullptr}, *VarMaxEdit{nullptr},
            *IterEdit{nullptr}, *RefreshEdit{nullptr}, *ParentsEdit{nullptr}, *ConstEdit{nullptr}, *CrossEdit{nullptr},
            *SeedEdit{nullptr}, *CostVarEdit{nullptr}, *CostObjEdit{nullptr}, *CostConEdit{nullptr},
            *GoalNameEdit{nullptr}, *GoalNumEdit{nullptr};
  QCheckBox *VarActiveCheck{nullptr};
  QComboBox *SimEdit{nullptr}, *GoalTypeCombo{nullptr}, *MethodCombo{nullptr}, *VarTypeCombo{nullptr};
  QTableWidget *VarTable{nullptr}, *GoalTable{nullptr};

  QRegExp Expr;
  QRegExpValidator *Validator{nullptr};
  QDoubleValidator *numVal{nullptr};
  QIntValidator *intVal{nullptr};
};

#endif
