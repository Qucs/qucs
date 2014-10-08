/***************************************************************************
                           qucssettingsdialog.h
                          ----------------------
    begin                : Fri Oct 3 2014
    copyright            : (C) 2014 by Yodalee
    email                : lc85301@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCSSHORTCUTDIALOG_H_
#define QUCSSHORTCUTDIALOG_H_ value

#include <QDialog>

#include "qucs.h"

class QListWidget;
class QTableWidget;
class QLabel;
class KeySequenceEdit;
class QPushButton;

class QucsShortcutDialog : public QDialog 
{
  Q_OBJECT
public:
  QucsShortcutDialog(QucsApp *parent = 0, const char *name = 0);
  virtual ~QucsShortcutDialog ();
private slots:
  void slotChooseMenu();
  void slotSetShortcut();
  void slotRemoveShortcut();
  void slotDefaultShortcut();
  void slotOK();

private: 
  void fillMenu();
private: 
  int current;
  QucsApp *App;

  QListWidget *menuList;
  QTableWidget *actionList;
  QLabel *messageLabel;
  KeySequenceEdit *sequenceInput;
  QPushButton *setButton, *removeButton, *defaultButton, *okButton;
};

#endif /* end of include guard: QUCSSHORTCUTDIALOG_H_ */
