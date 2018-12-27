/***************************************************************************
                          componentdialog.h  -  description
                             -------------------
    begin                : Tue Sep 9 2003
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

#ifndef COMPONENTDIALOG_H
#define COMPONENTDIALOG_H

#include "component.h"
#include "platform.h"
#include "schematic_dialog.h"

#include <QDialog>
#include <assert.h>

class Schematic;

class QValidator;
class QIntValidator;
class QRegExpValidator;
class QTableWidget;
class QTableWidgetItem;
class QLabel;
class QLineEdit;
class QCheckBox;
class QRegExp;
class QComboBox;
class QPushButton;
class QVBoxLayout;


class ComponentDialog : public SchematicDialog {
public:
  ComponentDialog(Schematic&);
  ~ComponentDialog();

  void attach(Component* c);

private: // slot overrides.
  void slotButtOK();
  void slotButtCancel();
  void slotSelectProperty(QTableWidgetItem *item);
  void slotApplyInput();
  void slotApplyState(int State);
  void slotBrowseFile();
  void slotEditFile();
  void slotApplyChange(const QString& Text);
  void slotApplyProperty();
  void slotApplyPropName();

  void slotButtAdd();
  void slotButtRem();

  void slotButtUp();
  void slotButtDown();

  void slotSimTypeChange(int);
  void slotNumberChanged(const QString&);
  void slotStepChanged(const QString&);

  void slotParamEntered();
  void slotSimEntered(int);
  void slotValuesEntered();
  void slotStartEntered();
  void slotStopEntered();
  void slotStepEntered();
  void slotNumberEntered();
  void slotHHeaderClicked(int headerIdx);

public:
  void enableButtons();
  void disableButtons();

protected slots:
    void reject();
    bool eventFilter(QObject *obj, QEvent *event);

private:
  QVBoxLayout *all;   // the mother of all widgets
  QValidator  *Validator, *ValRestrict, *Validator2;
  QRegExp     Expr;
  QIntValidator *ValInteger;
  QTableWidget  *prop;
  QLineEdit   *edit, *NameEdit, *CompNameEdit;
  QComboBox   *ComboEdit;
  QLabel      *Name, *Description;
  QPushButton *BrowseButt, *EditButt, *ButtAdd, *ButtRem;
  QPushButton *ButtUp, *ButtDown;
  QCheckBox   *disp;
  Component   *Comp;
  Schematic   *Doc;
  bool        changed;
  int         tx_Dist, ty_Dist;   // remember the text position
  bool        setAllVisible; // used for toggling visibility of properties
 
  QLabel    *textType;
  QLabel    *textSim, *textParam, *textValues, *textStart, *textStop,
            *textStep, *textNumber;
  QLineEdit *editParam, *editValues, *editStart, *editStop,
            *editStep, *editNumber;
  QCheckBox *checkSim, *checkParam, *checkValues, *checkStart, *checkStop,
            *checkNumber, *checkType, *showName;
  QComboBox *comboSim, *comboType;

  void updateCompPropsList(void);
};

#endif
