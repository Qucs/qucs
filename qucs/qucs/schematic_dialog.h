/***************************************************************************
                          schematic_dialog.h
                             -------------------
    begin                : 2018
    copyright            : Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SCHEMATIC_DIALOG_H
#define SCHEMATIC_DIALOG_H

#include <QDialog>
#include <assert.h>
#include "object.h"
#include "platform.h"

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
class QucsDoc;


class SchematicDialog : public QDialog {
   Q_OBJECT //?
public:
  SchematicDialog(QucsDoc* d);
  virtual ~SchematicDialog(){}

  // slot?
  virtual void attach(Object*){ incomplete(); } // not yet.

private slots: // BUG: sane interface!!
  virtual void slotButtOK(){incomplete();}
  virtual void slotButtCancel(){incomplete();}
  virtual void slotSelectProperty(QTableWidgetItem * /*item*/){incomplete();}
  virtual void slotApplyInput(){incomplete();}
  virtual void slotApplyState(int /* State */){incomplete();}
  virtual void slotBrowseFile(){incomplete();}
  virtual void slotEditFile(){incomplete();}
  virtual void slotApplyChange(const QString& /*Text*/){incomplete();}
  virtual void slotApplyProperty(){incomplete();}
  virtual void slotApplyPropName(){incomplete();}

  virtual void slotButtAdd(){incomplete();}
  virtual void slotButtRem(){incomplete();}

  virtual void slotButtUp(){incomplete();}
  virtual void slotButtDown(){incomplete();}

  virtual void slotSimTypeChange(int){incomplete();}
  virtual void slotNumberChanged(const QString&){incomplete();}
  virtual void slotStepChanged(const QString&){incomplete();}

  virtual void slotParamEntered(){incomplete();}
  virtual void slotSimEntered(int){incomplete();}
  virtual void slotValuesEntered(){incomplete();}
  virtual void slotStartEntered(){incomplete();}
  virtual void slotStopEntered(){incomplete();}
  virtual void slotStepEntered(){incomplete();}
  virtual void slotNumberEntered(){incomplete();}
  virtual void slotHHeaderClicked(int /*headerIdx*/){incomplete();}

public:
  void enableButtons(){}
  void disableButtons(){}

protected slots:
    virtual void reject(){}
    virtual bool eventFilter(QObject * /*obj*/, QEvent * /*event*/){return false;}

private:
  virtual void updateCompPropsList(void){}
protected:
  Schematic* schematic(){ untested();
	  return Doc;
  }

private:
  Schematic* Doc;
};

#endif
