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

#include "qucs.h"
#include "keysequenceedit.h"

class QPushButton;
class QLabel;
class QStandardItemModel;

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

  void slotCheckUnique();

private: 
  void fillMenu();
  void setShortcut(QString);
private: 
  //conflict record
  int conflictAt;
  QString conflictKey;

  QucsApp *App;

  QListWidget *menuList;
  QTableWidget *actionList;
  QLabel *messageLabel;
  KeySequenceEdit *sequenceInput;
  QPushButton *setButton, *removeButton, *defaultButton, *okButton;
};

#endif /* end of include guard: QUCSSHORTCUTDIALOG_H_ */
