/***************************************************************************
                              spicedialog.h
                             ---------------
    begin                : Tue May 3 2005
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

#ifndef SPICEDIALOG_H
#define SPICEDIALOG_H

#include <QDialog>
#include <QRegExp>
#include "schematic_dialog.h"

class SchematicDoc;
class SpiceFile;
class QLineEdit;
class QPushButton;
class QListWidget;
class QListWidgetItem;
class QCheckBox;
class QVBoxLayout;
class QProcess;
class QRegExpValidator;
class QComboBox;
class QucsApp;
class QTextStream;

class SpiceDialog : public SchematicDialog {
public:
  SpiceDialog(QucsDoc*); // , SpiceFile*, SchematicDoc*) wtf?
 ~SpiceDialog();

 void attach(Object* spicecomponent);
private: // slot overrides
  void slotButtOK();
  void slotButtCancel();
  void slotButtApply();
  void slotButtBrowse();
  void slotButtEdit();

  void slotButtAdd();
  void slotButtRemove();
  void slotAddPort(QListWidgetItem *);
  void slotRemovePort(QListWidgetItem *);

  void slotGetNetlist();
  void slotGetError();

  void slotSkipOut();
  void slotSkipErr();
  void slotGetPrepOut();
  void slotGetPrepErr();
  void slotPrepChanged(int);

protected slots:
    void reject();

private:
  bool loadSpiceNetList(const QString&);

  QVBoxLayout *all{nullptr};   // the mother of all widgets
  QRegExpValidator  *Validator{nullptr}, *ValRestrict{nullptr};
  QRegExp     Expr;
  QListWidget *NodesList{nullptr}, *PortsList{nullptr};
  QCheckBox   *FileCheck{nullptr}, *SimCheck{nullptr};
  QLineEdit   *FileEdit{nullptr}, *CompNameEdit{nullptr};
  QPushButton *ButtBrowse{nullptr}, *ButtEdit{nullptr}, *ButtAdd{nullptr}, *ButtRemove{nullptr},
              *ButtOK{nullptr}, *ButtApply{nullptr}, *ButtCancel{nullptr};
  QComboBox   *PrepCombo{nullptr};
  SpiceFile   *Comp{nullptr};
  SchematicDoc*Doc{nullptr};
  bool        changed;
  int         currentPrep;

  QTextStream *prestream{nullptr};
  QProcess *QucsConv{nullptr}, *SpicePrep{nullptr};
  QString Line, Error;  // to store the text read from QucsConv
  int textStatus; // to store with text data QucsConv will sent next

  QucsApp* App{nullptr};
};

#endif
