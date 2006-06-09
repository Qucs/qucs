/***************************************************************************
                             librarydialog.h
                            -----------------
    begin                : Sun Jun 04 2006
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

#ifndef LIBRARYDIALOG_H
#define LIBRARYDIALOG_H

#include <qdialog.h>
#include <qregexp.h>
#include <qptrlist.h>
#include <qstringlist.h>

class QLabel;
class QLineEdit;
class QPushButton;
class QVBoxLayout;
class QRadioButton;
class QVButtonGroup;
class QRegExpValidator;


class LibraryDialog : public QDialog {
   Q_OBJECT
public:
  LibraryDialog(QWidget*);
 ~LibraryDialog();

private slots:
  void slotDelete();
  void slotRename();

private:
  QVBoxLayout *all;   // the mother of all widgets
  QLabel *theLabel;
  QLineEdit *NameEdit;
  QVButtonGroup *Group;
  QPushButton *ButtDelete, *ButtRename, *ButtClose;
  QPtrList<QRadioButton> LibList;

  QRegExp Expr;
  QRegExpValidator *Validator;
};

#endif
