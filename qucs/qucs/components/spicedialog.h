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

#include <qdialog.h>
#include <qregexp.h>
#include <qdir.h>

class QucsDoc;
class SpiceFile;
class QLineEdit;
class QPushButton;
class QListBox;
class QListBoxItem;
class QCheckBox;
class QVBoxLayout;
class QRegExpValidator;


extern QDir QucsWorkDir;


class SpiceDialog : public QDialog {
   Q_OBJECT
public:
  SpiceDialog(SpiceFile*, QucsDoc*, QWidget *parent=0);
  ~SpiceDialog();

private slots:
  void slotButtOK();
  void slotButtCancel();
  void slotButtApply();
  void slotButtBrowse();
  void slotButtEdit();

  void slotButtAdd();
  void slotButtRemove();
  void slotAddPort(QListBoxItem*);
  void slotRemovePort(QListBoxItem*);

private:
  bool loadSpiceNetList(const QString&);

  QVBoxLayout *all;   // the mother of all widgets
  QRegExpValidator  *Validator, *ValRestrict;
  QRegExp     Expr;
  QListBox    *NodesList, *PortsList;
  QCheckBox   *FileCheck;
  QLineEdit   *FileEdit, *CompNameEdit;
  QPushButton *ButtBrowse, *ButtEdit, *ButtAdd, *ButtRemove;
  SpiceFile   *Comp;
  QucsDoc     *Doc;
  bool        changed;
};

#endif
