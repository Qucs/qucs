/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
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

#ifndef QUCS_CMDELTDLG_H
#define QUCS_CMDELTDLG_H

#include "task_element.h"
#include "platform.h"
#include "schematic_dialog.h"

#include <QDialog>
#include <assert.h>

class Schematic;
class QVBoxLayout;
class QValidator;
class QIntValidator;
class QTableWidget;
class QLineEdit;
class QComboBox;
class QCheckBox;
class QPushButton;
class QLabel;

class TaskElementDialog : public SchematicDialog {
public:
  TaskElementDialog(QucsDoc*);
  ~TaskElementDialog();

  void attach(ElementGraphics* c) override;

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
  QVBoxLayout *_all{nullptr};
  QValidator  *Validator{nullptr}, *ValRestrict{nullptr}, *Validator2{nullptr};
  QRegExp     Expr;
  QIntValidator *ValInteger{nullptr};
  QTableWidget  *prop{nullptr};
  QLineEdit   *edit{nullptr}, *NameEdit{nullptr}, *CompNameEdit{nullptr};
  QComboBox   *ComboEdit{nullptr};
  QLabel      *Name{nullptr}, *Description{nullptr};
  QPushButton *BrowseButt{nullptr}, *EditButt{nullptr}, *ButtAdd{nullptr}, *ButtRem{nullptr};
  QPushButton *ButtUp{nullptr}, *ButtDown{nullptr};
  QCheckBox   *disp{nullptr};
  bool        changed;
  int         tx_Dist, ty_Dist;   // remember the text position
  bool        setAllVisible; // used for toggling visibility of properties
 
  QLabel    *textType{nullptr};
  QLabel    *textSim{nullptr}, *textParam{nullptr}, *textValues{nullptr}, *textStart{nullptr}, *textStop{nullptr},
            *textStep{nullptr}, *textNumber{nullptr};
  QLineEdit *editParam{nullptr}, *editValues{nullptr}, *editStart{nullptr}, *editStop{nullptr},
            *editStep{nullptr}, *editNumber{nullptr};
  QCheckBox *checkSim{nullptr}, *checkParam{nullptr}, *checkValues{nullptr}, *checkStart{nullptr}, *checkStop{nullptr},
            *checkNumber{nullptr}, *checkType{nullptr}, *showName{nullptr};
  QComboBox *comboSim{nullptr}, *comboType{nullptr};

  void updateCompPropsList(void);

private:
  ElementGraphics* _gfx{nullptr};
  TaskElement const* _comp{nullptr};
};

#endif
