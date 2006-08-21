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

#include <qdialog.h>
#include <qregexp.h>

class Schematic;
class Optimize_Sim;
class QListView;
class QListViewItem;
class QCheckBox;
class QComboBox;
class QLineEdit;
class QPushButton;
class QVBoxLayout;
class QRegExpValidator;
class QDoubleValidator;
class QIntValidator;


class OptimizeDialog : public QDialog  {
Q_OBJECT
public:
  OptimizeDialog(Optimize_Sim*, Schematic*);
 ~OptimizeDialog();

private slots:
  void slotOK();
  void slotApply();
  void slotCancel();
  void slotAddVariable();
  void slotDeleteVariable();
  void slotAddGoal();
  void slotDeleteGoal();
  void slotEditGoal(QListViewItem*);
  void slotEditVariable(QListViewItem*);
  void slotChangeVarName(const QString&);
  void slotChangeVarActive(bool);
  void slotChangeVarInit(const QString&);
  void slotChangeVarMin(const QString&);
  void slotChangeVarMax(const QString&);
  void slotChangeGoalName(const QString&);
  void slotChangeGoalNum(const QString&);
  void slotChangeGoalType(const QString&);

private:
  void createASCOFiles();

public:
  Optimize_Sim *Comp;
  Schematic *Doc;
  bool changed;

  QVBoxLayout *all;
  QLineEdit *NameEdit, *VarNameEdit,
            *VarInitEdit, *VarMinEdit, *VarMaxEdit,
            *GoalNameEdit, *GoalNumEdit;
  QCheckBox *VarActiveCheck;
  QComboBox *SimEdit, *GoalTypeCombo;
  QListView *VarList, *GoalList;

  QRegExp Expr;
  QRegExpValidator *Validator;
  QDoubleValidator *numVal;
};

#endif
